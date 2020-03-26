/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/VersionedPutAll.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Versionable.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(102, VersionedPutAll);

using coherence::util::HashMap;
using coherence::util::Versionable;


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

VersionedPutAll::VersionedPutAll()
    : f_vMap(self()), m_fInsert(false), m_fReturn(false)
    {
    }

VersionedPutAll::VersionedPutAll(Map::View vMap, bool fAllowInsert,
        bool fReturn)
    : f_vMap(self(), vMap),
      m_fInsert(fAllowInsert),
      m_fReturn(fReturn)
    {
    COH_ENSURE_PARAM(vMap);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder VersionedPutAll::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    Object::Holder ohResult = processEntry(hEntry, f_vMap, m_fInsert, m_fReturn);
    return getNoResult() == ohResult ? NULL : ohResult;
    }

Map::View VersionedPutAll::processAll(Set::View vsetEntries) const
    {
    Map::Handle         hMapResults = HashMap::create();
    bool                fInsert     = m_fInsert;
    bool                fReturn     = m_fReturn;
    Map::View           vMap        = f_vMap;

    for (Iterator::Handle hIter = vsetEntries->iterator(); hIter->hasNext(); )
        {
        InvocableMap::Entry::Handle hEntry =
                cast<InvocableMap::Entry::Handle>(hIter->next());

        Object::Holder ohResult = processEntry(hEntry, vMap, fInsert, fReturn);

        if (ohResult != getNoResult())
            {
            hMapResults->put(hEntry->getKey(), ohResult);
            }
        }
    return hMapResults;
    }


// ----- PortableObject interface -------------------------------------------

void VersionedPutAll::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vMap, cast<Map::View>(hIn->readObject(0)));
    m_fInsert = hIn->readBoolean(1);
    m_fReturn = hIn->readBoolean(2);
    }

void VersionedPutAll::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vMap);
    hOut->writeBoolean(1,m_fInsert);
    hOut->writeBoolean(2,m_fReturn);
    }


// ----- Object interface ---------------------------------------------------

bool VersionedPutAll::equals(Object::View v) const
    {
    if (!instanceof<VersionedPutAll::View>(v))
        {
        return false;
        }
    VersionedPutAll::View vThat = cast<VersionedPutAll::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vMap, vThat->f_vMap)&&
            this->m_fInsert == vThat->m_fInsert &&
            this->m_fReturn == vThat->m_fReturn;
    }

size32_t VersionedPutAll::hashCode() const
    {
    return f_vMap->hashCode()+ (m_fInsert ? 1 : 2) +
            (m_fReturn ? 3 : 4);
    }


// ----- Helper functions ---------------------------------------------------

Object::Holder VersionedPutAll::processEntry(
        InvocableMap::Entry::Handle hEntry,
        Map::View vMap,
        bool fInsert,
        bool fReturn) const
    {
    Object::Holder ohKey = hEntry->getKey();

    if (vMap->containsKey(ohKey))
        {
        bool                fMatch;
        Versionable::Holder ohValueCur =
                cast<Versionable::Holder >(hEntry->getValue());
        Versionable::Holder ohValueNew =
                cast<Versionable::Holder >(vMap->get(ohKey));

        if (NULL == ohValueCur)
            {
            fMatch = fInsert;
            }
        else
            {
            Comparable::View verCur = ohValueCur->getVersionIndicator();
            Comparable::View verNew = ohValueNew->getVersionIndicator();

            fMatch = (verCur->compareTo(verNew) == 0);
            }

        if (fMatch)
            {
            Versionable::Handle hValueNew =
                    cast<Versionable::Handle>(ohValueNew);
            hValueNew->incrementVersion();
            hEntry->setValue(ohValueNew, false);
            return getNoResult();
            }
        return fReturn ? (Object::View) ohValueCur : getNoResult();
        }
    return getNoResult();
    }

COH_CLOSE_NAMESPACE3
