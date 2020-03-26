/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MEMBER_EVENT_HPP
#define COH_MEMBER_EVENT_HPP

#include "coherence/lang.ns"

#include "coherence/net/Member.hpp"
#include "coherence/util/Event.hpp"
#include "coherence/util/Listeners.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::util::Event;
using coherence::util::Listeners;

class Service;


/**
* An event which indicates that membership has changed:
* <ul>
* <li>a Member has joined
* <li>a Member is leaving
* <li>a Member has left
* </ul>
* A MemberEvent object is sent as an argument to the MemberListener
* interface methods.
*
* @see MemberListener
*
* @author jh  2007.12.20
*/
class COH_EXPORT MemberEvent
    : public class_spec<MemberEvent,
        extends<Event> >
    {
    friend class factory<MemberEvent>;

    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Service Handle definition.
        */
        typedef TypedHandle<Service> ServiceHandle;

        /**
        * Service View definition.
        */
        typedef TypedHandle<const Service> ServiceView;


    // ----- Id definition --------------------------------------------------

    public:
        typedef enum
            {
            member_joined  = 1, // the Member has joined
            member_leaving = 2, // the Member is leaving
            member_left    = 3  // the Member has left
            } Id;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new MemberEvent instance.
        *
        * @param vService  the Service that fired the event
        * @param identity  the event's ID, one of the Id enum values
        * @param vMember   the Member associated with the new MemberEvent
        */
        MemberEvent(ServiceView vService, Id identity, Member::View vMember);


    // ----- MemberEvent interface ------------------------------------------

    public:
        /**
        * Dispatch this event to the specified listeners collection.
        *
        * @param vListeners the listeners collection
        *
        * @throws ClassCastException if any of the targets is not an instance of
        *         the MemberListener interface
        */
        virtual void dispatch(Listeners::View vListeners) const;

        /**
        * Check whether a Member object for this event represents the local
        * member of the cluster.
        *
        * @return true iff the event's Member object represents the local
        *              cluster member
        */
        virtual bool isLocal() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return this event's ID.
        *
        * @return the event ID, one of the Id enum values
        */
        virtual Id getId() const;

        /**
        * Return the Service that fired the event.
        *
        * @return the Service
        */
        virtual ServiceView getService() const;

        /**
        * Return the Member associated with this event.
        *
        * @return the Member
        */
        virtual Member::View getMember() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The event's ID.
        */
        Id m_id;

        /**
        * The Member associated with this event.
        */
        FinalView<Member> f_vMember;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MEMBER_EVENT_HPP
