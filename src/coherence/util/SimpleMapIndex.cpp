/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SimpleMapIndex.hpp"

#include "coherence/util/Arrays.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LiteSet.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/NullImplementation.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/SafeHashSet.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/TreeMap.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/AbstractExtractor.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/extractor/MultiExtractor.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"
#include "private/coherence/util/SimpleIterator.hpp"
#include "private/coherence/util/SimpleMapEntry.hpp"
#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::AbstractExtractor;
using coherence::util::extractor::KeyExtractor;
using coherence::util::extractor::MultiExtractor;
using coherence::util::logging::Logger;


// ----- constructors -------------------------------------------------------

SimpleMapIndex::SimpleMapIndex(ValueExtractor::View vExtractor,
        bool fOrdered,
        Comparator::View vComparator,
        bool fInit)
        : f_vValueExtractor(self(), vExtractor),
          m_fOrdered(fOrdered),
          f_vComparator(self(), vComparator),
          f_hMapForward(self()),
          f_hMapInverse(self()),
          m_fSplitCollection(!instanceof<MultiExtractor::View>(vExtractor)),
          f_hSetKeyExcluded(self()),
          m_fImmutableValues(instanceof<KeyExtractor::View>(vExtractor) ||
              (instanceof<AbstractExtractor::View>(vExtractor) &&
              cast<AbstractExtractor::View>(vExtractor)->getTarget() == AbstractExtractor::key))
    {
    if (fInit)
        {
        init();
        }
    }


// ----- MapIndex interface -------------------------------------------------

ValueExtractor::View SimpleMapIndex::getValueExtractor() const
    {
    return f_vValueExtractor;
    }

bool SimpleMapIndex::isOrdered() const
    {
    return m_fOrdered;
    }

bool SimpleMapIndex::isPartial() const
    {
    // an index is partial if there are (corrupted) entries
    // which are in the cache but not in the index
    return !f_hSetKeyExcluded->isEmpty();
    }

Map::View SimpleMapIndex::getIndexContents() const
    {
    return f_hMapInverse;
    }

Object::Holder SimpleMapIndex::get(Object::View vKey) const
    {
    return f_hMapForward->get(vKey);
    }

Object::Holder SimpleMapIndex::get(Object::View vKey)
    {
    return f_hMapForward->get(vKey);
    }

void SimpleMapIndex::insert(Map::Entry::View vEntry)
    {
    try
        {
        insertInternal(vEntry);
        }
    catch (RuntimeException::View vEx)
        {
        // COH-6447: exclude corrupted entries from index and keep track of them
        updateExcludedKeys(vEntry, true);

        COH_THROW(vEx);
        }
    }

void SimpleMapIndex::update(Map::Entry::View vEntry)
    {
    try
        {
        if (!m_fImmutableValues)
            {
            updateInternal(vEntry);
            updateExcludedKeys(vEntry, false);
            }
        }
    catch (RuntimeException::View vEx)
        {
        // COH-6447: exclude corrupted entries from index and keep track of them
        updateExcludedKeys(vEntry, true);

        COH_THROW(vEx);
        }
    }

void SimpleMapIndex::remove(Map::Entry::View vEntry)
    {
    try
        {
        removeInternal(vEntry);
        }
    catch (RuntimeException::View)
        {
        // make sure the entry is no longer tracked as excluded
        updateExcludedKeys(vEntry, false);
        }
    }

Comparator::View SimpleMapIndex::getComparator() const
    {
    return f_vComparator;
    }

void SimpleMapIndex::updateExcludedKeys(Map::Entry::View vEntry, bool fExclude)
    {
    Set::Handle hSetExcluded = f_hSetKeyExcluded;
    if (fExclude || !hSetExcluded->isEmpty())
        {
        Object::View vKey = vEntry->getKey();
        if (fExclude)
            {
            hSetExcluded->add(vKey);
            }
        else
            {
            hSetExcluded->remove(vKey);
            }
        }
    }

bool SimpleMapIndex::isKeyExcluded(Object::View vKey) const
    {
    return f_hSetKeyExcluded->contains(vKey);
    }


// ----- Object interface ---------------------------------------------------

