/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/RemoteCacheService.hpp"

#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Arrays.hpp"
#include "coherence/util/SafeHashMap.hpp"

#include "private/coherence/component/net/extend/protocol/cache/NamedCacheProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/DestroyCacheRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/EnsureCacheRequest.hpp"
#include "private/coherence/net/URI.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::protocol::cache::NamedCacheProtocol;
using coherence::component::net::extend::protocol::cache::service::CacheServiceProtocol;
using coherence::component::net::extend::protocol::cache::service::DestroyCacheRequest;
using coherence::component::net::extend::protocol::cache::service::EnsureCacheRequest;
using coherence::net::URI;
using coherence::net::internal::ScopedReferenceStore;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionInitiator;
using coherence::net::messaging::Protocol;
using coherence::security::auth::Subject;
using coherence::security::SecurityHelper;
using coherence::util::ArrayList;
using coherence::util::Arrays;
using coherence::util::SafeHashMap;


// ----- constructors -------------------------------------------------------

RemoteCacheService::RemoteCacheService()
    : f_hStoreRemoteNamedCache(self())
    {
    }


// ----- CacheService interface ---------------------------------------------

NamedCache::Handle RemoteCacheService::ensureCache(String::View vsName)
    {
    if (NULL == vsName || vsName->length() == 0)
        {
        vsName = "Default";
        }

    ScopedReferenceStore::Handle hStoreCache = f_hStoreRemoteNamedCache;
    RemoteNamedCache::Handle     hCache      = cast<RemoteNamedCache::Handle>
                (hStoreCache->getCache(vsName));
    if (NULL == hCache || !hCache->isActive())
        {
        COH_SYNCHRONIZED (hStoreCache)
            {
            hCache =
                cast<RemoteNamedCache::Handle>(hStoreCache->getCache(vsName));
            if (NULL == hCache || !hCache->isActive())
                {
                hCache = createRemoteNamedCache(vsName);
                hStoreCache->putCache(hCache);
                }
            }
        }
    return hCache;
    }

Collection::View RemoteCacheService::getCacheNames() const
    {
    return Arrays::asList(f_hStoreRemoteNamedCache->getNames()->toArray());
    }

void RemoteCacheService::releaseCache(NamedCache::Handle hMap)
    {
    if (!instanceof<RemoteNamedCache::Handle>(hMap)) // instanceof check
        {
        COH_THROW_STREAM (IllegalArgumentException, "illegal map: " << hMap);
        }

    String::View                  vsName      = hMap->getCacheName();
    ScopedReferenceStore::Handle  hStoreCache = f_hStoreRemoteNamedCache;

    COH_SYNCHRONIZED (hStoreCache)
        {
        hStoreCache->remove(vsName);
        releaseRemoteNamedCache(cast<RemoteNamedCache::Handle>(hMap));
        }
    }

void RemoteCacheService::releaseCaches()
    {
    ScopedReferenceStore::Handle hStoreCache = f_hStoreRemoteNamedCache;

    COH_SYNCHRONIZED (hStoreCache)
        {
        for (Iterator::Handle hIter = hStoreCache->getAllCaches()->iterator();
                hIter->hasNext();)
            {
            RemoteNamedCache::Handle hCache = cast<RemoteNamedCache::Handle>(hIter->next());
            releaseRemoteNamedCache(hCache);
            }
        hStoreCache->clear();
        }
    }

void RemoteCacheService::destroyCache(NamedCache::Handle hMap)
    {
    if (!instanceof<RemoteNamedCache::Handle>(hMap))
        {
        COH_THROW_STREAM (IllegalArgumentException, "illegal map: " << hMap);
        }

    RemoteNamedCache::Handle    hCache       = cast<RemoteNamedCache::Handle>(hMap);
    String::View                vsName       = hCache->getCacheName();
    ScopedReferenceStore::Handle hStoreCache = f_hStoreRemoteNamedCache;

    COH_SYNCHRONIZED (hStoreCache)
        {
        hStoreCache->remove(vsName);
        destroyRemoteNamedCache(hCache);
        }
    }


// ----- ServiceInfo interface ----------------------------------------------

ServiceInfo::ServiceType RemoteCacheService::getServiceType() const
    {
    return ServiceInfo::remote_cache;
    }


