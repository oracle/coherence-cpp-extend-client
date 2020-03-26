/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "common/TestMapListener.hpp"
#include "common/TestUtils.hpp"
#include "common/TestClasses.hpp"
#include "mock/CommonMocks.hpp"

#include "coherence/lang.ns"

#include "coherence/internal/net/NamedCacheDeactivationListener.hpp"

#include "coherence/net/cache/ContinuousQueryCache.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/util/aggregator/Integer64Max.hpp"
#include "coherence/util/aggregator/Integer64Min.hpp"

#include "coherence/util/Collection.hpp"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/AndFilter.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/filter/InFilter.hpp"
#include "coherence/util/filter/LessFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"
#include "coherence/util/filter/MapEventFilter.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/Supplier.hpp"

#include "coherence/util/processor/UpdaterProcessor.hpp"

using namespace std;
using namespace mock;

using coherence::internal::net::NamedCacheDeactivationListener;
using coherence::net::cache::ContinuousQueryCache;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::aggregator::Integer64Max;
using coherence::util::aggregator::Integer64Min;
using coherence::util::Collection;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::KeyExtractor;
using coherence::util::extractor::ReflectionExtractor;
using coherence::util::extractor::ReflectionUpdater;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::AndFilter;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterFilter;
using coherence::util::filter::InFilter;
using coherence::util::filter::LessFilter;
using coherence::util::filter::LimitFilter;
using coherence::util::filter::MapEventFilter;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Map;
using coherence::util::Muterator;
using coherence::util::Set;
using coherence::util::Supplier;
using coherence::util::processor::UpdaterProcessor;
using common::test::ExampleAddress;
using common::test::FilterFactory;
using common::test::TestContact;
using common::test::TestMapListener;

// used to match event args in mock calls
bool matchListenerEvent(ArrayList::View vExpected, ArrayList::View vActual)
        {
        MapEvent::View vExpectedEvent = cast<MapEvent::View>(vExpected->get(0));
        MapEvent::View vActualEvent   = cast<MapEvent::View>(vActual->get(0));

        return Object::equals(vExpectedEvent->getKey(), vActualEvent->getKey())           &&
               vExpectedEvent->getId() == vActualEvent->getId()                           &&
               Object::equals(vExpectedEvent->getOldValue(), vActualEvent->getOldValue()) &&
               Object::equals(vExpectedEvent->getNewValue(), vActualEvent->getNewValue()) &&
               vExpectedEvent->getMap() == vActualEvent->getMap();
        }

//TODO: release in finally

