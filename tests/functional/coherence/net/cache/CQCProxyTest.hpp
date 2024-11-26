/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/Invocable.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/MemberEvent.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include "coherence/tests/ProxyControlInvocable.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MultiplexingMapListener.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"

#include "common/TestUtils.hpp"

#include "private/coherence/util/logging/Logger.hpp"

using namespace coherence::lang;

using coherence::net::CacheFactory;
using coherence::net::Invocable;
using coherence::net::InvocationService;
using coherence::net::MemberEvent;
using coherence::net::MemberListener;
using coherence::net::NamedCache;
using coherence::net::cache::ContinuousQueryCache;
using coherence::tests::ProxyControlInvocable;
using coherence::util::HashMap;
using coherence::util::Map;
using coherence::util::MapEvent;
using coherence::util::MultiplexingMapListener;
using coherence::util::filter::AlwaysFilter;
using coherence::util::logging::Logger;

/**
* Coherence*Extend test for the ContinuousQueryCache receiving events
* after restarting the proxy server.
* Tests COH-8145 (Bug14768607)
* Tests COH-8470 (Bug15966691)
*
* @author par  2012.11.5
*/
class CQCProxyTest : public CxxTest::TestSuite
    {
    /**
    * MapListener that continuously receives events from the cache.
    */
    class TestCQCListener : public class_spec<TestCQCListener,
            extends<MultiplexingMapListener> >
        {
        friend class factory<TestCQCListener>;

        public:
            TestCQCListener(int32_t count)
                    : m_cCount(count)
                {
                }

        // ----- MultiplexingMapListener interface ---------------------------

        public:
            /**
            * @inheritdoc
            */
            virtual void onMapEvent(MapEvent::View /* vEvent */)
                {
                ++m_cEvents;
                checkTotals();
                }

        // ----- TestCQCListener methods -------------------------------------

        public:
            /**
            * Total number of events listener actually received.
            *
            * @return  number of event received
            */
            int getActualTotal()
                {
                return m_cEvents;
                }

            /**
            * Reset the number of events received.
            */
            void resetActualTotal()
                {
                m_cEvents = 0;
                }

        private:
            /**
             * If total events has been reached notify all waiters.
             */
            void checkTotals()
                {
                if (m_cEvents == m_cCount)
                    {
                    COH_SYNCHRONIZED(this)
                        {
                        this->notifyAll();
                        }
                    }
                }

        // ----- data members ------------------------------------------------

        private:
            /**
            * Number of events listener should receive
            */
            const int32_t m_cCount;

            /**
            * Number of events received
            */
            int32_t m_cEvents;
        };

    /**
    * MemberListener that receives memberLeft event from the cache
    * when the proxy has stopped.
    */
    class CQCMemberListener : public class_spec<CQCMemberListener,
            extends<Object>, implements<MemberListener> >
        {
        friend class factory<CQCMemberListener>;

        public:
            CQCMemberListener()
                {
                }

            void memberJoined(MemberEvent::View /* evt */)
                {
                }

            void memberLeaving(MemberEvent::View /* evt */)
                {
                }

            void memberLeft(MemberEvent::View  /* evt */)
                {
                m_fEventReceived = true;
                COH_SYNCHRONIZED(this)
                    {
                    this->notifyAll();
                    }
                }

            bool isEventReceived()
                {
                return m_fEventReceived;
                }

        //----- data members ----------------------------

        private:
            /**
            * memberLeft event received
            */
            bool m_fEventReceived;
        };

    /**
    * Test the behavior of proxy returning events.
    * COH-8145 reports the CQC doesn't receive events after
    * the proxy is restarted.
    * COH-8470 reports that the CQC resynchronizes multiple
    * times, giving double or triple events.
    *
    * Put a known number of data items into the inner cache,
    * then count the number of events the listener receives
    * after restarting the proxy.
    */
    public:
        void testEvents()
            {
            int cEventCount = 100;

            // start the ProxyService on just one cluster node
            controlProxy(ProxyControlInvocable::start_operation);

            // put data items into inner cache to generate events
            NamedCache::Handle hTestCache = ensureCleanCache("dist-proxy-stop-test");

            String::View vBaseKey   = "TestKey";
            Map::Handle  hPutAllMap = HashMap::create();
            for (int32_t i = 0; i < cEventCount; i++)
                {
                String::View vsKey = COH_TO_STRING(vBaseKey << i);
                hPutAllMap->put(vsKey, Integer32::valueOf(i));
                }
            hTestCache->putAll(hPutAllMap);

            // create listener for CQC
            TestCQCListener::Handle hListener = TestCQCListener::create(cEventCount);

            // instantiate the CQC, will start the test running.
            ContinuousQueryCache::Handle hCQC =
                    ContinuousQueryCache::create(hTestCache, AlwaysFilter::create(), true, hListener);

            // add member listener to inner cache to receive memberLeft
            // event; intermittently, the "get" which restarts the
            // cache happens before the CQC receives the memberLeft,
            // the CQC isn't reinitialized, so the events are not re-sent.
            // adding this listener depends on the ordering of the
            // listeners for the cache service, we want this one last.
            // therefore, adding after CQC had been instantiated
            CQCMemberListener::Handle hMemListener = CQCMemberListener::create();
            hTestCache->getCacheService()->addMemberListener(hMemListener);

            // allow test time to receive the events.
            try
                {
                COH_TIMEOUT_AFTER(30000)
                    {
                    COH_SYNCHRONIZED(hListener)
                        {
                        while (hListener->getActualTotal() < cEventCount)
                            {
                            hListener->wait();
                            }
                        }
                    }
                }
            catch (InterruptedException::View ve)
                {
                COH_THROW_STREAM(InterruptedException, "Timeout waiting for events to reach "
                        << cEventCount << ". Received count: " << hListener->getActualTotal()
                        << ".\n" << ve->getMessage());
                }

            // check for additional events - there shouldn't be any
            Thread::sleep(250);
            TS_ASSERT_EQUALS(hListener->getActualTotal(), cEventCount);

            // reset event counting.
            hListener->resetActualTotal();

            // stop proxy
            controlProxy(ProxyControlInvocable::stop_operation);

            // three conditions to check for to indicate that the CQC considers itself disconnected:
            // 1. the test's MemberListener received the MemberLeft event
            // 2. CQC->isActive() == false - indicating underlying cache is disconnected
            // 3. CQC->getState() == state_disconnected - indicating CQC received MemberLeft event
            try
                {
                COH_TIMEOUT_AFTER(30000)
                    {
                    while (!hMemListener->isEventReceived() ||
                            hCQC->isActive() ||
                            hCQC->getState() != ContinuousQueryCache::state_disconnected)
                        {
                        if (!hMemListener->isEventReceived())
                            {
                            COH_SYNCHRONIZED(hMemListener)
                                {
                                if (!hMemListener->isEventReceived())
                                    {
                                    hMemListener->wait();
                                    }
                                }
                            }
                        else
                            {
                            // underlying cache is still active, or CQC has not yet received MemberLeft event
                            COH_LOG("hCQC->isActive()=" << hCQC->isActive(), Logger::level_info);
                            COH_LOG("hCQC->getState()=" << hCQC->getState(), Logger::level_info);
                            Thread::sleep(10);
                            }
                        }
                    }
                }
            catch(InterruptedException::View ve)
                {
                COH_THROW_STREAM(InterruptedException, "Timeout waiting for the MemberLeft event.\n"
                        << ve->getMessage());
                }
            TS_ASSERT(hMemListener->isEventReceived());
            TS_ASSERT(!hCQC->isActive());
            TS_ASSERT_EQUALS(hCQC->getState(), ContinuousQueryCache::state_disconnected);

            // start proxy
            controlProxy(ProxyControlInvocable::start_operation);

            // get junk here to restart the CQC
            hCQC->get(String::create("junkstuff"));

            TS_ASSERT(hCQC->isActive());
            TS_ASSERT_EQUALS(hCQC->getState(), ContinuousQueryCache::state_synchronized);

            // Allow test time to receive events again.
            try
                {
                COH_TIMEOUT_AFTER(30000)
                    {
                    COH_SYNCHRONIZED(hListener)
                        {
                        while (hListener->getActualTotal() < cEventCount)
                            {
                            hListener->wait();
                            }
                        }
                    }
                }
            catch(InterruptedException::View ve)
                {
                COH_THROW_STREAM(InterruptedException, "Timeout waiting for events to reach "
                        << cEventCount << ". Received count: " << hListener->getActualTotal()
                        << ".\n" << ve->getMessage());
                }

            // check for additional events - there shouldn't be any
            Thread::sleep(250);
            TS_ASSERT_EQUALS(hListener->getActualTotal(), cEventCount);

            hTestCache->getCacheService()->removeMemberListener(hMemListener);
            hCQC->release();

            // stop the proxy
            controlProxy(ProxyControlInvocable::stop_operation);
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }

    private:
        /**
         * utility method to control the proxy.
         */
        void controlProxy(int32_t nOperation)
            {
            InvocationService::Handle hInvocationService  = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpProxyControlService"));

            Invocable::Handle hInvocable = ProxyControlInvocable::create("ExtendTcpProxyServiceCOH8230",
                    nOperation);
            hInvocationService->query(hInvocable, NULL);
            }
    };