// ----- ConnectionListener interface ---------------------------------------

void RemoteCacheService::connectionClosed(ConnectionEvent::View vEvt)
    {
    RemoteCacheService::releaseCaches();

    RemoteService::connectionClosed(vEvt);
    }

void RemoteCacheService::connectionError(ConnectionEvent::View vEvt)
    {
    RemoteCacheService::releaseCaches();

    RemoteService::connectionError(vEvt);
    }


// ----- RemoteService interface ----------------------------------------

void RemoteCacheService::setOperationalContext(OperationalContext::View vContext)
    {
    RemoteService::setOperationalContext(vContext);
    initialize(f_hStoreRemoteNamedCache, ScopedReferenceStore::create(vContext));
    }


// ----- internal methods ---------------------------------------------------

RemoteNamedCache::Handle RemoteCacheService::createRemoteNamedCache(
        String::View vsName)
    {
    Channel::Handle                hChannel    = ensureChannel();
    Connection::Handle             hConnection = hChannel->getConnection();
    Protocol::MessageFactory::View vFactory    = hChannel->getMessageFactory();
    RemoteNamedCache::Handle       hCache      = RemoteNamedCache::create();
    Subject::View                  vSubject    = SecurityHelper::getCurrentSubject();

    EnsureCacheRequest::Handle     hRequest    = cast<EnsureCacheRequest::Handle>
            (vFactory->createMessage(EnsureCacheRequest::type_id));

    hRequest->setCacheName(vsName);

    URI::View vUri = URI::create(cast<String::View>(hChannel->request(hRequest)));

    hCache->setCacheName(vsName);
    hCache->setCacheService(this);
    hCache->setDeferKeyAssociationCheck(isDeferKeyAssociationCheck());
    hCache->setEventDispatcher(ensureEventDispatcher());

    hConnection->acceptChannel(vUri, hCache, vSubject);

    return hCache;
    }

void RemoteCacheService::releaseRemoteNamedCache(
        RemoteNamedCache::Handle hCache)
    {
    try
        {
        // when this is called due to certain connection error, e.g. ping
        // timeout, the channel could be null and closed.
        PofChannel::Handle hChannel = cast<PofChannel::Handle>(hCache->getChannel());
        if (NULL != hChannel)
            {
            hChannel->close();
            }
        }
    catch (...)
        {
        }
    }

void RemoteCacheService::destroyRemoteNamedCache(
        RemoteNamedCache::Handle hCache)
    {
    releaseRemoteNamedCache(hCache);

    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    DestroyCacheRequest::Handle    hRequest = cast<DestroyCacheRequest::Handle>
            (vFactory->createMessage(DestroyCacheRequest::type_id));

    hRequest->setCacheName(hCache->getCacheName());
    hChannel->request(hRequest);
    }

void RemoteCacheService::doConfigure(XmlElement::View vXml)
    {
    RemoteService::doConfigure(vXml);

    setDeferKeyAssociationCheck(vXml
            ->getSafeElement("defer-key-association-check")
            ->getBoolean(isDeferKeyAssociationCheck()));

    // register all Protocols
    ConnectionInitiator::Handle hInitiator = getInitiator();
    hInitiator->registerProtocol(CacheServiceProtocol::getInstance());
    hInitiator->registerProtocol(NamedCacheProtocol::getInstance());
    }

void RemoteCacheService::doShutdown()
    {
    RemoteService::doShutdown();

    f_hStoreRemoteNamedCache->clear();
    }

Channel::Handle RemoteCacheService::openChannel()
    {
    lookupProxyServiceAddress();
    
    Connection::Handle hConnection = getInitiator()->ensureConnection();
    return hConnection->openChannel(CacheServiceProtocol::getInstance(),
            "CacheServiceProxy",
            NULL,
            SecurityHelper::getCurrentSubject());
    }


// ----- accessors ----------------------------------------------------------

bool RemoteCacheService::isDeferKeyAssociationCheck() const
    {
    return fDeferKeyAssociationCheck;
    }

void RemoteCacheService::setDeferKeyAssociationCheck(bool fDefer)
    {
    fDeferKeyAssociationCheck = fDefer;
    }

COH_CLOSE_NAMESPACE4
