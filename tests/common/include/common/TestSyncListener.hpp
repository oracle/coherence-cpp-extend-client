/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_SYNC_LISTENER_HPP
#define COH_TEST_SYNC_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListenerSupport.hpp"

using coherence::util::MapEvent;
using coherence::util::MapListenerSupport;

/**
* Test synchronous listener class that stores an event
*/
class TestSyncListener
    : public class_spec<TestSyncListener,
        extends<Object>,
        implements<MapListenerSupport::SynchronousListener> >
    {
    friend class factory<TestSyncListener>;

    protected:
        TestSyncListener()
            : m_vEvent(self())
            {
            }

    public:
        virtual void entryInserted(MapEvent::View vEvent)
            {
            m_vEvent = vEvent;
            }

        virtual void entryUpdated(MapEvent::View vEvent)
            {
            m_vEvent = vEvent;
            }

        virtual void entryDeleted(MapEvent::View vEvent)
            {
            m_vEvent = vEvent;
            }

        MapEvent::View getEvent()
            {
            return m_vEvent;
            }

        void setEvent(MapEvent::View vEvent)
            {
            m_vEvent = vEvent;
            }

    MemberView<MapEvent> m_vEvent;
    };

#endif
