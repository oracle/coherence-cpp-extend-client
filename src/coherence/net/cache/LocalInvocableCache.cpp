/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/cache/LocalInvocableCache.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/extractor/EntryExtractor.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::ArrayList;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Set;
using coherence::util::extractor::EntryExtractor;
using coherence::util::extractor::KeyExtractor;

// ----- constructors -------------------------------------------------------

LocalInvocableCache::LocalInvocableCache(bool fEnforceLocking,
        int64_t              cWaitMillis,
        int32_t              cUnits,
        int32_t              cExpiryMillis,
        float64_t            dflPruneLevel,
        size32_t             cInitialBuckets,
        float32_t            flLoadFactory,
        float32_t            flGrowthRate)
    : super(fEnforceLocking, cWaitMillis, cUnits, cExpiryMillis,
            dflPruneLevel, cInitialBuckets, flLoadFactory, flGrowthRate)
    {
    }


// ----- InvocableMap interface ---------------------------------------------

Object::Holder LocalInvocableCache::invoke(Object::View vKey,
        EntryProcessor::Handle hAgent)
    {
    COH_CACHE_LOCK(this, vKey)
        {
        return hAgent->process(ensureEntry(vKey));
        }
    }

Map::View LocalInvocableCache::invokeAll(Collection::View vCollKeys,
        EntryProcessor::Handle hAgent)
    {
    // create the set of entries to process
    Set::Handle hSetEntries = makeEntrySet(vCollKeys);

    // try to lock them all at once
    List::Handle hListLocked = lockAll(vCollKeys, 0L);
    if (hListLocked == NULL)
        {
        // the attempt failed; do it one-by-one
        Map::Handle hMapResult = HashMap::create(vCollKeys->size());
        for(Iterator::Handle hIter = vCollKeys->iterator(); hIter->hasNext(); )
            {
            Object::View vKey = hIter->next();
            hMapResult->put(vKey, invoke(vKey, hAgent));
            }
        return hMapResult;
        }
    else
        {
        struct processAllFinally
            {
            processAllFinally(LocalInvocableCache::Handle _hCache,
                    Collection::View _vCollKeys)
                : hCache(_hCache), vKeys(_vCollKeys)
                {
                }
            ~processAllFinally()
                {
                for (Iterator::Handle hIterLocked = vKeys->iterator();
                        hIterLocked->hasNext(); )
                    {
                    hCache->unlock(hIterLocked->next());
                    }
                }
            LocalInvocableCache::Handle hCache;
            Collection::View            vKeys;
            } finally(this, vCollKeys);

        return hAgent->processAll(hSetEntries);
        }
    }

Map::View LocalInvocableCache::invokeAll(Filter::View vFilter,
        EntryProcessor::Handle hAgent)
    {
    return invokeAll((Collection::View)keySet(vFilter), hAgent);
    }

Object::Holder LocalInvocableCache::aggregate(Collection::View vCollKeys,
        EntryAggregator::Handle hAgent) const
    {
    return hAgent->aggregate(makeEntrySet(vCollKeys));
    }

Object::Holder LocalInvocableCache::aggregate(Filter::View vFilter,
        EntryAggregator::Handle hAgent) const
    {
    return aggregate((Collection::View)keySet(vFilter), hAgent);
    }


// ----- helper methods -----------------------------------------------------

List::Handle LocalInvocableCache::lockAll(Collection::View vCollKeys, int64_t cWait) const
    {
    // remove the duplicates

    Set::View vSet;
    if (instanceof<Set::View>(vCollKeys))
        {
        vSet = cast<Set::View>(vCollKeys);
        }
    else
        {
        HashSet::Handle hSet = HashSet::create(vCollKeys->size());
        hSet->addAll(vCollKeys);
        vSet = hSet;
        }

    // TODO NSA - Replace with ArrayList
    List::Handle hListKeys   = ArrayList::create(vSet);
    List::Handle hListLocked = ArrayList::create();
    size32_t     cKeys       = hListKeys->size();
    bool         fSuccess    = true;

    do
        {
        int64_t cWaitNext = cWait; // allow blocking wait for the very first key
        for (size32_t i = 0; i < cKeys; i++)
            {
            Object::View vKey = hListKeys->get(i);

            if ((fSuccess = lock(vKey, cWaitNext)))
                {
                // add the last locked item into the front of the locked
                // list so it behaves as a stack (FILO strategy)
                hListLocked->add(0, vKey);

                // to prevent a deadlock don't wait afterwards
                cWaitNext = 0l;
                }
            else
                {
                if (i == 0)
                    {
                    // the very first key cannot be locked -- timeout
                    return NULL;
                    }

                // unlock all we hold and try again
                for (Iterator::Handle hIterLocked = hListLocked->iterator();
                        hIterLocked->hasNext(); )
                    {
                    unlock(hIterLocked->next());
                    }
                hListLocked->clear();

                // move the "offending" key to the top of the list
                // so next iteration we will attempt to lock it first
                hListKeys->remove(i);
                hListKeys->add(0, vKey);
                }
            }
        } while (!fSuccess);

    return hListLocked;
    }

Set::Handle LocalInvocableCache::makeEntrySet(Collection::View vCollKeys)
    {
    Set::Handle hSetEntries = HashSet::create(vCollKeys->size());
    for(Iterator::Handle hIter = vCollKeys->iterator(); hIter->hasNext(); )
        {
        hSetEntries->add(ensureEntry(hIter->next()));
        }

    return hSetEntries;
    }

