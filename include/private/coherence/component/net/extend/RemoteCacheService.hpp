/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REMOTE_CACHE_SERVICE_HPP
#define COH_REMOTE_CACHE_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"
#include "private/coherence/component/net/extend/RemoteService.hpp"
#include "private/coherence/net/internal/ScopedReferenceStore.hpp"
#include "private/coherence/net/messaging/Channel.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::net::OperationalContext;
using coherence::net::ServiceInfo;
using coherence::net::internal::ScopedReferenceStore;
using coherence::net::messaging::Channel;
using coherence::run::xml::XmlElement;
using coherence::util::Collection;
using coherence::util::Map;


/**
* CacheService implementation that allows a process to use a remote
* CacheService without having to join the Cluster.
*
* @author jh  2008.03.07
*/
class COH_EXPORT RemoteCacheService
    : public class_spec<RemoteCacheService,
        extends<RemoteService>,
        implements<CacheService> >
    {
    friend class factory<RemoteCacheService>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new RemoteCacheService.
        */
        RemoteCacheService();


    // ----- CacheService interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual NamedCache::Handle ensureCache(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual Collection::View getCacheNames() const;

        /**
        * {@inheritDoc}
        */
        virtual void releaseCache(NamedCache::Handle hMap);

        /**
        * {@inheritDoc}
        */
        virtual void destroyCache(NamedCache::Handle hMap);


    // ----- ServiceInfo interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ServiceInfo::ServiceType getServiceType() const;


    // ----- ConnectionListener interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void connectionClosed(ConnectionEvent::View vEvt);

        /**
        * {@inheritDoc}
        */
        virtual void connectionError(ConnectionEvent::View vEvt);


    // ----- RemoteService interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void setOperationalContext(
                OperationalContext::View vContext);


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Create a new RemoteNamedCache for the given NamedCache name.
        *
        * @param vsName  the name of the new RemoteNamedCache
        *
        * @return a new RemoteNamedCache
        */
        virtual RemoteNamedCache::Handle createRemoteNamedCache(
                String::View vsName);

        /**
        * Releases all the caches fetched from the store and then clears the store.
        */
        virtual void releaseCaches();

        /**
        * Release the given RemoteNamedCache.
        *
        * @param hCache  the RemoteNamedCache to release
        */
        virtual void releaseRemoteNamedCache(RemoteNamedCache::Handle hCache);

        /**
        * Destroy the given RemoteNamedCache.
        *
        * @param hCache  the RemoteNamedCache to destroy
        */
        virtual void destroyRemoteNamedCache(RemoteNamedCache::Handle hCache);

        /**
        * {@inheritDoc}
        */
        virtual void doConfigure(XmlElement::View vXml);

        /**
        * {@inheritDoc}
        */
        virtual void doShutdown();

        /**
        * {@inheritDoc}
        */
        virtual Channel::Handle openChannel();


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Determine whether a key should be checked for KeyAssociation by the
        * extend client (false) or deferred until the key is received by the
        * PartionedService (true).
        *
        * @return whether or not to defer the KeyAssociation check
        */
        virtual bool isDeferKeyAssociationCheck() const;

        /**
        * Configure whether a key should be checked for KeyAssociation by the
        * extend client (false) or deferred until the key is received by the
        * PartionedService (true).
        *
        * @param fDefer  whether or not to defer the KeyAssociation check
        */
        virtual void setDeferKeyAssociationCheck(bool fDefer);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Whether a key should be checked for KeyAssociation by the extend
        * client (false) or deferred until the key is received by the
        * PartionedService (true).
        */
        bool fDeferKeyAssociationCheck;

        /**
        * Store that holds cache references by name and optionally, if
        * configured, Subject.
        */
        FinalHandle<ScopedReferenceStore> f_hStoreRemoteNamedCache;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_REMOTE_CACHE_SERVICE_HPP
