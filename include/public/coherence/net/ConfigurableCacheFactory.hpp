/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONFIGURABLE_CACHE_FACTORY_HPP
#define COH_CONFIGURABLE_CACHE_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/run/xml/XmlConfigurable.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::run::xml::XmlConfigurable;

/**
* An interface for XML driven %cache factory.
*
* @see DefaultConfigurableCacheFactory
*/
class COH_EXPORT ConfigurableCacheFactory
    : public interface_spec<ConfigurableCacheFactory,
        implements<XmlConfigurable> >
    {
    // ----- ConfigurableCacheFactory interface -----------------------------

    public:
        /**
        * Ensure a %cache for the given name using the corresponding XML
        * configuration.
        *
        * @param vsCacheName the %cache name
        *
        * @return a handle to a NamedCache created according to the
        *         configuration XML
        */
        virtual NamedCache::Handle ensureCache(String::View vsCacheName) = 0;

        /**
        * Releases and destroys the specified NamedCache.
        *
        * <b>Warning:</b> This method is used to completely destroy the
        * specified %cache across the cluster. All references in the entire
        * cluster to this %cache will be invalidated, the cached data will be
        * cleared, and all resources will be released.
        *
        * @param hCache the NamedCache object to be destroyed
        *
        * @see releaseCache
        */
        virtual void destroyCache(NamedCache::Handle hCache) = 0;

        /**
        * Release local resources associated with the specified instance of
        * the %cache.
        *
        * Releasing a NamedCache reference makes it no longer usable, but
        * does not affect the content of the %cache. In other words, all other
        * references to the %cache will still be valid, and the %cache data is
        * not affected by releasing the reference.
        *
        * The reference that is released using this method can no longer be
        * used; any attempt to use the reference will result in an exception.
        *
        * @param hCache the NamedCache object to be released
        *
        * @see destroyCache
        */
        virtual void releaseCache(NamedCache::Handle hCache) = 0;

        /**
        * Ensure a service for the given name using the corresponding XML
        * configuration.
        *
        * @param vsServiceName the service name
        *
        * @return a handle to a service created according to the
        *         configuration XML
        */
        virtual Service::Handle ensureService(String::View vsServiceName) = 0;

        /**
        * Shutdown all services related to this ConfigurableCacheFactory.
        */
        virtual void shutdown() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONFIGURABLE_CACHE_FACTORY_HPP
