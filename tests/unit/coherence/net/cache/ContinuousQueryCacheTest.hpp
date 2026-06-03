/*
 * Copyright (c) 2000, 2026, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "mock/CommonMocks.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ObservableHashMap.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"

#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"

using coherence::net::NamedCache;
using coherence::net::cache::ContinuousQueryCache;
using coherence::util::ArrayList;
using coherence::util::Filter;
using coherence::util::HashSet;
using coherence::util::MapListener;
using coherence::util::ObservableHashMap;
using coherence::util::ObservableMap;
using coherence::util::SimpleMapEntry;
using coherence::util::ValueExtractor;
using coherence::util::filter::AlwaysFilter;

namespace
    {
    bool matchAll(ArrayList::View /*vExpected*/, ArrayList::View /*vActual*/)
        {
        return true;
        }

    HashSet::Handle createKeySet(size32_t cEntries)
        {
        HashSet::Handle hSet = HashSet::create();
        for (size32_t i = 0; i < cEntries; ++i)
            {
            hSet->add(Integer32::create((int32_t) i));
            }
        return hSet;
        }

    HashSet::Handle createEntrySet(size32_t cEntries)
        {
        HashSet::Handle hSet = HashSet::create();
        for (size32_t i = 0; i < cEntries; ++i)
            {
            hSet->add(SimpleMapEntry::create(
                    Integer32::create((int32_t) i),
                    Integer32::create((int32_t) i + 1000)));
            }
        return hSet;
        }

    class SizingContinuousQueryCache
        : public class_spec<SizingContinuousQueryCache,
            extends<ContinuousQueryCache> >
        {
        friend class factory<SizingContinuousQueryCache>;

        protected:
            SizingContinuousQueryCache(NamedCache::Handle hCache,
                    Filter::View vFilter, bool fCacheValues)
                : super(hCache, vFilter, fCacheValues,
                        (MapListener::Handle) NULL,
                        (ValueExtractor::View) NULL),
                  m_cObservedInitialBuckets(0),
                  m_fInstantiateCalled(false)
                {
                }

            virtual ObservableMap::Handle instantiateInternalCache() const
                {
                m_cObservedInitialBuckets = getInternalCacheInitialBuckets();
                m_fInstantiateCalled      = true;
                return super::instantiateInternalCache();
                }

        public:
            size32_t getObservedInitialBuckets() const
                {
                return m_cObservedInitialBuckets;
                }

            bool isInstantiateCalled() const
                {
                return m_fInstantiateCalled;
                }

        protected:
            mutable size32_t m_cObservedInitialBuckets;
            mutable bool     m_fInstantiateCalled;
        };

    class OverrideInternalMapContinuousQueryCache
        : public class_spec<OverrideInternalMapContinuousQueryCache,
            extends<ContinuousQueryCache> >
        {
        friend class factory<OverrideInternalMapContinuousQueryCache>;

        protected:
            OverrideInternalMapContinuousQueryCache(NamedCache::Handle hCache,
                    Filter::View vFilter, bool fCacheValues)
                : super(hCache, vFilter, fCacheValues,
                        (MapListener::Handle) NULL,
                        (ValueExtractor::View) NULL),
                  m_fInstantiateCalled(false)
                {
                }

            virtual ObservableMap::Handle instantiateInternalCache() const
                {
                m_fInstantiateCalled = true;
                return ObservableHashMap::create(3, 1.0F, 3.0F);
                }

        public:
            bool isInstantiateCalled() const
                {
                return m_fInstantiateCalled;
                }

        protected:
            mutable bool m_fInstantiateCalled;
        };
    }



