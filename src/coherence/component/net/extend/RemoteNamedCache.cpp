/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"

#include "coherence/internal/net/NamedCacheDeactivationListener.hpp"

#include "coherence/io/Serializer.hpp"

#include "coherence/io/pof/SimplePofContext.hpp"

#include "coherence/net/cache/CacheEvent.hpp"
#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/cache/KeyAssociation.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Arrays.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/ConcurrentMap.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/ConverterCollections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/LinkedList.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/LongArrayIterator.hpp"
#include "coherence/util/MapTriggerListener.hpp"
#include "coherence/util/NullImplementation.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/ReadOnlyMultiList.hpp"
#include "coherence/util/SerializationHelper.hpp"
#include "coherence/util/SparseArray.hpp"

#include "coherence/util/comparator/EntryComparator.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"

#include "coherence/util/filter/InKeySetFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"

#include "private/coherence/component/net/extend/protocol/cache/AggregateAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AggregateFilterRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ClearRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsKeyRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsValueRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/GetAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/GetRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/IndexRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/InvokeRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/InvokeAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/InvokeFilterRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ListenerFilterRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ListenerKeyRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/LockRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PartialResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/QueryRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/RemoveAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/RemoveRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/SizeRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/UnlockRequest.hpp"

#include "private/coherence/component/util/RunnableCacheEvent.hpp"

#include "private/coherence/net/messaging/Connection.hpp"

#include "private/coherence/util/PagedMuterator.hpp"
#include "private/coherence/util/SimpleMapEntry.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::protocol::cache::AggregateAllRequest;
using coherence::component::net::extend::protocol::cache::AggregateFilterRequest;
using coherence::component::net::extend::protocol::cache::ClearRequest;
using coherence::component::net::extend::protocol::cache::ContainsAllRequest;
using coherence::component::net::extend::protocol::cache::ContainsKeyRequest;
using coherence::component::net::extend::protocol::cache::ContainsValueRequest;
using coherence::component::net::extend::protocol::cache::GetAllRequest;
using coherence::component::net::extend::protocol::cache::GetRequest;
using coherence::component::net::extend::protocol::cache::LockRequest;
using coherence::component::net::extend::protocol::cache::IndexRequest;
using coherence::component::net::extend::protocol::cache::InvokeRequest;
using coherence::component::net::extend::protocol::cache::InvokeAllRequest;
using coherence::component::net::extend::protocol::cache::InvokeFilterRequest;
using coherence::component::net::extend::protocol::cache::ListenerFilterRequest;
using coherence::component::net::extend::protocol::cache::ListenerKeyRequest;
using coherence::component::net::extend::protocol::cache::NamedCacheProtocol;
using coherence::component::net::extend::protocol::cache::PartialResponse;
using coherence::component::net::extend::protocol::cache::PutAllRequest;
using coherence::component::net::extend::protocol::cache::PutRequest;
using coherence::component::net::extend::protocol::cache::QueryRequest;
using coherence::component::net::extend::protocol::cache::RemoveAllRequest;
using coherence::component::net::extend::protocol::cache::RemoveRequest;
using coherence::component::net::extend::protocol::cache::SizeRequest;
using coherence::component::net::extend::protocol::cache::UnlockRequest;
using coherence::component::util::RunnableCacheEvent;
using coherence::internal::net::NamedCacheDeactivationListener;
using coherence::io::pof::SimplePofContext;
using coherence::io::Serializer;
using coherence::net::cache::CacheEvent;
using coherence::net::cache::CacheMap;
using coherence::net::cache::KeyAssociation;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionException;
using coherence::util::ArrayList;
using coherence::util::Arrays;
using coherence::util::Binary;
using coherence::util::Collections;
using coherence::util::ConcurrentMap;
using coherence::util::ConcurrentModificationException;
using coherence::util::ConverterCollections;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::LinkedList;
using coherence::util::List;
using coherence::util::Listeners;
using coherence::util::logging::Logger;
using coherence::util::LongArrayIterator;
using coherence::util::MapTriggerListener;
using coherence::util::NullImplementation;
using coherence::util::PagedMuterator;
using coherence::util::ReadOnlyArrayList;
using coherence::util::ReadOnlyMultiList;
using coherence::util::SimpleMapEntry;
using coherence::util::SerializationHelper;
using coherence::util::SparseArray;
using coherence::util::comparator::EntryComparator;
using coherence::util::comparator::SafeComparator;
using coherence::util::filter::InKeySetFilter;
using coherence::util::filter::LimitFilter;


// ----- constructors -------------------------------------------------------

RemoteNamedCache::RemoteNamedCache()
    : f_hBinaryCache(self()),
      f_vsCacheName(self()),
      m_whCacheService(self()),
      m_hChannel(self(), (Channel::Handle) NULL, true),
      f_hConverterCache(self()),
      m_hConverterBinaryToDecoratedBinary(self()),
      m_hConverterBinaryToUndecoratedBinary(self()),
      m_hDeactivationListeners(self(), Listeners::create()),
      f_hConverterFromBinary(self()),
      f_hConverterKeyToBinary(self()),
      f_hConverterValueToBinary(self()),
      f_hEventDispatcher(self())
    {
    }

void RemoteNamedCache::onInit()
    {
    // initialize Converters first as some are used by BinaryCache
    ConverterFromBinary::Handle    hConvUp        = ConverterFromBinary::create();
    ConverterKeyToBinary::Handle   hConvKeyDown   = ConverterKeyToBinary::create(this);
    ConverterValueToBinary::Handle hConvValueDown = ConverterValueToBinary::create();

    ConverterBinaryToDecoratedBinary::Handle   hConvDecorate   =
            ConverterBinaryToDecoratedBinary::create(this);
    ConverterBinaryToUndecoratedBinary::Handle hConvUndecorate =
            ConverterBinaryToUndecoratedBinary::create(this);

    setConverterBinaryToDecoratedBinary(hConvDecorate);
    setConverterBinaryToUndecoratedBinary(hConvUndecorate);
    setConverterFromBinary(hConvUp);
    setConverterKeyToBinary(hConvKeyDown);
    setConverterValueToBinary(hConvValueDown);

    BinaryCache::Handle hCache = instantiateBinaryCache();
    setBinaryCache(hCache);
    setConverterCache(ConverterCollections::ConverterNamedCache::create(
            (NamedCache::Handle) hCache,
            ConverterCollections::EntryConverter::create(hConvUp, hConvUp),
            ConverterCollections::EntryConverter::create(hConvKeyDown, hConvValueDown)
            ));

    super::onInit();
    }

RemoteNamedCache::BinaryCache::BinaryCache(RemoteNamedCache::Handle hCache, MapListenerSupport::Handle hSupport)
    : m_whCache(self(), hCache),
      f_hChannel(self(), (Channel::Handle) NULL, true),
      m_hConverterBinaryToDecoratedBinary(self()),
      m_vConverterBinaryToUndecoratedBinary(self()),
      f_hEntrySet(self()),
      f_hEventDispatcher(self()),
      f_hKeySet(self()),
      f_hValues(self()),
      f_hListenerSupport(self(), hSupport == NULL ? MapListenerSupport::create() : hSupport),
      f_hlaFilters(self(), SparseArray::create())
    {
    }

void RemoteNamedCache::BinaryCache::onInit()
    {
    RemoteNamedCache::Handle hCache = m_whCache;

    setConverterBinaryToDecoratedBinary(
            hCache->getConverterBinaryToDecoratedBinary());
    setConverterBinaryToUndecoratedBinary(
            hCache->getConverterBinaryToUndecoratedBinary());
    initialize(f_hEntrySet, EntrySet::create(this));
    initialize(f_hKeySet, KeySet::create(this));
    initialize(f_hValues, Values::create(this));

    super::onInit();
    }

RemoteNamedCache::BinaryCache::EntrySet::EntrySet(BinaryCache::Handle hCache)
    : m_whCache(self(), hCache)
    {
    }

RemoteNamedCache::BinaryCache::EntrySet::EntryIterator::EntryIterator(
        BinaryCache::Holder ohCache)
    : m_vEntry(self()),
      f_hIterKeys(self(), ohCache->keySet()->iterator()),
      f_ohCache(self(), ohCache)
    {
    }

RemoteNamedCache::BinaryCache::KeySet::KeySet(BinaryCache::Handle hCache)
    : m_whCache(self(), hCache)
    {
    }

RemoteNamedCache::BinaryCache::KeySet::KeyIterator::KeyIterator(
        BinaryCache::Holder ohCache)
    : m_vKey(self()),
      f_hIterKeys(self(), ohCache->keySet(NULL)->iterator()),
      f_ohCache(self(), ohCache)
    {
    }

RemoteNamedCache::BinaryCache::KeySet::KeyAdvancer::KeyAdvancer(
        BinaryCache::Holder ohCache)
    : f_ohCache(self(), ohCache),
      m_vCookie(self()),
      m_fExhausted(false)
    {
    }

RemoteNamedCache::BinaryCache::Values::Values(BinaryCache::Handle hCache)
    : m_whCache(self(), hCache)
    {
    }

RemoteNamedCache::BinaryCache::Values::ValueIterator::ValueIterator(
        BinaryCache::Holder ohCache)
    : m_ohValue(self()),
      f_hIterKeys(self()),
      f_ohCache(self(), ohCache)
    {
    if (instanceof<BinaryCache::Handle>(ohCache))
        {
        initialize(f_hIterKeys, cast<BinaryCache::Handle>(ohCache)->keySet()->iterator());
        }
    else
        {
        initialize(f_hIterKeys, ohCache->keySet()->iterator());
        }
    }


// ----- BinaryCache inner class --------------------------------------------

// ----- BinaryCache interface ------------------------------------------

bool RemoteNamedCache::BinaryCache::containsAll(Collection::View vColKeys) const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    ContainsAllRequest::Handle     hRequest = cast<ContainsAllRequest::Handle>
            (vFactory->createMessage(ContainsAllRequest::type_id));

    hRequest->setKeySet(vColKeys);

    return (cast<Boolean::View>(hChannel->request(hRequest)))->getValue();
    }

Object::Holder RemoteNamedCache::BinaryCache::put(Object::View vKey,
        Object::Holder ohValue, int64_t cMillis, bool fReturn)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    PutRequest::Handle             hRequest = cast<PutRequest::Handle>
            (vFactory->createMessage(PutRequest::type_id));

    hRequest->setKey(vKey);
    hRequest->setValue(ohValue);
    hRequest->setExpiryDelay(cMillis);
    hRequest->setReturnRequired(fReturn);

    return hChannel->request(hRequest);
    }

Object::Holder RemoteNamedCache::BinaryCache::remove(Object::View vKey, bool fReturn)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    RemoveRequest::Handle          hRequest = cast<RemoveRequest::Handle>
            (vFactory->createMessage(RemoveRequest::type_id));

    hRequest->setKey(vKey);
    hRequest->setReturnRequired(fReturn);

    return hChannel->request(hRequest);
    }

bool RemoteNamedCache::BinaryCache::removeAll(Collection::View vColKeys)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    RemoveAllRequest::Handle       hRequest = cast<RemoveAllRequest::Handle>
            (vFactory->createMessage(RemoveAllRequest::type_id));

    hRequest->setKeySet(vColKeys);

    return (cast<Boolean::View>(hChannel->request(hRequest)))->getValue();
    }

