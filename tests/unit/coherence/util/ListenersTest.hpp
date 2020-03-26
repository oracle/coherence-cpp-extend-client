/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/util/EventListener.hpp"
#include "coherence/util/Listeners.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::util::EventListener;
using coherence::util::Listeners;

COH_OPEN_NAMESPACE_ANON(ListenersTest)

class TestEventListener
    : public class_spec<TestEventListener,
        extends<Object>,
        implements<EventListener> >
    {
    friend class factory<TestEventListener>;

    protected:
        TestEventListener()
            {
            }
    };

class TestSyncEventListener
    : public class_spec<TestSyncEventListener,
        extends<Object>,
        implements<SynchronousListener, EventListener> >
    {
    friend class factory<TestSyncEventListener>;

    protected:
        TestSyncEventListener()
            {
            }
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test suite for the Listeners class.
*/
class ListenersTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the add(EventListener::Handle hListener) method.
        */
        void testAdd()
            {
            Listeners::Handle hListeners = Listeners::create();
            TS_ASSERT(hListeners->isEmpty());

            ObjectArray::View va = hListeners->listeners();
            TS_ASSERT(va->length == 0);

            TestEventListener::Handle hListener = TestEventListener::create();
            hListeners->add(hListener);
            TS_ASSERT(!hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener);
            }

        /**
        * Test the remove(EventListener::Handle hListener) method.
        */
        void testRemove()
            {
            Listeners::Handle hListeners = Listeners::create();
            TS_ASSERT(hListeners->isEmpty());

            TestEventListener::Handle hListener = TestEventListener::create();
            hListeners->remove(hListener);
            TS_ASSERT(hListeners->isEmpty());
            TS_ASSERT(!hListeners->contains(hListener));

            hListeners->add(hListener);
            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(hListeners->contains(hListener));

            hListeners->remove(hListener);
            TS_ASSERT(hListeners->isEmpty());
            TS_ASSERT(!hListeners->contains(hListener));

            TestEventListener::Handle hListener2 = TestEventListener::create();
            TestEventListener::Handle hListener3 = TestEventListener::create();
            TestEventListener::Handle hListener4 = TestEventListener::create();
            hListeners->add(hListener);
            hListeners->add(hListener2);
            hListeners->add(hListener3);
            hListeners->add(hListener4);

            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(hListeners->contains(hListener));
            TS_ASSERT(hListeners->contains(hListener2));
            TS_ASSERT(hListeners->contains(hListener3));
            TS_ASSERT(hListeners->contains(hListener4));

            // remove from the middle
            hListeners->remove(hListener2);
            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(hListeners->contains(hListener));
            TS_ASSERT(!hListeners->contains(hListener2));
            TS_ASSERT(hListeners->contains(hListener3));
            TS_ASSERT(hListeners->contains(hListener4));

            ObjectArray::View va = hListeners->listeners();
            TS_ASSERT(va->length == 3);
            TS_ASSERT(va[0] == hListener);
            TS_ASSERT(va[1] == hListener3);
            TS_ASSERT(va[2] == hListener4);

            // remove from the end
            hListeners->remove(hListener4);
            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(hListeners->contains(hListener));
            TS_ASSERT(hListeners->contains(hListener3));
            TS_ASSERT(!hListeners->contains(hListener4));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 2);
            TS_ASSERT(va[0] == hListener);
            TS_ASSERT(va[1] == hListener3);

            // remove from the front
            hListeners->remove(hListener);
            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(!hListeners->contains(hListener));
            TS_ASSERT(hListeners->contains(hListener3));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener3);

            // remove only element
            hListeners->remove(hListener3);
            TS_ASSERT(hListeners->isEmpty());
            TS_ASSERT(!hListeners->contains(hListener3));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 0);

            TestSyncEventListener::Handle hSyncListener  = TestSyncEventListener::create();
            TestSyncEventListener::Handle hSyncListener2 = TestSyncEventListener::create();
            hListeners->add(hSyncListener);
            hListeners->add(hSyncListener2);

            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(hListeners->contains(hSyncListener));
            TS_ASSERT(hListeners->contains(hSyncListener2));

            hListeners->remove(hSyncListener);
            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(!hListeners->contains(hSyncListener));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hSyncListener2);

            hListeners->remove(hSyncListener2);
            TS_ASSERT(hListeners->isEmpty());
            TS_ASSERT(!hListeners->contains(hSyncListener2));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 0);
            }

        /**
        * Test the addAll(Listeners::View vListeners) method.
        */
        void testAddAll()
            {
            Listeners::Handle hListeners = Listeners::create();
            TS_ASSERT(hListeners->isEmpty());

            Listeners::Handle hListeners2 = Listeners::create();
            TS_ASSERT(hListeners2->isEmpty());

            hListeners->addAll(hListeners2);
            TS_ASSERT(hListeners->isEmpty());

            TestEventListener::Handle hListener = TestEventListener::create();
            hListeners->add(hListener);
            TS_ASSERT(!hListeners->isEmpty());

            ObjectArray::View va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener);

            hListeners->addAll(hListeners2);
            TS_ASSERT(!hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener);

            hListeners->remove(hListener);
            TS_ASSERT(hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 0);

            hListeners2->add(hListener);
            TS_ASSERT(!hListeners2->isEmpty());

            va = hListeners2->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener);

            hListeners->addAll(hListeners2);
            TS_ASSERT(!hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener);

            hListeners->addAll(hListeners2);
            TS_ASSERT(!hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener);

            TestEventListener::Handle hListener2 = TestEventListener::create();
            hListeners2->add(hListener2);
            TS_ASSERT(!hListeners2->isEmpty());
            TS_ASSERT(hListeners2->contains(hListener));
            TS_ASSERT(hListeners2->contains(hListener2));

            va = hListeners2->listeners();
            TS_ASSERT(va->length == 2);

            hListeners->addAll(hListeners2);
            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(hListeners->contains(hListener));
            TS_ASSERT(hListeners->contains(hListener2));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 2);

            TestSyncEventListener::Handle hSyncListener  = TestSyncEventListener::create();
            hListeners->add(hSyncListener);
            TS_ASSERT(hListeners->contains(hSyncListener));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 3);

            Listeners::Handle hListeners3 = Listeners::create();
            TS_ASSERT(hListeners3->isEmpty());

            TestSyncEventListener::Handle hSyncListener2 = TestSyncEventListener::create();
            hListeners3->add(hSyncListener);
            hListeners3->add(hSyncListener2);
            TS_ASSERT(!hListeners3->isEmpty());
            TS_ASSERT(hListeners3->contains(hSyncListener));
            TS_ASSERT(hListeners3->contains(hSyncListener2));

            va = hListeners3->listeners();
            TS_ASSERT(va->length == 2);

            hListeners->addAll(hListeners3);
            TS_ASSERT(!hListeners->isEmpty());
            TS_ASSERT(hListeners->contains(hSyncListener));
            TS_ASSERT(hListeners->contains(hSyncListener2));

            va = hListeners->listeners();
            TS_ASSERT(va->length == 4);
            }

        /**
        * Test the removeAll() method.
        */
        void testRemoveAll()
            {
            Listeners::Handle hListeners = Listeners::create();
            TS_ASSERT(hListeners->isEmpty());

            hListeners->removeAll();
            TS_ASSERT(hListeners->isEmpty());

            ObjectArray::View va = hListeners->listeners();
            TS_ASSERT(va->length == 0);

            TestEventListener::Handle hListener = TestEventListener::create();
            hListeners->add(hListener);
            TS_ASSERT(!hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 1);
            TS_ASSERT(va[0] == hListener);

            hListeners->removeAll();
            TS_ASSERT(hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 0);

            TestSyncEventListener::Handle hSyncListener = TestSyncEventListener::create();
            hListeners->add(hListener);
            hListeners->add(hSyncListener);
            TS_ASSERT(!hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 2);

            hListeners->removeAll();
            TS_ASSERT(hListeners->isEmpty());

            va = hListeners->listeners();
            TS_ASSERT(va->length == 0);
            }
    };

