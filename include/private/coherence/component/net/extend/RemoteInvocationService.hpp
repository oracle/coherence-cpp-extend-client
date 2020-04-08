/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REMOTE_INVOCATION_SERVICE_HPP
#define COH_REMOTE_INVOCATION_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/Invocable.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/component/net/extend/RemoteService.hpp"
#include "private/coherence/net/messaging/Channel.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::net::Invocable;
using coherence::net::InvocationService;
using coherence::net::ServiceInfo;
using coherence::net::messaging::Channel;
using coherence::run::xml::XmlElement;
using coherence::util::Map;
using coherence::util::Set;


/**
* Service implementation that allows a process to use a remote clustered
* Service without having to join the Cluster.
*
* @author jh  2008.02.20
*/
class COH_EXPORT RemoteInvocationService
    : public class_spec<RemoteInvocationService,
        extends<RemoteService>,
        implements<InvocationService> >
    {
    friend class factory<RemoteInvocationService>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new RemoteInvocationService.
        */
        RemoteInvocationService();

    private:
        /**
        * Blocked copy constructor.
        */
        RemoteInvocationService(const RemoteInvocationService&);


    // ----- InvocationService interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Map::View query(Invocable::Handle hTask, Set::View vSetMembers);


    // ----- ServiceInfo interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ServiceInfo::ServiceType getServiceType() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void doConfigure(XmlElement::View vXml);

        /**
        * {@inheritDoc}
        */
        virtual Channel::Handle openChannel();
    };

COH_CLOSE_NAMESPACE4

#endif // COH_REMOTE_INVOCATION_SERVICE_HPP
