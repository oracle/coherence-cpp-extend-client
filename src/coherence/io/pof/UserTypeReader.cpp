/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/UserTypeReader.hpp"

#include "coherence/io/pof/IdentityHolder.hpp"

#include <limits>

COH_OPEN_NAMESPACE3(coherence,io,pof)


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Fake End-Of-Property-Stream indicator.
    */
    int32_t static getEOPS()
        {
        static int32_t EOPS = Integer32::max_value;
        return EOPS;
        }
    COH_STATIC_INIT(getEOPS());
    }


// ----- constructors -------------------------------------------------------

UserTypeReader::UserTypeReader(ReadBuffer::BufferInput::Handle hIn,
        PofContext::View vCtx, int32_t nTypeId, int32_t nVersionId)
        : super(hIn, vCtx),
          f_hParent(self(), NULL),
          m_nTypeId(nTypeId),
          m_nVersionId(nVersionId),
          m_iPrevProp(-1),
          m_iNextProp(0),
          m_ofNextProp(0),
          m_hReaderNested(self(), NULL),
          m_iNestedProp(-1)
    {
    COH_ENSURE(nTypeId >= 0);
    COH_ENSURE(nVersionId >= 0);

    // prime the property reader by knowing the offset of index of
    // the next property to read
    m_ofNextProp  = hIn->getOffset();
    int32_t iProp = hIn->readInt32();
    m_iNextProp   = iProp < 0 ? getEOPS() : iProp;
    }

UserTypeReader::UserTypeReader(PofBufferReader::Handle hParent,
        ReadBuffer::BufferInput::Handle hIn,
        PofContext::View vCtx, int32_t nTypeId, int32_t nVersionId)
        : super(hIn, vCtx),
          f_hParent(self(), hParent),
          m_nTypeId(nTypeId),
          m_nVersionId(nVersionId),
          m_iPrevProp(-1),
          m_iNextProp(0),
          m_ofNextProp(0),
          m_hReaderNested(self(), NULL),
          m_iNestedProp(-1)
    {
    COH_ENSURE(nTypeId >= 0);
    COH_ENSURE(nVersionId >= 0);

    // prime the property reader by knowing the offset of index of
    // the next property to read
    m_ofNextProp  = hIn->getOffset();
    int32_t iProp = hIn->readInt32();

    m_iNextProp   = iProp < 0 ? getEOPS() : iProp;
    }

UserTypeReader::UserTypeReader(PofBufferReader::Handle hParent,
        ReadBuffer::BufferInput::Handle hIn, PofContext::View vCtx)
        : super(hIn, vCtx),
          f_hParent(self(), hParent),
          m_nTypeId(0),
          m_nVersionId(0),
          m_iPrevProp(-1),
          m_iNextProp(0),
          m_ofNextProp(0),
          m_hReaderNested(self(), NULL),
          m_iNestedProp(-1)
    {
    // read the type and version directly from the buffer
    m_nTypeId     = hIn->readInt32();
    m_nVersionId  = hIn->readInt32();

    // prime the property reader by knowing the offset of index of
    // the next property to read
    m_ofNextProp  = hIn->getOffset();

    int32_t iProp = hIn->readInt32();
    m_iNextProp   = iProp < 0 ? getEOPS() : iProp;
    }

UserTypeReader::UserTypeReader(PofBufferReader::Handle hParent,
                ReadBuffer::BufferInput::Handle hIn,
                PofContext::View vCtx, int32_t nTypeId)
        : super(hIn, vCtx),
          f_hParent(self(), hParent),
          m_nTypeId(nTypeId),
          m_nVersionId(0),
          m_iPrevProp(-1),
          m_iNextProp(0),
          m_ofNextProp(0),
          m_hReaderNested(self(), NULL),
          m_iNestedProp(-1)
    {
    COH_ENSURE(nTypeId >= 0);

    // prime the property reader by knowing the offset of index of
    // the next property to read
    m_ofNextProp  = hIn->getOffset();
    m_iNextProp   = getEOPS();
    }

// ----- PofReader interface ------------------------------------------------

int32_t UserTypeReader::getUserTypeId() const
    {
    return m_nTypeId;
    }

int32_t UserTypeReader::getVersionId() const
    {
    return m_nVersionId;
    }

void UserTypeReader::registerIdentity(Object::Holder oh)
    {
    IdentityHolder::reset(this, -1, oh);
    }