void RemoteNamedCache::BinaryCache::dispatch(int32_t nEventId,
        Array<int64_t>::View alFilterIds, Object::View vKey, Object::View vValueOld,
        Object::View vValueNew, bool fSynthetic, int32_t nTransformationState,
        bool fPriming)
    {
    MapListenerSupport::Handle hSupport    = getMapListenerSupport();
    int32_t                    cFilters    = alFilterIds == NULL ? 0 : alFilterIds->length;
    CacheEvent::Handle         hEvt        = NULL;

    CacheEvent::TransformationState transformState 
            = static_cast<CacheEvent::TransformationState>(nTransformationState);

    // collect key-based listeners
    Listeners::View   vListeners = transformState == CacheEvent::transformed
            ? NULL : hSupport->getListeners(vKey);

    Listeners::Handle hListeners = NULL;
    if (cFilters > 0)
        {
        LongArray::Handle hlaFilters   = getFilterArray();
        List::Handle      hListFilters = NULL;

        // collect filter-based listeners
        COH_SYNCHRONIZED (hSupport)
            {
            for (int32_t i = 0; i < cFilters; i++)
                {
                int64_t lFilterId = alFilterIds[i];
                if (hlaFilters->exists(lFilterId))
                    {
                    Filter::View vFilter = cast<Filter::View>
                        (hlaFilters->get(lFilterId));
                    if (hListFilters == NULL)
                        {
                        hListFilters = LinkedList::create();
                        
                        // clone the key listeners before merging filter listeners
                        Listeners::Handle hListenersTemp = Listeners::create();
                        hListenersTemp->addAll(vListeners);
                        hListeners = hListenersTemp;
                        }
                    hListFilters->add(vFilter);
                    hListeners->addAll(hSupport->getListeners(vFilter));
                    }
                }                   
            if (hListeners != NULL)
                {
                vListeners = hListeners;
                }
            }

        if (hListFilters != NULL)
            {
            ObjectArray::Handle hFilterArray = ObjectArray::create(hListFilters->size());
            hFilterArray = hListFilters->toArray(hFilterArray);

            hEvt = MapListenerSupport::FilterEvent::create(this, nEventId,
                   vKey, vValueOld, vValueNew, fSynthetic, transformState, fPriming, hFilterArray);
            }
        }

    if (NULL == vListeners || vListeners->isEmpty())
        {
        // we cannot safely remove the orphaned listener because of the
        // following race condition: if another thread registers a listener
        // for the same key or filter associated with the event between the
        // time that this thread detected the orphaned listener, but before
        // either sends a message to the server, it is possible for this
        // thread to inadvertantly remove the new listener.
        //
        // since it is only possible for synchronous listeners to be leaked
        // (due to the extra synchronization in the SafeNamedCache), let's
        // err on the side of leaking a listener instead of possibily
        // incorrectly removing a listener
        if (cFilters > 0)
            {
            String::View vsMsg = COH_TO_STRING ("Received an orphaned event; " <<
                    MapEvent::getDescription(nEventId) <<
                    " key=" << vKey << " filters=[");

            LongArray::Handle hlaFilters   = getFilterArray();
            for (int32_t i = 0; i < cFilters; i++)
                {
                if (i > 0)
                    {
                    vsMsg = COH_TO_STRING(vsMsg << ", ");
                    }
                Filter::View vFilter = cast<Filter::View>
                    (getFilterArray()->get(alFilterIds[i]));
                vsMsg = COH_TO_STRING(vsMsg << vFilter);
                }
            vsMsg = COH_TO_STRING(vsMsg << "]");
            COH_LOG (vsMsg, Logger::level_d6);
            }
        else {
            COH_LOG (COH_TO_STRING ("Received an orphaned event; " <<
                    MapEvent::getDescription(nEventId) <<
                    " key=" << vKey), Logger::level_d6);
            }
        }
    else
        {
        if (hEvt == NULL)
            {
            // MapEvent was sent by a key-based MapListener
            hEvt = CacheEvent::create(this, nEventId, vKey, vValueOld, vValueNew,
                    fSynthetic, transformState, fPriming);
            }

        RunnableCacheEvent::dispatchSafe(hEvt, vListeners,
                getEventDispatcher()->getQueue());
        }
    }

// ----- CacheMap interface ---------------------------------------------

Map::View RemoteNamedCache::BinaryCache::getAll(Collection::View vColKeys) const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    GetAllRequest::Handle          hRequest = cast<GetAllRequest::Handle>
            (vFactory->createMessage(GetAllRequest::type_id));

    hRequest->setKeySet(vColKeys);

    Map::View vResponse = cast<Map::View>(hChannel->request(hRequest));

    return vResponse == NULL
            ? vResponse
            : (Map::View) ConverterCollections::ConverterMap::create(vResponse,
                    ConverterCollections::EntryConverter::create(
                            NullImplementation::getConverter(),
                            NullImplementation::getConverter()),
                    ConverterCollections::EntryConverter::create(
                            getConverterBinaryToUndecoratedBinary(),
                            NullImplementation::getConverter()));
    }

Object::Holder RemoteNamedCache::BinaryCache::put(Object::View vKey,
    Object::Holder ohValue, int64_t cMillis)
    {
    return put(vKey, ohValue, cMillis, true);
    }

// ----- ConcurrentMap interface ----------------------------------------

bool RemoteNamedCache::BinaryCache::lock(Object::View vKey, int64_t cWait) const
    {
    if (ConcurrentMap::getLockAll() == vKey)
        {
        COH_THROW (UnsupportedOperationException::create(
                "RemoteNamedCache does not support LOCK_ALL"));
        }

    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    LockRequest::Handle            hRequest = cast<LockRequest::Handle>
            (vFactory->createMessage(LockRequest::type_id));

    hRequest->setKey(vKey);
    hRequest->setTimeoutMillis(cWait);

    return (cast<Boolean::View>(hChannel->request(hRequest)))->getValue();
    }

bool RemoteNamedCache::BinaryCache::lock(Object::View vKey) const
    {
    return lock(vKey, 0);
    }

bool RemoteNamedCache::BinaryCache::unlock(Object::View vKey) const
    {
    if (ConcurrentMap::getLockAll() == vKey)
        {
        COH_THROW (UnsupportedOperationException::create(
                "RemoteNamedCache does not support LOCK_ALL"));
        }

    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    UnlockRequest::Handle          hRequest = cast<UnlockRequest::Handle>
            (vFactory->createMessage(UnlockRequest::type_id));

    hRequest->setKey(vKey);

    return (cast<Boolean::View>(hChannel->request(hRequest)))->getValue();
    }

// ----- InvocableMap interface -----------------------------------------

Object::Holder RemoteNamedCache::BinaryCache::invoke(Object::View vKey,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    InvokeRequest::Handle          hRequest = cast<InvokeRequest::Handle>
            (vFactory->createMessage(InvokeRequest::type_id));

    hRequest->setKey(vKey);
    hRequest->setProcessor(hAgent);

    return hChannel->request(hRequest);
    }

Map::View RemoteNamedCache::BinaryCache::invokeAll(Collection::View vCollKeys,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    InvokeAllRequest::Handle       hRequest = cast<InvokeAllRequest::Handle>
            (vFactory->createMessage(InvokeAllRequest::type_id));

    hRequest->setKeySet(vCollKeys);
    hRequest->setProcessor(hAgent);

    Map::View vResponse = cast<Map::View>(hChannel->request(hRequest));

    return vResponse == NULL
            ? vResponse
            : (Map::View) ConverterCollections::ConverterMap::create(vResponse,
                    ConverterCollections::EntryConverter::create(
                            NullImplementation::getConverter(),
                            NullImplementation::getConverter()),
                    ConverterCollections::EntryConverter::create(
                            getConverterBinaryToUndecoratedBinary(),
                            NullImplementation::getConverter()));
    }

Map::View RemoteNamedCache::BinaryCache::invokeAll(Filter::View vFilter,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    Binary::View                   vBinCookie = NULL;
    Map::Handle                    hMapResult = HashMap::create();

    do
        {
        InvokeFilterRequest::Handle    hRequest = cast<InvokeFilterRequest::Handle>
                (vFactory->createMessage(InvokeFilterRequest::type_id));

        hRequest->setCookie(vBinCookie);
        hRequest->setFilter(vFilter);
        hRequest->setProcessor(hAgent);

        Request::Status::Handle         hStatus   = hChannel->send(
                (Request::Handle)hRequest);
        AbstractPartialResponse::Handle hResponse = cast<AbstractPartialResponse::Handle>(
                hStatus->waitForResponse());
        Map::View                       hMap      = cast<Map::View>(
                processResponse(hResponse));
        if (NULL == hMap || hMap->isEmpty())
            {
            // nothing to do
            }
        else
            {
            hMapResult->putAll(hMap);
            }

        vBinCookie = hResponse->getCookie();
        }
    while (vBinCookie != NULL);

    return hMapResult == NULL
            ? (Map::View) hMapResult
            : (Map::View) ConverterCollections::ConverterMap::create(hMapResult,
                    ConverterCollections::EntryConverter::create(
                            NullImplementation::getConverter(),
                            NullImplementation::getConverter()),
                    ConverterCollections::EntryConverter::create(
                            getConverterBinaryToUndecoratedBinary(),
                            NullImplementation::getConverter()));
    }

Object::Holder RemoteNamedCache::BinaryCache::aggregate(Collection::View vCollKeys,
        InvocableMap::EntryAggregator::Handle hAgent) const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    AggregateAllRequest::Handle    hRequest = cast<AggregateAllRequest::Handle>
            (vFactory->createMessage(AggregateAllRequest::type_id));

    hRequest->setAggregator(hAgent);
    hRequest->setKeySet(vCollKeys);

    return hChannel->request(hRequest);
    }

Object::Holder RemoteNamedCache::BinaryCache::aggregate(Filter::View vFilter,
        InvocableMap::EntryAggregator::Handle hAgent) const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    AggregateFilterRequest::Handle hRequest = cast<AggregateFilterRequest::Handle>
            (vFactory->createMessage(AggregateFilterRequest::type_id));

    hRequest->setAggregator(hAgent);
    hRequest->setFilter(vFilter);

    return hChannel->request(hRequest);
    }

// ----- QueryMap interface ---------------------------------------------

Set::View RemoteNamedCache::BinaryCache::keySet(Filter::View vFilter) const
    {
    // NOTE: The following optimization cannot be made, as the KeySet inner
    // class (and its Iterator implementation) depends upon this behavior
    /*
    if (NULL == vFilter)
        {
        return keySet();
        }
    */

    return query(vFilter, true);
    }

Set::View RemoteNamedCache::BinaryCache::entrySet(Filter::View vFilter) const
    {
    if (NULL == vFilter)
        {
        return entrySet();
        }

    return query(vFilter, false);
    }

Set::View RemoteNamedCache::BinaryCache::entrySet(Filter::View /* vFilter */,
        Comparator::View /* vComparator */) const
    {
    COH_THROW (UnsupportedOperationException::create(
            "RemoteNamedCache::BinaryCache::entrySet() is not supported"));
    }