bool SimpleMapIndex::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }
    if (!(instanceof<SimpleMapIndex::View>(v)))
        {
        return false;
        }
    SimpleMapIndex::View vThat = cast<SimpleMapIndex::View>(v);
    return Object::equals(getComparator(),     vThat->getComparator()) &&
           Object::equals(getValueExtractor(), vThat->getValueExtractor()) &&
                          isOrdered()       == vThat->isOrdered();
    }

size32_t SimpleMapIndex::hashCode() const
    {
    return Object::hashCode(f_vComparator) + getValueExtractor()->hashCode() +
            (m_fOrdered ? 1 : 0);
    }

TypedHandle<const String> SimpleMapIndex::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) <<
        ": Extractor=" << getValueExtractor() <<
        ", Ordered="   << isOrdered() <<
        ", Content="   << getIndexContents()->keySet());
    }


// ----- helpers ------------------------------------------------------------

void SimpleMapIndex::init(bool fForwardIndex)
    {
    initialize(f_hMapForward, fForwardIndex ? instantiateForwardIndex() : NULL);
    initialize(f_hMapInverse, instantiateInverseIndex(m_fOrdered, f_vComparator));
    initialize(f_hSetKeyExcluded, SafeHashSet::create());
    }

Map::Entry::View SimpleMapIndex::getForwardEntry(Object::View vKey) const
    {
    return cast<Map::Entry::View>(cast<SafeHashMap::View>(f_hMapForward)->getEntry(vKey));
    }

Map::Entry::Handle SimpleMapIndex::getForwardEntry(Object::View vKey)
    {
    return cast<Map::Entry::Handle>(cast<SafeHashMap::Handle>(f_hMapForward)->getEntry(vKey));
    }

void SimpleMapIndex::removeForwardEntry(Object::View vKey)
    {
    if (NULL != f_hMapForward)
        {
        f_hMapForward->remove(vKey);
        }
    }

Object::Holder SimpleMapIndex::extractNewValue(Map::Entry::View vEntry)
    {
    try
        {
        return InvocableMapHelper::extractFromEntry(f_vValueExtractor, vEntry);
        }
    catch (RuntimeException::View vEx)
        {
        COH_LOG("An Exception occurred during index update for key "
            << vEntry->getKey()
            << ". The entry will be excluded from the index.",
            Logger::level_warning);
        COH_LOG(vEx << ":\n" << vEx->getStackTrace(), Logger::level_warning);

        return MapIndex::getNoValue();
        }
    }

Object::Holder SimpleMapIndex::extractOldValue(MapTrigger::Entry::View vEntry)
    {
    try
        {
        return InvocableMapHelper::extractOriginalFromEntry(f_vValueExtractor, vEntry);
        }
    catch (RuntimeException::View)
        {
        return MapIndex::getNoValue();
        }
    }

Collection::View SimpleMapIndex::ensureCollection(Object::View vValue)
        {
        if (vValue == MapIndex::getNoValue())
            {
            return Collections::emptySet();
            }
        else if (instanceof<Collection::View>(vValue))
            {
            return cast<Collection::View>(vValue);
            }
        else if (instanceof<ObjectArray::View>(vValue))
            {
            return Arrays::asList(cast<ObjectArray::View>(vValue));
            }
        else
            {
            return Collections::singleton(vValue);
            }
        }

Map::Handle SimpleMapIndex::instantiateForwardIndex() const
    {
    return SafeHashMap::create();
    }

Map::Handle SimpleMapIndex::instantiateInverseIndex(bool fOrdered,
        Comparator::View vComparator) const
    {
    if (fOrdered)
        {
        if (!(instanceof<SafeComparator::View>(vComparator)))
            {
            vComparator = SafeComparator::create(vComparator);
            }
        return Collections::synchronizedSortedMap
                (cast<SortedMap::Handle>(TreeMap::create(vComparator)));
        }
    return SafeHashMap::create();
    }

