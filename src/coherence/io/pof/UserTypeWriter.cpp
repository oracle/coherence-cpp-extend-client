/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/UserTypeWriter.hpp"

#include "coherence/io/EOFException.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::EOFException;


// ----- constructors -------------------------------------------------------

UserTypeWriter::UserTypeWriter(WriteBuffer::BufferOutput::Handle hOut,
        PofContext::View vCtx, int32_t nTypeId, int32_t iProp)
        : super(hOut, vCtx),
          f_hWriterParent(self(), NULL),
          m_nTypeId(nTypeId),
          m_nVersionId(0),
          m_iProp(iProp),
          m_nId(-1),
          m_iPrevProp(-1),
          m_fUserTypeBegin(false),
          m_fUserTypeEnd(false),
          m_hComplex(self()),
          m_nComplexId(0),
          m_hWriterNested(self(), NULL)
    {
    COH_ENSURE(nTypeId >= 0);
    }

UserTypeWriter::UserTypeWriter(PofBufferWriter::Handle hParent,
        WriteBuffer::BufferOutput::Handle hOut,
        PofContext::View vCtx, int32_t nTypeId, int32_t iProp)
        : super(hOut, vCtx),
          f_hWriterParent(self(), hParent),
          m_nTypeId(nTypeId),
          m_nVersionId(0),
          m_iProp(iProp),
          m_nId(-1),
          m_iPrevProp(-1),
          m_fUserTypeBegin(false),
          m_fUserTypeEnd(false),
          m_hComplex(self()),
          m_nComplexId(0),
          m_hWriterNested(self(), NULL)
    {
    COH_ENSURE(nTypeId >= 0);
    initialize(f_hRefs, NULL == hParent
            ? (ReferenceLibrary::Handle) NULL
            : hParent->f_hRefs);
    }

UserTypeWriter::UserTypeWriter(WritingPofHandler::Handle hHandler,
        PofContext::View vCtx, int32_t nTypeId, int32_t iProp)
        : super(hHandler, vCtx),
          f_hWriterParent(self(), NULL),
          m_nTypeId(nTypeId),
          m_nVersionId(0),
          m_iProp(iProp),
          m_nId(-1),
          m_iPrevProp(-1),
          m_fUserTypeBegin(false),
          m_fUserTypeEnd(false),
          m_hComplex(self()),
          m_nComplexId(0),
          m_hWriterNested(self(), NULL)
    {
    COH_ENSURE(nTypeId >= 0);
    }

UserTypeWriter::UserTypeWriter(PofBufferWriter::Handle hParent,
        WritingPofHandler::Handle hHandler,
        PofContext::View vCtx, int nTypeId, int iProp, int nId)
        : super(hHandler, vCtx),
          f_hWriterParent(self(), hParent),
          m_nTypeId(nTypeId),
          m_nVersionId(0),
          m_iProp(iProp),
          m_nId(nId),
          m_iPrevProp(-1),
          m_fUserTypeBegin(false),
          m_fUserTypeEnd(false),
          m_hComplex(self()),
          m_nComplexId(0),
          m_hWriterNested(self(), NULL)
    {
    COH_ENSURE(nTypeId >= 0);
    initialize(f_hRefs, NULL == hParent
            ? (ReferenceLibrary::Handle) NULL
            : hParent->f_hRefs);
    }


// ----- PofWriter interface ------------------------------------------------

void UserTypeWriter::writeObject(int32_t iProp, Object::View v)
    {
    // force the user type information to be written, if necessary,
    // because otherwise the effect of a call to registerIdentity()
    // will be lost
    writeUserTypeInfo();

    PofBufferWriter::writeObject(iProp, v);
    }

int32_t UserTypeWriter::getUserTypeId() const
    {
    return m_nTypeId;
    }

int32_t UserTypeWriter::getVersionId() const
    {
    return m_nVersionId;
    }

void UserTypeWriter::setVersionId(int32_t nVersionId)
    {
    if (nVersionId < 0)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "negative version identifier: " << nVersionId);
        }
    m_nVersionId = nVersionId;
    }

PofWriter::Handle UserTypeWriter::createNestedPofWriter(int32_t iProp)
    {
    return createNestedPofWriter(iProp, m_nTypeId);
	}
	