Set::Handle LocalInvocableCache::makeEntrySet(Collection::View vCollKeys) const
    {
    Set::Handle hSetEntries = HashSet::create(vCollKeys->size());
    for(Iterator::Handle hIter = vCollKeys->iterator(); hIter->hasNext(); )
        {
        // store entries as views so that aggregators don't have write access
        hSetEntries->add((Object::View) ensureEntry(hIter->next()));
        }

    return hSetEntries;
    }

// ----- data members -------------------------------------------------------

Object::Handle LocalInvocableCache::getUnknown()
    {
    static FinalHandle<Object> hUnKnown(System::common(),
            Object::create());
    return hUnKnown;
    }
COH_STATIC_INIT(LocalInvocableCache::getUnknown());


// ----- inner class: Entry -------------------------------------------------

SafeHashMap::Entry::Handle LocalInvocableCache::instantiateEntry(
        Object::View vKey, Object::Holder ohValue, size32_t nHash)
    {
    return Entry::create(vKey, ohValue, nHash, this);
    }

SafeHashMap::Entry::Handle LocalInvocableCache::instantiateEntry(
        SafeHashMap::Entry::View vEntry)
    {
    return Entry::create(cast<LocalInvocableCache::Entry::View>(vEntry));
    }

LocalInvocableCache::Entry::Handle LocalInvocableCache::instantiateSurrogateEntry(
        Object::View vKey)
    {
    LocalInvocableCache::Entry::Handle hEntry =
        cast<LocalInvocableCache::Entry::Handle>(instantiateEntry(vKey,
            NULL, getHashCode(vKey)));
    hEntry->setSurrogate(true);
    return hEntry;
    }

LocalInvocableCache::Entry::Handle LocalInvocableCache::ensureEntry(Object::View vKey)
    {
    Entry::Handle hEntry = cast<Entry::Handle>(getEntry(vKey));
    return NULL == hEntry
        ? instantiateSurrogateEntry(vKey)
        : hEntry;
    }

LocalInvocableCache::Entry::View LocalInvocableCache::ensureEntry(Object::View vKey) const
    {
    Entry::View vEntry = cast<Entry::View>(getEntry(vKey));
    if (NULL == vEntry)
        {
        // instantiation requires non-const access
        LocalInvocableCache::Handle hThis = const_cast<LocalInvocableCache*>(this);
        COH_ENSURE(hThis != NULL);
        return hThis->instantiateSurrogateEntry(vKey);
        }
    return vEntry;
    }

LocalInvocableCache::Entry::Entry(const Entry& that)
    : super(that), m_fSurrogate(false)
    {
    }

LocalInvocableCache::Entry::Entry(Entry::View vEntry)
    : super(vEntry), m_fSurrogate(false)
    {
    }

LocalInvocableCache::Entry::Entry(
        Object::View vKey, Object::Holder ohValue, size32_t nHash,
        LocalInvocableCache::Handle hCache, bool fSurrogate)
    : super(vKey, ohValue, nHash, hCache), m_fSurrogate(fSurrogate)
    {
    }

Object::Holder LocalInvocableCache::Entry::setValue(Object::Holder ohValue)
    {
    return isSurrogate()
        ?  f_hCache->put(inherited::getKey(), ohValue)
        : inherited::setValue(ohValue);
    }

Object::Holder LocalInvocableCache::Entry::setValue(Object::Holder ohValue,
        bool /*fSynthetic*/)
    {
    return setValue(ohValue);
    }

Object::Holder LocalInvocableCache::Entry::getValue()
    {
    return isSurrogate()
        ? f_hCache->get(inherited::getKey())
        : inherited::getValue();
    }

Object::Holder LocalInvocableCache::Entry::getValue() const
    {
    return isSurrogate()
        ? ((Map::View) f_hCache)->get(inherited::getKey())
        : inherited::getValue();
    }

void LocalInvocableCache::Entry::update(ValueUpdater::View vUpdater,
        Object::Holder ohValue)
    {
    Object::Handle hTarget = inherited::getValue()->clone();
    vUpdater->update(hTarget, ohValue);

    // Make the new object immutable to prevent additional clone
    Object::View vTarget = hTarget;
    hTarget = NULL;
    setValue(vTarget, false);
    }

/**
* {@inheritDoc}
*/
bool LocalInvocableCache::Entry::isPresent() const
    {
    Object::Holder ohValue = inherited::getValue();
    return (ohValue != LocalInvocableCache::getUnknown() && ohValue != NULL)
        || f_hCache->containsKey(inherited::getKey());
    }

void LocalInvocableCache::Entry::remove(bool fSynthetic)
    {
    Object::View vKey = inherited::getKey();
    if (fSynthetic)
        {
        f_hCache->evict(vKey);
        }
    else
        {
        f_hCache->keySet()->remove(vKey);
        }
    }

bool LocalInvocableCache::Entry::isSurrogate() const
    {
    return m_fSurrogate;
    }

void LocalInvocableCache::Entry::setSurrogate(bool fSurrogate)
    {
    m_fSurrogate = fSurrogate;
    }

// ----- QueryMap::Entry interface --------------------------------------

Object::Holder LocalInvocableCache::Entry::extract(
        ValueExtractor::View vExtractor) const
    {
    return vExtractor->extract(
            instanceof<EntryExtractor::View>(vExtractor)
                    ? (Object::Holder) (Entry::View) this
                    : instanceof<KeyExtractor::View>(vExtractor)
                        ? (Object::Holder) inherited::getKey()
                        : (Object::Holder) inherited::getValue());
    }

COH_CLOSE_NAMESPACE3