Object::Holder SimpleMapIndex::addInverseMapping(Object::Holder ohIxValue,
        Object::View vKey)
    {
    Map::Handle hMapInverse = f_hMapInverse;

    // add a new mapping(s) to the inverse index
    // if the extracted value has been already "known", substitute
    // the one we just extracted with a previously extracted one
    // to avoid having multiple copies of equivalent values
    if ((m_fSplitCollection && instanceof<Collection::Holder>(ohIxValue))
                ||  instanceof<ObjectArray::Holder>(ohIxValue))
        {
        ohIxValue = addInverseCollectionMapping(f_hMapInverse, ohIxValue, vKey);
        }
    else
        {
        ohIxValue = addInverseMapping(f_hMapInverse, ohIxValue, vKey);
        }

    return ohIxValue;
    }

Object::Holder SimpleMapIndex::addInverseMapping(Map::Handle hMapIndex,
        Object::Holder ohIxValue, Object::View vKey)
    {
    Set::Handle hSetKeys = cast<Set::Handle>(hMapIndex->get(ohIxValue));

    if (hSetKeys == NULL)
        {
        hSetKeys = instantiateSet();
        hMapIndex->put(ohIxValue, hSetKeys);
        }

    hSetKeys->add(vKey);
    return ohIxValue;
    }

Object::Holder SimpleMapIndex::addInverseCollectionMapping(Map::Handle hMapIndex,
        Object::Holder ohIxValue, Object::View vKey)
    {
    // in addition to adding the reverse index we want to find any entry
    // in the forward index that is equal to the currently extracted
    // collection (oIxValue); naturally that match, if exists, could only
    // be a forward index value for some of the keys that "match" all the
    // values in the passed in collection

    Set::Handle hSetCandidateKeys = NULL;
    bool        fCandidate        = true;  // search for an existing value
    bool        fScanForward      = false; // search by scanning the forward index

    Iterator::Handle hIterator = instanceof<ObjectArray::Holder>(ohIxValue) ?
            cast<Iterator::Handle>(SimpleIterator::create(cast<ObjectArray::Holder>(ohIxValue))) :
            cast<Collection::Holder>(ohIxValue)->iterator();

    while (hIterator->hasNext())
        {
        Object::View vValue   = hIterator->next();
        Set::Handle  hSetKeys = cast<Set::Handle>(hMapIndex->get(vValue));

        if (NULL == hSetKeys)
            {
            fCandidate = false;
            hSetKeys   = instantiateSet();
            hMapIndex->put(vValue, hSetKeys);
            }
        else if (fCandidate)
            {
            if (NULL == hSetCandidateKeys)
                {
                size32_t cThreshold = 50;
                if (NULL != f_hMapForward && hSetKeys->size() > cThreshold)
                    {
                    // for values that are associated with a large number of
                    // keys, the repeated retainAll() operation is more
                    // expensive than simply scanning the forward index
                    fScanForward = true;
                    hSetKeys->add(vKey);
                    continue;
                    }

                hSetCandidateKeys = LiteSet::create(hSetKeys);
                }
            else
                {
                hSetCandidateKeys->retainAll(hSetKeys);
                }
            }

        hSetKeys->add(vKey);
        }

    if (fCandidate)
        {
        // find an existing reference in the forward index that is "equal"
        // to the given (collection or array) value

        // If we are scanning the forward index, the iterator holds the values
        // to check, otherwise it holds the keys whose values are checked
        Iterator::Handle hIter = fScanForward
            ? cast<Iterator::Handle>(f_hMapForward->values()->iterator())
            : NULL == hSetCandidateKeys
                ? NullImplementation::getIterator()
                : cast<Iterator::Handle>(hSetCandidateKeys->iterator());

        while (hIter->hasNext())
            {
            Object::View vCandidateValue = fScanForward ? hIter->next() : get(hIter->next());
            if (Object::equals(vCandidateValue, ohIxValue))
                {
                // found an "equal" reference
                return vCandidateValue;
                }
            }
        }

    // no match is found; the passed in value should be used for the
    // forward map
    return ohIxValue;
    }

