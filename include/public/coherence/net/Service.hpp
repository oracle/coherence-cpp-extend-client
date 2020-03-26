/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NET_SERVICE_HPP
#define COH_NET_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/util/Service.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::Serializer;


/**
* This Service interface represents a controllable service that operates in a
* clustered network environment.
*
* @author jh  2007.12.20
*/
class COH_EXPORT Service
    : public interface_spec<Service,
        implements<coherence::util::Service> >
    {
    // ----- Service interface ----------------------------------------------

    public:
        /**
        * Return the ServiceInfo object for this Service.
        *
        * @return the ServiceInfo object
        */
        virtual ServiceInfo::View getInfo() const = 0;

        /**
        * Add a Member listener.
        * <p/>
        * MemberListeners will be invoked in the order in which they are registered.
        *
        * @param hListener  the {@link MemberListener} to add
        */
        virtual void addMemberListener(MemberListener::Handle hListener) = 0;

        /**
        * Remove a Member listener.
        *
        * @param hListener  the {@link MemberListener} to remove
        */
        virtual void removeMemberListener(MemberListener::Handle hListener) = 0;

        /**
        * Return the user context object associated with this Service.
        *
        * The data type and semantics of this context object are entirely
        * application specific and are opaque to the Service itself.
        *
        * @return an associated user context object or NULL if a context
        *         has not been set
        */
        virtual Object::Holder getUserContext() const = 0;

        /**
        * Associate a user context object with this Service.
        *
        * @param ohCtx  a user context object
        */
        virtual void setUserContext(Object::Holder ohCtx) = 0;

        /**
        * Return a Serializer used by this Service.
        *
        * @return the Serializer object
        *
        * @since Coherence 3.4
        */
        virtual Serializer::View getSerializer() const = 0;

        /**
        * Specifies the ThreadGroup within which the Service thread will be
        * created. If not specified, the current Thread's ThreadGroup will be
        * used.
        *
        * This property can only be set at runtime, and must be configured
        * before start() is invoked to cause the Service thread to be created
        * within the specified ThreadGroup.
        *
        * @param hGroup  the group to use
        */
        virtual void setThreadGroup(ThreadGroup::Handle hGroup) = 0;

        /**
        * Return the ThreadGroup for the Daemon.
        *
        * @return the ThreadGroup for the Daemon.
        */
        virtual ThreadGroup::Handle getThreadGroup() = 0;

        /**
        * Return the ThreadGroup for the Daemon.
        *
        * @return the ThreadGroup for the Daemon.
        */
        virtual ThreadGroup::View getThreadGroup() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NET_SERVICE_HPP
