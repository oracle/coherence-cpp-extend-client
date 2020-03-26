/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REMOTE_NAME_SERVICE_HPP
#define COH_REMOTE_NAME_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/NameService.hpp"
#include "coherence/net/ServiceInfo.hpp"

#include "coherence/run/xml/XmlElement.hpp"

#include "private/coherence/component/net/extend/RemoteService.hpp"

#include "private/coherence/net/messaging/Channel.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::net::NameService;
using coherence::net::ServiceInfo;
using coherence::net::messaging::Channel;
using coherence::run::xml::XmlElement;

/**
* NameService implementation that allows a client to use a remote NameService
* without having to join the Cluster.
*
* @author Wei Lin  2012.06.20
*
* @since Coherence 12.1.2
*/
class COH_EXPORT RemoteNameService
    : public class_spec<RemoteNameService,
        extends<RemoteService>,
        implements<NameService> >
    {
    friend class factory<RemoteNameService>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new RemoteNameService.
        */
        RemoteNameService();

    private:
        /**
        * Blocked copy constructor.
        */
        RemoteNameService(const RemoteNameService&);


    // ----- NameService interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void bind(String::View sName, Object::View o);

        /**
        * {@inheritDoc}
        */
       virtual Object::View lookup(String::View sName);

        /**
        * {@inheritDoc}
        */
        virtual void unbind(String::View sName);

    // ----- ServiceInfo interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ServiceInfo::ServiceType getServiceType() const;

    // ----- RemoteService methods ------------------------------------------

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

#endif // COH_REMOTE_NAME_SERVICE_HPP
