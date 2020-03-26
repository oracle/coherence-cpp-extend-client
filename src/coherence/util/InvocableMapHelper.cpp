/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/InvocableMapHelper.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Arrays.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapListenerSupport.hpp"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/SubSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/comparator/EntryComparator.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/AbstractUpdater.hpp"
#include "coherence/util/extractor/EntryExtractor.hpp"
#include "coherence/util/extractor/IndexAwareExtractor.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"
#include "coherence/util/filter/KeyAssociatedFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"
#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::util::comparator::EntryComparator;
using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::AbstractExtractor;
using coherence::util::extractor::AbstractUpdater;
using coherence::util::extractor::IndexAwareExtractor;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::EntryFilter;
using coherence::util::filter::IndexAwareFilter;
using coherence::util::filter::KeyAssociatedFilter;
using coherence::util::filter::LimitFilter;
using coherence::util::logging::Logger;

COH_OPEN_NAMESPACE_ANON(InvocableMapHelper)

// ------- local class: IndexAdapter ------------------------------------

/**
* MapListener implementation that routes the map events into the
* corresponding MapIndex calls.
*/
class COH_EXPORT IndexAdapter
    : public class_spec<IndexAdapter,
            extends<Object>,
            implements<MapListenerSupport::SynchronousListener> >
    {
    friend class factory<IndexAdapter>;


    // --------- constructors/destructor ----------------------------

    protected:
        /**
        * Construct an IndexAdapter.
        *
        * @param hIndex  the MapIndex being wrapped
        */
        IndexAdapter(MapIndex::Handle hIndex)
            : f_hIndex(self(), hIndex)
            {
            }


    // ----------------- MapListener interface ----------------------

    public:

        /**
        * {@inheritDoc}
        */
        void entryInserted(MapEvent::View vEvt)
            {
            f_hIndex->insert(SimpleMapEntry::create(vEvt->getKey(),
                    vEvt->getNewValue()));
            }

        /**
        * {@inheritDoc}
        */
        void entryUpdated(MapEvent::View vEvt)
            {
            f_hIndex->update(SimpleMapEntry::create(vEvt->getKey(),
                    vEvt->getNewValue(), vEvt->getOldValue()));

            }

        /**
        * {@inheritDoc}
        */
        void entryDeleted(MapEvent::View vEvt)
            {
            f_hIndex->remove(SimpleMapEntry::create(vEvt->getKey(),
                    (Object::View) NULL, vEvt->getOldValue()));
            }


    // ----------------- Object methods -----------------------------

    public:
        /**
        * Compare this IndexMapListener with another object for equality.
        *
        * @param v  an object reference or NULL
        *
        * @return true iff the passed object reference is a
        *              IndexMapListener object with the same index
        */
        virtual bool equals(Object::View v) const
            {
            return (this == v) ||
                (instanceof<IndexAdapter::View>(v) &&
                Object::equals(this->f_hIndex,
                        cast<IndexAdapter::View>(v)->f_hIndex));
            }

        /**
        * Return a hash code value for the IndexMapListener object.
        */
        virtual size32_t hashCode() const
            {
            return f_hIndex->hashCode();
            }


    // --------- data fields ----------------------------------------

    private:
        /**
        * The wrapped index.
        */
        FinalHandle<MapIndex> f_hIndex;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- InvocableMapHelper interface ---------------------------------------

bool InvocableMapHelper::evaluateEntry(Filter::View vFilter,
            Map::Entry::View vEntry)
    {
    return instanceof<EntryFilter::View>(vFilter)
            ? cast<EntryFilter::View>(vFilter)->evaluateEntry(vEntry)
            : vFilter->evaluate(vEntry->getValue());
    }

bool evaluateEntry(Filter::View vFilter,
        Object::View vKey, Object::View vValue)
    {
    return instanceof<EntryFilter::View>(vFilter)
               ? cast<EntryFilter::View>(vFilter)->evaluateEntry(
                       SimpleMapEntry::create(vKey, vValue))
               : vFilter->evaluate(vValue);
    }

bool InvocableMapHelper::evaluateOriginalEntry(Filter::View vFilter,
        MapTrigger::Entry::View vEntry)
    {
    if (vEntry->isOriginalPresent())
        {
        Object::View vValueOrig = vEntry->getOriginalValue();
        return instanceof<EntryFilter::View>(vFilter)
                ? cast<EntryFilter::View>(vFilter)->evaluateEntry(
                        SimpleMapEntry::create(vEntry->getKey(), vValueOrig))
                : vFilter->evaluate(vValueOrig);
        }

    return false;
    }

Set::View InvocableMapHelper::query(Map::View vMap, Map::View vMapIndexes,
        Filter::View vFilter, bool fEntries, bool fSort,
        Comparator::View vComparator)
    {
    Filter::View vFilterOrig = vFilter;
    if (AlwaysFilter::getInstance()->equals(vFilter))
        {
        vFilter = NULL;
        }

    ObjectArray::Handle haoResult; // may contain keys or entries

    // try to apply an index (if available)
    if (vMapIndexes != NULL && instanceof<IndexAwareFilter::View>(vFilter))
        {
        IndexAwareFilter::View vFilterIx        = cast<IndexAwareFilter::View>(vFilter);
        Set::Handle            hSetFilteredKeys = SubSet::create(vMap->keySet());

        try
            {
            vFilter = vFilterIx->applyIndex(vMapIndexes, hSetFilteredKeys);
            }
        catch (ConcurrentModificationException::View e)
            {
            // applyIndex failed; try again with a snapshot of the key set
            hSetFilteredKeys = SubSet::create(
                    ReadOnlyArrayList::create(vMap->keySet()->toArray()));
            vFilter = vFilterIx->applyIndex(vMapIndexes, hSetFilteredKeys);
            }
        haoResult = hSetFilteredKeys->toArray();
        }
    else
        {
        haoResult = vMap->keySet()->toArray();
        }

    size32_t cResults = 0;
    if (vFilter == NULL && !fEntries)
        {
        cResults = haoResult->length;
        }
    else
        {
        // we still have a filter to evaluate or we need an entry set
        for (size32_t i = 0, c = haoResult->length; i < c; i++)
            {
            Object::View   vKey   = haoResult[i];
            Object::Holder hValue = vMap->get(vKey);

            if (hValue != NULL || vMap->containsKey(vKey))
                {
                Map::Entry::View vEntry = SimpleMapEntry::create(vKey, hValue);
                if (vFilter == NULL || evaluateEntry(vFilter, vEntry))
                    {
                    haoResult[cResults++] = fEntries ? (Object::Holder) vEntry :
                            (Object::Holder) vKey;
                    }
                }
            }
        }

    LimitFilter::View vFilterLimit = instanceof<LimitFilter::View>(vFilterOrig) ?
            cast<LimitFilter::View>(vFilterOrig) : (LimitFilter::View)NULL;

    if (vFilterLimit != NULL || (fEntries && fSort))
        {
        if (cResults < haoResult->length)
            {
            ObjectArray::Handle hao = ObjectArray::create(cResults);
            Array<MemberHolder<Object> >::copy(haoResult, 0, hao, 0, cResults);
            haoResult = hao;
            }

        if (fEntries && fSort)
            {
            if (vComparator == NULL)
                {
                vComparator = SafeComparator::getInstance();
                }

            Arrays::sort(haoResult, EntryComparator::create(vComparator));
            }

        // jh: The commented out code is just an optimization.
        // I believe that the Comparator will always be NULL for local caches
        // (in other words, this is a facility used by the DistributedCache
        // service only).
        if (vFilterLimit != NULL)
            {
            // vFilterLimit->setComparator(NULL);
            haoResult = vFilterLimit->extractPage(haoResult);
            cResults = haoResult->length;
            // vFilterLimit->setComparator(vComparator); // for debug output only
            }
        }

    return ReadOnlyArrayList::create(haoResult, 0, cResults)->getSet();
    }

void InvocableMapHelper::addIndex(ValueExtractor::View vExtractor,
        bool fOrdered, Comparator::View vComparator,
        ObservableMap::Handle hMap, Map::Handle hMapIndex)
    {
    MapIndex::Handle hIndex =
            cast<MapIndex::Handle>(hMapIndex->get(vExtractor));

    if (NULL == hIndex)
        {
        for (int32_t cAttempts = 4; true;)
            {
            if (instanceof<IndexAwareExtractor::View>(vExtractor))
                {
                hIndex = cast<IndexAwareExtractor::View>
                         (vExtractor)->createIndex(fOrdered, vComparator, hMapIndex);
                if (NULL == hIndex)
                    {
                    return;
                    }
                }
            else
                {
                hIndex = SimpleMapIndex::create(vExtractor, fOrdered, vComparator);
                hMapIndex->put(vExtractor, hIndex);
                }

            MapListener::Handle hListener = ensureListener(hIndex);
            hMap->addFilterListener(hListener, NULL, false);

            try
                {
                for (Iterator::Handle hIter = hMap->entrySet()->iterator(); hIter->hasNext();)
                    {
                    Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                    hIndex->insert(vEntry);
                    }
                break;
                }
            catch (ConcurrentModificationException::View cme)
                {
                hMap->removeFilterListener(hListener);
                if (--cAttempts == 0)
                    {
                    removeIndex(vExtractor, hMap, hMapIndex);
                    COH_LOGEXMSG (cme, "Exception occurred during index rebuild: ",
                            Logger::level_warning);
                    COH_THROW(cme);
                    }
                }
            }
        }
    // if the index already existed but had different settings
    else if (!(fOrdered == hIndex->isOrdered() &&
            Object::equals(vComparator, hIndex->getComparator())))
        {
        COH_THROW_STREAM(IllegalArgumentException, "Index for " <<
                vExtractor << " already exists;" <<
                " remove the index and add it with the new settings");
        }
    }

void InvocableMapHelper::removeIndex(ValueExtractor::View vExtractor,
        ObservableMap::Handle hMap, Map::Handle hMapIndex)
    {
    MapIndex::Handle hIndex = instanceof<IndexAwareExtractor::View>(vExtractor) ?
        cast<IndexAwareExtractor::View>(vExtractor)->destroyIndex(hMapIndex) :
        cast<MapIndex::Handle>(hMapIndex->remove(vExtractor));

    if (NULL != hIndex)
        {
        hMap->removeFilterListener(ensureListener(hIndex));
        }
    }

Object::Holder InvocableMapHelper::extractFromEntry(
        ValueExtractor::View vExtractor, Map::Entry::View vEntry)
    {
    return instanceof<AbstractExtractor::View>(vExtractor)
            ? cast<AbstractExtractor::View>(vExtractor)->extractFromEntry(vEntry)
            : vExtractor->extract(vEntry->getValue());
    }

Object::Holder InvocableMapHelper::extractOriginalFromEntry(
        ValueExtractor::View vExtractor, MapTrigger::Entry::View vEntry)
    {
    return instanceof<AbstractExtractor::View>(vExtractor)
            ? cast<AbstractExtractor::View>(vExtractor)->extractOriginalFromEntry(vEntry)
            : vEntry->isOriginalPresent()
                ? vExtractor->extract(vEntry->getOriginalValue()) : NULL;
    }

void InvocableMapHelper::updateEntry(ValueUpdater::View vUpdater,
        Map::Entry::Handle hEntry, Object::Holder ohValue)
    {
    if (instanceof<AbstractUpdater::View>(vUpdater))
        {
        cast<AbstractUpdater::View>(vUpdater)->updateEntry(hEntry, ohValue);
        }
    else
        {
        Object::Holder ohTarget = hEntry->getValue();

        vUpdater->update(cast<Object::Handle>(ohTarget), ohValue);

        if (instanceof<InvocableMap::Entry::Handle>(hEntry))
            {
            cast<InvocableMap::Entry::Handle>(hEntry)->setValue(ohTarget, false);
            }
        else
            {
            cast<Map::Entry::Handle>(hEntry)->setValue(ohTarget);
            }
        }
    }


// ------------- helpers ----------------------------------------------------

MapListener::Handle InvocableMapHelper::ensureListener(MapIndex::Handle hIndex)
    {
    return instanceof<MapListenerSupport::SynchronousListener::Handle>(hIndex)
            ? cast<MapListener::Handle>(hIndex)
            : cast<MapListener::Handle>(IndexAdapter::create(hIndex));
    }

COH_CLOSE_NAMESPACE2