PofReader::Handle UserTypeReader::createNestedPofReader(int32_t iProp)
    {
    UserTypeReader::Handle hReader;
    if (advanceTo(iProp))
        {
        hReader = UserTypeReader::create(this,
                (ReadBuffer::BufferInput::Handle) f_hIn,
                (PofContext::View) getPofContext());

        // note: there is no complete() call at this point, since the
        //       property has yet to be read
        }
    else
        {
        // nothing to read for that property
        complete(iProp);

        // return a "fake" reader that contains no data
        hReader = UserTypeReader::create(this,
                (ReadBuffer::BufferInput::Handle) f_hIn,
                (PofContext::View) getPofContext(), getUserTypeId());
        }

    m_hReaderNested = hReader;
    m_iNestedProp   = iProp;

    return hReader;
    }

Binary::View UserTypeReader::readRemainder()
    {
    // close nested buffer if it exists
    closeNested();

    // check if the property stream is aready exhausted
    int32_t iNextProp = m_iNextProp;
    if (getEOPS() == iNextProp)
        {
        return NULL;
        }

    // skip over all the remaining properties
    size32_t ofBegin = m_ofNextProp;
    size32_t ofEnd;
    do
        {
        skipValue(f_hIn);
        ofEnd     = f_hIn->getOffset();
        iNextProp = f_hIn->readInt32();
        }
    while (iNextProp != -1);

    m_iNextProp  = getEOPS();
    m_ofNextProp = ofEnd;

    // return all the properties that were skipped
    Array<octet_t>::View vab = f_hIn->getBuffer()->toOctetArray(ofBegin,
            ofEnd - ofBegin);
    return Binary::create(vab, 0, vab->length);
    }


// ----- internal methods ---------------------------------------------------

bool UserTypeReader::advanceTo(int32_t iProp)
    {
    // if a nested writer is still open, then "end" that property
    if (m_iNestedProp != -1)
        {
        closeNested();
        }

    // the terminating index is -1; if searching for -1, re-order the
    // goal to come after all other properties (which assumes that
    // there is no valid property index getEOPS())
    if (-1 == iProp)
        {
        iProp = getEOPS();
        }

    // check for backwards movement
    if (iProp <= m_iPrevProp)
        {
        COH_THROW_STREAM (IllegalStateException, "previous property index="
                << m_iPrevProp << ", requested property index=" << iProp
                << " while reading user type " << getUserTypeId());
        }

    // check if the stream is already in the correct location
    // (common case)
    int32_t iNextProp = m_iNextProp;
    if (iProp == iNextProp)
        {
        return true;
        }

    size32_t ofNextProp = m_ofNextProp;
    while (getEOPS() != iNextProp && iNextProp < iProp)
        {
        skipValue(f_hIn);

        ofNextProp = f_hIn->getOffset();
        iNextProp  = f_hIn->readInt32();
        if (iNextProp < 0)
            {
            iNextProp = getEOPS();
            }
        }

    m_ofNextProp = ofNextProp;
    m_iNextProp  = iNextProp;

    return iProp == iNextProp;
    }

void UserTypeReader::complete(int32_t iProp)
    {
    if (m_iNextProp == iProp)
        {
        m_ofNextProp  = f_hIn->getOffset();
        int iNextProp = f_hIn->readInt32();
        m_iNextProp   = iNextProp < 0 ? getEOPS() : iNextProp;
        }

    m_iPrevProp = iProp;
    }

void UserTypeReader::closeNested()
    {
    // check if a nested PofReader is open
    if (m_iNestedProp != -1)
        {
        UserTypeReader::Handle hReaderNested = m_hReaderNested;
        // check if there is some remainder that haven't been skipped
        if (hReaderNested->m_iNextProp != getEOPS())
            {
            hReaderNested->readRemainder();
            }
        // close it
        hReaderNested->closeNested();

        // finish reading the property that the nested PofReader was
        // reading from; this is the "complete()" call that was
        // deferred when the nested stream was opened
        complete(m_iNestedProp);

        m_hReaderNested = NULL;
        m_iNestedProp   = -1;
        }
    }

int32_t UserTypeReader::getNextPropertyIndex()
    {
    closeNested();
    return m_iNextProp == getEOPS() ? -1 : m_iNextProp;
    }

PofBufferReader::Handle UserTypeReader::getParentParser()
    {
    return f_hParent;
    }

PofBufferReader::View UserTypeReader::getParentParser() const
    {
    return f_hParent;
    }

int32_t UserTypeReader::getPreviousPropertyIndex() const
    {
    return m_iPrevProp;
    }

COH_CLOSE_NAMESPACE3
