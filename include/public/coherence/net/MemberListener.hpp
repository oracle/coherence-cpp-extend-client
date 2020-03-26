/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MEMBER_LISTENER_HPP
#define COH_MEMBER_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/net/MemberEvent.hpp"
#include "coherence/util/EventListener.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::util::EventListener;


/**
* The listener interface for receiving MemberEvents.
*
* @author jh  2007.12.20
*
* @see MemberEvent
* @see Service
*/
class COH_EXPORT MemberListener
    : public interface_spec<MemberListener,
        implements<EventListener> >
    {
    // ----- MemberListener interface ---------------------------------------

    public:
        /**
        * Invoked when a Member has joined the service.
        *
        * Note: this event could be called during the service restart on the
        * local node ({@link MemberEvent#isLocal() evt.isLocal()}) in which
        * case the listener's code should not attempt to use any clustered
        * cache or service functionality.
        *
        * The most critical situation arises when a number of threads are
        * waiting for a local service restart, being blocked by a Service
        * object synchronization monitor. Since the Joined event should be
        * fired only once, it is called on an event dispatcher thread
        * <b>while holding a synchronization monitor</b>. An attempt to use
        * other clustered service functionality during this local event
        * notification may result in a deadlock.
        *
        * @param vEvt  the MemberEvent::member_joined event
        */
        virtual void memberJoined(MemberEvent::View vEvt) = 0;

        /**
        * Invoked when a Member is leaving the service.
        *
        * @param vEvt  the MemberEvent::member_leaving event
        */
        virtual void memberLeaving(MemberEvent::View vEvt) = 0;

        /**
        * Invoked when a Member has left the service.
        *
        * Note: this event could be called during the service restart on the
        * local node ({@link MemberEvent#isLocal() evt.isLocal()}) in which
        * case the listener's code should not attempt to use any clustered
        * cache or service functionality.
        *
        * @param vEvt  the MemberEvent::member_joined event
        */
        virtual void memberLeft(MemberEvent::View vEvt) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MEMBER_LISTENER_HPP