PofWriter::Handle UserTypeWriter::createNestedPofWriter(int32_t iProp, int32_t nTypeId)
    {
    beginProperty(iProp);
    f_hHandler->registerIdentity(-1);

    // create a new PofWriter for the user type
    PofContext::View vCtx          = getPofContext();
    UserTypeWriter::Handle hWriter = UserTypeWriter::create(this,
            (WritingPofHandler::Handle) f_hHandler, vCtx, nTypeId, iProp);
    if (isReferenceEnabled())
        {
        hWriter->enableReference();
        }
    m_hWriterNested = hWriter;
    return hWriter;
    // note: there is no endProperty() call at this point, since the
    //       property has yet to be written
    }

void UserTypeWriter::writeRemainder(Binary::View vBinProps)
    {
    // if a nested writer is still open, then "end" that property
    closeNested();

    // write out the type and version identifiers, if necessary
    writeUserTypeInfo();

    if (NULL != vBinProps)
        {
        f_hOut->writeBuffer(vBinProps);
        }
    f_hHandler->endComplexValue();

    m_fUserTypeEnd = true; // EOF
    m_hComplex     = NULL;
    m_nComplexId   = 0;
    }


// ----- internal methods ---------------------------------------------------

PofBufferWriter::Handle UserTypeWriter::getParentWriter()
    {
    return f_hWriterParent;
    }

PofBufferWriter::View UserTypeWriter::getParentWriter() const
    {
    return f_hWriterParent;
    }

void UserTypeWriter::beginProperty(int32_t iProp)
    {
    // if a nested writer is still open, then "end" that property
    closeNested();

    // check for negative index
    if (iProp < 0)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "negative property index: " << iProp);
        }

    // write out the type and version identifiers, if necessary
    writeUserTypeInfo();

    // check for backwards movement
    if (iProp <= m_iPrevProp && f_hHandler->getComplexId() == m_nComplexId)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "previous property index=" << m_iPrevProp <<
                ", requested property index=" << iProp <<
                " while writing user type " << getUserTypeId());
        }
    }

void UserTypeWriter::endProperty(int32_t iProp)
    {
    // c++ specific optimization: the reference comparison of PofHandler's
    // complex to our complex is surprisingly expensive according to the profiler.
    // this is because a read of a MemberHandle involves an increment of the
    // referenced object's reference count. The begin/endPropery methods are
    // heavily used and need to be "free"
    if (f_hHandler->getComplexId() == m_nComplexId)
        {
        m_iPrevProp = iProp;
        }
    }

void UserTypeWriter::closeNested()
    {
    // check if a nested PofWriter is open
    UserTypeWriter::Handle hWriterNested = m_hWriterNested;
    if (NULL != hWriterNested)
        {
        // make sure that the entire nested is terminated
        if (!hWriterNested->m_fUserTypeEnd)
            {
            f_hHandler->endComplexValue();
            }

        // close it
        hWriterNested->closeNested();

        // finish writing the property that the nested PofWriter was
        // writing into
        endProperty(hWriterNested->m_iProp);

        m_hWriterNested = NULL;
        }
    }

void UserTypeWriter::writeUserTypeInfo()
    {
    // check for EOF
    if (m_fUserTypeEnd)
        {
        COH_THROW (EOFException::create("user type POF stream terminated"));
        }

    if (!m_fUserTypeBegin)
        {
        f_hHandler->beginUserType(m_iProp, m_nId, getUserTypeId(), getVersionId());

        m_fUserTypeBegin = true;
        m_hComplex   = f_hHandler->getComplex();
        m_nComplexId = f_hHandler->getComplexId();
        }
    }

bool UserTypeWriter::isEvolvable()
    {
    if (!m_fEvolvable)
        {
        PofBufferWriter::Handle hParent = f_hWriterParent;
        if (NULL != hParent)
            {
            m_fEvolvable = hParent->isEvolvable();
            }
        }
    return m_fEvolvable;
    }


// ----- accessors ---------------------------------------------------

void UserTypeWriter::enableReference()
    {
    if (NULL == f_hRefs)
        {
        PofBufferWriter::Handle hParent = f_hWriterParent;
        if (NULL != hParent)
            {
            hParent->enableReference();
            initialize(f_hRefs, (ReferenceLibrary::Handle) hParent->f_hRefs);
            }
        else
            {
            initialize(f_hRefs, ReferenceLibrary::create());
            }

        UserTypeWriter::Handle hChild = m_hWriterNested;
        if (NULL != hChild)
            {
            hChild->enableReference();
            }
        }
    }

COH_CLOSE_NAMESPACE3
