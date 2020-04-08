/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_LISTENER_HPP
#define COH_TEST_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"

using coherence::util::MapEvent;
using coherence::util::MapListener;

/**
* Test listener class that stores an event
*/
class TestListener
    : public class_spec<TestListener,
        extends<Object>,
        implements<MapListener> >
    {
    friend class factory<TestListener>;

    protected:
        TestListener()
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
