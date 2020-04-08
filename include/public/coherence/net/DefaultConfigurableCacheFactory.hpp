/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DEFAULT_CONFIGURABLE_CACHE_FACTORY_HPP
#define COH_DEFAULT_CONFIGURABLE_CACHE_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/net/ConfigurableCacheFactory.hpp"
#include "coherence/net/OperationalContext.hpp"

#include "coherence/net/cache/AbstractBundler.hpp"
#include "coherence/net/cache/BundlingNamedCache.hpp"
#include "coherence/net/cache/CacheLoader.hpp"
#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include "coherence/run/xml/XmlDocument.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/run/xml/XmlValue.hpp"

#include "coherence/util/Map.hpp"
#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::net::cache::AbstractBundler;
using coherence::net::cache::BundlingNamedCache;
using coherence::net::cache::CacheLoader;
using coherence::net::cache::CacheMap;
using coherence::net::cache::ContinuousQueryCache;
using coherence::run::xml::XmlDocument;
using coherence::run::xml::XmlElement;
using coherence::run::xml::XmlValue;
using coherence::util::Map;
using coherence::util::MapListener;


/**
* DefaultConfigurableCacheFactory provides a facility to access caches
* declared in a "cache-config.dtd" compliant configuration file.
*
* This class is designed to be easily extendable with a collection of factory
* methods allowing subclasses to customize it by overriding any subset of
* %cache instantiation routines or even allowing addition of custom schemes.
*
* There are various ways of using this factory:
* <pre>
*   ConfigurableCacheFactory::Handle factory =
*       DefaultConfigurableCacheFactory::create(sPath);
*   ...
*   NamedCache::Handle cacheOne = factory->ensureCache("one");
*   NamedCache::Handle cacheTwo = factory->ensureCache("two");
* </pre>
* This approach allows an easy customization by extending the
* DefaultConfigurableCacheFactory and changing the instantiation line:
* <pre>
*   ConfigurableCacheFactory::Handle factory =
*       CustomConfigurableCacheFactory::create();
*   ...
* </pre>
*
* Another option is using the static version of the "ensureCache" call:
* <pre>
*   NamedCache cacheOne = CacheFactory::getCache("one");
*   NamedCache cacheTwo = CacheFactory::getCache("two");
* </pre>
* which uses an instance of ConfigurableCacheFactory obtained by
* {@link CacheFactory#getConfigurableCacheFactory()}.
*
* @see CacheFactory#getCache(String::View)
*
* @author mf  2008.04.08
*/
class COH_EXPORT DefaultConfigurableCacheFactory
    : public class_spec<DefaultConfigurableCacheFactory,
        extends<Object>,
        implements<ConfigurableCacheFactory> >
    {
    friend class factory<DefaultConfigurableCacheFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new %cache factory.
        *
        * @param vsFile  the name of the configuration file to load relative
        *                to the current working directory, or NULL for an
        *                unconfigured CacheFactory
        */
        DefaultConfigurableCacheFactory(String::View vsFile = String::null_string);


    // ----- typedef: SchemeType --------------------------------------------

    public:
        /**
        * Scheme types.
        */
        typedef enum
            {
            scheme_local,
            scheme_class,
            scheme_near,
            scheme_remote_cache,
            scheme_remote_invocation,
            scheme_view,
            scheme_unknown
            } SchemeType;


    // ----- nested class: CacheInfo ----------------------------------------

    public:
        /**
        * CacheInfo is a placeholder for cache attributes retrieved during
        * parsing the corresponding cache mapping element.
        */
        class CacheInfo
            : public class_spec<CacheInfo>
            {
            friend class factory<CacheInfo>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new CacheInfo.
                *
                * @param vsCacheName    the cache name
                * @param vsSchemeName   the corresponding scheme name
                * @param vMapAttribute  the corresponding map of attributes
                */
                CacheInfo(String::View vsCacheName, String::View vsSchemeName,
                        Map::View vMapAttribute);

            // ----- accessors ------------------------------------------

            public:
                /**
                * Obtain the cache name.
                *
                * @return the cache name
                */
                virtual String::View getCacheName() const;

                /**
                * Obtain the scheme name.
                *
                * @return the scheme name
                */
                virtual String::View getSchemeName() const;

                /**
                * Obtain the attribute map.
                *
                * @return the attribute map
                */
                virtual Map::View getAttributes() const;

            // ----- helpers --------------------------------------------

            public:
                /**
                * Find and replace the attributes names in "{}" format with
                * the corresponding values for this cache info.
                *
                * Note: the content of the specified XmlElement could be
                * modified, so the caller is supposed to clone the passed in
                * XML if necessary.
                *
                * @param hXml  the XmlElement to replace "{}" attributes at
                */
                virtual void replaceAttributes(XmlElement::Handle hXml) const;

                /**
                * Generate a synthetic CacheInfo for a cache that has a name
                * suffixed with the specified string.
                *
                * @param vsSuffix  the cache name suffix
                *
                * @return the "cloned" synthetic CacheInfo
                */
                virtual CacheInfo::Handle getSyntheticInfo(String::View vsSuffix) const;


            // ----- data fields ----------------------------------------

            protected:
                /**
                * The cache name.
                */
                FinalView<String> f_vsCacheName;

                /**
                * The corresponding scheme name.
                */
                FinalView<String> f_vsSchemeName;

                /**
                * Map of scheme attributes.
                */
                FinalView<Map> f_vMapAttribute;
            };

    // ----- DefaultConfigurableCacheFactory interface ----------------------

    public:
        /**
        * In the configuration XML find a "cache-mapping" element associated with a
        * given cache name.
        *
        * @param vsCacheName  the value of the "cache-name" element to look for
        *
        * @return a CacheInfo object associated with a given cache name
        */
        virtual CacheInfo::View findSchemeMapping(String::View vsCacheName);

        /**
        * In the configuration XML find a "scheme" element associated with a
        * given cache and resolve it (recursively) using the "scheme-ref"
        * elements. The returned XML is always a clone of the actual configuration
        * and could be safely modified.
        *
        * @param vInfo  the cache info
        *
        * @return a resolved "scheme" element associated with a given cache
        */
        virtual XmlElement::View resolveScheme(CacheInfo::View vInfo);

        /**
        * Translate the scheme name into the scheme type. Valid scheme types are
        * any of the SCHEME_* constants.
        *
        * @param vsScheme  the scheme name
        *
        * @return the scheme type
        */
        virtual SchemeType translateSchemeType(String::View vsScheme);

        /**
        * Create an Object using "class-scheme" element.
        *
        * @param vInfo       the cache info
        * @param vXmlScheme  "class-scheme" element.
        *
        * @return a newly instantiated Object
        */
        virtual Object::Handle instantiateAny(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Ensures a cache for given scheme.
        *
        * @param vInfo       the cache info
        * @param vXmlScheme  the corresponding scheme
        *
        * @return a named cache created according to the description
        *         in the configuration
        */
        virtual NamedCache::Handle configureCache(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);


    protected:
        /**
        * Resolve the specified "XYZ-scheme" by retrieving the base element
        * refered to by the "scheme-ref" element, resolving it recursively,
        * and combining it with the specified overrides and cache specific attributes.
        *
        * @param vXmlScheme  a scheme element to resolve
        * @param vInfo       the cache info (optional)
        * @param fChild      if true, the actual cache scheme is the only "xyz-scheme"
        *                    child of the specified xmlScheme element;
        *                    otherwise it's the xmlScheme element itself
        * @param fRequired  if true, the child scheme must be present; false otherwise
        *
        * @return a "scheme" element associated with a given cache name; NULL if
        *         the child is missing and is not required
        */
        virtual XmlElement::Handle resolveScheme(XmlElement::View vXmlScheme,
                CacheInfo::View vInfo, bool fChild, bool fRequired);

        /**
        * In the configuration XML find a "scheme" element associated with a
        * given cache name.
        *
        * @param vsSchemeName  the value of the "cache-name" element to look for
        *
        * @return a "scheme" element associated with a given cache name
        */
        virtual XmlElement::Handle findScheme(String::View vsSchemeName);

        /**
        * In the configuration XML find a "scheme" element associated with a
        * given service name.
        *
        * @param vsServiceName  the value of the "service-name" element to look for
        *
        * @return a "scheme" element associated with a given service name
        */
        virtual XmlElement::Handle findServiceScheme(String::View vsServiceName);

        /**
        * Release a cache managed by this factory, optionally destroying it.
        *
        * @param cache     the cache to release
        * @param fDestroy  true to destroy the cache as well
        */
        virtual void releaseCache(NamedCache::Handle hCache, bool fDestroy);

        /**
        * Ensure the service for the specified scheme.
        *
        * @param vXmlScheme  the scheme
        *
        * @return running Service corresponding to the scheme
        */
        virtual Service::Handle configureService(XmlElement::View vXmlScheme);

        /**
        * Configures a backing map according to the scheme.
        *
        * @param vInfo         the cache info
        * @param vXmlScheme    the scheme element for cache configuration
        *
        * @return a backing map configured according to the scheme
        */
        virtual CacheMap::Handle configureBackingMap(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Instantiate a BundlingNamedCache based on the "operation-bundling" 
        * configuration.
        *
        * @param hCache        the wrapped cache
        * @param vXmlBundling  the "operation-bundling" element
        *
        * @return a newly instantiated BundlingNamedCache
        */
        virtual BundlingNamedCache::Handle instantiateBundlingNamedCache(
                NamedCache::Handle hCache, XmlElement::View vXmlBundling);

        /**
        * Initialize the specified bundler using the "bundle-config" element.
        *
        * @param bundler    the bundler
        * @param xmlBundle  a "bundle-config" element
        */
        virtual void initializeBundler(AbstractBundler::Handle hBundler, XmlElement::View vXmlBundle);

        /**
        * Instantiate a custom (class-name) based cache based on the supplied
        * configuration and scheme.
        *
        * @param vInfo       the CacheInfo
        * @param vXmlScheme  the cache scheme
        *
        * @return a new NamedCache instance
        */
        virtual NamedCache::Handle instantiateCustomCache(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Instantiate a local cache based on the supplied configuration and
        * scheme.
        *
        * @param vInfo       the CacheInfo
        * @param vXmlScheme  the cache scheme
        *
        * @return a new NamedCache instance
        */
        virtual NamedCache::Handle instantiateLocalCache(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Create a MapListener using the using the "class-scheme" element.
        * If the value of any "param-value" element contains the literal
        * "{cache-name}", replace it with the actual cache name.
        *
        * @param vInfo      the cache info
        * @param vXmlClass  "class-scheme" element
        *
        * @return a newly instantiated MapListener
        *
        * @since Coherence 12.1.2
        */
        virtual MapListener::Handle instantiateMapListener(CacheInfo::View vInfo, 
                XmlElement::View vXmlClass);

        /**
        * Instantiate a remote cache based on the supplied configuration and
        * scheme.
        *
        * @param vInfo       the CacheInfo
        * @param vXmlScheme  the cache scheme
        *
        * @return a new NamedCache instance
        */
        virtual NamedCache::Handle ensureRemoteCache(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Instantiate a near cache based on the supplied configuration and
        * scheme.
        *
        * @param vInfo       the CacheInfo
        * @param vXmlScheme  the cache scheme
        *
        * @return a new NamedCache instance
        */
        virtual NamedCache::Handle ensureNearCache(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Instantiate a cache view based on the supplied configuration and
        * scheme.
        *
        * @param vInfo       the CacheInfo
        * @param vXmlScheme  the cache scheme
        *
        * @return a new NamedCache instance
        * 
        * @since 12.2.1.4
        */
        virtual NamedCache::Handle ensureCacheView(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Create a backing Map using the "class-scheme" element.
        * This method is a thin wrapper around instantiateAny.
        *
        * @param vInfo       the cache info
        * @param vXmlScheme  "class-scheme" element.
        *
        * @return a newly instantiated Map
        */
        virtual Map::Handle instantiateMap(CacheInfo::View vInfo,
                XmlElement::View vXmlScheme);

        /**
        * Create a CacheLoader or CacheStore using the "cachestore-scheme" element.
        *
        * @param vInfo      the cache info
        * @param vXmlStore  "cachestore-scheme" element for the store or loader
        *
        * @return a newly instantiated CacheStore or CacheLoader
        */
        virtual CacheLoader::Handle instantiateCacheStore(CacheInfo::View vInfo,
                    XmlElement::View vXmlStore);

        /**
        * Convert the value in the specified {@link XmlValue} to an int.  If the
        * conversion fails, a warning will be logged.
        *
        * @param vXmlValue  the element expected to contain an int value
        *
        * @return the int value in the provided element, or 0 upon a
        *         conversion failure
        */
        virtual int32_t convertInt(XmlValue::View vXmlValue) const;

        /**
        * Convert the value in the specified {@link XmlValue} to an int.  If the
        * conversion fails, a warning will be logged.
        *
        * @param vXmlValue  the element expected to contain an int value
        * @param nDefault   the value that will be returned if the element does
        *                   not contain a value that can be converted to int
        *
        * @return the int value in the provided element, or nDefault upon a
        *         conversion failure
        */
        virtual int32_t convertInt(XmlValue::View vXmlValue, int32_t nDefault) const;

        /**
        * Log a failed type conversion.
        *
        * @param vXmlValue  element that contains the value that failed conversion
        * @param vsType     type that conversion was attempted to
        * @param vsDefault  default value that will be substituted
        * @param e          root cause of failed type conversion
        */
        virtual void reportConversionError(XmlValue::View vXmlValue, String::View vsType,
                String::View vsDefault, RuntimeException::View e) const;

        /**
        * Resolve and inject service serializer elements based on defaults
        * defined in the cache configuration.
        *
        * @param hXmlConfig  the configuration element to examine and modify
        *
        * @since Coherence 12.1.2
        */
        virtual void resolveSerializer(XmlElement::Handle hXmlConfig) const;

        /**
        * Check whether or not a MapListener has to be instantiated and
        * added to a Map according to a scheme definition.
        *
        * @param vInfo          the cache info
        * @param hMap           an ObservableMap to add a listener to
        * @param vXmlScheme     the corresponding scheme
        * @param hMapListeners  map of registered map listeners keyed by the
        *                       corresponding map references
        *
        * @throws IllegalArgumentException if the listener is required, but the
        *         map does not implement ObservableMap interface or if the
        *         listener cannot be instantiated
        *
        * @since Coherence 12.1.2
        */
        virtual void verifyMapListener(CacheInfo::View vInfo, Map::Handle hMap, 
                XmlElement::View vXmlScheme, Map::Handle hMapListeners);

    // ----- ConfigurableCacheFactory interface -----------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual NamedCache::Handle ensureCache(String::View vsCacheName);

        /**
        * {@inheritDoc}
        */
        virtual void destroyCache(NamedCache::Handle hCache);

        /**
        * {@inheritDoc}
        */
        virtual void releaseCache(NamedCache::Handle hCache);

        /**
        * {@inheritDoc}
        */
        virtual Service::Handle ensureService(String::View vsServiceName);

        /**
        * {@inheritDoc}
        */
        virtual void shutdown();


    // ----- XmlConfigurable interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual XmlElement::View getConfig() const;

        /**
        * {@inheritDoc}
        */
        virtual void setConfig(XmlElement::View vXml);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the OperationalContext for this cache factory.
        *
        * @return the OperationalContext for this cache factory
        *
        * @since Coherence 3.7
        */
        virtual OperationalContext::View ensureOperationalContext();

        /**
        * Set the OperationalContext for this cache factory.
        *
        * @param vContext  the OperationalContext for this cache factory
        *
        * @throws IllegalStateException if an OperationalContext was already
        *         set
        *
        * @since Coherence 3.7
        */
        virtual void setOperationalContext(OperationalContext::View vContext);

        /**
        * The default XML configuration used when one isn't explicitly passed
        * in the constructor for this class.
        *
        * @return the default XML configuration
        *
        * @since Coherence 3.7
        */
        static XmlDocument::Handle getDefaultCacheConfig();


    protected:
        /**
        * Return the cache reference store for this cache factory.
        *
        * @return the cache reference store for this cache factory
        *
        * @since Coherence 3.7
        */
        virtual Object::Handle ensureStoreCache();

        /**
        * Return the service reference store for this cache factory.
        *
        * @return the service reference store for this cache factory
        *
        * @since Coherence 3.7
        */
        virtual Object::Handle ensureStoreService();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Operational Context for all Services associated with this factory.
        */
        FinalView<OperationalContext> f_vContext;

        /**
        * XmlElement that corresponds to used XML cache configuration.
        */
        FinalView<XmlElement> f_vXmlConfig;

        /**
        * Store that holds cache references by name and optionally,
        * if configured, Subject.
        */
        FinalHandle<Object> f_hStoreCache;

        /**
        * Store that holds service references by name and optionally,
        * if configured, Subject.
        */
        FinalHandle<Object> f_hStoreService;

        /**
        * The parent ThreadGroup for all Services associated with this factory.
        */
        FinalHandle<ThreadGroup> f_hThreadGroup;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DEFAULT_CONFIGURABLE_CACHE_FACTORY_HPP
