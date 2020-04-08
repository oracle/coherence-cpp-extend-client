/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAMED_CACHE_FACTORY_HPP
#define COH_NAMED_CACHE_FACTORY_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofMessageFactory;


/**
* MessageFactory for version 1 of the NamedCache Protocol.
*
* The type identifiers of the Message classes instantiated by this
* MessageFactory are organized as follows:
*
* Map (1-10):
*
* (1) SizeRequest
* (2) ContainsKeyRequest
* (3) ContainsValueRequest
* (4) GetRequest
* (5) PutRequest
* (6) RemoveRequest
* (7) PutAllRequest
* (8) ClearRequest
* (9) ContainsAllRequest
* (10) RemoveAllRequest
*
* ObservableMap (11-20):
*
* (11) ListenerKeyRequest
* (12) ListenerFilterRequest
* (13) MapEvent
*
* CacheMap (21-30):
*
* (21) GetAllRequest
*
* ConcurrentMap (31-40):
*
* (31) LockRequest
*
* QueryMap (41-50):
*
* (41) QueryRequest
* (42) IndexRequest
*
* InvocableMap (51-60):
*
* (51) AggregateAllRequest
* (52) AggregateFilterRequest
* (53) InvokeRequest
* (54) InvokeAllRequest
* (55) InvokeFilterRequest
*
* @author jh  2008.02.18
*/
class COH_EXPORT NamedCacheFactory
    : public class_spec<NamedCacheFactory,
        extends<AbstractPofMessageFactory> >
    {
    friend class factory<NamedCacheFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheFactory instance.
        */
        NamedCacheFactory();

    private:
        /**
        * Blocked copy constructor.
        */
        NamedCacheFactory(const NamedCacheFactory&);
		

    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessage::Handle instantiateMessage(int32_t nType) const;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_NAMED_CACHE_FACTORY_HPP
