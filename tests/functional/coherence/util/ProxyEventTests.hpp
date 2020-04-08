/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"

#include "coherence/tests/TestObject.hpp"

#include "coherence/util/AtomicCounter.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/PofExtractor.hpp"

#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/LessFilter.hpp"
#include "coherence/util/filter/MapEventFilter.hpp"
#include "coherence/util/filter/MapEventTransformerFilter.hpp"

#include "coherence/util/transformer/SemiLiteEventTransformer.hpp"

#include "common/TestUtils.hpp"

#include "common/TestClasses.hpp"

using coherence::net::NamedCache;

using coherence::tests::TestObject;

using coherence::util::AtomicCounter;
using coherence::util::Filter;
using coherence::util::MapEvent;
using coherence::util::MapListener;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::PofExtractor;

using coherence::util::filter::EqualsFilter;
using coherence::util::filter::LessFilter;
using coherence::util::filter::MapEventFilter;
using coherence::util::filter::MapEventTransformerFilter;

using coherence::util::transformer::SemiLiteEventTransformer;

/**
* Coherence*Extend test for map listeners receiving events.
* Tests:
* COH-8157 (Bug14778008)
* COH-8238 (Bug - none)
* COH-8578 (Bug16023459)
*
* Test the behavior of proxy returning events.
* COH-8157 reported multiple events received when there is
* both a key and a filter listener that should receive the
* single event. Test cases;
*  - one key listener
*  - one filter listener
*  - multiple key listeners
*  - multiple filter listeners
*  - one key, one filter listener
*  - one key, multiple filter
*  - multiple key, one filter
*  - multiple key, multiple filter
* - regression test for COH-9355 use case
* @author par  2013.1.22
*/
class ProxyEventTests : public CxxTest::TestSuite
    {
    /**
    * MapListener that continuously receives events from the cache.
    */
    class TestListener : public class_spec<TestListener,
            extends<Object>, implements<MapListener> >
        {
        friend class factory<TestListener>;

        public: 
            TestListener(uint32_t nExpected, String::View vsListenerName)
                    : m_cExpected(nExpected), f_vListenerName(self(), vsListenerName), 
                      m_cInserts(0), m_cUpdates(0), m_cDeletes(0)
                {
                }

            // ----- MapListener interface ------------------------------------

            void entryUpdated(MapEvent::View /* evt */)
                {
                m_cUpdates++;
                }

            void entryInserted(MapEvent::View /* evt */)
                {
                m_cInserts++;
                }

            void entryDeleted(MapEvent::View /* evt */)
                {
                m_cDeletes++;
                }

            // ----- accessors ------------------------------------------------

            /**
            * Number of update events listener actually received.
            *
            * @return  number of event received
            */
            uint32_t getUpdateCount()
                {
                return m_cUpdates;
                }

            /**
            * Number of insert events listener actually received.
            *
            * @return  number of events received
            */
            uint32_t getInsertCount()
                {
                return m_cInserts;
                }

            /**
            * Number of delete events listener actually received.
            *
            * @return  number of event received
            */
            uint32_t getDeleteCount()
                {
                return m_cDeletes;
                }

            /**
            * Number of delete events listener actually received.
            *
            * @return  number of event received
            */
            uint32_t getUpdateExpected()
                {
                return m_cExpected;
                }

            /**
            * Number of delete events listener actually received.
            *
            * @return  number of event received
            */
            uint32_t getInsertExpected()
                {
                return m_cExpected;
                }

            /**
            * Number of delete events listener actually received.
            *
            * @return  number of event received
            */
            uint32_t getDeleteExpected()
                {
                return m_cExpected;
                }

            // ----- data members -----------------------------------------------

        private:
            /**
            * Number of events listener should receive
            */
            uint32_t m_cExpected;

            /**
            * Listener name
            */
            FinalView<String> f_vListenerName;

            /**
            * Number of insert events actually received
            */
            uint32_t m_cInserts;

            /**
            * Number of update events actually received
            */
            uint32_t m_cUpdates;

            /**
            * Number of delete events actually received
            */
            uint32_t m_cDeletes;
        };


    // ----- inner class: WaitListener --------------------------------------

    /**
    * Custom listener that waits until the listeners have been added to the
    * cache.  Stops waiting when it receives its event or it times out,
    * whichever comes first.
    */
    class WaitListener : public class_spec<WaitListener,
            extends<Object>, implements<MapListener> >
        {
        friend class factory<WaitListener>;

        public: 
            WaitListener(int64_t cTimeout, String::View vsListenerName)
                    : m_cTimeout(cTimeout), f_vListenerName(self(), vsListenerName), 
                      m_fEventReceived(false), m_cStartTime(0)
                {
                }

            void entryUpdated(MapEvent::View /* evt */)
                {
                m_fEventReceived = true;
                }

            void entryInserted(MapEvent::View /* evt */)
                {
                m_fEventReceived = true;
                }

            void entryDeleted(MapEvent::View /* evt */)
                {
                m_fEventReceived = true;
                }

            bool getEventReceived()
                {
                return m_fEventReceived;
                }

            bool getTimedOut()
                {
                return (m_cStartTime + m_cTimeout < System::safeTimeMillis() ? true : false);
                }

            void startWait()
                {
                m_cStartTime = System::safeTimeMillis();
                }

            void reset(int64_t cTimeout)
                {
                m_cStartTime     = 0L;
                m_cTimeout       = cTimeout;
                m_fEventReceived = false;
                }

        // ----- data members -----------------------------------------------

        private:
            /**
            * Length of timeout
            */
            int64_t m_cTimeout;

            /**
            * Which listener is this, WAIT
            */
            FinalView<String> f_vListenerName;

            /**
            * Events received
            */
            bool m_fEventReceived;

            /**
            * Start time
            */
            int64_t m_cStartTime;
        };

    /**
    * MapListener that receives events from the cache to
    * test COH-9355.
    */
    class TestEventListener : public class_spec<TestEventListener,
            extends<Object>, implements<MapListener> >
        {
        friend class factory<TestEventListener>;

        public: 
             TestEventListener() :
                 m_hUpdateCounter(self(), AtomicCounter::create(0L)),
                 m_hInsertCounter(self(), AtomicCounter::create(0L)),
                 m_hDeleteCounter(self(), AtomicCounter::create(0L))
             {}

            // ----- MapListener interface ------------------------------------

            void entryUpdated(MapEvent::View /* evt */)
                {
                m_hUpdateCounter->increment();
                }

            void entryInserted(MapEvent::View /* evt */)
                {
                m_hInsertCounter->increment();
                }

            void entryDeleted(MapEvent::View /* evt */)
                {
                m_hDeleteCounter->increment();
                }

            /**
            * Number of update events listener received.
            *
            * @return  number of events received
            */
            int64_t getUpdateCount()
                {
                return m_hUpdateCounter->getCount();
                }

            /**
            * Number of insert events listener received.
            *
            * @return  number of events received
            */
            int64_t getInsertCount()
                {
                return m_hInsertCounter->getCount();
                }

            /**
            * Number of delete events listener received.
            *
            * @return  number of evenst received
            */
            int64_t getDeleteCount()
                {
                return m_hDeleteCounter->getCount();
                }


            // ----- data members ---------------------------------------------

         private:
            /**
            * Number of update events listener received.
            */
            MemberHandle<AtomicCounter> m_hUpdateCounter;

            /**
            * Number of insert events listener received.
            */
            MemberHandle<AtomicCounter> m_hInsertCounter;

            /**
            * Number of delete events listener received.
            */
            MemberHandle<AtomicCounter> m_hDeleteCounter;
        };


    // ----- Test Methods -----------------------------------------------------

    public:
        /**
        * Test events returned to one key listener.
        */
        void testOneKeyListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hKeyListener = TestListener::create(1, String::create("KEY"));

            String::View vsListenerKey = String::create("TestKey1");  
            hCache->addKeyListener(hKeyListener, vsListenerKey, false);

            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hKeyListener->getInsertCount() == hKeyListener->getInsertExpected());
            TS_ASSERT(hKeyListener->getUpdateCount() == hKeyListener->getUpdateExpected());
            TS_ASSERT(hKeyListener->getDeleteCount() == hKeyListener->getDeleteExpected());

            hCache->removeKeyListener(hKeyListener, vsListenerKey);
            }

        /**
        * Test events returned to one filter listener.
        */
        void testOneFilterListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hFilterListener = TestListener::create(5, String::create("FILTER"));

            Filter::Handle hFilter = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(5));
            MapEventFilter::View vEventFilter = MapEventFilter::create(MapEventFilter::e_all, hFilter);

            hCache->addFilterListener(hFilterListener, vEventFilter, false);

            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hFilterListener->getInsertCount() == hFilterListener->getInsertExpected());
            TS_ASSERT(hFilterListener->getUpdateCount() == hFilterListener->getUpdateExpected());
            TS_ASSERT(hFilterListener->getDeleteCount() == hFilterListener->getDeleteExpected());

            hCache->removeFilterListener(hFilterListener, vEventFilter);
            }

        /**
        * Test events returned to multiple key listeners.
        */
        void testMultipleKeyListeners()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hKeyListener  = TestListener::create(1, String::create("KEY"));
            TestListener::Handle hKeyListener2 = TestListener::create(1, String::create("KEY2"));

            String::View vsListenerKey = String::create("TestKey1");  
            hCache->addKeyListener(hKeyListener, vsListenerKey, false);

            String::View vsListenerKey2 = String::create("TestKey2");  
            hCache->addKeyListener(hKeyListener2, vsListenerKey2, false);


            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hKeyListener->getInsertCount() == hKeyListener->getInsertExpected());
            TS_ASSERT(hKeyListener->getUpdateCount() == hKeyListener->getUpdateExpected());
            TS_ASSERT(hKeyListener->getDeleteCount() == hKeyListener->getDeleteExpected());

            TS_ASSERT(hKeyListener2->getInsertCount() == hKeyListener2->getInsertExpected());
            TS_ASSERT(hKeyListener2->getUpdateCount() == hKeyListener2->getUpdateExpected());
            TS_ASSERT(hKeyListener2->getDeleteCount() == hKeyListener2->getDeleteExpected());

            hCache->removeKeyListener(hKeyListener, vsListenerKey);
            hCache->removeKeyListener(hKeyListener2, vsListenerKey2);
            }

        /**
        * Test events returned to multiple filter listeners.
        */
        void testMultipleFilterListeners()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hFilterListener  = TestListener::create(5, String::create("FILTER"));
            TestListener::Handle hFilterListener2 = TestListener::create(1,           String::create("FILTER2"));

            Filter::Handle hFilter = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(5));
            MapEventFilter::Handle hEventFilter = MapEventFilter::create(MapEventFilter::e_all, hFilter);

            Filter::Handle hFilter2 = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(1));
            MapEventFilter::Handle hEventFilter2 = MapEventFilter::create(MapEventFilter::e_all, hFilter2);

            hCache->addFilterListener(hFilterListener,  hEventFilter, false);
            hCache->addFilterListener(hFilterListener2, hEventFilter2, false);

            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hFilterListener->getInsertCount() == hFilterListener->getInsertExpected());
            TS_ASSERT(hFilterListener->getUpdateCount() == hFilterListener->getUpdateExpected());
            TS_ASSERT(hFilterListener->getDeleteCount() == hFilterListener->getDeleteExpected());

            TS_ASSERT(hFilterListener2->getInsertCount() == hFilterListener2->getInsertExpected());
            TS_ASSERT(hFilterListener2->getUpdateCount() == hFilterListener2->getUpdateExpected());
            TS_ASSERT(hFilterListener2->getDeleteCount() == hFilterListener2->getDeleteExpected());

            hCache->removeFilterListener(hFilterListener,  hEventFilter);
            hCache->removeFilterListener(hFilterListener2, hEventFilter2);
            }

        /**
        * Test events returned to one key and one filter listeners.
        */
        void testOneKeyOneFilterListeners()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hKeyListener     = TestListener::create(1, String::create("KEY"));
            TestListener::Handle hFilterListener  = TestListener::create(5, String::create("FILTER"));

            Filter::Handle hFilter = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(5));
            MapEventFilter::View vEventFilter = MapEventFilter::create(MapEventFilter::e_all, hFilter);

            String::View vsTestKey = String::create("TestKey1");
            hCache->addKeyListener(hKeyListener, vsTestKey,  false);

            hCache->addFilterListener(hFilterListener, vEventFilter, false);

            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hKeyListener->getInsertCount() == hKeyListener->getInsertExpected());
            TS_ASSERT(hKeyListener->getUpdateCount() == hKeyListener->getUpdateExpected());
            TS_ASSERT(hKeyListener->getDeleteCount() == hKeyListener->getDeleteExpected());

            TS_ASSERT(hFilterListener->getInsertCount() == hFilterListener->getInsertExpected());
            TS_ASSERT(hFilterListener->getUpdateCount() == hFilterListener->getUpdateExpected());
            TS_ASSERT(hFilterListener->getDeleteCount() == hFilterListener->getDeleteExpected());

            hCache->removeKeyListener(hKeyListener, vsTestKey);
            hCache->removeFilterListener(hFilterListener, vEventFilter);
            }

        /**
        * Test events returned to one key and multiple filter listeners.
        */
        void testOneKeyMultipleFilterListeners()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hKeyListener     = TestListener::create(1, String::create("KEY"));
            TestListener::Handle hFilterListener  = TestListener::create(5, String::create("FILTER"));
            TestListener::Handle hFilterListener2 = TestListener::create(1, String::create("FILTER2"));

            Filter::Handle hFilter  = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(5));
            MapEventFilter::View vEventFilter  = MapEventFilter::create(MapEventFilter::e_all, hFilter);

            Filter::Handle hFilter2 = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(1));
            MapEventFilter::View vEventFilter2 = MapEventFilter::create(MapEventFilter::e_all, hFilter2);

            String::View vsTestKey = String::create("TestKey1");
            hCache->addKeyListener(hKeyListener, vsTestKey,  false);

            hCache->addFilterListener(hFilterListener,  vEventFilter, false);
            hCache->addFilterListener(hFilterListener2, vEventFilter2, false);

            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hKeyListener->getInsertCount() == hKeyListener->getInsertExpected());
            TS_ASSERT(hKeyListener->getUpdateCount() == hKeyListener->getUpdateExpected());
            TS_ASSERT(hKeyListener->getDeleteCount() == hKeyListener->getDeleteExpected());

            TS_ASSERT(hFilterListener->getInsertCount() == hFilterListener->getInsertExpected());
            TS_ASSERT(hFilterListener->getUpdateCount() == hFilterListener->getUpdateExpected());
            TS_ASSERT(hFilterListener->getDeleteCount() == hFilterListener->getDeleteExpected());

            TS_ASSERT(hFilterListener2->getInsertCount() == hFilterListener2->getInsertExpected());
            TS_ASSERT(hFilterListener2->getUpdateCount() == hFilterListener2->getUpdateExpected());
            TS_ASSERT(hFilterListener2->getDeleteCount() == hFilterListener2->getDeleteExpected());

            hCache->removeKeyListener(hKeyListener, vsTestKey);
            hCache->removeFilterListener(hFilterListener,  vEventFilter);
            hCache->removeFilterListener(hFilterListener2, vEventFilter2);
            }

        /**
        * Test events returned to multiple key and one filter listeners.
        */
        void testMultipleKeyOneFilterListeners()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hKeyListener    = TestListener::create(1, String::create("KEY"));
            TestListener::Handle hKeyListener2   = TestListener::create(1, String::create("KEY2"));
            TestListener::Handle hFilterListener = TestListener::create(5, String::create("FILTER"));

            Filter::Handle hFilter  = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(5));
            MapEventFilter::View vEventFilter  = MapEventFilter::create(MapEventFilter::e_all, hFilter);

            String::View vsTestKey  = String::create("TestKey1");
            hCache->addKeyListener(hKeyListener,  vsTestKey,  false);
            String::View vsTestKey2 = String::create("TestKey2");
            hCache->addKeyListener(hKeyListener2, vsTestKey2,  false);

            hCache->addFilterListener(hFilterListener,  vEventFilter, false);

            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hKeyListener->getInsertCount() == hKeyListener->getInsertExpected());
            TS_ASSERT(hKeyListener->getUpdateCount() == hKeyListener->getUpdateExpected());
            TS_ASSERT(hKeyListener->getDeleteCount() == hKeyListener->getDeleteExpected());

            TS_ASSERT(hKeyListener2->getInsertCount() == hKeyListener2->getInsertExpected());
            TS_ASSERT(hKeyListener2->getUpdateCount() == hKeyListener2->getUpdateExpected());
            TS_ASSERT(hKeyListener2->getDeleteCount() == hKeyListener2->getDeleteExpected());

            TS_ASSERT(hFilterListener->getInsertCount() == hFilterListener->getInsertExpected());
            TS_ASSERT(hFilterListener->getUpdateCount() == hFilterListener->getUpdateExpected());
            TS_ASSERT(hFilterListener->getDeleteCount() == hFilterListener->getDeleteExpected());

            hCache->removeKeyListener(hKeyListener, vsTestKey);
            hCache->removeKeyListener(hKeyListener2, vsTestKey2);
            hCache->removeFilterListener(hFilterListener, vEventFilter);
            }

        /**
        * Test events returned to multiple key and filter listeners.
        */
        void testMultipleKeyMultipleFilterListeners()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();
            TS_ASSERT(hCache->isEmpty());

            TestListener::Handle hKeyListener     = TestListener::create(1, String::create("KEY"));
            TestListener::Handle hKeyListener2   = TestListener::create(1, String::create("KEY2"));

            TestListener::Handle hFilterListener  = TestListener::create(5, String::create("FILTER"));
            TestListener::Handle hFilterListener2 = TestListener::create(1, String::create("FILTER2"));

            Filter::Handle hFilter  = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(5));
            MapEventFilter::View vEventFilter  = MapEventFilter::create(MapEventFilter::e_all, hFilter);

            Filter::Handle hFilter2 = LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(1));
            MapEventFilter::View vEventFilter2 = MapEventFilter::create(MapEventFilter::e_all, hFilter2);

            String::View vsTestKey  = String::create("TestKey1");
            hCache->addKeyListener(hKeyListener,  vsTestKey,  false);

            String::View vsTestKey2 = String::create("TestKey2");
            hCache->addKeyListener(hKeyListener2, vsTestKey2,  false);

            hCache->addFilterListener(hFilterListener,  vEventFilter, false);
            hCache->addFilterListener(hFilterListener2, vEventFilter2, false);

            // Wait for event that reports listeners have been configured.
            waitForEvents(hCache);

            // check how many were received.
            TS_ASSERT(hKeyListener->getInsertCount() == hKeyListener->getInsertExpected());
            TS_ASSERT(hKeyListener->getUpdateCount() == hKeyListener->getUpdateExpected());
            TS_ASSERT(hKeyListener->getDeleteCount() == hKeyListener->getDeleteExpected());

            TS_ASSERT(hKeyListener2->getInsertCount() == hKeyListener2->getInsertExpected());
            TS_ASSERT(hKeyListener2->getUpdateCount() == hKeyListener2->getUpdateExpected());
            TS_ASSERT(hKeyListener2->getDeleteCount() == hKeyListener2->getDeleteExpected());

            TS_ASSERT(hFilterListener->getInsertCount() == hFilterListener->getInsertExpected());
            TS_ASSERT(hFilterListener->getUpdateCount() == hFilterListener->getUpdateExpected());
            TS_ASSERT(hFilterListener->getDeleteCount() == hFilterListener->getDeleteExpected());

            TS_ASSERT(hFilterListener2->getInsertCount() == hFilterListener2->getInsertExpected());
            TS_ASSERT(hFilterListener2->getUpdateCount() == hFilterListener2->getUpdateExpected());
            TS_ASSERT(hFilterListener2->getDeleteCount() == hFilterListener2->getDeleteExpected());

            hCache->removeKeyListener(hKeyListener,  vsTestKey);
            hCache->removeKeyListener(hKeyListener2, vsTestKey2);
            hCache->removeFilterListener(hFilterListener,  vEventFilter);
            hCache->removeFilterListener(hFilterListener2, vEventFilter2);
            }

        /**
        * Regression test for COH-9355 event delivery semantics.
        */
        void testCoh9355()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
            hCache->clear();

            TestEventListener::Handle hAllListener     = TestEventListener::create();
            TestEventListener::Handle hAllLiteListener = TestEventListener::create();

            TestEventListener::Handle hKeyListener     = TestEventListener::create();
            TestEventListener::Handle hKeyLiteListener = TestEventListener::create();

            TestEventListener::Handle hFilterListener      = TestEventListener::create();
            TestEventListener::Handle hFilterLiteListener  = TestEventListener::create();

            TestEventListener::Handle hTransformFilterListener      = TestEventListener::create();
            TestEventListener::Handle hTransformFilterLiteListener  = TestEventListener::create();
           
            hCache->addFilterListener(hAllListener,     AlwaysFilter::getInstance(), false);
            hCache->addFilterListener(hAllLiteListener, AlwaysFilter::getInstance(), true);

            String::View vKey1 = String::create("Key1");
            hCache->addKeyListener(hKeyListener,     vKey1, false);
            hCache->addKeyListener(hKeyLiteListener, vKey1, true);

            MapEventFilter::View vEventFilter = MapEventFilter::create(
                    MapEventFilter::e_all,
                        LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(50)));
            hCache->addFilterListener(hFilterListener,     vEventFilter, false);
            hCache->addFilterListener(hFilterLiteListener, vEventFilter, true);
 
            MapEventTransformerFilter::View vTransformerFilter =
                    MapEventTransformerFilter::create(
                            MapEventFilter::create(MapEventFilter::e_all,
                                   LessFilter::create(IdentityExtractor::getInstance(), Integer32::valueOf(50))),
                                   SemiLiteEventTransformer::getInstance());
            hCache->addFilterListener(hTransformFilterListener,     vTransformerFilter, false);
            hCache->addFilterListener(hTransformFilterLiteListener, vTransformerFilter, true);

            generateEvents(hCache, String::create("Key"), 100);
            Thread::sleep(1000);

            TS_ASSERT(100L == hAllListener->getInsertCount());
            TS_ASSERT(100L == hAllListener->getUpdateCount());
            TS_ASSERT(100L == hAllListener->getDeleteCount());

            TS_ASSERT(100L == hAllLiteListener->getInsertCount());
            TS_ASSERT(100L == hAllLiteListener->getUpdateCount());
            TS_ASSERT(100L == hAllLiteListener->getDeleteCount());

            TS_ASSERT(1L == hKeyListener->getInsertCount());
            TS_ASSERT(1L == hKeyListener->getUpdateCount());
            TS_ASSERT(1L == hKeyListener->getDeleteCount());

            TS_ASSERT(1L == hKeyLiteListener->getInsertCount());
            TS_ASSERT(1L == hKeyLiteListener->getUpdateCount());
            TS_ASSERT(1L == hKeyLiteListener->getDeleteCount());

            TS_ASSERT(50L == hFilterListener->getInsertCount());
            TS_ASSERT(50L == hFilterListener->getUpdateCount());
            TS_ASSERT(50L == hFilterListener->getDeleteCount());

            TS_ASSERT(50L == hFilterLiteListener->getInsertCount());
            TS_ASSERT(50L == hFilterLiteListener->getUpdateCount());
            TS_ASSERT(50L == hFilterLiteListener->getDeleteCount());

            TS_ASSERT(50L == hTransformFilterListener->getInsertCount());
            TS_ASSERT(50L == hTransformFilterListener->getUpdateCount());
            TS_ASSERT(50L == hTransformFilterListener->getDeleteCount());

            TS_ASSERT(50L == hTransformFilterLiteListener->getInsertCount());
            TS_ASSERT(50L == hTransformFilterLiteListener->getUpdateCount());
            TS_ASSERT(50L == hTransformFilterLiteListener->getDeleteCount());

            hCache->removeFilterListener(hAllListener,     AlwaysFilter::getInstance());
            hCache->removeFilterListener(hAllLiteListener, AlwaysFilter::getInstance());

            hCache->removeKeyListener(hKeyListener,     vKey1);
            hCache->removeKeyListener(hKeyLiteListener, vKey1);

            hCache->removeFilterListener(hFilterListener,     vEventFilter);
            hCache->removeFilterListener(hFilterLiteListener, vEventFilter);
 
            hCache->removeFilterListener(hTransformFilterListener,     vTransformerFilter);
            hCache->removeFilterListener(hTransformFilterLiteListener, vTransformerFilter);
            }

        /**
        * Test for COH-11015.
        */
        void testCoh11015()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");

            hCache->put(String::create("1"), TestObject::create(1, "AAAAA"));

            // create Listener 1 and add to cache 
            TestListener::Handle hListener1    = TestListener::create(2, String::create("COH11015; hListener1"));
            Filter::Handle       hFilter1      = EqualsFilter::create(PofExtractor::create(typeid(void), 1), 
                    String::create("AAAAA"));
            MapEventFilter::View vEventFilter1 = MapEventFilter::create(MapEventFilter::e_all, hFilter1);

            hCache->addFilterListener(hListener1, vEventFilter1, false);

            // update entry in the cache and one listener is active 
            hCache->put(String::create("1"), TestObject::create(1, "AAAAA")); 

            int64_t cTimeout = System::safeTimeMillis() + 30000L;
            while (hListener1->getUpdateCount() < 1L 
                    && cTimeout > System::safeTimeMillis())
                {
                Thread::sleep(250);
                }
            TS_ASSERT(1L == hListener1->getUpdateCount());

            // create Listener 2 and add to cache 
            TestListener::Handle hListener2    = TestListener::create(1, String::create("COH11015; hListener2"));
            Filter::Handle       hFilter2      = EqualsFilter::create(PofExtractor::create(typeid(void), 1), 
                    String::create("AAAAA"));
            MapEventFilter::View vEventFilter2 = MapEventFilter::create(MapEventFilter::e_all, hFilter2);

            hCache->addFilterListener(hListener2, vEventFilter2, false); 

            hCache->put(String::create("1"), TestObject::create(1, "AAAAA")); 

            cTimeout = System::safeTimeMillis() + 30000L;
            while (hListener1->getUpdateCount() < 2L 
                    && cTimeout > System::safeTimeMillis())
                {
                Thread::sleep(250);
                }
            TS_ASSERT(2L == hListener1->getUpdateCount());

            cTimeout = System::safeTimeMillis() + 30000L;
            while (hListener2->getUpdateCount() < 1L 
                    && cTimeout > System::safeTimeMillis())
                {
                Thread::sleep(250);
                }
            TS_ASSERT(1L == hListener2->getUpdateCount());
 
            hCache->removeFilterListener(hListener1, vEventFilter1);
            hCache->removeFilterListener(hListener2, vEventFilter2);
            }

    private:
        /**
        * Wait for event that signals cache is up, running, and returning events.
        */
        void waitForEvents(NamedCache::Handle hCache)
            {
            WaitListener::Handle hWaitListener = WaitListener::create((int64_t) 10000L, String::create("WAIT"));
            String::View vsWaitKey = String::create("WaitKey");

            hCache->addKeyListener(hWaitListener, vsWaitKey, false);
            hCache->put(vsWaitKey, Integer32::valueOf(5 + 1));

            hWaitListener->startWait();
            while (!hWaitListener->getEventReceived())
                {
                TS_ASSERT(!hWaitListener->getTimedOut());
                Thread::sleep(250);
                }
            hCache->removeKeyListener(hWaitListener, vsWaitKey);

            generateEvents(hCache, String::create("TestKey"), (uint32_t) 10);

            // Wait for event that reports events are coming back.
            hWaitListener->reset((int64_t) 10000L);

            hCache->addKeyListener(hWaitListener, vsWaitKey, false);
            hCache->put(vsWaitKey, Integer32::valueOf(5 + 1));

            hWaitListener->startWait();
            while (!hWaitListener->getEventReceived())
                {
                TS_ASSERT(!hWaitListener->getTimedOut());
                Thread::sleep(250);
                }
            hCache->removeKeyListener(hWaitListener, vsWaitKey);
            }

        /**
        * Generate the data that will cause events to be returned.
        */
        void generateEvents(NamedCache::Handle hCache, String::View vBaseKey, uint32_t cNumber)
            {
            // insert events
            for (uint32_t i = 0; i < cNumber; i++)
                {
                String::View vsKey = COH_TO_STRING(vBaseKey << i);
                hCache->put(vsKey, Integer32::valueOf(i));
                }

            // update events
            for (uint32_t i = 0; i < cNumber; i++)
                {
                String::View vsKey = COH_TO_STRING(vBaseKey << i);
                hCache->put(vsKey, Integer32::valueOf(i));
                }

            // delete events
            for (uint32_t i = 0; i < cNumber; i++)
                {
                String::View vsKey = COH_TO_STRING(vBaseKey << i);
                hCache->remove(vsKey);
                }
            }
    };