void RemoteNamedCache::BinaryCache::addIndex(ValueExtractor::View vExtractor,
        bool fOrdered, Comparator::View vComparator)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    IndexRequest::Handle           hRequest = cast<IndexRequest::Handle>
            (vFactory->createMessage(IndexRequest::type_id));

    hRequest->setAdd(true);
    hRequest->setComparator(vComparator);
    hRequest->setExtractor(vExtractor);
    hRequest->setOrdered(fOrdered);

    hChannel->request(hRequest);
    }

void RemoteNamedCache::BinaryCache::removeIndex(ValueExtractor::View vExtractor)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    IndexRequest::Handle           hRequest = cast<IndexRequest::Handle>
            (vFactory->createMessage(IndexRequest::type_id));

    hRequest->setExtractor(vExtractor);
    hChannel->request(hRequest);
    }

// ----- CacheLoader interface ------------------------------------------

Object::Holder RemoteNamedCache::BinaryCache::load(Object::View /*vKey*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

Map::View RemoteNamedCache::BinaryCache::loadAll(Collection::View /*vColKeys*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

// ----- NamedCache interface -------------------------------------------

String::View RemoteNamedCache::BinaryCache::getCacheName() const
    {
    return getRemoteNamedCache()->getCacheName();
    }

CacheService::Handle RemoteNamedCache::BinaryCache::getCacheService()
    {
    COH_THROW (UnsupportedOperationException::create());
    }

CacheService::View RemoteNamedCache::BinaryCache::getCacheService() const
    {
    COH_THROW (UnsupportedOperationException::create());
    }

bool RemoteNamedCache::BinaryCache::isActive() const
    {
    Channel::View vChannel = getChannel();

    return NULL == vChannel ? false : vChannel->isOpen();
    }

void RemoteNamedCache::BinaryCache::release()
    {
    COH_THROW (UnsupportedOperationException::create());
    }

void RemoteNamedCache::BinaryCache::destroy()
    {
    COH_THROW (UnsupportedOperationException::create());
    }

void RemoteNamedCache::truncate()
    {
    getBinaryCache()->truncate();
    }


// ----- ObservableMap interface ----------------------------------------

void RemoteNamedCache::BinaryCache::addKeyListener(
        MapListener::Handle hListener, Object::View vKey, bool fLite)
    {
    COH_ENSURE_PARAM(vKey);

    MapTriggerListener::Handle hTrigger = NULL;
    if (NULL != (hTrigger = cast<MapTriggerListener::Handle>(hListener, false)))
        {
        addRemoteMapListener(vKey, fLite, hTrigger->getTrigger(), false /*fPriming*/);
        }
    else
        {
        bool fWasEmpty = false;
        bool fWasLite  = false;

        // returned keys will not be int decorated
        Binary::View vBinKey = cast<Binary::View>(
                getConverterBinaryToUndecoratedBinary()->convert(vKey));

        MapListenerSupport::Handle hSupport = getMapListenerSupport();
        COH_SYNCHRONIZED (hSupport)
            {
            fWasEmpty = hSupport->isEmpty(cast<Object::View>(vBinKey));
            fWasLite  = !fWasEmpty
                    && !hSupport->containsStandardListeners(cast<Object::View>(vBinKey));
            hSupport->addListener(hListener, cast<Object::View>(vBinKey), fLite);
            }

        bool fPriming  = hSupport->isPrimingListener(hListener);

        // "priming" request should be sent regardless
        if (fWasEmpty || (fWasLite && !fLite) || fPriming)
            {
            try
                {
                addRemoteMapListener(vKey, fLite, NULL, fPriming);
                }
            catch (RuntimeException::View e)
                {
                hSupport->removeListener(hListener, cast<Object::View>(vBinKey));
                COH_THROW (e);
                }
            }
        }
    }

void RemoteNamedCache::BinaryCache::removeKeyListener(
        MapListener::Handle hListener, Object::View vKey)
    {
    COH_ENSURE_PARAM(vKey);

    bool fSync = instanceof<MapListenerSupport::SynchronousListener::Handle>
            (hListener);

    if (instanceof<MapTriggerListener::Handle>(hListener))
        {
        removeRemoteMapListener(vKey, fSync, (cast<MapTriggerListener::Handle>(hListener))->getTrigger(), false/*fPriming*/);
        }
    else
        {
        bool fIsEmpty = false;

        // returned keys will not be int decorated
        Binary::View vBinKey = cast<Binary::View>(
                getConverterBinaryToUndecoratedBinary()->convert(vKey));

        MapListenerSupport::Handle hSupport = getMapListenerSupport();
        COH_SYNCHRONIZED (hSupport)
            {
            hSupport->removeListener(hListener, cast<Object::View>(vBinKey));
            fIsEmpty = hSupport->isEmpty(cast<Object::View>(vBinKey));
            }

        bool fPriming = hSupport->isPrimingListener(hListener);
        if (fIsEmpty || fPriming)
            {
            removeRemoteMapListener(vKey, fSync, NULL, fPriming);
            }
        }
    }

void RemoteNamedCache::BinaryCache::addMapListener(
        MapListener::Handle hListener)
    {
    addFilterListener(hListener, (Filter::View) NULL, false);
    }

void RemoteNamedCache::BinaryCache::removeMapListener(
        MapListener::Handle hListener)
    {
    removeFilterListener(hListener, (Filter::View) NULL);
    }

void RemoteNamedCache::BinaryCache::addFilterListener(
        MapListener::Handle hListener, Filter::View vFilter, bool fLite)
    {
    MapListenerSupport::Handle hSupport = getMapListenerSupport();

    if (instanceof<MapTriggerListener::Handle>(hListener))
        {
        addRemoteMapListener(vFilter, 0, fLite, (cast<MapTriggerListener::Handle>(hListener))->getTrigger(),
                             false/*fPriming*/);
        }
    else if (hSupport->isPrimingListener(hListener)
             && instanceof<InKeySetFilter::View>(vFilter))
        {
        Set::View setKeys = (cast<InKeySetFilter::View>(vFilter))->getKeys();
        ConverterBinaryToUndecoratedBinary::View conv = getConverterBinaryToUndecoratedBinary();
        
        COH_SYNCHRONIZED (hSupport)
            {
            for (Iterator::Handle iter = setKeys->iterator(); iter->hasNext();)
                {
                Binary::View vBinKey = cast<Binary::View>(conv->convert(iter->next()));
                hSupport->addListener(hListener, cast<Object::View>(vBinKey), fLite);
                }
            }
        try
            {
            addRemoteMapListener(vFilter, 1 /*dummy*/, fLite, NULL, true/*fPriming*/);
            }
        catch (RuntimeException::View e)
            {
            COH_SYNCHRONIZED (hSupport)
                {
                for (Iterator::Handle iter = setKeys->iterator(); iter->hasNext();)
                    {
                    Binary::View vBinKey = cast<Binary::View>(conv->convert(iter->next()));
                    hSupport->removeListener(hListener, cast<Object::View>(vBinKey));
                    }
                }
            COH_THROW (e);
            }
        }
    else
        {
        bool    fWasEmpty = false;
        bool    fWasLite  = false;
        int64_t lFilterId = 0;

        COH_SYNCHRONIZED (hSupport)
            {
            fWasEmpty = hSupport->isEmpty(vFilter);
            fWasLite  = !fWasEmpty &&
                    !hSupport->containsStandardListeners(vFilter);

            lFilterId = fWasEmpty ? registerFilter(vFilter) :
                    getFilterId(vFilter);
            hSupport->addListener(hListener, vFilter, fLite);
            }

        if (fWasEmpty || (fWasLite && !fLite))
            {
            try
                {
                addRemoteMapListener(vFilter, lFilterId, fLite, NULL, false/*fPriming*/);
                }
            catch (RuntimeException::View e)
                {
                COH_SYNCHRONIZED (hSupport)
                    {
                    if (fWasEmpty)
                        {
                        getFilterArray()->remove(lFilterId);
                        }
                    hSupport->removeListener(hListener, vFilter);
                    }
                COH_THROW (e);
                }
            }
        }
    }

void RemoteNamedCache::BinaryCache::removeFilterListener(
        MapListener::Handle hListener, Filter::View vFilter)
    {
    MapListenerSupport::Handle hSupport = getMapListenerSupport();
    bool fSync = instanceof<MapListenerSupport::SynchronousListener::Handle>
            (hListener);

    if (instanceof<MapTriggerListener::Handle>(hListener))
        {
        removeRemoteMapListener(vFilter, 0, fSync, (cast<MapTriggerListener::Handle>(hListener))->getTrigger(),
                                false/*fPriming*/);
        }
    else if (hSupport->isPrimingListener(hListener)
             && instanceof<InKeySetFilter::View>(vFilter))
        {
        Set::View setKeys = (cast<InKeySetFilter::View>(vFilter))->getKeys();
        ConverterBinaryToUndecoratedBinary::View conv = getConverterBinaryToUndecoratedBinary();
        
        COH_SYNCHRONIZED (hSupport)
            {
            for (Iterator::Handle iter = setKeys->iterator(); iter->hasNext();)
                {
                Binary::View vBinKey = cast<Binary::View>(conv->convert(iter->next()));
                hSupport->removeListener(hListener, cast<Object::View>(vBinKey));
                }
            }
            
        removeRemoteMapListener(vFilter, 1 /*dummy*/, fSync, NULL, true/*fPriming*/);
        }
    else
        {
        bool    fIsEmpty  = false;
        int64_t lFilterId = 0;

        COH_SYNCHRONIZED (hSupport)
            {
            hSupport->removeListener(hListener, vFilter);
            fIsEmpty = hSupport->isEmpty(vFilter);
            if (fIsEmpty)
                {
                lFilterId = getFilterId(vFilter);
                getFilterArray()->remove(lFilterId);
                }
            }

        if (fIsEmpty)
            {
            removeRemoteMapListener(vFilter, lFilterId, fSync, NULL, false/*fPriming*/);
            }
        }
    }

// ----- Map interface --------------------------------------------------

size32_t RemoteNamedCache::BinaryCache::size() const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    SizeRequest::Handle            hRequest = cast<SizeRequest::Handle>
            (vFactory->createMessage(SizeRequest::type_id));

    return (size32_t) (cast<Integer32::View>(hChannel->request(hRequest)))->getValue();
    }

bool RemoteNamedCache::BinaryCache::isEmpty() const
    {
    return 0 == size();
    }

bool RemoteNamedCache::BinaryCache::containsKey(Object::View vKey) const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    ContainsKeyRequest::Handle     hRequest = cast<ContainsKeyRequest::Handle>
            (vFactory->createMessage(ContainsKeyRequest::type_id));

    hRequest->setKey(vKey);

    return (cast<Boolean::View>(hChannel->request(hRequest)))->getValue();
    }

bool RemoteNamedCache::BinaryCache::containsValue(Object::View vValue) const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    ContainsValueRequest::Handle   hRequest = cast<ContainsValueRequest::Handle>
            (vFactory->createMessage(ContainsValueRequest::type_id));

    hRequest->setValue(vValue);

    return (cast<Boolean::View>(hChannel->request(hRequest)))->getValue();
    }

