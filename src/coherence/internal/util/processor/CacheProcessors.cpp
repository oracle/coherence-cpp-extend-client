/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/internal/util/processor/CacheProcessors.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE4(coherence,internal,util,processor)

COH_REGISTER_NAMED_CLASS(ProcNull, TypedClass<CacheProcessors::Null>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(470, ProcNull);

COH_REGISTER_NAMED_CLASS(ProcGet, TypedClass<CacheProcessors::Get>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(471, ProcGet);

COH_REGISTER_NAMED_CLASS(ProcGetOrDefault, TypedClass<CacheProcessors::GetOrDefault>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(472, ProcGetOrDefault);

COH_REGISTER_NAMED_CLASS(ProcPut, TypedClass<CacheProcessors::Put>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(473, ProcPut);

COH_REGISTER_NAMED_CLASS(ProcPutAll, TypedClass<CacheProcessors::PutAll>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(474, ProcPutAll);

COH_REGISTER_NAMED_CLASS(ProcPutIfAbsent, TypedClass<CacheProcessors::PutIfAbsent>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(475, ProcPutIfAbsent);

COH_REGISTER_NAMED_CLASS(ProcRemove, TypedClass<CacheProcessors::Remove>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(476, ProcRemove);

COH_REGISTER_NAMED_CLASS(ProcRemoveBlind, TypedClass<CacheProcessors::RemoveBlind>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(477, ProcRemoveBlind);

COH_REGISTER_NAMED_CLASS(ProcRemoveValue, TypedClass<CacheProcessors::RemoveValue>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(478, ProcRemoveValue);

COH_REGISTER_NAMED_CLASS(ProcReplace, TypedClass<CacheProcessors::Replace>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(479, ProcReplace);

COH_REGISTER_NAMED_CLASS(ProcReplaceValue, TypedClass<CacheProcessors::ReplaceValue>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(480, ProcReplaceValue);

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::HashMap;
using coherence::util::InvocableMap;

// ----- constructors -------------------------------------------------------

CacheProcessors::CacheProcessors()
    {
    }

// ----- CacheProcessors methods --------------------------------------------

InvocableMap::EntryProcessor::Handle CacheProcessors::nop()
    {
    return Null::create();
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::get()
    {
    return Get::create();
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::getOrDefault()
    {
    return GetOrDefault::create();
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::put(Object::Holder ohValue, int64_t cMillis)
    {
    return Put::create(ohValue, cMillis);
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::putAll(Map::View vMap)
    {
    return PutAll::create(vMap);
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::putIfAbsent(Object::Holder ohValue)
    {
    return PutIfAbsent::create(ohValue);
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::remove()
    {
    return Remove::create();
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::removeBlind()
    {
    return RemoveBlind::create();
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::remove(Object::View v)
    {
    return RemoveValue::create(v);
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::replace(Object::Holder oh)
    {
    return CacheProcessors::Replace::create(oh);
    }

InvocableMap::EntryProcessor::Handle CacheProcessors::replace(Object::View vOldValue, Object::View vNewValue)
    {
    return ReplaceValue::create(vOldValue, vNewValue);
    }

// ---- Entry Processors --------------------------------------------

// ---- BaseProcessor entry processor -------------------------------

void CacheProcessors::BaseProcessor::readExternal(PofReader::Handle /*hIn*/)
    {
    }

void CacheProcessors::BaseProcessor::writeExternal(PofWriter::Handle /*hOut*/) const
    {
    }

// ---- Null entry processor ----------------------------------------

Object::Holder CacheProcessors::Null::process(InvocableMap::Entry::Handle /*hEntry*/) const
    {
    return NULL;
    }

// ---- Get entry processor -----------------------------------------

Object::Holder CacheProcessors::Get::process(InvocableMap::Entry::Handle hEntry) const
    {
    return hEntry->getValue();
    }

// ---- GetOrDefault entry processor --------------------------------

CacheProcessors::GetOrDefault::GetOrDefault()
    {
    }

Object::Holder CacheProcessors::GetOrDefault::process(InvocableMap::Entry::Handle hEntry) const
    {
    return NULL == hEntry ? NULL : hEntry->getValue();
    }

// ---- Put entry processor -----------------------------------------

CacheProcessors::Put::Put()
        : f_ohValue(self()), m_cMillis(0)
    {
    }

CacheProcessors::Put::Put(Object::Holder ohValue, int64_t cMillis)
        : f_ohValue(self(), ohValue), m_cMillis(cMillis)
    {
    }

Object::Holder CacheProcessors::Put::process(InvocableMap::Entry::Handle hEntry) const
    {
    hEntry->setValue(f_ohValue);
    return NULL;
    }

void CacheProcessors::Put::readExternal(PofReader::Handle hIn)
    {
    initialize(f_ohValue, hIn->readObject(0));
    m_cMillis = hIn->readInt64(1);
    }

void CacheProcessors::Put::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_ohValue);
    hOut->writeInt64(1, m_cMillis);
    }

// ---- PutAll entry processor --------------------------------------

CacheProcessors::PutAll::PutAll()
        : f_vMap(self())
    {
    }

CacheProcessors::PutAll::PutAll(Map::View vMap)
        : f_vMap(self(), vMap)
    {
    }

Object::Holder CacheProcessors::PutAll::process(InvocableMap::Entry::Handle hEntry) const
    {
    hEntry->setValue(f_vMap->get(hEntry->getKey()));
    return NULL;
    }

void CacheProcessors::PutAll::readExternal(PofReader::Handle hIn)
    {
    Map::Handle hMap = HashMap::create();
    initialize(f_vMap, hIn->readMap(0, hMap));
    }

void CacheProcessors::PutAll::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeMap(0, f_vMap);
    }

// ---- PutIfAbsent entry processor ---------------------------------

CacheProcessors::PutIfAbsent::PutIfAbsent()
        : f_ohValue(self())
    {
    }

CacheProcessors::PutIfAbsent::PutIfAbsent(Object::Holder ohValue)
        : f_ohValue(self(), ohValue)
    {
    }

Object::Holder CacheProcessors::PutIfAbsent::process(InvocableMap::Entry::Handle hEntry) const
    {
    if (hEntry->isPresent() && hEntry->getValue() != NULL)
        {
        return hEntry->getValue();
        }
    else
        {
        hEntry->setValue(f_ohValue);
        return NULL;
        }
    }

void CacheProcessors::PutIfAbsent::readExternal(PofReader::Handle hIn)
    {
    initialize(f_ohValue, hIn->readObject(0));
    }

void CacheProcessors::PutIfAbsent::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_ohValue);
    }

// ---- Remove entry processor --------------------------------------

 Object::Holder CacheProcessors::Remove::process(InvocableMap::Entry::Handle hEntry) const
     {
     Object::Holder ohValue = hEntry->getValue();
     hEntry->remove(false);
     return ohValue;
     }

Object::Holder CacheProcessors::RemoveBlind::process(InvocableMap::Entry::Handle hEntry) const
    {
    hEntry->remove(false);
    return NULL;
    }

// ---- RemoveValue entry processor ---------------------------------

CacheProcessors::RemoveValue::RemoveValue()
        : f_ov(self())
    {
    }

CacheProcessors::RemoveValue::RemoveValue(Object::View v)
        : f_ov(self(), v)
    {
    }

Object::Holder CacheProcessors::RemoveValue::process(InvocableMap::Entry::Handle hEntry) const
    {
    if (hEntry->isPresent())
        {
        Object::View vCurrent = hEntry->getValue();
        if (vCurrent->equals(f_ov))
            {
            hEntry->remove(false);
            return (Object::View) Boolean::valueOf(true);
            }
         }

    return (Object::View) Boolean::valueOf(false);
    }

void CacheProcessors::RemoveValue::readExternal(PofReader::Handle hIn)
    {
    initialize(f_ov, hIn->readObject(0));
    }

void CacheProcessors::RemoveValue::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_ov);
    }

// ---- Replace entry processor -------------------------------------

CacheProcessors::Replace::Replace()
        : f_ohValue(self())
    {
    }

CacheProcessors::Replace::Replace(Object::Holder oh)
        : f_ohValue(self(), oh)
    {
    }

Object::Holder CacheProcessors::Replace::process(InvocableMap::Entry::Handle hEntry) const
    {
    return hEntry->isPresent() ? hEntry->setValue(f_ohValue) : NULL;
    }

void CacheProcessors::Replace::readExternal(PofReader::Handle hIn)
    {
    initialize(f_ohValue, hIn->readObject(0));
    }

void CacheProcessors::Replace::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_ohValue);
    }

// ---- ReplaceValue entry processor --------------------------------

CacheProcessors::ReplaceValue::ReplaceValue()
        : f_vOldValue(self()), f_vNewValue(self())
    {
    }

CacheProcessors::ReplaceValue::ReplaceValue(Object::View vOldValue, Object::View vNewValue)
        : f_vOldValue(self(), vOldValue), f_vNewValue(self(), vNewValue)
    {
    }

Object::Holder CacheProcessors::ReplaceValue::process(InvocableMap::Entry::Handle hEntry) const
    {
    if (hEntry->isPresent())
        {
        Object::View vValueCurrent = hEntry->getValue();
        if (vValueCurrent->equals(f_vOldValue))
            {
            hEntry->setValue(f_vNewValue);
            return (Object::View) Boolean::valueOf(true);
            }
        }

    return (Object::View) Boolean::valueOf(false);
    }

void CacheProcessors::ReplaceValue::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vOldValue, hIn->readObject(0));
    initialize(f_vNewValue, hIn->readObject(1));
    }

void CacheProcessors::ReplaceValue::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vOldValue);
    hOut->writeObject(1, f_vNewValue);
    }

COH_CLOSE_NAMESPACE4