class ContinuousQueryCacheTest : public CxxTest::TestSuite
    {
    // sample cache listener for CQC instance
    class SampleMapListener : public class_spec<SampleMapListener,
            extends<Object>, implements<MapListener> >
        {
        friend class factory<SampleMapListener>;

        protected:
            SampleMapListener()
                {
                }

        public:
            void entryInserted(MapEvent::View /* vEvt */)
                {
                }

        void entryUpdated(MapEvent::View /* vEvt */)
                {
                }

            void entryDeleted(MapEvent::View /* vEvt */)
                {
                }
        };

    // ----- local class: TestCQCListener -----------------------------------

    /**
    * MapListener that continuously receives events from the cache.
    */
    class TestCQCListener : public class_spec<TestCQCListener,
            extends<Object>, implements<MapListener> >
        {
        friend class factory<TestCQCListener>;

        // ----- constructors -----------------------------------------------

        public:
            TestCQCListener(int cCount) : 
                m_cActualInserts(0), m_cActualUpdates(0), m_cActualDeletes(0), m_cCount(cCount) 
            {
            }

        // ----- MapListener methods ----------------------------------------

        public:
            void entryUpdated(MapEvent::View /* vEvent */)
                {
                m_cActualUpdates++;
                }

            void entryInserted(MapEvent::View /* vEvent */)
                {
                m_cActualInserts++;
                }

            void entryDeleted(MapEvent::View /* vEvent */)
                {
                m_cActualDeletes++;
                }

        // ----- helper methods ---------------------------------------------

        public:
        /**
        * Signal listener has received all the events.
        *
        * @return  true if has received all events, false if not.
        */
        bool isFinished()
            {
            return getActualTotal() >= m_cCount;
            }

        /**
        * Number of insert events listener actually received.
        *
        * @return  number of event received
        */
        int getActualInserts()
            {
            return m_cActualInserts;
            }

        /**
        * Number of update events listener actually received.
        *
        * @return  number of event received
        */
        int getActualUpdates()
            {
            return m_cActualUpdates;
            }

        /**
        * Number of delete events listener actually received.
        *
        * @return  number of event received
        */
        int getActualDeletes()
            {
            return m_cActualDeletes;
            }

        /**
        * Total number of events listener actually received.
        *
        * @return  number of event received
        */
        int getActualTotal()
            {
            return m_cActualInserts + m_cActualUpdates + m_cActualDeletes;
            }

        /**
        * Reset the number of events received.
        */
        void resetActualTotal()
            {
            m_cActualUpdates = 0;
            m_cActualInserts = 0;
            m_cActualDeletes = 0;
            }

        // ----- data members -----------------------------------------------

        protected:
            /**
            * Number of insert events actually received
            */
            int m_cActualInserts;

            /**
            * Number of update events actually received
            */
            int m_cActualUpdates;

            /**
            * Number of delete events actually received
            */
            int m_cActualDeletes;

            /**
            * Number of events listener is expected to receive
            */
            int m_cCount;
        };

    // ----- local class: CacheNameSupplier ---------------------------------

    class CacheNameSupplier : public class_spec<CacheNameSupplier,
            extends<Object>, implements<Supplier> >
        {
        friend class factory<CacheNameSupplier>;

        // ----- constructors -----------------------------------------------

        public:
            CacheNameSupplier(String::Handle vsCacheName) : 
                f_vsCacheName(self(), vsCacheName)
            {
            }

        // ----- Supplier methods -------------------------------------------

        Object::Holder get() const
            {
            return f_vsCacheName;
            }

        Object::Holder get()
            {
            return f_vsCacheName;
            }

        // ----- data members -----------------------------------------------

        protected:
            FinalHandle<String> f_vsCacheName;
        };

    // ----- local class: CacheSupplier -------------------------------------

    class CacheSupplier : public class_spec<CacheSupplier,
            extends<Object>, implements<Supplier> >
        {
        friend class factory<CacheSupplier>;

        // ----- constructors -----------------------------------------------

        protected:
            CacheSupplier()
            {
            }

        // ----- Supplier methods -------------------------------------------

        Object::Holder get() const
            {
            NamedCache::Handle hTestCache = ensureCleanCache("dist-test-cqc");
                for (int i = 0; i < 100; i++)
                    {
                    hTestCache->put(COH_TO_STRING("TestKey" << i), COH_TO_STRING("" << i));
                    }
                return hTestCache;
            }

        Object::Holder get()
            {
            NamedCache::Handle hTestCache = ensureCleanCache("dist-test-cqc");
                for (int i = 0; i < 100; i++)
                    {
                    hTestCache->put(COH_TO_STRING("TestKey" << i), COH_TO_STRING("" << i));
                    }
                return hTestCache;
            }
        };

    // ----- local class: TestContinuousQueryCache --------------------------

#ifdef COH_CC_MSVC
    #pragma warning(disable:4250)
#endif
    class TestContinuousQueryCache : public class_spec<TestContinuousQueryCache,
            extends<ContinuousQueryCache> >
        {
        friend class factory<TestContinuousQueryCache>;

        // ----- constructors -----------------------------------------------

        protected:
            TestContinuousQueryCache(Supplier::View vCacheSupplier,
                                        Filter::View vFilter,
                                        bool fCacheValues) 
                    : super((Supplier::View) vCacheSupplier, vFilter, fCacheValues, (MapListener::Handle) NULL, (ValueExtractor::View) NULL)
                {
                }

        public:
            void changeState(int32_t nState) const
                {
                if (m_fLockState)
                    {
                    return;
                    }
                super::changeState(nState);
                }

            void lockState()
                {
                m_fLockState = true;
                }

            void unlockState()
                {
                m_fLockState = false;
                }

        // ----- data members -----------------------------------------------

        protected:
            bool m_fLockState;
        };
#ifdef COH_CC_MSVC
    #pragma warning(default:4250)
#endif

    // ----- test methods ---------------------------------------------------

    public:
        void testCachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), true);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);
            // update vKey3
            hCache->put(vKey3, vVal2);
            // delete vKey2

            TS_ASSERT(vVal2->equals(hCache->remove(vKey2)));

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->get(vKey0)->equals(vVal0) &&
                          hCqc->get(vKey1)->equals(vVal1) &&
                          hCqc->get(vKey2) == NULL        &&
                          hCqc->get(vKey3)->equals(vVal2);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            // clear the cache
            hCache->clear();

            fResult = false;
            nInitialTime = System::currentTimeMillis();
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 0;
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            // putAll
            Map::Handle hMap = HashMap::create();
            hMap->put(vKey1, vVal1);
            hMap->put(vKey2, vVal2);
            hMap->put(vKey3, vVal3);

            hCqc->putAll(hMap);

            fResult = false;
            nInitialTime = System::currentTimeMillis();
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 3 && hCache->size() == 3;
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);
            TS_ASSERT(hCqc->containsKey(vKey1) && hCqc->containsKey(vKey2)
                  && hCqc->containsKey(vKey3));

            hCqc->release();
            }

        void testNotCachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);
            // update vKey3
            hCache->put(vKey3, vVal2);
            // delete vKey2
            hCache->remove(vKey2);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->get(vKey0)->equals(vVal0) &&
                          hCqc->get(vKey1)->equals(vVal1) &&
                          hCqc->get(vKey2) == NULL        &&
                          hCqc->get(vKey3)->equals(vVal2);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            // clear the cache
            hCache->clear();

            fResult = false;
            nInitialTime = System::currentTimeMillis();
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 0;
                Thread::sleep(5);
                }
            hCqc->release();
            TS_ASSERT(fResult);
            }

        void testCqcListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // the mock listener that receives the event notifications
            MockMapListener::Handle hMockListener = MockMapListener::create();
            // the mock used in verifying the MapEvent which is passed to the listener
            MockMapEvent::Handle hMockMapEvent = MockMapEvent::create();

            // create CQC
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), false, hMockListener);

            Object::View vKey   = String::create("key1");
            Object::View vVal   = String::create("val1");
            Object::View vVal2  = String::create("val2");

            hMockListener->setStrict(true);
            hMockListener->entryInserted(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryUpdated(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryDeleted(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->replay();

            // setup mock for pattern matcher.
            hMockMapEvent->setStrict(true);
            // insert
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_inserted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(vVal);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            // update
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_updated);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vVal);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(vVal2);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            //delete
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_deleted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vVal2);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            hMockMapEvent->replay();

            // insert
            hCache->put(vKey, vVal);
            // update
            hCache->put(vKey, vVal2);
            // delete
            hCache->remove(vKey);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hMockListener->verifyAndReturnResult();
              Thread::sleep(5);
              }
            hCqc->release();
            TS_ASSERT(fResult);
            }

        void testCqcListenerInsertBeforeCqcCreated()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            Object::View vKey = String::create("key");
            Object::View vVal = String::create("val");
            // put into map before creating CQC
            hCache->put(vKey, vVal);

            MockMapListener::Handle hMockListener = MockMapListener::create();
            hMockListener->setStrict(true);
            hMockListener->entryInserted(NULL);
            hMockListener->lastExpectation()->ignoreArguments();
            hMockListener->replay();

            // create CQC
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), false, hMockListener);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hMockListener->verifyAndReturnResult();
              Thread::sleep(5);
              }
            hCqc->release();
            TS_ASSERT(fResult);
            }

        void testKeySet_CachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), true);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);

            // wait until CQC has been fully updated
            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hCqc->size() == 4;
              Thread::sleep(5);
              }
            Set::Handle hsetKeys = hCqc->keySet();
            TS_ASSERT(!hsetKeys->isEmpty() && hsetKeys->size() == 4);
            TS_ASSERT(hsetKeys->contains(vKey0));
            TS_ASSERT(hsetKeys->contains(vKey1));
            TS_ASSERT(hsetKeys->contains(vKey2));
            TS_ASSERT(hsetKeys->contains(vKey3));

            ObjectArray::Handle ha = hsetKeys->toArray();
            TS_ASSERT(ha->length == 4);
            for (size32_t i = 0; i < 4; ++i)
                {
                TS_ASSERT(hCqc->get(ha[i]) != NULL);
                }
            // remove and assert
            TS_ASSERT(hsetKeys->remove(vKey0));
            // try removing non-existant key
            TS_ASSERT(!hsetKeys->remove(String::create("bogus")));
            ha = hsetKeys->toArray();
            nInitialTime = System::currentTimeMillis();
            fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hCache->size() == 3 && hCqc->size() == 3 &&
                        hsetKeys->toArray()->length == 3;
              Thread::sleep(5);
              }
            TS_ASSERT(fResult);

            hsetKeys->clear();

            nInitialTime = System::currentTimeMillis();
            fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hCache->size() == 0 && hCqc->size() == 0 &&
                        hsetKeys->toArray()->length == 0;
              Thread::sleep(5);
              }
            hCqc->release();
            TS_ASSERT(fResult);
            }

        void testKeySet_NotCachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);

            // wait until CQC has been fully updated
            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hCqc->size() == 4;
              Thread::sleep(5);
              }
            Set::Handle hsetKeys = hCqc->keySet();
            TS_ASSERT(!hsetKeys->isEmpty() && hsetKeys->size() == 4);
            TS_ASSERT(hsetKeys->contains(vKey0));
            TS_ASSERT(hsetKeys->contains(vKey1));
            TS_ASSERT(hsetKeys->contains(vKey2));
            TS_ASSERT(hsetKeys->contains(vKey3));

            ObjectArray::Handle ha = hsetKeys->toArray();
            TS_ASSERT(ha->length == 4);
            for (size32_t i = 0; i < 4; ++i)
                {
                TS_ASSERT(hCqc->get(ha[i]) != NULL);
                }
            // remove and assert
            TS_ASSERT(hsetKeys->remove(vKey0));
            // try removing non-existant key
            TS_ASSERT(!hsetKeys->remove(String::create("bogus")));
            ha = hsetKeys->toArray();
            nInitialTime = System::currentTimeMillis();
            fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hCache->size() == 3 && hCqc->size() == 3 &&
                        hsetKeys->toArray()->length == 3;
              Thread::sleep(5);
              }
            TS_ASSERT(fResult);

            hsetKeys->clear();

            nInitialTime = System::currentTimeMillis();
            fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
              {
              fResult = hCache->size() == 0 && hCqc->size() == 0 &&
                        hsetKeys->toArray()->length == 0;
              Thread::sleep(5);
              }
            hCqc->release();
            TS_ASSERT(fResult);
            }

        void testKeySet_Iterator_CachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), true);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 4 && hCache->size() == 4;
                Thread::sleep(5);
                }

            Set::Handle hsetKeys = hCqc->keySet();
            Muterator::Handle hMuter = hsetKeys->iterator();
            Object::View v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();

            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 3 && hCache->size() == 3 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 2 && hCache->size() == 2 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 1 && hCache->size() == 1 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 0 && hCache->size() == 0 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);
            TS_ASSERT(!hMuter->hasNext());
            hCqc->release();
            }

        void testKeySet_Iterator_NotCachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 4 && hCache->size() == 4;
                Thread::sleep(5);
                }

            Set::Handle hsetKeys = hCqc->keySet();
            Muterator::Handle hMuter = hsetKeys->iterator();
            Object::View v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();

            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 3 && hCache->size() == 3 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 2 && hCache->size() == 2 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 1 && hCache->size() == 1 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = hMuter->next();
            TS_ASSERT(hCqc->get(v) != NULL);
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 0 && hCache->size() == 0 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);
            TS_ASSERT(!hMuter->hasNext());
            hCqc->release();
            }

        void testEntrySet_CachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), true);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 4 && hCache->size() == 4;
                Thread::sleep(5);
                }

            Set::Handle hsetEntries = hCqc->entrySet();
            TS_ASSERT(!hsetEntries->isEmpty() && hsetEntries->size() == 4);

            // pass in bad type should return false
            TS_ASSERT(!hsetEntries->contains(String::create("bogus")));
            Map::Entry::View vEntry = cast<Map::Entry::View>(hsetEntries->toArray()[0]);
            TS_ASSERT(hsetEntries->contains(vEntry));

            TS_ASSERT(hsetEntries->remove(vEntry));

            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 3 && hCache->size() == 3 &&
                        hsetEntries->size() == 3;
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);
            TS_ASSERT(!hsetEntries->contains(vEntry));

            ObjectArray::Handle ha = hsetEntries->toArray();
            TS_ASSERT(ha->length == 3);
            vEntry = cast<Map::Entry::View>(ha[0]);
            TS_ASSERT(hCqc->get(vEntry->getKey()) == vEntry->getValue());
            vEntry = cast<Map::Entry::View>(ha[1]);
            TS_ASSERT(hCqc->get(vEntry->getKey()) == vEntry->getValue());
            vEntry = cast<Map::Entry::View>(ha[2]);
            TS_ASSERT(hCqc->get(vEntry->getKey()) == vEntry->getValue());
            hsetEntries->clear();

            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 0 && hCache->size() == 0 &&
                        hsetEntries->size() == 0 && hsetEntries->isEmpty();
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);
            hCqc->release();
            }

        void testEntrySet_Iterator_CachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), true);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 4 && hCache->size() == 4;
                Thread::sleep(5);
                }

            Set::Handle hsetEntries = hCqc->entrySet();
            Muterator::Handle hMuter = hsetEntries->iterator();
            Object::View v = cast<Map::Entry::View>(hMuter->next())->getValue();
            hMuter->remove();

            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 3 && hCache->size() == 3 &&
                        !hCqc->containsValue(v) && !hCache->containsValue(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = cast<Map::Entry::View>(hMuter->next())->getKey();
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 2 && hCache->size() == 2 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = cast<Map::Entry::View>(hMuter->next())->getValue();
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 1 && hCache->size() == 1 &&
                        !hCqc->containsValue(v) && !hCache->containsValue(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = cast<Map::Entry::View>(hMuter->next())->getValue();
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 0 && hCache->size() == 0 &&
                        !hCqc->containsValue(v) && !hCache->containsValue(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);
            TS_ASSERT(!hMuter->hasNext());
            hCqc->release();
            }

        void testEntrySet_Iterator_NotCachingValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            // do put prior to creating CQC
            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            hCache->put(vKey0, vVal0);

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            hCache->put(vKey1, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->put(vKey3, vVal3);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 4 && hCache->size() == 4;
                Thread::sleep(5);
                }

            Set::Handle hsetEntries = hCqc->entrySet();
            Muterator::Handle hMuter = hsetEntries->iterator();
            Map::Entry::View vEntry = cast<Map::Entry::View>(hMuter->next());
            Object::View v = vEntry->getValue();
            TS_ASSERT(v != NULL);
            hCqc->put(vEntry->getKey(), String::create("new-value"));
            // Ensure same value is returned
            TS_ASSERT(v == vEntry->getValue());

            hMuter->remove();

            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 3 && hCache->size() == 3 &&
                        !hCqc->containsValue(v) && !hCache->containsValue(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = cast<Map::Entry::View>(hMuter->next())->getKey();
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 2 && hCache->size() == 2 &&
                        !hCqc->containsKey(v) && !hCache->containsKey(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = cast<Map::Entry::View>(hMuter->next())->getValue();
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 1 && hCache->size() == 1 &&
                        !hCqc->containsValue(v) && !hCache->containsValue(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            v = cast<Map::Entry::View>(hMuter->next())->getValue();
            hMuter->remove();
            fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 0 && hCache->size() == 0 &&
                        !hCqc->containsValue(v) && !hCache->containsValue(v);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);
            TS_ASSERT(!hMuter->hasNext());
            hCqc->release();
            }

        void testPutGetAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");
            Object::View vKey3  = String::create("key3");
            Object::View vVal3  = String::create("val3");

            // populate map that will be passed into putAll()
            Map::Handle hmap = HashMap::create();
            hmap->put(vKey0, vVal0);
            hmap->put(vKey1, vVal1);
            hmap->put(vKey2, vVal2);
            hmap->put(vKey3, vVal3);

            hCqc->putAll(hmap);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hCqc->size() == 4 && hCache->size() == 4 &&
                    hCqc->containsKey(vKey0) && hCqc->containsKey(vKey1) &&
                    hCqc->containsKey(vKey2) && hCqc->containsKey(vKey3) &&
                    hCache->containsKey(vKey0) && hCache->containsKey(vKey1) &&
                    hCache->containsKey(vKey2) && hCache->containsKey(vKey3);
                Thread::sleep(5);
                }
            TS_ASSERT(fResult);

            // getAll()
            Set::Handle hKeys = HashSet::create();
            hKeys->add(vKey0);
            hKeys->add(vKey1);
            hKeys->add(vKey2);
            hKeys->add(vKey3);
            Map::View vmapAll = hCqc->getAll(hKeys);
            TS_ASSERT(hmap->equals(vmapAll));

            hCqc->release();
            }

        void testGetCache()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            TS_ASSERT(hCqc->getCache() == hCache);
            TS_ASSERT(hCqc->getCacheService()->isRunning());

            CacheService::Handle hService = hCqc->getCacheService();
            hService->shutdown();

            TS_ASSERT(!hService->isRunning());

            hCqc->release();
            }

        void testClear()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), true);

            Object::View vKey0  = String::create("key0");
            Object::View vVal0  = String::create("val0");
            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");

            hCqc->put(vKey0, vVal0);
            hCqc->put(vKey1, vVal1);
            hCqc->put(vKey2, vVal2);

            TS_ASSERT(hCqc->size() == 3);

            hCqc->clear();
            TS_ASSERT(hCqc->size() == 0);
            hCqc->release();
            }

        void testAddRemoveKeyListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            // the mock listener that receives the event notifications
            MockMapListener::Handle hMockListener = MockMapListener::create();
            // the mock used in verifying the MapEvent which is passed to the listener
            MockMapEvent::Handle hMockMapEvent = MockMapEvent::create();
            // keys/values
            Object::View vKey1  = String::create("key1");
            Object::View vVal1  = String::create("val1");
            Object::View vKey2  = String::create("key2");
            Object::View vVal2  = String::create("val2");

            hMockListener->setStrict(true);
            hMockListener->entryInserted(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryUpdated(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryDeleted(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->replay();

            // setup mock for pattern matcher.
            hMockMapEvent->setStrict(true);
            // insert
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey1);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_inserted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(vVal1);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            // update
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey1);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_updated);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vVal1);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(vVal2);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            //delete
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey1);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_deleted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vVal2);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            hMockMapEvent->replay();

            hCqc->addKeyListener(hMockListener, String::create("key1"), false);

            // insert/update/delete
            hCache->put(vKey1, vVal1);
            hCache->put(vKey1, vVal2);
            hCache->remove(vKey1);

            // insert/update/delete with a different key shouldn't receive events
            hCache->put(vKey2, vVal1);
            hCache->put(vKey2, vVal2);
            hCache->remove(vKey2);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hMockListener->verifyAndReturnResult();
                Thread::sleep(5);
                }

            // remove the listener and ensure that further events aren't received
            hCqc->removeKeyListener(hMockListener, vKey1);
            hCache->put(vKey1, vVal1);
            hCache->put(vKey1, vVal2);
            hCache->remove(vKey1);

            hMockListener->verify();
            hCqc->release();
            }

        void testAddRemoveFilterListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                    hCache, AlwaysFilter::create(), false);

            Filter::View vFilter = EqualsFilter::create(
                    IdentityExtractor::create(), String::create("value"));

            // the mock listener that receives the event notifications
            MockMapListener::Handle hMockListener = MockMapListener::create();
            // the mock used in verifying the MapEvent which is passed to the listener
            MockMapEvent::Handle hMockMapEvent = MockMapEvent::create();

            hMockListener->setStrict(true);
            hMockListener->entryInserted(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryUpdated(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryDeleted(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->replay();

            // setup mock for pattern matcher.
            hMockMapEvent->setStrict(true);
            // insert
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(String::create("key"));
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_inserted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(String::create("value"));
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            // update
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(String::create("key2"));
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_updated);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(String::create("some-value"));
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(String::create("value"));
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            //delete
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(String::create("key2"));
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_deleted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(String::create("value"));
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCqc);
            hMockMapEvent->replay();

            MapEventFilter::View vEventFilter = MapEventFilter::create(vFilter);
            hCqc->addFilterListener(hMockListener, vEventFilter, false);

            // insert
            hCache->put(String::create("key"), String::create("value"));
            // update
            hCache->put(String::create("key2"), String::create("some-value"));
            hCache->put(String::create("key2"), String::create("value"));
            //delete
            hCache->remove(String::create("key2"));

            // insert/update/delete with a different key shouldn't receive events
            hCache->put(String::create("key3"), String::create("not-what-Im-looking-for"));
            hCache->put(String::create("key3"), String::create("not-what-Im-looking-for-either"));
            hCache->remove(String::create("key3"));

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hMockListener->verifyAndReturnResult();
                Thread::sleep(5);
                }

            // remove the listener and ensure that further events aren't received
            hCqc->removeFilterListener(hMockListener, vEventFilter);
            hCache->put(String::create("key4"), String::create("value"));
            hCache->put(String::create("key4"), String::create("some-value"));
            //delete
            hCache->remove(String::create("key"));
            hCache->remove(String::create("key4"));

            hMockListener->verify();
            hCqc->release();
            }

        void testKeySetWithFilter_CacheValues()
            {
            Filter::View vCqcFilter = GreaterFilter::create(
                    IdentityExtractor::getInstance(), String::create("1000"));

            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, vCqcFilter, true);

            hCache->put(String::create("1"), String::create("100"));
            hCache->put(String::create("2"), String::create("500"));
            hCache->put(String::create("3"), String::create("1001"));
            hCache->put(String::create("4"), String::create("1500"));
            hCache->put(String::create("5"), String::create("2500"));

            Filter::View vFilter = LessFilter::create(
                    IdentityExtractor::getInstance(), String::create("2000"));

            Set::View vSet = hCqc->keySet(vFilter);
            TS_ASSERT(vSet->size() == 2 &&
                    vSet->contains(String::create("3")) &&
                    vSet->contains(String::create("4")));
            hCqc->release();
            }

        void testKeySetWithFilter_DontCacheValues()
            {
            Filter::View vCqcFilter = GreaterFilter::create(
                    IdentityExtractor::getInstance(), String::create("1000"));

            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, vCqcFilter, false);

            hCache->put(String::create("100"), String::create("100"));
            hCache->put(String::create("500"), String::create("500"));
            hCache->put(String::create("1001"), String::create("1001"));
            hCache->put(String::create("1500"), String::create("1500"));
            hCache->put(String::create("2500"), String::create("2500"));

            // test using ReflectionExtractor
            Filter::View vFilter = LessFilter::create(
                  ReflectionExtractor::create("trim"), String::create("2000"));

            Set::View vSet = hCqc->keySet(vFilter);
            TS_ASSERT(vSet->size() == 2 &&
                    vSet->contains(String::create("1001")) &&
                    vSet->contains(String::create("1500")));
            hCqc->release();
            }

        void testEntrySetWithFilter_CacheValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), true);

            hCache->put(String::create("100"), String::create("100"));
            hCache->put(String::create("500"), String::create("500"));
            hCache->put(String::create("1001"), String::create("1001"));
            hCache->put(String::create("1500"), String::create("1500"));
            hCache->put(String::create("2500"), String::create("2500"));

            Filter::View vFilter = AndFilter::create(
                    GreaterFilter::create(IdentityExtractor::getInstance(), String::create("1000")),
                    LessFilter::create(IdentityExtractor::getInstance(), String::create("2000")));

            Set::View vSet = hCqc->keySet(vFilter);
            TS_ASSERT(vSet->size() == 2 &&
                    vSet->contains(String::create("1001")) &&
                    vSet->contains(String::create("1500")));
            hCqc->release();
            }

        void testEntrySetWithFilter_DontCacheValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), false);

            hCache->put(String::create("100"), String::create("100"));
            hCache->put(String::create("500"), String::create("500"));
            hCache->put(String::create("1001"), String::create("1001"));
            hCache->put(String::create("1500"), String::create("1500"));
            hCache->put(String::create("2500"), String::create("2500"));

            Filter::View vFilter = AndFilter::create(
                    GreaterFilter::create(IdentityExtractor::getInstance(), String::create("1000")),
                    LessFilter::create(IdentityExtractor::getInstance(), String::create("2000")));

            Set::View vSet = hCqc->keySet(vFilter);
            TS_ASSERT(vSet->size() == 2 &&
                    vSet->contains(String::create("1001")) &&
                    vSet->contains(String::create("1500")));
            hCqc->release();
            }

        void testInvoke()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), true);

            ReflectionUpdater::View vUpdater = ReflectionUpdater::create(
                        String::create("setPage"));

            UpdaterProcessor::Handle vUpdaterProcessor =
                    UpdaterProcessor::create(vUpdater, Integer32::create(33));

            String::View vKey = String::create("key");

            // use a LimitFilter to invoke on simply because pof already knows
            // about it and it has a convenient public set method to call.
            Object::Handle hValue = LimitFilter::create();
            hCqc->put(vKey, hValue);

            // call the invoker and assert that true is returned
            TS_ASSERT(cast<Boolean::View>(hCqc->invoke(vKey, vUpdaterProcessor))->getValue());

            // assert that the value was changed
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey))->getPage() == 33);
            hCqc->release();
            }

        void testInvokeAllWithCollection()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), false);

            ReflectionUpdater::View vUpdater = ReflectionUpdater::create(
                        String::create("setPage"));

            UpdaterProcessor::Handle vUpdaterProcessor =
                    UpdaterProcessor::create(vUpdater, Integer32::create(33));

            String::View vKey = String::create("key");
            String::View vKey2 = String::create("key2");
            String::View vKey3 = String::create("key3");
            String::View vKey4 = String::create("key4");

            // use a LimitFilter to invoke on simply because pof already knows
            // about it and it has a convenient public set method to call.
            Object::Handle hValue = LimitFilter::create();
            Object::Handle hValue2 = LimitFilter::create();
            Object::Handle hValue3 = LimitFilter::create();
            Object::Handle hValue4 = LimitFilter::create();
            hCqc->put(vKey, hValue);
            hCqc->put(vKey2, hValue2);
            hCqc->put(vKey3, hValue3);
            hCqc->put(vKey4, hValue4);

            // add some of the keys to the collection
            Collection::Handle hKeys = HashSet::create();
            hKeys->add(vKey2);
            hKeys->add(vKey4);

            // call the invoker and assert that true is returned
            Map::View vMap = hCqc->invokeAll((Collection::View)hKeys, vUpdaterProcessor);
            TS_ASSERT(vMap->size() == 2);
            TS_ASSERT(vMap->containsKey(vKey2) && vMap->containsKey(vKey4));
            TS_ASSERT(cast<Boolean::View>(vMap->get(vKey2))->getValue() &&
                      cast<Boolean::View>(vMap->get(vKey4))->getValue());

            // assert that the value was changed
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey))->getPage() != 33);
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey2))->getPage() == 33);
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey3))->getPage() != 33);
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey4))->getPage() == 33);
            hCqc->release();
            }

        void testInvokeAllWithFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), false);

            ReflectionUpdater::View vUpdater = ReflectionUpdater::create(
                        String::create("setPage"));

            UpdaterProcessor::Handle vUpdaterProcessor =
                    UpdaterProcessor::create(vUpdater, Integer32::create(33));

            String::View vKey = String::create("1");
            String::View vKey2 = String::create("2");
            String::View vKey3 = String::create("3");
            String::View vKey4 = String::create("4");

            // use a LimitFilter to invoke on simply because pof already knows
            // about it and it has a convenient public set method to call.
            Object::Handle hValue = LimitFilter::create();
            Object::Handle hValue2 = LimitFilter::create();
            Object::Handle hValue3 = LimitFilter::create();
            Object::Handle hValue4 = LimitFilter::create();
            hCqc->put(vKey, hValue);
            hCqc->put(vKey2, hValue2);
            hCqc->put(vKey3, hValue3);
            hCqc->put(vKey4, hValue4);

            // add some of the keys to the Set
            HashSet::Handle hKeys = HashSet::create();
            hKeys->add(vKey2);
            hKeys->add(vKey4);

            Filter::View vFilter = InFilter::create(
                    KeyExtractor::create(IdentityExtractor::getInstance()), hKeys);

            // call the invoker and assert that true is returned
            Map::View vMap = hCqc->invokeAll(vFilter, vUpdaterProcessor);
            TS_ASSERT(vMap->size() == 2);
            TS_ASSERT(vMap->containsKey(vKey2) && vMap->containsKey(vKey4));
            TS_ASSERT(cast<Boolean::View>(vMap->get(vKey2))->getValue() &&
                      cast<Boolean::View>(vMap->get(vKey4))->getValue());

            // assert that the value was changed
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey))->getPage() != 33);
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey2))->getPage() == 33);
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey3))->getPage() != 33);
            TS_ASSERT(cast<LimitFilter::View>(hCqc->get(vKey4))->getPage() == 33);
            hCqc->release();
            }

        void testAggregateWithCollection_DontCacheValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), false);

            String::View vKey = String::create("1");
            String::View vKey2 = String::create("2");
            String::View vKey3 = String::create("3");
            String::View vKey4 = String::create("4");

            Object::Handle hValue = Integer32::valueOf(10);
            Object::Handle hValue2 = Integer32::valueOf(15);
            Object::Handle hValue3 = Integer32::valueOf(20);
            Object::Handle hValue4 = Integer32::valueOf(25);
            hCqc->put(vKey, hValue);
            hCqc->put(vKey2, hValue2);
            hCqc->put(vKey3, hValue3);
            hCqc->put(vKey4, hValue4);

            // add some of the keys to the Set
            HashSet::Handle hKeys = HashSet::create();
            hKeys->add(vKey2);
            hKeys->add(vKey3);

            // call aggregate to get the max value for the entries
            // represented by the keys in hKeys
            Integer64::View vMax = cast<Integer64::View>
                    (hCqc->aggregate( (Collection::View) hKeys,
                    Integer64Max::create((ValueExtractor::View)
                                         IdentityExtractor::getInstance())));
            TS_ASSERT(vMax->equals(Integer64::create(20)));
            hCqc->release();
            }

        void testAggregateWithCollection_CacheValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), true);

            String::View vKey = String::create("1");
            String::View vKey2 = String::create("2");
            String::View vKey3 = String::create("3");
            String::View vKey4 = String::create("4");

            Object::Handle hValue = Integer32::valueOf(10);
            Object::Handle hValue2 = Integer32::valueOf(15);
            Object::Handle hValue3 = Integer32::valueOf(20);
            Object::Handle hValue4 = Integer32::valueOf(25);
            hCqc->put(vKey, hValue);
            hCqc->put(vKey2, hValue2);
            hCqc->put(vKey3, hValue3);
            hCqc->put(vKey4, hValue4);

            // add some of the keys to the Set
            HashSet::Handle hKeys = HashSet::create();
            hKeys->add(vKey2);
            hKeys->add(vKey3);

            // call aggregate to get the max value for the entries
            // represented by the keys in hKeys
            Integer64::View vMax = cast<Integer64::View>
                    (hCqc->aggregate( (Collection::View) hKeys,
                    Integer64Max::create((ValueExtractor::View)
                                         IdentityExtractor::getInstance())));
            TS_ASSERT(vMax->equals(Integer64::create(20)));
            hCqc->release();
            }

        void testAggregateWithFilter_DontCacheValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), false);

            String::View vKey = String::create("1");
            String::View vKey2 = String::create("2");
            String::View vKey3 = String::create("3");
            String::View vKey4 = String::create("4");

            Object::Handle hValue = Integer32::valueOf(10);
            Object::Handle hValue2 = Integer32::valueOf(15);
            Object::Handle hValue3 = Integer32::valueOf(20);
            Object::Handle hValue4 = Integer32::valueOf(25);
            hCqc->put(vKey, hValue);
            hCqc->put(vKey2, hValue2);
            hCqc->put(vKey3, hValue3);
            hCqc->put(vKey4, hValue4);

            // add some of the keys to the Set
            HashSet::Handle hKeys = HashSet::create();
            hKeys->add(vKey2);
            hKeys->add(vKey3);

            Filter::View vFilter = InFilter::create(
                    KeyExtractor::create(IdentityExtractor::getInstance()), hKeys);

            // call aggregate to get the min value for the entries
            // represented by the keys in hKeys
            Integer64::View vMin = cast<Integer64::View>
                    (hCqc->aggregate(vFilter,
                    Integer64Min::create((ValueExtractor::View)
                                         IdentityExtractor::getInstance())));
            TS_ASSERT(vMin->equals(Integer64::create(15)));
            hCqc->release();
            }

        void testAggregateWithFilter_CacheValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                  hCache, AlwaysFilter::create(), true);

            String::View vKey = String::create("1");
            String::View vKey2 = String::create("2");
            String::View vKey3 = String::create("3");
            String::View vKey4 = String::create("4");

            Object::Handle hValue = Integer32::valueOf(10);
            Object::Handle hValue2 = Integer32::valueOf(15);
            Object::Handle hValue3 = Integer32::valueOf(20);
            Object::Handle hValue4 = Integer32::valueOf(25);
            hCqc->put(vKey, hValue);
            hCqc->put(vKey2, hValue2);
            hCqc->put(vKey3, hValue3);
            hCqc->put(vKey4, hValue4);

            // add some of the keys to the Set
            HashSet::Handle hKeys = HashSet::create();
            hKeys->add(vKey2);
            hKeys->add(vKey3);

            Filter::View vFilter = InFilter::create(
                    KeyExtractor::create(IdentityExtractor::getInstance()), hKeys);

            // call aggregate to get the min value for the entries
            // represented by the keys in hKeys
            Integer64::View vMin = cast<Integer64::View>
                    (hCqc->aggregate(vFilter,
                    Integer64Min::create((ValueExtractor::View)
                                         IdentityExtractor::getInstance())));
            TS_ASSERT(vMin->equals(Integer64::create(15)));
            hCqc->release();
            }

        /*
        * Test for COH-2532.
        */
        void testCoh2532()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test-cqc");

            hCache->put(String::create("1"), String::create("Homer Simpson"));
            hCache->put(String::create("2"), String::create("Marge Simpson"));
            hCache->put(String::create("3"), String::create("Bart Simpson"));
            hCache->put(String::create("4"), String::create("Lisa Simpson"));

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hCache,
               AlwaysFilter::create(), true,
               (MapListener::Handle) NULL,
               IdentityExtractor::getInstance());
            TS_ASSERT(hCqc->isReadOnly());

            Map::Handle hExpectedView = HashMap::create(5);
            hExpectedView->put(String::create("1"), String::create("Homer Simpson"));
            hExpectedView->put(String::create("2"), String::create("Marge Simpson"));
            hExpectedView->put(String::create("3"), String::create("Bart Simpson"));
            hExpectedView->put(String::create("4"), String::create("Lisa Simpson"));
            TS_ASSERT(hExpectedView->equals(hCqc));

            hCache->put(String::create("5"), String::create("Maggie Simpson"));
            hExpectedView->put(String::create("5"), String::create("Maggie Simpson"));
            TS_ASSERT(hExpectedView->equals(hCqc));
            hCqc->release();
            }

        /*
        * Test for COH-6270
        */
        void testCOH6270()
            {
            NamedCache::Handle hCache = ensureCleanCache("TestCache");

            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(
                hCache, AlwaysFilter::create(), false, SampleMapListener::create());
            hCache->put(String::create("Key6270"), String::create("Value6270"));
            hCqc->release();
            }

        /*
        * Test for COH-10013
        */
        void testCoh10013()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-extend-direct");

            TestContact::View vT1 = TestContact::create("John", "Loehe",
                 ExampleAddress::create("675 Beacon St.", "", "Dthaba", "SC", "91666", "USA"));
            hCache->put(String::create("1"), vT1);

            TestContact::View vT2 = TestContact::create("John", "Sydqtiinz",
                 ExampleAddress::create("336 Beacon St.", "", "Wltowuixs", "MA", "00595", "USA"));
            hCache->put(String::create("2"), vT2);

            // find all contacts who live in Massachusetts - direct cache access
            Collection::View vResults = hCache->keySet
                    (FilterFactory::createFilter("homeAddress.state = 'MA'", "ExtendTcpInvocationService"));

            // assert we got one result
            TS_ASSERT(vResults->size() == 1);
            Iterator::Handle hIter = vResults->iterator();
            TestContact::View vT3 = cast<TestContact::View>(hCache->get(hIter->next()), false);
            TS_ASSERT(vT2->compareTo(vT3) == 0);

            // query with an InFilter created on the client
            HashSet::Handle hValues = HashSet::create();
            hValues->add(String::create("Loehe"));
            InFilter::View vInFil = InFilter::create(ReflectionExtractor::create(String::create("getLastName")), hValues);
            ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hCache, vInFil, true);
            vResults = hCqc->keySet();

            // assert we got one result
            TS_ASSERT(vResults->size() == 1);
            vT3 = cast<TestContact::View>(hCqc->get(vResults->iterator()->next()),false);
            TS_ASSERT(vT1->compareTo(vT3) == 0);

            // query with an InFilter created on the cache
            Filter::View vFilter = FilterFactory::createFilter("lastName in ('Loehe')", "ExtendTcpInvocationService");
            hCqc = ContinuousQueryCache::create(hCache, vFilter, false);
            vResults = hCqc->keySet();

            // assert we got one result
            TS_ASSERT(vResults->size() == 1);
            hIter = vResults->iterator();
            vT3 = cast<TestContact::View>(hCqc->get(hIter->next()), false);
            TS_ASSERT(vT1->compareTo(vT3) == 0);
            hCqc->release();
        }

        /**
        * Test to validate truncate.
        */
        void testTruncate()
            {
            NamedCache::Handle           hNamedCache = getAndPopulateNamedCache("dist-test-cqc");
            TestCQCListener::Handle      hListener   = TestCQCListener::create(100);
            ContinuousQueryCache::Handle hTheCQC     = ContinuousQueryCache::create(
                    hNamedCache, 
                    AlwaysFilter::create(), 
                    true, 
                    hListener);

            // check that the hListener received the correct number of events
            // and the CQC has set its state to STATE_SYNCHRONIZED
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            COH_EVENTUALLY_ASSERT_THAT(hListener->getActualTotal() == 100);

            hListener->resetActualTotal();

            hTheCQC->truncate();
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            COH_EVENTUALLY_ASSERT_THAT(hListener->getActualTotal() == 0);
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->isEmpty() == true);
            TS_ASSERT(hTheCQC->isActive() == true);
            hTheCQC->release();
            }

        /**
        * Ensure truncate is a no-op on the ContinuousQueryCache-side if it is read-only.
        */
        void testTruncateReadOnly()
            {
            NamedCache::Handle           hNamedCache = getAndPopulateNamedCache("dist-test-cqc");
            TestCQCListener::Handle      hListener   = TestCQCListener::create(100);
            ContinuousQueryCache::Handle hTheCQC     = ContinuousQueryCache::create(
                    hNamedCache, 
                    AlwaysFilter::create(), 
                    true, 
                    hListener);

            // check that the hListener received the correct number of events
            // and the CQC has set its state to STATE_SYNCHRONIZED
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            COH_EVENTUALLY_ASSERT_THAT(hListener->getActualTotal() == 100);

            hListener->resetActualTotal();

            hTheCQC->setReadOnly(true);
            TS_ASSERT_THROWS(hTheCQC->truncate(), IllegalStateException::View);
            hTheCQC->release();
            }

        /**
        * Ensure truncate is properly honored if invoked against the back cache, even if the ContinuousQueryCache
        * is marked read-only.
        */
        void testTruncateReadOnlyBackCacheTruncation()
            {
            NamedCache::Handle           hNamedCache = getAndPopulateNamedCache("dist-test-cqc");
            TestCQCListener::Handle      hListener   = TestCQCListener::create(100);
            ContinuousQueryCache::Handle hTheCQC     = ContinuousQueryCache::create(
                    hNamedCache, 
                    AlwaysFilter::create(), 
                    true, 
                    hListener);

            // check that the listener received the correct number of events
            // and the CQC has set its state to STATE_SYNCHRONIZED
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            COH_EVENTUALLY_ASSERT_THAT(hListener->getActualTotal() == 100);

            hListener->resetActualTotal();

            hTheCQC->setReadOnly(true);
            hTheCQC->getCache()->truncate();
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            COH_EVENTUALLY_ASSERT_THAT(hListener->getActualTotal() == 0);
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->isEmpty() == true);
            TS_ASSERT(hTheCQC->isActive() == true);
            hTheCQC->release();
            }

        /**
        * Ensure an UnsupportedOperationException is thrown if the backing cache doesn't support truncation.
        */
        void testTruncateNotSupported()
            {
            NamedCache::Handle           hNamedCache = getAndPopulateNamedCache("repl-test-cqc");
            TestCQCListener::Handle      hListener   = TestCQCListener::create(100);
            ContinuousQueryCache::Handle hTheCQC     = ContinuousQueryCache::create(
                    hNamedCache, 
                    AlwaysFilter::create(), 
                    true, 
                    hListener);

            // check that the hListener received the correct number of events
            // and the CQC has set its state to STATE_SYNCHRONIZED
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            COH_EVENTUALLY_ASSERT_THAT(hListener->getActualTotal() == 100);

            hListener->resetActualTotal();

            TS_ASSERT_THROWS_ANYTHING(hTheCQC->truncate());
            hTheCQC->release();
            }

        /**
        * Validate release() puts the ContinuousQueryCache into a disconnected state.
        * Ensure the ContinuousQueryCache can properly resynchronize after a local release.
        */
        void testRelease()
            {
            NamedCache::Handle           hNamedCache = getAndPopulateNamedCache("dist-test-cqc");
            ContinuousQueryCache::Handle hTheCQC     = ContinuousQueryCache::create(
                    hNamedCache, 
                    AlwaysFilter::create(), 
                    true);

            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);

            TS_ASSERT(hTheCQC->isActive());
            hTheCQC->release();
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);
            COH_EVENTUALLY_ASSERT_THAT(!hTheCQC->isActive());

            // trigger CQC to re-synchronize
            hTheCQC->get(String::View("someKey"));
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            TS_ASSERT(hTheCQC->isActive())
            hTheCQC->release();
            }

        /**
        * Validate destroy() puts the ContinuousQueryCache into a disconnected state.
        * Ensure the ContinuousQueryCache can properly resynchronize after a local release.
        */
        void testDestroy()
            {
            NamedCache::Handle           hNamedCache = getAndPopulateNamedCache("dist-test-cqc");
            ContinuousQueryCache::Handle hTheCQC     = ContinuousQueryCache::create(
                    hNamedCache, 
                    AlwaysFilter::create(), 
                    true);

            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);

            TS_ASSERT(hTheCQC->isActive());
            hTheCQC->destroy();
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);
            COH_EVENTUALLY_ASSERT_THAT(!hTheCQC->isActive());

            // trigger CQC to re-synchronize
            hTheCQC->get(String::View("someKey"));
            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            TS_ASSERT(hTheCQC->isActive())
            hTheCQC->release();
            }

        /**
        * Validate the name of the ContinuousQueryCache with and without a Supplier
        * for the cache name.
        */  
        void testCacheNameSupplier()
            {
            NamedCache::Handle           hNamedCache = getAndPopulateNamedCache("dist-test-cqc");
            ContinuousQueryCache::Handle hTheCQC     = ContinuousQueryCache::create(
                    hNamedCache, 
                    AlwaysFilter::create(), 
                    true);

            TS_ASSERT(String::View("ContinuousQueryCache{Cache=dist-test-cqc, Filter=AlwaysFilter, Transformer=NULL}")->equals(hTheCQC->getCacheName()));

            hTheCQC->setCacheNameSupplier(CacheNameSupplier::create("dist-test-cqc"));

            TS_ASSERT(String::View("dist-test-cqc")->equals(hTheCQC->getCacheName()));

            hTheCQC->setCacheNameSupplier(NULL);

            TS_ASSERT(String::View("ContinuousQueryCache{Cache=dist-test-cqc, Filter=AlwaysFilter, Transformer=NULL}")->equals(hTheCQC->getCacheName()));
            hTheCQC->release();
            }

        /**
        * Unit test to validate CQC reconnect interval state transitions.
        */
        void testReconnectInterval()
            {
            ContinuousQueryCache::Handle hTheCQC = ContinuousQueryCache::create(
                    (Supplier::View) CacheSupplier::create(), 
                    AlwaysFilter::create(),
                    true,
                    (MapListener::Handle) NULL,
                    (ValueExtractor::View) NULL);

            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);

            hTheCQC->setReconnectInterval(3000);
            hTheCQC->getCache()->destroy();

            COH_EVENTUALLY_ASSERT_THAT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);

            TS_ASSERT(String::create("1")->equals(hTheCQC->get(String::create("TestKey1"))));
            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);

            hTheCQC->put(String::create("MyKey"), String::create("MyValue"));

            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);
            TS_ASSERT(hTheCQC->get(String::create("MyKey")) == NULL);
            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);

            Thread::sleep(4000);

            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);
            TS_ASSERT(String::create("MyValue")->equals(hTheCQC->get(String::create("MyKey"))));
            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            }

        /**
        * Unit test to validate CQC reconnect interval state transitions.
        */
        void testReconnectIntervalSetAtZero()
            {
            TestContinuousQueryCache::Handle hTheCQC = TestContinuousQueryCache::create(
                    (Supplier::View) CacheSupplier::create(), 
                    AlwaysFilter::create(),
                    true);

            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);

            hTheCQC->changeState(ContinuousQueryCache::state_disconnected);
            hTheCQC->lockState();

            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);
            TS_ASSERT_THROWS(hTheCQC->get(String::create("TestKey1")), RuntimeException::View);

            hTheCQC->unlockState();

            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_disconnected);
            TS_ASSERT(String::create("1")->equals(hTheCQC->get(String::create("TestKey1"))));
            TS_ASSERT(hTheCQC->getState() == ContinuousQueryCache::state_synchronized);
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }

        // ----- helper methods ---------------------------------------------

        private:
            NamedCache::Handle getAndPopulateNamedCache(String::View vsCacheName)
                {
                NamedCache::Handle hTestCache = ensureCleanCache(vsCacheName);
                for (int i = 0; i < 100; i++)
                    {
                    hTestCache->put(COH_TO_STRING("TestKey" << i), COH_TO_STRING("" << i));
                    }
                return hTestCache;
                }
    };