Object::Holder RemoteNamedCache::BinaryCache::get(Object::View vKey) const
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    GetRequest::Handle             hRequest = cast<GetRequest::Handle>
            (vFactory->createMessage(GetRequest::type_id));

    hRequest->setKey(vKey);

    return hChannel->request(hRequest);
    }

Object::Holder RemoteNamedCache::BinaryCache::get(Object::View vKey)
    {
    return ((const BinaryCache*) this)->get(vKey);
    }

Object::Holder RemoteNamedCache::BinaryCache::put(Object::View vKey,
        Object::Holder ohValue)
    {
    return put(vKey, ohValue, CacheMap::expiry_default, true);
    }

Object::Holder RemoteNamedCache::BinaryCache::remove(Object::View vKey)
    {
    return remove(vKey, true);
    }

void RemoteNamedCache::BinaryCache::putAll(Map::View vMap)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    PutAllRequest::Handle          hRequest = cast<PutAllRequest::Handle>
            (vFactory->createMessage(PutAllRequest::type_id));

    hRequest->setMap(vMap);
    hChannel->request(hRequest);
    }

void RemoteNamedCache::BinaryCache::clear()
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    ClearRequest::Handle           hRequest = cast<ClearRequest::Handle>
            (vFactory->createMessage(ClearRequest::type_id));

    hChannel->request(hRequest);
    }

void RemoteNamedCache::BinaryCache::truncate()
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    ClearRequest::Handle           hRequest = cast<ClearRequest::Handle>
            (vFactory->createMessage(ClearRequest::type_id));

    // COH-13916
    if (hRequest->getImplVersion() <= 5)
        {
        COH_THROW_STREAM(UnsupportedOperationException, "NamedCache.truncate is not supported by the current proxy. "
              << "Either upgrade the version of Coherence on the proxy or connect to a proxy "
              << "that supports the truncate operation.");
        }

    hRequest->setTruncate(true);

    hChannel->request(hRequest);
    }

Set::View RemoteNamedCache::BinaryCache::keySet() const
    {
    return f_hKeySet;
    }

Set::Handle RemoteNamedCache::BinaryCache::keySet()
    {
    return f_hKeySet;
    }

Collection::View RemoteNamedCache::BinaryCache::values() const
    {
    return f_hValues;
    }

Collection::Handle RemoteNamedCache::BinaryCache::values()
    {
    return f_hValues;
    }

Set::View RemoteNamedCache::BinaryCache::entrySet() const
    {
    return f_hEntrySet;
    }

Set::Handle RemoteNamedCache::BinaryCache::entrySet()
    {
    return f_hEntrySet;
    }

// ----- CacheStore interface -------------------------------------------

void RemoteNamedCache::BinaryCache::store(Object::View vKey, Object::Holder ohValue)
    {
    put(vKey, ohValue, CacheMap::expiry_default, false);
    }

void RemoteNamedCache::BinaryCache::storeAll(Map::View vMapEntries)
    {
    putAll(vMapEntries);
    }

void RemoteNamedCache::BinaryCache::erase(Object::View vKey)
    {
    remove(vKey, false);
    }

void RemoteNamedCache::BinaryCache::eraseAll(Collection::View vColKeys)
    {
    removeAll(vColKeys);
    }

// ----- CacheLoader interface ------------------------------------------

Object::Holder RemoteNamedCache::load(Object::View vKey)
    {
    return get(vKey);
    }

Map::View RemoteNamedCache::loadAll(Collection::View vColKeys)
    {
    return getAll(vColKeys);
    }

// ----- Receiver interface ---------------------------------------------

String::View RemoteNamedCache::getName() const
    {
    return COH_TO_STRING("RemoteNamedCache(Cache=" << getCacheName() << ")");
    }

Protocol::View RemoteNamedCache::getProtocol() const
    {
    return NamedCacheProtocol::getInstance();
    }

void RemoteNamedCache::registerChannel(Channel::Handle hChannel)
    {
    setChannel(hChannel);
    getBinaryCache()->setChannel(hChannel);

    Serializer::View vSerializer = hChannel->getSerializer();
    getConverterFromBinary()->setSerializer(vSerializer);
    getConverterValueToBinary()->setSerializer(vSerializer);

    if (instanceof<SimplePofContext::View>(vSerializer))
        {
        SimplePofContext::View vSpc = cast<SimplePofContext::View>(vSerializer);
        if (vSpc->isReferenceEnabled())
            {
            vSpc = cast<SimplePofContext::View>(vSpc->clone());
            vSpc->setReferenceEnabled(false);
            vSerializer = vSpc;
            }
        }
    getConverterKeyToBinary()->setSerializer(vSerializer);
    }

void RemoteNamedCache::onMessage(Message::Handle hMessage)
    {
    hMessage->run();
    }

void RemoteNamedCache::unregisterChannel(Channel::Handle /*hChannel*/)
    {
    setChannel(NULL);
    }

void RemoteNamedCache::onChannelClosed(Channel::Handle /*hChannel*/)
    {
    Object::View oNull = NULL;

    Listeners::Handle listeners = getDeactivationListeners();
    if (!listeners->isEmpty())
        {
        CacheEvent::Handle hEvt = CacheEvent::create(
               this, 3 /*entry_deleted*/, oNull, oNull, oNull, true);

        // dispatch the event to the listeners, which are all synchronous (hence the null Queue)
        RunnableCacheEvent::dispatchSafe(hEvt, listeners, NULL /*Queue*/);
        }
    }

// ----- internal methods -----------------------------------------------

Channel::Handle RemoteNamedCache::BinaryCache::ensureChannel() const
    {
    Channel::Handle hChannel = getChannel();
    if (NULL == hChannel || !hChannel->isOpen())
        {
        String::View     vsCause     = "released";
        Connection::View vConnection = NULL;

        if (NULL != hChannel)
            {
            vConnection = hChannel->getConnection();
            if (NULL == vConnection || !vConnection->isOpen())
                {
                vsCause = "closed";
                }
            }

        COH_THROW (ConnectionException::create(COH_TO_STRING("NamedCache \""
                << getCacheName() << "\" has been " << vsCause),
                (Exception::View) NULL, vConnection));
        }

    return hChannel;
    }

void RemoteNamedCache::BinaryCache::setMapListenerSupport(
        MapListenerSupport::Handle hListenerSupport)
    {
    initialize(f_hListenerSupport, hListenerSupport);
    }

void RemoteNamedCache::BinaryCache::addRemoteMapListener(Filter::View vFilter,
        int64_t lFilterId, bool fLite, MapTrigger::Handle hTrigger, bool fPriming)
    {
    Channel::Handle hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();

    ListenerFilterRequest::Handle hRequest = cast<ListenerFilterRequest::Handle>
            (vFactory->createMessage(ListenerFilterRequest::type_id));

    // COH-4615
    if (hRequest->getImplVersion() <= 5 && instanceof<InKeySetFilter::View>(vFilter) && fLite)
        {
        COH_THROW (UnsupportedOperationException::create("Priming events are not supported"));
        }

    hRequest->setAdd(true);
    hRequest->setFilter(vFilter);
    hRequest->setFilterId(lFilterId);
    hRequest->setLite(fLite);
    hRequest->setTrigger(hTrigger);
    hRequest->setPriming(fPriming);

    hChannel->request(hRequest);
    }

void RemoteNamedCache::BinaryCache::addRemoteMapListener(Object::View vKey,
        bool fLite, MapTrigger::Handle hTrigger, bool fPriming)
    {
    Channel::Handle hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();

    ListenerKeyRequest::Handle hRequest = cast<ListenerKeyRequest::Handle>
            (vFactory->createMessage(ListenerKeyRequest::type_id));

    hRequest->setAdd(true);
    hRequest->setKey(vKey);
    hRequest->setLite(fLite);
    hRequest->setTrigger(hTrigger);
    hRequest->setPriming(fPriming);

    hChannel->request(hRequest);
    }

void RemoteNamedCache::BinaryCache::removeRemoteMapListener(
        Filter::View vFilter, int64_t lFilterId, bool fSync,
        MapTrigger::Handle hTrigger, bool fPriming)
    {
    Channel::Handle hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();

    ListenerFilterRequest::Handle hRequest = cast<ListenerFilterRequest::Handle>
            (vFactory->createMessage(ListenerFilterRequest::type_id));

    hRequest->setFilter(vFilter);
    hRequest->setFilterId(lFilterId);
    hRequest->setTrigger(hTrigger);
    hRequest->setPriming(fPriming);

    if (fSync)
        {
        // necessary to support the removal of a SynchronousListener from
        // within another SynchronousListener (as is the case in NearCache)
        hChannel->send((Request::Handle) hRequest);
        }
    else
        {
        hChannel->request(hRequest);
        }
    }

void RemoteNamedCache::BinaryCache::removeRemoteMapListener(Object::View vKey,
        bool fSync, MapTrigger::Handle hTrigger, bool fPriming)
    {
    Channel::Handle hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    ListenerKeyRequest::Handle hRequest = cast<ListenerKeyRequest::Handle>
            (vFactory->createMessage(ListenerKeyRequest::type_id));

    hRequest->setKey(vKey);
    hRequest->setTrigger(hTrigger);
    hRequest->setPriming(fPriming);

    if (fSync)
        {
        // necessary to support the removal of a SynchronousListener from
        // within another SynchronousListener (as is the case in NearCache)
        hChannel->send((Request::Handle) hRequest);
        }
    else
        {
        hChannel->request(hRequest);
        }
    }

int64_t RemoteNamedCache::BinaryCache::getFilterId(Filter::View vFilter)
    {
    for (LongArrayIterator::Handle hIter = getFilterArray()->iterator();
            hIter->hasNext();)
        {
        Filter::View vFilterThat = cast<Filter::View>(hIter->next());
        if (Object::equals(vFilter, vFilterThat))
            {
            return hIter->getIndex();
            }
        }
    return 0;
    }

int64_t RemoteNamedCache::BinaryCache::registerFilter(Filter::View vFilter)
    {
    LongArray::Handle hlaFilter = getFilterArray();
    if (hlaFilter->isEmpty())
        {
        hlaFilter->set(1, vFilter);
        return 1;
        }
    else
        {
        return hlaFilter->add(vFilter);
        }
    }

AbstractPartialResponse::Handle RemoteNamedCache::BinaryCache::keySetPage (
        Binary::View vBinCookie) const
    {

    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();

    QueryRequest::Handle hRequest = cast<QueryRequest::Handle>
            (vFactory->createMessage(QueryRequest::type_id));

    hRequest->setCookie(vBinCookie);
    hRequest->setKeysOnly(true);

    Request::Status::Handle     hStatus   = hChannel->send(
            (Request::Handle)hRequest);

    AbstractPartialResponse::Handle hResponse = cast<AbstractPartialResponse::Handle>(
            hStatus->waitForResponse());

    processResponse(hResponse);

    return hResponse;
    }