class ContinuousQueryCacheTest : public CxxTest::TestSuite
    {
    public:

    void testGetCache()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(hCqc->getCache() == hMockNamedCache);
        }


    void testGetFilter()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        Filter::Handle hFilter = DummyFilter::create();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, false);
        TS_ASSERT(hCqc->getFilter() == hFilter);
        }


    void testIsCacheValues()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        Filter::Handle hFilter = DummyFilter::create();

        //set expectations
        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        //replay
        hMockNamedCache->replay();

        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, false);
        TS_ASSERT(!hCqc->isCacheValues());

        hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, true);

        TS_ASSERT(hCqc->isCacheValues());
        }

        void testIsCacheValuesWithListener()
            {
            MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
            MockMapListener::Handle hMockListener = MockMapListener::create();
            Filter::Handle hFilter = DummyFilter::create();

            //set expectations
            hMockNamedCache->entrySet(hFilter);
            hMockNamedCache->setMatcher(&matchAll);
            hMockNamedCache->setObjectReturn(Collections::emptySet());

            hMockNamedCache->keySet(hFilter);
            hMockNamedCache->setMatcher(&matchAll);
            hMockNamedCache->setObjectReturn(Collections::emptySet());

            hMockNamedCache->entrySet(hFilter);
            hMockNamedCache->setMatcher(&matchAll);
            hMockNamedCache->setObjectReturn(Collections::emptySet());

            hMockNamedCache->keySet(hFilter);
            hMockNamedCache->setMatcher(&matchAll);
            hMockNamedCache->setObjectReturn(Collections::emptySet());

            //replay
            hMockNamedCache->replay();

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, false, hMockListener);
            TS_ASSERT(!hCqc->isCacheValues());

            hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, true, hMockListener);
            TS_ASSERT(hCqc->isCacheValues());
            }
    


    void testSetCacheValues()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        Filter::Handle hFilter = DummyFilter::create();
        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        //set expectations
        hMockNamedCache->getAll(ArrayList::create());
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(HashMap::create());

        //set expectations
        hMockNamedCache->getAll(ArrayList::create());
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(HashMap::create());

        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, false);
        TS_ASSERT(!hCqc->isCacheValues());

        hCqc->setCacheValues(true);
        TS_ASSERT(hCqc->isCacheValues());

        hCqc->setCacheValues(false);
        TS_ASSERT(!hCqc->isCacheValues());
        }

    void testIsReadOnly()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(!hCqc->isReadOnly());
        }

    void testSetReadOnly()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        hCqc->setReadOnly(true);
        TS_ASSERT(hCqc->isReadOnly());
        try
            {
            hCqc->setReadOnly(false);
            TS_FAIL("Expected exception!");
            }
        catch( IllegalStateException::View )
            {
            //expected
            }
        }

    void testGetState()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(hCqc->getState()==ContinuousQueryCache::state_synchronized);
        }

    void testGetReconnectInterval()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(hCqc->getReconnectInterval()==0);
        }

    void testInitialCacheSizeFromEntrySet()
        {
        const size32_t cEntries = 47;
        const size32_t cInitialBuckets = 61;
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        Filter::Handle hFilter = DummyFilter::create();

        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(createEntrySet(cEntries));

        hMockNamedCache->replay();

        SizingContinuousQueryCache::Handle hCqc =
                SizingContinuousQueryCache::create(hMockNamedCache, hFilter, true);

        TS_ASSERT(hCqc->isInstantiateCalled());
        TS_ASSERT_EQUALS(hCqc->getObservedInitialBuckets(), cInitialBuckets);
        TS_ASSERT_EQUALS(hCqc->size(), cEntries);
        }

    void testInitialCacheSizeFromKeySet()
        {
        const size32_t cEntries = 79;
        const size32_t cInitialBuckets = 103;
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        Filter::Handle hFilter = DummyFilter::create();

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(createKeySet(cEntries));

        hMockNamedCache->replay();

        SizingContinuousQueryCache::Handle hCqc =
                SizingContinuousQueryCache::create(hMockNamedCache, hFilter, false);

        TS_ASSERT(hCqc->isInstantiateCalled());
        TS_ASSERT_EQUALS(hCqc->getObservedInitialBuckets(), cInitialBuckets);
        TS_ASSERT_EQUALS(hCqc->size(), cEntries);
        }

    void testInstantiateInternalCacheOverrideIsPreserved()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        Filter::Handle hFilter = DummyFilter::create();

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(createKeySet(5));

        hMockNamedCache->replay();

        OverrideInternalMapContinuousQueryCache::Handle hCqc =
                OverrideInternalMapContinuousQueryCache::create(
                        hMockNamedCache, hFilter, false);

        TS_ASSERT(hCqc->isInstantiateCalled());
        TS_ASSERT_EQUALS(hCqc->size(), (size32_t) 5);
        }

    void testSetReconnectInterval()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        hCqc->setReconnectInterval(999);
        TS_ASSERT(hCqc->getReconnectInterval()==999);
        }


    void testGetPutWithCacheValues()
        {
/*
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        Filter::Handle hFilter = AlwaysFilter::create();

        //set expectations
        hMockNamedCache->entrySet(DummyFilter::create());
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        //replay
        hMockNamedCache->replay();

        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, true);

        String::View vsKey = String::create("key");
        String::Handle hsValue = String::create("value");

        TS_ASSERT(hCqc->get(vsKey)== NULL);

        hCqc->put(vsKey, hsValue);

        TS_ASSERT(hCqc->get(vsKey)->equals(hsValue));
        */
        }

    };