void SimpleMapIndex::removeInverseMapping(Object::Holder ohIxValue,
      Object::View vKey, Collection::View vColIgnore)
    {
    Map::Handle hMapInverse = f_hMapInverse;
    if (ohIxValue == MapIndex::getNoValue() && !isKeyExcluded(vKey))
        {
        // the old value could not be obtained; must resort to a full-scan
        for (Iterator::Handle hIter = hMapInverse->keySet()->iterator(); hIter->hasNext(); )
            {
            removeInverseMapping(hIter->next(), vKey, vColIgnore);
            }
        }
    else if (instanceof<Collection::Holder>(ohIxValue) && m_fSplitCollection)
        {
        for (Iterator::Handle hIter = cast<Collection::Holder>(ohIxValue)->iterator();
             hIter->hasNext(); )
            {
            Object::Holder ohElemValue = hIter->next();
            if (NULL == vColIgnore || !vColIgnore->contains(ohElemValue))
                {
                removeInverseMapping(hMapInverse, ohElemValue, vKey);
                }
            }
        }
    else if (instanceof<ObjectArray::Holder>(ohIxValue))
        {
        ObjectArray::Holder ohaIxValueOld = cast<ObjectArray::Holder>(ohIxValue);

        for (int i = 0, c = ohaIxValueOld->length; i < c; ++i)
            {
            Object::Holder ohElemValue = ohaIxValueOld[i];
            if (NULL == vColIgnore || !vColIgnore->contains(ohElemValue))
                {
                removeInverseMapping(hMapInverse, ohElemValue, vKey);
                }
            }
        }
    else
        {
        removeInverseMapping(hMapInverse, ohIxValue, vKey);
        }
    }

void SimpleMapIndex::removeInverseMapping(Object::Holder ohIxValue, Object::View vKey)
    {
    removeInverseMapping(ohIxValue, vKey, NULL);
    }

void SimpleMapIndex::removeInverseMapping(Map::Handle hMapIndex,
        Object::Holder ohIxValue,
        Object::View vKey)
    {
    Set::Handle hSetKeys = cast<Set::Handle>(hMapIndex->get(ohIxValue));
    if (NULL == hSetKeys)
        {
        if (!isPartial() && NULL != ohIxValue)
           {
           logMissingIdx(ohIxValue, vKey);
           }
        }
    else
        {
        hSetKeys->remove(vKey);
        if (hSetKeys->isEmpty())
            {
            hMapIndex->remove(ohIxValue);
            }
        }
    }

Set::Handle SimpleMapIndex::instantiateSet() const
    {
    return HashSet::create();
    }

void SimpleMapIndex::insertInternal(Map::Entry::View vEntry)
    {
    Object::View   vKey      = vEntry->getKey();
    Object::Holder ohIxValue = extractNewValue(vEntry);

    COH_SYNCHRONIZED (this)
        {
        if (ohIxValue == MapIndex::getNoValue())
            {
            // COH-6447: exclude corrupted entries from index and keep track of them
            updateExcludedKeys(vEntry, true);
            }
        else
            {
            // add a new mapping(s) to the inverse index
            ohIxValue = addInverseMapping(ohIxValue, vKey);

            // add a new mapping to the forward index
            Map::Handle hMapForward = f_hMapForward;
            if (NULL != hMapForward)
                {
                hMapForward->put(vKey, ohIxValue);
                }
            }
        }
    }