Object::Holder RemoteNamedCache::BinaryCache::processResponse(
        AbstractPofResponse::Handle hResponse) const
    {
    if (hResponse->isFailure())
        {
        Object::Holder ohResult = hResponse->getResult();
        if (instanceof<Exception::Handle>(ohResult))
            {
            COH_THROW(cast<Exception::Handle>(ohResult));
            }
        else if (instanceof<Exception::View>(ohResult))
            {
            COH_THROW(cast<Exception::View>(ohResult));
            }
        else
            {
            COH_THROW_STREAM (RuntimeException,
                    "received error: " << ohResult);
            }
        }
    else
        {
        return hResponse->getResult();
        }
    }

Set::View RemoteNamedCache::BinaryCache::query(Filter::View vFilter,
        bool fKeysOnly) const
    {
    Channel::Handle                hChannel   = ensureChannel();
    Protocol::MessageFactory::View vFactory   = hChannel->getMessageFactory();
    Binary::View                   vBinCookie = NULL;
    Set::View                      vSetResult = NULL;
    List::Handle                   hListPart  = NULL;

    do
        {
        QueryRequest::Handle hRequest = cast<QueryRequest::Handle>
                (vFactory->createMessage(QueryRequest::type_id));

        hRequest->setCookie(vBinCookie);
        hRequest->setFilter(vFilter);
        hRequest->setKeysOnly(fKeysOnly);
        if (instanceof<LimitFilter::View>(vFilter))
            {
            hRequest->setFilterCookie(cast<LimitFilter::View>(vFilter)->getCookie());
            }

        AbstractPartialResponse::Handle hResponse =
                cast<AbstractPartialResponse::Handle>(hChannel->send(
                        (Request::Handle) hRequest)->waitForResponse());
        Set::View vSet = cast<Set::View>(processResponse(hResponse));

        if (vSetResult == NULL || vSetResult->isEmpty())
            {
            // first non-empty result set
            vSetResult = vSet;
            }
        else if (vSet == NULL || vSet->isEmpty())
            {
            // empty result set; nothing to do
            }
        else
            {
            // additional non-empty result set
            if (hListPart == NULL)
                {
                // start recording each result set
                hListPart = ArrayList::create();
                hListPart->add(vSetResult->toArray());
                }
            hListPart->add(vSet->toArray());
            }

        if (instanceof<LimitFilter::View>(vFilter))
            {
            LimitFilter::View vFilterLimit = cast<LimitFilter::View>(vFilter);
            PartialResponse::View vPartialResponse = cast<PartialResponse::View>(hResponse);
            LimitFilter::View vFilterReturned = cast<LimitFilter::View>(vPartialResponse->getFilter());
            
            // update LimitFilter with the state of the returned LimitFilter/cookie
            vFilterLimit->setBottomAnchor(vFilterReturned->getBottomAnchor());
            vFilterLimit->setTopAnchor(vFilterReturned->getTopAnchor());
            vFilterLimit->setCookie(vPartialResponse->getFilterCookie());
            }
        else
            {
            vBinCookie = hResponse->getCookie();
            }
        }
    while (vBinCookie != NULL);

    Set::View vSetReturn = hListPart == NULL
        ? vSetResult
        : (Set::View) ReadOnlyMultiList::create(hListPart->toArray());

    return vSetReturn == NULL
        ? vSetReturn
        : fKeysOnly
            ? (Set::View) ConverterCollections::ConverterSet::create(
                    vSetReturn,
                    NullImplementation::getConverter(),
                    getConverterBinaryToUndecoratedBinary())
            : (Set::View) ConverterCollections::ConverterSet::create(
                    vSetReturn,
                    ConverterCollections::EntryConverter::create(
                            NullImplementation::getConverter(),
                            NullImplementation::getConverter()),
                    ConverterCollections::EntryConverter::create(
                            getConverterBinaryToUndecoratedBinary(),
                            NullImplementation::getConverter()));
    }

// ----- accessors ------------------------------------------------------

RemoteNamedCache::View RemoteNamedCache::BinaryCache::getRemoteNamedCache() const
    {
    return m_whCache;
    }

RemoteNamedCache::ConverterBinaryToDecoratedBinary::View
        RemoteNamedCache::BinaryCache::getConverterBinaryToDecoratedBinary() const
    {
    return m_hConverterBinaryToDecoratedBinary;
    }

RemoteNamedCache::ConverterBinaryToDecoratedBinary::Handle
        RemoteNamedCache::BinaryCache::getConverterBinaryToDecoratedBinary()
    {
    return m_hConverterBinaryToDecoratedBinary;
    }

void RemoteNamedCache::BinaryCache::setConverterBinaryToDecoratedBinary(
        RemoteNamedCache::ConverterBinaryToDecoratedBinary::Handle hConverter)
    {
    initialize(m_hConverterBinaryToDecoratedBinary, hConverter);
    }

RemoteNamedCache::ConverterBinaryToUndecoratedBinary::View
        RemoteNamedCache::BinaryCache::getConverterBinaryToUndecoratedBinary() const
    {
    return m_vConverterBinaryToUndecoratedBinary;
    }

void RemoteNamedCache::BinaryCache::setConverterBinaryToUndecoratedBinary(
        RemoteNamedCache::ConverterBinaryToUndecoratedBinary::View vConv)
    {
    initialize(m_vConverterBinaryToUndecoratedBinary, vConv);
    }

void RemoteNamedCache::BinaryCache::setChannel(Channel::Handle hChannel)
    {
    initialize(f_hChannel, hChannel);
    getConverterBinaryToDecoratedBinary()->setSerializer(hChannel->getSerializer());
    }

Channel::Handle RemoteNamedCache::BinaryCache::getChannel() const
    {
    return f_hChannel;
    }

MapListenerSupport::Handle RemoteNamedCache::BinaryCache::getMapListenerSupport()
    {
    return f_hListenerSupport;
    }

MapListenerSupport::View RemoteNamedCache::BinaryCache::getMapListenerSupport() const
    {
    return f_hListenerSupport;
    }

LongArray::Handle RemoteNamedCache::BinaryCache::getFilterArray()
    {
    return f_hlaFilters;
    }

void RemoteNamedCache::BinaryCache::setEventDispatcher(
        QueueProcessor::Handle hDispacher)
    {
    initialize(f_hEventDispatcher, hDispacher);
    }

QueueProcessor::Handle RemoteNamedCache::BinaryCache::getEventDispatcher()
    {
    return f_hEventDispatcher;
    }

QueueProcessor::View RemoteNamedCache::BinaryCache::getEventDispatcher() const
    {
    return f_hEventDispatcher;
    }

// ----- EntrySet inner class -------------------------------------------

size32_t RemoteNamedCache::BinaryCache::EntrySet::size() const
    {
    return m_whCache->size();
    }

bool RemoteNamedCache::BinaryCache::EntrySet::isEmpty() const
    {
    return m_whCache->isEmpty();
    }

bool RemoteNamedCache::BinaryCache::EntrySet::contains(Object::View v) const
    {
    if (instanceof<Map::Entry::View>(v))
        {
        Map::Entry::View vEntry  = cast<Map::Entry::View>(v);
        BinaryCache::View vCache = m_whCache;
        Object::View      vKey   = vEntry->getKey();

        // REVIEW: This should really be done using a dedicated RNC method
        return vCache->containsKey(vKey) &&
               Object::equals(vCache->get(vKey), vEntry->getValue());
        }
    return false;
    }

bool RemoteNamedCache::BinaryCache::EntrySet::containsAll(Collection::View vCol) const
    {
    for (Iterator::Handle hIter = vCol->iterator(); hIter-> hasNext(); )
        {
        if (!contains(hIter->next()))
            {
            return false;
            }
        }
    return true;
    }

Iterator::Handle RemoteNamedCache::BinaryCache::EntrySet::iterator() const
    {
    return EntryIterator::create((BinaryCache::View) m_whCache);
    }

Muterator::Handle RemoteNamedCache::BinaryCache::EntrySet::iterator()
    {
    return EntryIterator::create((BinaryCache::Handle) m_whCache);
    }

ObjectArray::Handle RemoteNamedCache::BinaryCache::EntrySet::toArray(
        ObjectArray::Handle hao) const
    {
    while (true)
        {
        try
            {
            return Collections::toArray(this, hao);
            }
        catch (ConcurrentModificationException::View) {}
        }
    }

