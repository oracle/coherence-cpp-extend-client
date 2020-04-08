/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/VersionedPut.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(101, VersionedPut);

// ----- constants ------------------------------------------------------

namespace
    {
    /**
    * Used internally to differentiate between "no result" and a NULL
    * result.
    * "no result" should never be returned through the public APIs.
    * A value of "no result" may be returned from processEntry() and will
    * never be included in the result map returned from processAll().
    */
    Object::View getNoResult()
        {
        static FinalView<Object> vo(System::common(), Object::create());
        return vo;
        }
    COH_STATIC_INIT(getNoResult());
    }

// ----- constructors -------------------------------------------------------

VersionedPut::VersionedPut()
    : f_hValue(self(), Versionable::Handle(NULL), true),
      m_fInsert(false),
      m_fReturn(false)
    {
    }

VersionedPut::VersionedPut(Versionable::Handle hValue,
    bool fAllowInsert, bool fReturn)
    : f_hValue(self(), hValue, true),
    m_fInsert(fAllowInsert),
    m_fReturn(fReturn)
    {
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder VersionedPut::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    Object::Holder ohResult = processEntry(hEntry, f_hValue, m_fInsert, m_fReturn);
    return getNoResult() == ohResult ? NULL : ohResult;
    }

Map::View VersionedPut::processAll(Set::View vsetEntries) const
    {
    Map::Handle         hMapResults = HashMap::create();
    bool                fInsert     = m_fInsert;
    bool                fReturn     = m_fReturn;
    Versionable::Holder ohValue     = f_hValue;

    for (Iterator::Handle hIter = vsetEntries->iterator(); hIter->hasNext(); )
        {
        InvocableMap::Entry::Handle hEntry =
                cast<InvocableMap::Entry::Handle>(hIter->next());

        Object::Holder ohResult = processEntry(hEntry, ohValue, fInsert, fReturn);

        if (ohResult != getNoResult())
            {
            hMapResults->put(hEntry->getKey(), ohResult);
            }
        }
    return hMapResults;
    }


// ----- PortableObject interface -------------------------------------------

void VersionedPut::readExternal(PofReader::Handle hIn)
    {
    initialize(f_hValue, cast<Versionable::Handle>(hIn->readObject(0)));
    m_fInsert = hIn->readBoolean(1);
    m_fReturn = hIn->readBoolean(2);
    }

void VersionedPut::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_hValue);
    hOut->writeBoolean(1,m_fInsert);
    hOut->writeBoolean(2,m_fReturn);
    }


// ----- Object interface ---------------------------------------------------

bool VersionedPut::equals(Object::View v) const
    {
    if (!instanceof<VersionedPut::View>(v))
        {
        return false;
        }
    VersionedPut::View vThat = cast<VersionedPut::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_hValue, vThat->f_hValue) &&
            this->m_fInsert == vThat->m_fInsert &&
            this->m_fReturn == vThat->m_fReturn;
    }

size32_t VersionedPut::hashCode() const
    {
    return f_hValue->hashCode() + (m_fInsert ? 1 : 2) +
            (m_fReturn ? 3 : 4);
    }


// ----- Helper functions ---------------------------------------------------

Object::Holder VersionedPut::processEntry(
        InvocableMap::Entry::Handle hEntry,
        Versionable::Holder ohValueNew,
        bool fInsert,
        bool fReturn) const
    {
    bool                fMatch;
    Versionable::Holder ohValueCur = cast<Versionable::Holder >(
            hEntry->getValue());

    if (NULL == ohValueCur)
        {
        fMatch = fInsert;
        }
    else
        {
        Comparable::View vVerCur = ohValueCur->getVersionIndicator();
        Comparable::View vVerNew = ohValueNew->getVersionIndicator();

        fMatch = (vVerCur->compareTo(vVerNew) == 0);
        }

    if (fMatch)
        {
        Versionable::Handle hValueNew = cast<Versionable::Handle>(ohValueNew);
        hValueNew->incrementVersion();
        hEntry->setValue(ohValueNew, false);
        return getNoResult();
        }

    return fReturn ? (Object::View) ohValueCur : getNoResult();
    }


COH_CLOSE_NAMESPACE3