void SimpleMapIndex::updateInternal(Map::Entry::View vEntry)
    {
    Object::View   vKey         = vEntry->getKey();
    Object::Holder ohIxValueNew = extractNewValue(vEntry);

    COH_SYNCHRONIZED (this)
        {
        Object::Holder     ohIxValueOld;
        Map::Entry::Handle hEntryFwd = getForwardEntry(vKey);
        if (NULL == hEntryFwd)
            {
            if (instanceof<MapTrigger::Entry::View>(vEntry))
                {
                ohIxValueOld = extractOldValue(cast<MapTrigger::Entry::View>(vEntry));
                }
            else
                {
                COH_THROW(IllegalStateException::create(
                        "Cannot extract the old value "));
                }
            }
        else
            {
            // replace the key reference with a pre-existing equivalent
            vKey         = hEntryFwd->getKey();
            ohIxValueOld = hEntryFwd->getValue();
            }

        // replace the mapping(s) in the inverse index (if necessary)
        if (!Object::equals(ohIxValueOld, ohIxValueNew))
            {
            // remove the old mapping(s) from the inverse index
            Object::Holder no_value = MapIndex::getNoValue();
            if (ohIxValueOld == no_value)
                {
                // extraction of old value failed; must do a full-scan, ensuring that
                // any mappings to collection element values in the "new" value will remain
                // (COH-7206)
                removeInverseMapping(no_value, vKey, ensureCollection(ohIxValueNew));
                }
            else if ((m_fSplitCollection && 
                    instanceof<Collection::Holder>(ohIxValueOld)) ||
                    instanceof<ObjectArray::Holder>(ohIxValueOld))
                {
                // Note: it's important to only remove the elements that are no longer
                //       present in the new value (see COH-7206)
                removeInverseMapping(collectRemoved(ohIxValueOld, ohIxValueNew), vKey);
                }
            else
                {
                removeInverseMapping(ohIxValueOld, vKey);
                }

            if (ohIxValueNew == no_value)
                {
                // COH-6447: exclude corrupted entries from index and keep track of them
                if (NULL != hEntryFwd)
                    {
                    removeForwardEntry(vKey);
                    }

                updateExcludedKeys(vEntry, true);
                }
            else
                {
                // add a new mapping(s) to the inverse index
                ohIxValueNew = addInverseMapping(ohIxValueNew, vKey);

                // replace the mapping in the forward index
                if (NULL == hEntryFwd)
                    {
                    Map::Handle hMap = f_hMapForward;
                    if (NULL != hMap)
                        {
                        hMap->put(vKey, ohIxValueNew);
                        }
                    }
                else
                    {
                    hEntryFwd->setValue(ohIxValueNew);
                    }

                updateExcludedKeys(vEntry, false);
                }
            }
        }
    }

void SimpleMapIndex::removeInternal(Map::Entry::View vEntry)
    {
    Object::View vKey = vEntry->getKey();

    COH_SYNCHRONIZED (this)
        {
        Object::Holder ohIxValueOld;
        Map::Handle    hMapForward = f_hMapForward;
        if (NULL == hMapForward)
            {
            if (instanceof<MapTrigger::Entry::View>(vEntry))
                {
                ohIxValueOld = extractOldValue(cast<MapTrigger::Entry::View>(vEntry));
                }
            else
                {
                COH_THROW(IllegalStateException::create(
                        "Cannot extract the old value"));
                }
            }
        else
            {
            // remove the mapping from the forward index
            ohIxValueOld = hMapForward->remove(vKey);
            }

        // remove the mapping(s) from the inverse index
        removeInverseMapping(ohIxValueOld, vKey);
        // make sure the entry is no longer tracked as excluded
        updateExcludedKeys(vEntry, false);
        }
    }

void SimpleMapIndex::logMissingIdx(Object::View vIxValue, Object::View vKey)
    {
    // COH-5939: limit logging frequency to 10 messages for every 5 minutes interval
    int64_t ldtNow       = System::safeTimeMillis();
    int64_t ldtLogResume = m_ldtLogMissingIdx + 300000L;

    if (ldtNow > ldtLogResume)
        {
        m_ldtLogMissingIdx = ldtNow;
        m_cLogMissingIdx   = 0;
        }

    int cLog = ++m_cLogMissingIdx;
    if (cLog < 10)
        {
        COH_LOG("Missing inverse index: value=" << vIxValue << ", key=" << vKey, Logger::level_warning);
        }
    else if (cLog == 10)
        {
        COH_LOG("Suppressing missing inverse index messages for " << 
            (ldtLogResume - ldtNow) / 1000 << " seconds", Logger::level_warning);
        }
    }

Set::Handle SimpleMapIndex::collectRemoved(Object::View vIxValueOld, Object::View vIxValueNew)
    {
    Set::Handle hSetRemove;
    if (instanceof<Collection::View>(vIxValueOld))
        {
        // clone the original collection
        hSetRemove = HashSet::create(cast<Collection::View>(vIxValueOld));
        }
    else // vIxValueOld instanceof ObjectArray
        {
        hSetRemove = HashSet::create(Arrays::asList(cast<ObjectArray::View>(vIxValueOld)));
        }

    hSetRemove->removeAll(ensureCollection(vIxValueNew));
    return hSetRemove;
    }

COH_CLOSE_NAMESPACE2