bool RemoteNamedCache::BinaryCache::EntrySet::add(Object::Holder /*oh*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

bool RemoteNamedCache::BinaryCache::EntrySet::addAll(Collection::View /*vCol*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

bool RemoteNamedCache::BinaryCache::EntrySet::remove(Object::View v)
    {
    if (instanceof<Map::Entry::View>(v))
        {
        BinaryCache::Handle hCache = m_whCache;
        return hCache->removeAll(Collections::singleton(
                hCache->getConverterBinaryToDecoratedBinary()
                ->convert(cast<Map::Entry::View>(v)->getKey())));
        }
    return false;
    }

bool RemoteNamedCache::BinaryCache::EntrySet::removeAll(Collection::View vCol)
    {
    Set::Handle hSetKeys = HashSet::create();
    for (Iterator::Handle hIter = vCol->iterator(); hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        hSetKeys->add(vEntry->getKey());
        }

    BinaryCache::Handle hCache = m_whCache;
    return hCache->removeAll(ConverterCollections::ConverterSet::create(
            hSetKeys, hCache->getConverterBinaryToDecoratedBinary(),
            NullImplementation::getConverter()));
    }

bool RemoteNamedCache::BinaryCache::EntrySet::retainAll(Collection::View vCol)
    {
    BinaryCache::Handle hCache = m_whCache;

    bool fModified = false;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        if (!vCol->contains(vEntry))
            {
            hCache->remove(hCache->getConverterBinaryToDecoratedBinary()
                    ->convert(vEntry->getKey()));
            fModified = true;
            }
        }
    return fModified;
    }

void RemoteNamedCache::BinaryCache::EntrySet::clear()
    {
    m_whCache->clear();
    }

bool RemoteNamedCache::BinaryCache::EntrySet::equals(Object::View v) const
    {
    if (instanceof<Set::View>(v))
        {
        Set::View vSetThat = cast<Set::View>(v);
        return size() == vSetThat->size() && vSetThat->containsAll(this);
        }
    return false;
    }

size32_t RemoteNamedCache::BinaryCache::EntrySet::hashCode() const
    {
    size32_t h = 0;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        Object::View v = hIter->next();
        if (NULL != v)
            {
            h += v->hashCode();
            }
        }
    return h;
    }

// ----- EntryIterator inner class --------------------------------------

bool RemoteNamedCache::BinaryCache::EntrySet::EntryIterator::hasNext() const
    {
    return f_hIterKeys->hasNext();
    }

Object::Holder RemoteNamedCache::BinaryCache::EntrySet::EntryIterator::next()
    {
    Object::View        vKey    = f_hIterKeys->next();
    return m_vEntry = SimpleMapEntry::create(vKey,
            f_ohCache->get(f_ohCache->getConverterBinaryToDecoratedBinary()->convert(vKey)));
    }

void RemoteNamedCache::BinaryCache::EntrySet::EntryIterator::remove()
    {
    BinaryCache::Holder ohCache = f_ohCache;

    if (instanceof<BinaryCache::Handle>(ohCache))
        {
        Map::Entry::View vEntry = m_vEntry;
        if (NULL == vEntry)
            {
            COH_THROW (IllegalStateException::create());
            }

        cast<BinaryCache::Handle>(ohCache)->remove(
                ohCache->getConverterBinaryToDecoratedBinary()
                ->convert(vEntry->getKey()));
        m_vEntry = NULL;
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    }

// ----- KeySet inner class ---------------------------------------------

size32_t RemoteNamedCache::BinaryCache::KeySet::size() const
    {
    return m_whCache->size();
    }

bool RemoteNamedCache::BinaryCache::KeySet::isEmpty() const
    {
    return m_whCache->isEmpty();
    }

bool RemoteNamedCache::BinaryCache::KeySet::contains(Object::View v) const
    {
    return m_whCache->containsKey(v);
    }

bool RemoteNamedCache::BinaryCache::KeySet::containsAll(Collection::View vCol) const
    {
    return m_whCache->containsAll(vCol);
    }

Iterator::Handle RemoteNamedCache::BinaryCache::KeySet::iterator() const
    {
    return PagedIterator::create(KeyAdvancer::create(
            (BinaryCache::View) m_whCache));
    }

Muterator::Handle RemoteNamedCache::BinaryCache::KeySet::iterator()
    {
    return PagedMuterator::create(KeyAdvancer::create(
            (BinaryCache::Handle) m_whCache));
    }

ObjectArray::Handle RemoteNamedCache::BinaryCache::KeySet::toArray(
        ObjectArray::Handle hao) const
    {
    return m_whCache->keySet(NULL)->toArray(hao);
    }

bool RemoteNamedCache::BinaryCache::KeySet::add(Object::Holder /*oh*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

bool RemoteNamedCache::BinaryCache::KeySet::addAll(Collection::View /*vCol*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

bool RemoteNamedCache::BinaryCache::KeySet::remove(Object::View v)
    {
    return m_whCache->removeAll(Collections::singleton(v));
    }

bool RemoteNamedCache::BinaryCache::KeySet::removeAll(Collection::View vCol)
    {
    return m_whCache->removeAll(vCol);
    }

bool RemoteNamedCache::BinaryCache::KeySet::retainAll(Collection::View vCol)
    {
    BinaryCache::Handle hCache = m_whCache;

    bool fModified = false;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        Object::View vKey = hIter->next();
        if (!vCol->contains(vKey))
            {
            hCache->remove(hCache->getConverterBinaryToDecoratedBinary()->convert(vKey));
            fModified = true;
            }
        }
    return fModified;
    }

void RemoteNamedCache::BinaryCache::KeySet::clear()
    {
    m_whCache->clear();
    }

bool RemoteNamedCache::BinaryCache::KeySet::equals(Object::View v) const
    {
    Set::View vSetThat = cast<Set::View>(v, false);
    if (NULL == vSetThat)
        {
        return false;
        }
    else
        {
        Set::View vSetThis = m_whCache->keySet(NULL);
        return vSetThat->size() == vSetThis->size() &&
               vSetThat->containsAll(vSetThis);
        }
    }

size32_t RemoteNamedCache::BinaryCache::KeySet::hashCode() const
    {
    size32_t h = 0;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        Object::View v = hIter->next();
        if (NULL != v)
            {
            h += v->hashCode();
            }
        }
    return h;
    }

// ----- KeyIterator inner class ----------------------------------------

bool RemoteNamedCache::BinaryCache::KeySet::KeyIterator::hasNext() const
    {
    return f_hIterKeys->hasNext();
    }

Object::Holder RemoteNamedCache::BinaryCache::KeySet::KeyIterator::next()
    {
    Object::View vKey = f_hIterKeys->next();
    m_vKey = vKey;

    return vKey;
    }

void RemoteNamedCache::BinaryCache::KeySet::KeyIterator::remove()
    {
    BinaryCache::Holder ohCache = f_ohCache;
    if (instanceof<BinaryCache::Handle>(ohCache))
        {
        cast<BinaryCache::Handle>(ohCache)
                ->remove(ohCache->getConverterBinaryToDecoratedBinary()
                        ->convert(m_vKey), false);
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    }

// ----- KeyAdvancer inner class ----------------------------------------

Collection::Handle RemoteNamedCache::BinaryCache::KeySet::KeyAdvancer::nextPage()
    {
    if (m_fExhausted)
        {
        return NULL;
        }

    AbstractPartialResponse::Handle hResponse  = f_ohCache->keySetPage(m_vCookie);
    Binary::View   vBinCookie = hResponse->getCookie();

    m_vCookie = vBinCookie;
    if (NULL == vBinCookie)
        {
        m_fExhausted = true;
        }
    return cast<Collection::Handle>(hResponse->getResult());
    }

void RemoteNamedCache::BinaryCache::KeySet::KeyAdvancer::remove(Object::View vCurr)
    {
    BinaryCache::Holder ohCache = f_ohCache;
    if (instanceof<BinaryCache::Handle>(ohCache))
        {
        cast<BinaryCache::Handle>(ohCache)->remove(
                ohCache->getConverterBinaryToDecoratedBinary()->convert(vCurr), false);
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    }

// ----- Values inner class ---------------------------------------------

size32_t RemoteNamedCache::BinaryCache::Values::size() const
    {
    return m_whCache->size();
    }

bool RemoteNamedCache::BinaryCache::Values::isEmpty() const
    {
    return m_whCache->isEmpty();
    }

bool RemoteNamedCache::BinaryCache::Values::contains(Object::View v) const
    {
    return m_whCache->containsValue(v);
    }

bool RemoteNamedCache::BinaryCache::Values::containsAll(Collection::View vCol) const
    {
    for (Iterator::Handle hIter = vCol->iterator(); hIter-> hasNext(); )
        {
        if (!contains(hIter->next()))
            {
            return false;
            }
        }
    return true;
    }

Iterator::Handle RemoteNamedCache::BinaryCache::Values::iterator() const
    {
    return ValueIterator::create((BinaryCache::View) m_whCache);
    }

Muterator::Handle RemoteNamedCache::BinaryCache::Values::iterator()
    {
    return ValueIterator::create((BinaryCache::Handle) m_whCache);
    }

ObjectArray::Handle RemoteNamedCache::BinaryCache::Values::toArray(
        ObjectArray::Handle hao) const
    {
    ObjectArray::Handle haoh = hao;

    do
        {
        size32_t         c     = size();
        Iterator::Handle hIter = iterator();

        if (haoh == NULL || haoh->length < c)
            {
            haoh = ObjectArray::create(c);
            }
        else if (haoh->length > c)
            {
            haoh[c] = NULL;
            }

        for (size32_t i = 0; i < c; ++i)
            {
            try
                {
                haoh[i] = hIter->next();
                }
            catch (NoSuchElementException::View)
                {
                haoh = NULL;
                break;
                }
            }
        }
    while (NULL == haoh);

    return haoh;
    }

bool RemoteNamedCache::BinaryCache::Values::add(Object::Holder /*oh*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

bool RemoteNamedCache::BinaryCache::Values::addAll(Collection::View /*vCol*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

bool RemoteNamedCache::BinaryCache::Values::remove(Object::View v)
    {
    BinaryCache::Handle hCache = m_whCache;
    for (Iterator::Handle hIter = hCache->entrySet()->iterator();
         hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        if (Object::equals(vEntry->getValue(), v))
            {
            hCache->remove(hCache->getConverterBinaryToDecoratedBinary()
                    ->convert(vEntry->getKey()));
            return true;
            }
        }
    return false;
    }

bool RemoteNamedCache::BinaryCache::Values::removeAll(Collection::View vCol)
    {
    bool fModified = false;

    BinaryCache::Handle hCache = m_whCache;
    for (Iterator::Handle hIter = hCache->entrySet()->iterator();
         hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        if (vCol->contains(vEntry->getValue()))
            {
            hCache->remove(hCache->getConverterBinaryToDecoratedBinary()
                    ->convert(vEntry->getKey()));
            fModified = true;
            }
        }
    return fModified;
    }

bool RemoteNamedCache::BinaryCache::Values::retainAll(Collection::View vCol)
    {
    bool fModified = false;

    BinaryCache::Handle hCache = m_whCache;
    for (Iterator::Handle hIter = hCache->entrySet()->iterator();
         hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        if (!vCol->contains(vEntry->getValue()))
            {
            hCache->remove(hCache->getConverterBinaryToDecoratedBinary()
                    ->convert(vEntry->getKey()));
            fModified = true;
            }
        }
    return fModified;
    }

void RemoteNamedCache::BinaryCache::Values::clear()
    {
    m_whCache->clear();
    }

bool RemoteNamedCache::BinaryCache::Values::equals(Object::View v) const
    {
    if (instanceof<Collection::View>(v))
        {
        Collection::View vColThat = cast<Collection::View>(v);
        return size() == vColThat->size() && vColThat->containsAll(this);
        }
    return false;
    }

size32_t RemoteNamedCache::BinaryCache::Values::hashCode() const
    {
    size32_t h = 1;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        Object::View v = hIter->next();
        h = 31*h + (NULL == v ? 0 : v->hashCode());
        if (NULL != v)
            {
            h += v->hashCode();
            }
        }
    return h;
    }


// ----- ConverterBinaryToDecoratedBinary inner class -----------------------

RemoteNamedCache::ConverterBinaryToDecoratedBinary::ConverterBinaryToDecoratedBinary(
        RemoteNamedCache::View vCache)
    : m_wvCache(self(), vCache)
    {
    }

Object::Holder RemoteNamedCache::ConverterBinaryToDecoratedBinary::convert(
        Object::Holder oh) const
    {
    if (oh == NULL || m_wvCache->isDeferKeyAssociationCheck())
        {
        return oh;
        }

    Binary::View vBin = cast<Binary::View>(oh);

    if (SerializationHelper::isIntDecorated(vBin))
        {
        return vBin;
        }

    Binary::View vBinDeco = vBin;

    oh = SerializationHelper::fromBinary(vBin, getSerializer());
    if (instanceof<KeyAssociation::View>(oh))
        {
        oh = cast<KeyAssociation::View>(oh)->getAssociatedKey();
        if (oh != NULL)
            {
            vBinDeco = SerializationHelper::toBinary(oh, getSerializer());
            }
        }

    return SerializationHelper::decorateBinary(vBin, vBinDeco->calculateNaturalPartition(0));
    }


// ----- ConverterBinaryToUndecoratedBinary inner class ---------------------

RemoteNamedCache::ConverterBinaryToUndecoratedBinary::ConverterBinaryToUndecoratedBinary(
        RemoteNamedCache::View vCache)
    : m_wvCache(self(), vCache)
    {
    }

Object::Holder RemoteNamedCache::ConverterBinaryToUndecoratedBinary::convert(
        Object::Holder oh) const
    {
    if (oh == NULL || m_wvCache->isDeferKeyAssociationCheck())
        {
        return oh;
        }

    Binary::View vBin = cast<Binary::View>(oh);

    return SerializationHelper::isIntDecorated(vBin)
            ? SerializationHelper::removeIntDecoration(vBin) : vBin;
    }


// ----- ConverterFromBinary inner class --------------------------------

RemoteNamedCache::ConverterFromBinary::ConverterFromBinary()
    {
    }

Object::Holder RemoteNamedCache::ConverterFromBinary::convert(
        Object::Holder oh) const
    {
    return oh == NULL
            ? oh
            : SerializationHelper::fromBinary(cast<Binary::View>(oh), getSerializer());
    }


// ----- ConverterKeyToBinary inner class -----------------------------------

RemoteNamedCache::ConverterKeyToBinary::ConverterKeyToBinary(
        RemoteNamedCache::View vCache)
    : m_wvCache(self(), vCache)
    {
    }

Object::Holder RemoteNamedCache::ConverterKeyToBinary::convert(
        Object::Holder oh) const
    {
    Binary::View vBin = SerializationHelper::toBinary(oh, getSerializer());
    if (m_wvCache->isDeferKeyAssociationCheck())
        {
        return vBin;
        }

    Binary::View vBinDeco = vBin;
    if (instanceof<KeyAssociation::View>(oh))
        {
        oh = cast<KeyAssociation::View>(oh)->getAssociatedKey();
        if (oh != NULL)
            {
            vBinDeco = SerializationHelper::toBinary(oh, getSerializer());
            }
        }

    return SerializationHelper::decorateBinary(vBin, vBinDeco->calculateNaturalPartition(0));
    }


// ----- ConverterValueToBinary inner class ---------------------------------

RemoteNamedCache::ConverterValueToBinary::ConverterValueToBinary()
    {
    }

Object::Holder RemoteNamedCache::ConverterValueToBinary::convert(
        Object::Holder oh) const
    {
    return SerializationHelper::toBinary(oh, getSerializer());
    }


// ----- ValueIterator inner class --------------------------------------

bool RemoteNamedCache::BinaryCache::Values::ValueIterator::hasNext() const
    {
    return f_hIterKeys->hasNext();
    }

Object::Holder RemoteNamedCache::BinaryCache::Values::ValueIterator::next()
    {
    return m_ohValue = f_ohCache->get(
            f_ohCache->getConverterBinaryToDecoratedBinary()->convert(f_hIterKeys->next()));
    }

void RemoteNamedCache::BinaryCache::Values::ValueIterator::remove()
    {
    Muterator::Handle hMiter = cast<Muterator::Handle>(f_hIterKeys, false);
    if (NULL == hMiter)
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    hMiter->remove();
    }


// ----- inner class: ConverterListener ---------------------------------

RemoteNamedCache::ConverterListener::ConverterListener()
        : f_vConverter(self()), f_hNamedCache(self()), f_hListener(self())
    {
    }

void RemoteNamedCache::ConverterListener::entryInserted(
        MapEvent::View vEvent)
    {
    dispatch(vEvent);
    }

void RemoteNamedCache::ConverterListener::entryUpdated(
        MapEvent::View vEvent)
    {
    dispatch(vEvent);
    }

void RemoteNamedCache::ConverterListener::entryDeleted(
        MapEvent::View vEvent)
    {
    dispatch(vEvent);
    }

bool RemoteNamedCache::ConverterListener::equals(Object::View v) const
    {
    RemoteNamedCache::ConverterListener::View vThat =
            cast<RemoteNamedCache::ConverterListener::View>(v, false);
    return vThat != NULL && getListener()->equals(vThat->getListener());
    }

size32_t RemoteNamedCache::ConverterListener::hashCode() const
    {
    return getListener()->hashCode();
    }

TypedHandle<const String> RemoteNamedCache::ConverterListener::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) << ": " << getListener());
    }

void RemoteNamedCache::ConverterListener::setConverter(
        Converter::View vConverter)
    {
    initialize(f_vConverter, vConverter);
    }

Converter::View RemoteNamedCache::ConverterListener::getConverter() const
    {
    return f_vConverter;
    }

void RemoteNamedCache::ConverterListener::setNamedCache(
        NamedCache::Handle hNamedCache)
    {
    initialize(f_hNamedCache, hNamedCache);
    }

NamedCache::Handle RemoteNamedCache::ConverterListener::getNamedCache()
    {
    return f_hNamedCache;
    }

NamedCache::View RemoteNamedCache::ConverterListener::getNamedCache() const
    {
    return f_hNamedCache;
    }

void RemoteNamedCache::ConverterListener::setListener(
        MapListener::Handle hListener)
    {
    initialize(f_hListener, hListener);
    }

MapListener::Handle RemoteNamedCache::ConverterListener::getListener()
    {
    return f_hListener;
    }

MapListener::View RemoteNamedCache::ConverterListener::getListener() const
    {
    return f_hListener;
    }

void RemoteNamedCache::ConverterListener::dispatch(MapEvent::View vEvent)
    {
    Converter::View vConv = getConverter();
    MapListenerSupport::convertEvent(
            vEvent, getNamedCache(), vConv, vConv)->dispatch(getListener());
    }


// ----- NamedCache interface -----------------------------------------------

String::View RemoteNamedCache::getCacheName() const
    {
    return f_vsCacheName;
    }

CacheService::Handle RemoteNamedCache::getCacheService()
    {
    return m_whCacheService;
    }

CacheService::View RemoteNamedCache::getCacheService() const
    {
    return m_whCacheService;
    }

bool RemoteNamedCache::isActive() const
    {
    return getBinaryCache()->isActive();
    }

void RemoteNamedCache::release()
    {
    getCacheService()->releaseCache(this);
    }

void RemoteNamedCache::destroy()
    {
    getCacheService()->destroyCache(this);
    }

// ----- CacheMap interface -------------------------------------------------

Map::View RemoteNamedCache::getAll(Collection::View vColKeys) const
    {
    return getConverterCache()->getAll(vColKeys);
    }

Object::Holder RemoteNamedCache::put(Object::View vKey,
    Object::Holder ohValue, int64_t cMillis)
    {
    return getConverterCache()->put(vKey, ohValue, cMillis);
    }

// ----- ConcurrentMap interface --------------------------------------------

bool RemoteNamedCache::lock(Object::View vKey, int64_t cWait) const
    {
    printLockDeprecatedMessage();
    if (ConcurrentMap::getLockAll() == vKey)
        {
        COH_THROW (UnsupportedOperationException::create(
                "RemoteNamedCache does not support LOCK_ALL"));
        }

    return getConverterCache()->lock(vKey, cWait);
    }

bool RemoteNamedCache::lock(Object::View vKey) const
    {
    return lock(vKey, 0);
    }

bool RemoteNamedCache::unlock(Object::View vKey) const
    {
    printLockDeprecatedMessage();
    if (ConcurrentMap::getLockAll() == vKey)
        {
        COH_THROW (UnsupportedOperationException::create(
                "RemoteNamedCache does not support LOCK_ALL"));
        }

    return getConverterCache()->unlock(vKey);
    }


// ----- InvocableMap interface ---------------------------------------------

Object::Holder RemoteNamedCache::invoke(Object::View vKey,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    return getConverterCache()->invoke(vKey, hAgent);
    }

Map::View RemoteNamedCache::invokeAll(Collection::View vCollKeys,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    return getConverterCache()->invokeAll(vCollKeys, hAgent);
    }

Map::View RemoteNamedCache::invokeAll(Filter::View vFilter,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    return getConverterCache()->invokeAll(vFilter, hAgent);
    }

Object::Holder RemoteNamedCache::aggregate(Collection::View vCollKeys,
        InvocableMap::EntryAggregator::Handle hAgent) const
    {
    return getConverterCache()->aggregate(vCollKeys, hAgent);
    }

Object::Holder RemoteNamedCache::aggregate(Filter::View vFilter,
        InvocableMap::EntryAggregator::Handle hAgent) const
    {
    return getConverterCache()->aggregate(vFilter, hAgent);
    }


// ----- QueryMap interface -------------------------------------------------

Set::View RemoteNamedCache::keySet(Filter::View vFilter) const
    {
    return getConverterCache()->keySet(vFilter);
    }

Set::View RemoteNamedCache::entrySet(Filter::View vFilter) const
    {
    // COH-2717
    if (instanceof<LimitFilter::View>(vFilter))
        {
        cast<LimitFilter::View>(vFilter)->setComparator(NULL); 
        }

    return getConverterCache()->entrySet(vFilter);
    }

Set::View RemoteNamedCache::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    if (NULL == vComparator)
           {
           vComparator = SafeComparator::getInstance();
           }

    LimitFilter::View vFilterLimit = NULL;
    if (instanceof<LimitFilter::View>(vFilter))
        {
        vFilterLimit = cast<LimitFilter::View>(vFilter);
        vFilterLimit->setComparator(vComparator);
        }

    Set::View vSet = getConverterCache()->entrySet(vFilter);
    if (vSet->size() <= 1)
        {
        return vSet;
        }

    ObjectArray::Handle haoEntry   = vSet->toArray();
    Comparator::Handle  hCompEntry = EntryComparator::create(vComparator);

    Arrays::sort(haoEntry, hCompEntry);

    if (vFilterLimit == NULL)
        {
        return ReadOnlyArrayList::create(haoEntry);
        }
    else
        {
        LimitFilter::View vTmpFilter = LimitFilter::create(vFilterLimit->getFilter(), vFilterLimit->getPageSize());
        vTmpFilter->setComparator(hCompEntry);
        vTmpFilter->setTopAnchor(vFilterLimit->getTopAnchor());
        vTmpFilter->setBottomAnchor(vFilterLimit->getBottomAnchor());
        vTmpFilter->setCookie(vFilterLimit->getCookie());
        vSet = ReadOnlyArrayList::create(vTmpFilter->extractPage(haoEntry));
        vFilterLimit->setTopAnchor(vTmpFilter->getTopAnchor());
        vFilterLimit->setBottomAnchor(vTmpFilter->getBottomAnchor());
        vFilterLimit->setCookie(vTmpFilter->getCookie());
        return vSet;
        }
    }

void RemoteNamedCache::addIndex(ValueExtractor::View vExtractor,
        bool fOrdered, Comparator::View vComparator)
    {
    return getConverterCache()->addIndex(vExtractor, fOrdered, vComparator);
    }

void RemoteNamedCache::removeIndex(ValueExtractor::View vExtractor)
    {
    return getConverterCache()->removeIndex(vExtractor);
    }


// ----- Map interface ------------------------------------------------------

size32_t RemoteNamedCache::size() const
    {
    return getBinaryCache()->size();
    }

bool RemoteNamedCache::isEmpty() const
    {
    return getBinaryCache()->isEmpty();
    }

bool RemoteNamedCache::containsKey(Object::View vKey) const
    {
    return getConverterCache()->containsKey(vKey);
    }

bool RemoteNamedCache::containsValue(Object::View vValue) const
    {
    return getConverterCache()->containsValue(vValue);
    }

Object::Holder RemoteNamedCache::get(Object::View vKey) const
    {
    return getConverterCache()->get(vKey);
    }

Object::Holder RemoteNamedCache::get(Object::View vKey)
    {
    return ((const RemoteNamedCache*) this)->get(vKey);
    }

Object::Holder RemoteNamedCache::put(Object::View vKey,
        Object::Holder ohValue)
    {
    return getConverterCache()->put(vKey, ohValue);
    }

Object::Holder RemoteNamedCache::remove(Object::View vKey)
    {
    return getConverterCache()->remove(vKey);
    }

void RemoteNamedCache::putAll(Map::View vMap)
    {
    getConverterCache()->putAll(vMap);
    }

void RemoteNamedCache::clear()
    {
    getBinaryCache()->clear();
    }

Set::View RemoteNamedCache::keySet() const
    {
    return getConverterCache()->keySet();
    }

Set::Handle RemoteNamedCache::keySet()
    {
    return getConverterCache()->keySet();
    }

Collection::View RemoteNamedCache::values() const
    {
    return getConverterCache()->values();
    }

Collection::Handle RemoteNamedCache::values()
    {
    return getConverterCache()->values();
    }

Set::View RemoteNamedCache::entrySet() const
    {
    return getConverterCache()->entrySet();
    }

Set::Handle RemoteNamedCache::entrySet()
    {
    return getConverterCache()->entrySet();
    }


// ----- CacheStore interface -----------------------------------------------

void RemoteNamedCache::store(Object::View vKey, Object::Holder ohValue)
    {
    getBinaryCache()->store(getConverterKeyToBinary()->convert(vKey),
            getConverterValueToBinary()->convert(ohValue));
    }

void RemoteNamedCache::storeAll(Map::View vMapEntries)
    {
    getConverterCache()->putAll(vMapEntries);
    }

void RemoteNamedCache::erase(Object::View vKey)
    {
    getBinaryCache()->remove(getConverterKeyToBinary()->convert(vKey));
    }

void RemoteNamedCache::eraseAll(Collection::View vColKeys)
    {
    getBinaryCache()->removeAll(ConverterCollections::ConverterCollection::create(
            vColKeys, getConverterKeyToBinary(), getConverterFromBinary()));
    }


// ----- ObservableMap interface --------------------------------------------

void RemoteNamedCache::addKeyListener(MapListener::Handle hListener,
        Object::View vKey, bool fLite)
    {
    getBinaryCache()->addKeyListener(instantiateConverterListener(hListener),
            getConverterKeyToBinary()->convert(vKey), fLite);
    }

void RemoteNamedCache::removeKeyListener(MapListener::Handle hListener,
        Object::View vKey)
    {
    getBinaryCache()->removeKeyListener(
            instantiateConverterListener(hListener),
            getConverterKeyToBinary()->convert(vKey));
    }

void RemoteNamedCache::addMapListener(MapListener::Handle hListener)
    {
    getBinaryCache()->addMapListener(instantiateConverterListener(hListener));
    }

void RemoteNamedCache::removeMapListener(MapListener::Handle hListener)
    {
    getBinaryCache()->removeMapListener(instantiateConverterListener(hListener));
    }

void RemoteNamedCache::addFilterListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    if (instanceof<NamedCacheDeactivationListener::Handle>(hListener))
        {
        RemoteNamedCache::getDeactivationListeners()->add(hListener);
        }
    else
        {
        if (instanceof<InKeySetFilter::View>(vFilter))
            {
            (cast<InKeySetFilter::View>(vFilter))->ensureConverted(getConverterKeyToBinary());
            }

        getBinaryCache()->addFilterListener(instantiateConverterListener(hListener),
                vFilter, fLite);
        }
    }

void RemoteNamedCache::removeFilterListener(MapListener::Handle hListener,
        Filter::View vFilter)
    {
    if (instanceof<NamedCacheDeactivationListener::Handle>(hListener))
        {
        RemoteNamedCache::getDeactivationListeners()->remove(hListener);
        }
    else
        {
        if (instanceof<InKeySetFilter::View>(vFilter))
            {
            (cast<InKeySetFilter::View>(vFilter))->ensureConverted(getConverterKeyToBinary());
            }

        getBinaryCache()->removeFilterListener(
                instantiateConverterListener(hListener), vFilter);
        }
    }


// ----- Describable interface ----------------------------------------------

String::View RemoteNamedCache::getDescription() const
    {
    return COH_TO_STRING("NamedCache=" << getCacheName() <<
        ", Service=" << getCacheService()->getInfo()->getServiceName());
    }


// ----- accessors ----------------------------------------------------------

RemoteNamedCache::BinaryCache::View RemoteNamedCache::getBinaryCache() const
    {
    return f_hBinaryCache;
    }

RemoteNamedCache::BinaryCache::Handle RemoteNamedCache::getBinaryCache()
    {
    return f_hBinaryCache;
    }

void RemoteNamedCache::setBinaryCache(RemoteNamedCache::BinaryCache::Handle hCache)
    {
    initialize(f_hBinaryCache, hCache);
    }

void RemoteNamedCache::setCacheName(String::View vsName)
    {
    initialize(f_vsCacheName, vsName);
    }

void RemoteNamedCache::setCacheService(CacheService::Handle hService)
    {
    m_whCacheService = hService;
    }

Channel::Handle RemoteNamedCache::getChannel() const
    {
    return m_hChannel;
    }

void RemoteNamedCache::setChannel(Channel::Handle hChannel)
    {
    m_hChannel = hChannel;
    }

NamedCache::View RemoteNamedCache::getConverterCache() const
    {
    return f_hConverterCache;
    }

NamedCache::Handle RemoteNamedCache::getConverterCache()
    {
    return f_hConverterCache;
    }

void RemoteNamedCache::setConverterCache(NamedCache::Handle hCache)
    {
    initialize(f_hConverterCache, hCache);
    }

RemoteNamedCache::ConverterBinaryToDecoratedBinary::View
        RemoteNamedCache::getConverterBinaryToDecoratedBinary() const
    {
    return m_hConverterBinaryToDecoratedBinary;
    }

RemoteNamedCache::ConverterBinaryToDecoratedBinary::Handle
        RemoteNamedCache::getConverterBinaryToDecoratedBinary()
    {
    return m_hConverterBinaryToDecoratedBinary;
    }

void RemoteNamedCache::setConverterBinaryToDecoratedBinary(
        RemoteNamedCache::ConverterBinaryToDecoratedBinary::Handle hConverter)
    {
    initialize(m_hConverterBinaryToDecoratedBinary, hConverter);
    }

RemoteNamedCache::ConverterBinaryToUndecoratedBinary::View
        RemoteNamedCache::getConverterBinaryToUndecoratedBinary() const
    {
    return m_hConverterBinaryToUndecoratedBinary;
    }

RemoteNamedCache::ConverterBinaryToUndecoratedBinary::Handle
        RemoteNamedCache::getConverterBinaryToUndecoratedBinary()
    {
    return m_hConverterBinaryToUndecoratedBinary;
    }

void RemoteNamedCache::setConverterBinaryToUndecoratedBinary(
        RemoteNamedCache::ConverterBinaryToUndecoratedBinary::Handle hConverter)
    {
    initialize(m_hConverterBinaryToUndecoratedBinary, hConverter);
    }

RemoteNamedCache::ConverterFromBinary::View
        RemoteNamedCache::getConverterFromBinary() const
    {
    return f_hConverterFromBinary;
    }

RemoteNamedCache::ConverterFromBinary::Handle
        RemoteNamedCache::getConverterFromBinary()
    {
    return f_hConverterFromBinary;
    }

void RemoteNamedCache::setConverterFromBinary(
        RemoteNamedCache::ConverterFromBinary::Handle hConverter)
    {
    initialize(f_hConverterFromBinary, hConverter);
    }

RemoteNamedCache::ConverterKeyToBinary::View
        RemoteNamedCache::getConverterKeyToBinary() const
    {
    return f_hConverterKeyToBinary;
    }

RemoteNamedCache::ConverterKeyToBinary::Handle
        RemoteNamedCache::getConverterKeyToBinary()
    {
    return f_hConverterKeyToBinary;
    }

void RemoteNamedCache::setConverterKeyToBinary(
        RemoteNamedCache::ConverterKeyToBinary::Handle hConverter)
    {
    initialize(f_hConverterKeyToBinary, hConverter);
    }

RemoteNamedCache::ConverterValueToBinary::View
        RemoteNamedCache::getConverterValueToBinary() const
    {
    return f_hConverterValueToBinary;
    }

RemoteNamedCache::ConverterValueToBinary::Handle
        RemoteNamedCache::getConverterValueToBinary()
    {
    return f_hConverterValueToBinary;
    }

void RemoteNamedCache::setConverterValueToBinary(
        RemoteNamedCache::ConverterValueToBinary::Handle hConverter)
    {
    initialize(f_hConverterValueToBinary, hConverter);
    }

Listeners::Handle RemoteNamedCache::getDeactivationListeners()
    {
    return m_hDeactivationListeners;
    }

void RemoteNamedCache::setDeactivationListeners(Listeners::Handle hListeners)
    {
    m_hDeactivationListeners = hListeners;
    }

bool RemoteNamedCache::isDeferKeyAssociationCheck() const
    {
    return fDeferKeyAssociationCheck;
    }

void RemoteNamedCache::setDeferKeyAssociationCheck(bool fDefer)
    {
    fDeferKeyAssociationCheck = fDefer;
    }

QueueProcessor::Handle RemoteNamedCache::getEventDispatcher()
    {
    return f_hEventDispatcher;
    }

QueueProcessor::View RemoteNamedCache::getEventDispatcher() const
    {
    return f_hEventDispatcher;
    }

void RemoteNamedCache::setEventDispatcher(QueueProcessor::Handle hDispacher)
    {
    initialize(f_hEventDispatcher, hDispacher);

    getBinaryCache()->setEventDispatcher(hDispacher);
    }


// ----- helper methods -----------------------------------------------------

MapListener::Handle RemoteNamedCache::instantiateConverterListener(
        MapListener::Handle hListener)
    {
    COH_ENSURE_PARAM(hListener);

    if (instanceof<MapTriggerListener::Handle>(hListener))
        {
        return hListener;
        }

    RemoteNamedCache::ConverterListener::Handle hConvListener =
            RemoteNamedCache::ConverterListener::create();
    hConvListener->setConverter(getConverterFromBinary());
    hConvListener->setListener(hListener);
    hConvListener->setNamedCache(this);

    if (instanceof<MapListenerSupport::PrimingListener::Handle>(hListener))
        {
        return (MapListener::Handle) MapListenerSupport::WrapperPrimingListener::create(hConvListener);
        }
    else
        {
        return instanceof<MapListenerSupport::SynchronousListener::Handle>(hListener) ?
                (MapListener::Handle) MapListenerSupport::WrapperSynchronousListener::create(hConvListener) :
                (MapListener::Handle) hConvListener;
        }
    }

RemoteNamedCache::BinaryCache::Handle RemoteNamedCache::instantiateBinaryCache()
    {
    return BinaryCache::create(this);
    }

void RemoteNamedCache::printLockDeprecatedMessage() const
    {
    if (!m_fLockDeprecateWarned)
        {
        COH_LOG ("Using the lock API from a Coherence*Extend client is deprecated and will be removed in a future release",
            Logger::level_warning);
        m_fLockDeprecateWarned = true;
        }
    }

COH_CLOSE_NAMESPACE4
