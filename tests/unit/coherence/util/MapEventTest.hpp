/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "mock/CommonMocks.hpp"

#include "coherence/util/Listeners.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/ObservableMap.hpp"

#include <sstream>

using namespace coherence::util;
using namespace mock;
using namespace std;


/**
* MapEvent test suite.
*/

class MapEventTest : public CxxTest::TestSuite
    {
    public:

        void testGetMap()
            {
            ObservableMap::Handle hMap = TestObservableMap::create();
            MapEvent::View vEvt = MapEvent::create(hMap, 0, String::create("key"),
                    String::create("old"), String::create("new"));
            TS_ASSERT(hMap == vEvt->getMap());
            }

        void testGetId()
            {
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), String::create("old"),
                    String::create("new"));
            TS_ASSERT(1 == vEvt->getId());
            }

        void testGetKey()
            {
            String::View vsKey = String::create("key");
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    1, vsKey, String::create("old"), String::create("new"));
            TS_ASSERT(vsKey == vEvt->getKey());
            }

        void testGetOldValue()
            {
            String::View vsOld = String::create("old");
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), vsOld, String::create("new"));
            TS_ASSERT(vsOld == vEvt->getOldValue());
            }

        void testGetNewValue()
            {
            String::View vsNew = String::create("new");
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), String::create("old"), vsNew);
            TS_ASSERT(vsNew == vEvt->getNewValue());
            }

        void testDispatch()
            {
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    MapEvent::entry_inserted, String::create("key"),
                    String::create("old"), String::create("new"));

            Listeners::Handle hListeners = Listeners::create();
            MockMapListener::Handle hListener1 = MockMapListener::create();
            MockMapListener::Handle hListener2 = MockMapListener::create();
            hListener1->setStrict(true);
            hListener2->setStrict(true);
            // set expectations
            hListener1->entryInserted(vEvt);
            hListener2->entryInserted(vEvt);
            hListener1->replay();
            hListener2->replay();

            hListeners->add(hListener1);
            hListeners->add(hListener2);
            // method being tested
            vEvt->dispatch((Listeners::View)hListeners);
            hListener1->verify();
            hListener2->verify();
            }

        void testDispatchListerThrowsException()
            {
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    MapEvent::entry_updated, String::create("key"),
                    String::create("old"), String::create("new"));

            Listeners::Handle hListeners = Listeners::create();
            MockMapListener::Handle hListener1 = MockMapListener::create();
            MockMapListener::Handle hListener2 = MockMapListener::create();
            hListener1->setStrict(true);
            hListener2->setStrict(true);
            // set expectations
            hListener2->entryUpdated(vEvt);
            hListener2->lastExpectation()->setException(Exception::create("TEST EXCEPTION"));
            hListener1->replay();
            hListener2->replay();

            hListeners->add(hListener1);
            hListeners->add(hListener2);
            // method being tested
            TS_ASSERT_THROWS(vEvt->dispatch((Listeners::View)hListeners), Exception::View);
            hListener1->verify();
            hListener2->verify();
            }

        void testDispatchStrictListerThrowsException()
            {
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    MapEvent::entry_updated, String::create("key"),
                    String::create("old"), String::create("new"));

            Listeners::Handle hListeners = Listeners::create();
            MockMapListener::Handle hListener1 = MockMapListener::create();
            MockMapListener::Handle hListener2 = MockMapListener::create();
            hListener1->setStrict(true);
            hListener2->setStrict(true);
            // set expectations
            hListener2->entryUpdated(vEvt);
            hListener2->lastExpectation()->setException(Exception::create("TEST EXCEPTION"));
            hListener1->replay();
            hListener2->replay();

            hListeners->add(hListener1);
            hListeners->add(hListener2);
            // method being tested
            TS_ASSERT_THROWS(vEvt->dispatch((Listeners::View)hListeners, true), Exception::View);
            hListener1->verify();
            hListener2->verify();
            }

        //TODO: Assert msg was logged
        void testDispatchNonStrictListerThrowsException()
            {
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    MapEvent::entry_deleted, String::create("key"),
                    String::create("old"), String::create("new"));

            Listeners::Handle hListeners = Listeners::create();
            MockMapListener::Handle hListener1 = MockMapListener::create();
            MockMapListener::Handle hListener2 = MockMapListener::create();
            hListener1->setStrict(true);
            hListener2->setStrict(true);
            // set expectations
            hListener2->entryDeleted(vEvt);
            hListener2->lastExpectation()->setException(Exception::create("EXPECTED TEST EXCEPTION"));
            hListener1->entryDeleted(vEvt);
            hListener1->replay();
            hListener2->replay();

            hListeners->add(hListener1);
            hListeners->add(hListener2);

            // intercept std::cerr
            streambuf*     pOrigCerr   = cerr.rdbuf();
            ostringstream* pTestStream = new ostringstream;
            streambuf*     pNewCerr    = pTestStream->rdbuf();
            cerr.rdbuf(pNewCerr);

            // delete allocated cerr replacement
            struct Finally
                {
                Finally(ostringstream* pTestStr, streambuf* pOrig)
                    {
                    m_pTestStr = pTestStr;
                    m_pOrig    = pOrig;
                    }

                ~Finally()
                    {
                    delete m_pTestStr;
                    cerr.rdbuf(m_pOrig);
                    }
                ostringstream* m_pTestStr;
                streambuf* m_pOrig;

                } finally(pTestStream, pOrigCerr);


            // method being tested
            vEvt->dispatch((Listeners::View)hListeners, false);
            hListener1->verify();
            hListener2->verify();
            }

        void testDispatchNullListeners()
            {
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), String::create("old"),
                    String::create("new"));
            // ensure no exception is thrown
            vEvt->dispatch((Listeners::View)NULL);
            }

        void testStaticGetDescription()
            {
            TS_ASSERT(MapEvent::getDescription(
                    MapEvent::entry_inserted)->equals("inserted"));
            TS_ASSERT(MapEvent::getDescription(
                    MapEvent::entry_updated)->equals("updated"));
            TS_ASSERT(MapEvent::getDescription(
                    MapEvent::entry_deleted)->equals("deleted"));
            TS_ASSERT(MapEvent::getDescription(100)->equals("<unknown: 100>"));
            }

        void testGetDescription()
            {
            MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                    MapEvent::entry_inserted, String::create("key"),
                    String::create("old"), String::create("new"));

            TS_ASSERT(vEvt->getDescription()->indexOf(" added: key=key, value=new") != String::npos);

            vEvt = MapEvent::create(TestObservableMap::create(),
                    MapEvent::entry_updated, String::create("key"),
                    String::create("old"), String::create("new"));

            TS_ASSERT(vEvt->getDescription()->indexOf(" updated: key=key, old value=old, new value=new") != String::npos);

            vEvt = MapEvent::create(TestObservableMap::create(),
                    MapEvent::entry_deleted, String::create("key"),
                    String::create("old"), String::create("new"));

            TS_ASSERT(vEvt->getDescription()->indexOf(" deleted: key=key, value=old") != String::npos);

            vEvt = MapEvent::create(TestObservableMap::create(),
                    100, String::create("key"), String::create("old"),
                    String::create("new"));

            TS_ASSERT_THROWS(vEvt->getDescription(), IllegalStateException::View);
            }

        //TODO
        void testToString()
            {
                MapEvent::View vEvt = MapEvent::create(TestObservableMap::create(),
                        MapEvent::entry_inserted, String::create("key"),
                        String::create("old"), String::create("new"));


                vEvt->toString();
                //String::View vs = "coherence::util::MapEvent{`anonymous namespace'::TestObservableMap added: key=key, value=new}";
                //TS_ASSERT(vs->equals(ss.str().c_str()));
            }

    };
