/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "common/TestUtils.hpp"
#include "mock/CommonMocks.hpp"

#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

#include "coherence/util/ObservableMap.hpp"

using coherence::net::NamedCache;
using coherence::util::ArrayList;
using coherence::util::Filter;
using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::ObservableMap;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::NeverFilter;
using namespace mock;
using namespace std;

namespace
    {
    bool matchListenerEvent(ArrayList::View vExpected, ArrayList::View vActual)
        {
        MapEvent::View vExpectedEvent = cast<MapEvent::View>(vExpected->get(0));
        MapEvent::View vActualEvent   = cast<MapEvent::View>(vActual->get(0));

//      cout << "\n\n\n************************************************************\n\n\n";
//      cout << vExpectedEvent->getKey() << " : " << vActualEvent->getKey() << endl;
//      cout << vExpectedEvent->getId() << " : " << vActualEvent->getId() << endl;
//      cout << vExpectedEvent->getOldValue() << " : " << vActualEvent->getOldValue() << endl;
//      cout << vExpectedEvent->getNewValue() << " : " << vActualEvent->getNewValue() << endl;
//      cout << vExpectedEvent->getMap() << " : " << vActualEvent->getMap() << endl;

        return Object::equals(vExpectedEvent->getKey(), vActualEvent->getKey())           &&
               vExpectedEvent->getId() == vActualEvent->getId()                           &&
               Object::equals(vExpectedEvent->getOldValue(), vActualEvent->getOldValue()) &&
               Object::equals(vExpectedEvent->getNewValue(), vActualEvent->getNewValue()) &&
               vExpectedEvent->getMap() == vActualEvent->getMap();
        }
    }



class ObservableMapTest : public CxxTest::TestSuite
    {
    public:

        void testKeyListener()
        {
            NamedCache::Handle hCache = ensureCleanCache("dist-observable-map");
            // the mock which receives the events
            MockMapListener::Handle hMockListener = MockMapListener::create();
            // the mock used in verifying the MapEvent which is passed to the listener
            MockMapEvent::Handle hMockMapEvent = MockMapEvent::create();
            // map key
            Object::View vKey = String::create("key");
            // values
            String::View vsInsertedVal = String::create("inserted-val");
            String::View vsUpdatedVal  = String::create("updated-val");

            // set mock listener expectations
            hMockListener->setStrict(true);
            hMockListener->entryInserted(hMockMapEvent);
            // use an argument matcher to assert MapEvent state
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
            hMockMapEvent->setObjectReturn(vsInsertedVal);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            // update
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_updated);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vsInsertedVal);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(vsUpdatedVal);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            //delete
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_deleted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vsUpdatedVal);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            hMockMapEvent->replay();

            hCache->addKeyListener(hMockListener, vKey, false);
            hCache->put(vKey, vsInsertedVal);
            hCache->put(vKey, vsUpdatedVal);
            hCache->remove(vKey);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hMockListener->verifyAndReturnResult();
                Thread::sleep(5);
                }

            // remove the listener and ensure that further events aren't received
            hCache->removeKeyListener(hMockListener, vKey);
            hCache->put(vKey, vsInsertedVal);
            hCache->put(vKey, vsUpdatedVal);
            hCache->remove(vKey);

            hMockListener->verify();
        }

        void testFilterListenerNullFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-observable-map");
            // the mock which receives the events
            MockMapListener::Handle hMockListener = MockMapListener::create();
            // the mock used in verifying the MapEvent which is passed to the listener
            MockMapEvent::Handle hMockMapEvent = MockMapEvent::create();
            // map key
            Object::View vKey = String::create("key");
            // values
            String::View vsInsertedVal = String::create("inserted-val");
            String::View vsUpdatedVal  = String::create("updated-val");

            // set mock listener expectations
            hMockListener->setStrict(true);
            hMockListener->entryInserted(hMockMapEvent);
            // use an argument matcher to assert MapEvent state
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
            hMockMapEvent->setObjectReturn(vsInsertedVal);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            // update
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_updated);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vsInsertedVal);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(vsUpdatedVal);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            //delete
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_deleted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vsUpdatedVal);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            hMockMapEvent->replay();

            hCache->addFilterListener(hMockListener);
            hCache->put(vKey, vsInsertedVal);
            hCache->put(vKey, vsUpdatedVal);
            hCache->remove(vKey);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hMockListener->verifyAndReturnResult();
                Thread::sleep(5);
                }

            // remove the listener and ensure that further events aren't received
            hCache->removeFilterListener(hMockListener);
            hCache->put(vKey, vsInsertedVal);
            hCache->put(vKey, vsUpdatedVal);
            hCache->remove(vKey);

            hMockListener->verify();
            }

        void testFilterListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-observable-map");
            // the mock which receives the events
            MockMapListener::Handle hMockListener = MockMapListener::create();
            // the mock which shouldn't receive any events due to NeverFilter
            MockMapListener::Handle hMockNeverListener = MockMapListener::create();
            // the mock used in verifying the MapEvent which is passed to the listener
            MockMapEvent::Handle hMockMapEvent = MockMapEvent::create();
            // filter always evaluates to true
            Filter::View vAlwaysFilter = AlwaysFilter::create();
            // filter always evaluates to false
            Filter::View vNeverFilter = NeverFilter::create();
            // map key
            Object::View vKey = String::create("key");
            // values
            String::View vsInsertedVal = String::create("inserted-val");
            String::View vsUpdatedVal  = String::create("updated-val");

            // set mock listener expectations
            hMockListener->setStrict(true);
            hMockListener->entryInserted(hMockMapEvent);
            // use an argument matcher to assert MapEvent state
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryUpdated(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->entryDeleted(hMockMapEvent);
            hMockListener->setMatcher(&matchListenerEvent);
            hMockListener->replay();

            // mock listener with NeverFilter
            hMockNeverListener->setStrict(true);
            hMockNeverListener->replay();

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
            hMockMapEvent->setObjectReturn(vsInsertedVal);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            // update
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_updated);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vsInsertedVal);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(vsUpdatedVal);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            //delete
            hMockMapEvent->getKey();
            hMockMapEvent->setObjectReturn(vKey);
            hMockMapEvent->getId();
            hMockMapEvent->setInt32Return(MapEvent::entry_deleted);
            hMockMapEvent->getOldValue();
            hMockMapEvent->setObjectReturn(vsUpdatedVal);
            hMockMapEvent->getNewValue();
            hMockMapEvent->setObjectReturn(NULL);
            hMockMapEvent->getMap();
            hMockMapEvent->setObjectReturn(hCache);
            hMockMapEvent->replay();

            hCache->addFilterListener(hMockListener, vAlwaysFilter);
            hCache->addFilterListener(hMockNeverListener, vNeverFilter);
            hCache->put(vKey, vsInsertedVal);
            hCache->put(vKey, vsUpdatedVal);
            hCache->remove(vKey);

            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = hMockListener->verifyAndReturnResult();
                Thread::sleep(5);
                }

            // remove the listener and ensure that further events aren't received
            hCache->removeFilterListener(hMockListener, vAlwaysFilter);
            hCache->put(vKey, vsInsertedVal);
            hCache->put(vKey, vsUpdatedVal);
            hCache->remove(vKey);

            hMockListener->verify();
            }
    };

