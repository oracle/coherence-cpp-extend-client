/*
 * Copyright (c) 2000, 2021, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"

#include "coherence/io/IOException.hpp"

#include "coherence/net/DefaultOperationalContext.hpp"
#include "coherence/net/ServiceInfo.hpp"

#include "coherence/net/cache/CacheStore.hpp"
#include "coherence/net/cache/CachingMap.hpp"
#include "coherence/net/cache/ContinuousQueryCache.hpp"
#include "coherence/net/cache/EvictionPolicy.hpp"
#include "coherence/net/cache/NearCache.hpp"
#include "coherence/net/cache/UnitCalculator.hpp"

#include "coherence/util/Filter.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/Supplier.hpp"
#include "coherence/util/ValueExtractor.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"

#include "private/coherence/component/util/SafeCacheService.hpp"
#include "private/coherence/component/util/SafeInvocationService.hpp"
#include "private/coherence/component/util/SafeService.hpp"

#include "private/coherence/net/internal/ScopedReferenceStore.hpp"

#include "private/coherence/net/cache/LocalNamedCache.hpp"

#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"

#include "private/coherence/security/SecurityHelper.hpp"

#include "private/coherence/util/StringHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <fstream>

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::component::util::SafeCacheService;
using coherence::component::util::SafeInvocationService;
using coherence::component::util::SafeService;
using coherence::io::IOException;
using coherence::net::cache::CacheLoader;
using coherence::net::cache::CacheStore;
using coherence::net::cache::CachingMap;
using coherence::net::cache::ContinuousQueryCache;
using coherence::net::cache::EvictionPolicy;
using coherence::net::cache::LocalNamedCache;
using coherence::net::cache::UnitCalculator;
using coherence::net::cache::NearCache;
using coherence::net::internal::ScopedReferenceStore;
using coherence::run::xml::SimpleElement;
using coherence::run::xml::XmlHelper;
using coherence::security::SecurityHelper;
using coherence::util::Filter;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::List;
using coherence::util::MapListener;
using coherence::util::SafeHashMap;
using coherence::util::StringHelper;
using coherence::util::Supplier;
using coherence::util::ValueExtractor;
using coherence::util::filter::AlwaysFilter;
using coherence::util::logging::Logger;

COH_REGISTER_TYPED_CLASS(DefaultConfigurableCacheFactory);

COH_OPEN_NAMESPACE_ANON(DefaultConfigurableCacheFactory)

/**
* Obtain the name from the back cache currently associated
* with the associated ContinuousQueryCache.
*
* @since 12.2.1.4
*/
class ViewCacheNameSupplier
        : public class_spec<ViewCacheNameSupplier,
          extends<Object>,
          implements<Supplier> >
    {
    friend class factory<ViewCacheNameSupplier>;

    // ---- constructors ----------------------------------------------------

    protected:
        /**
        * Create a new Supplier that uses the provided ContinuousQueryCache
        * to provide NamedCache instances.
        *
        * @param hNamedCache the NamedCache
        */
        ViewCacheNameSupplier(ContinuousQueryCache::View vQueryCache)
                : f_vQueryCache(self(), vQueryCache)
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        ViewCacheNameSupplier(const ViewCacheNameSupplier&);

    // ----- Supplier interface -----------------------------------------

    public:
        virtual Object::Holder get() const
            {
            return f_vQueryCache->getCache()->getCacheName();
            }

        virtual Object::Holder get()
            {
            return f_vQueryCache->getCache()->getCacheName();
            }

    // ----- data members -----------------------------------------------

    protected:
        FinalView<ContinuousQueryCache> f_vQueryCache;
    };

/**
* Returns the back cache based on the provided CacheInfo
* and XML scheme configuration.
*
* This is primarily used by view-scheme processing.
*
* @since 12.2.1.4
*/
class ViewCacheSupplier
        : public class_spec<ViewCacheSupplier,
          extends<Object>,
          implements<Supplier> >
    {
    friend class factory<ViewCacheSupplier>;

    // ---- constructors ----------------------------------------------------

    protected:
        /**
        * Create a new Supplier that uses the provided ContinuousQueryCache
        * to provide NamedCache instances.
        *
        * @param hNamedCache the NamedCache
        */
        ViewCacheSupplier(DefaultConfigurableCacheFactory::Handle hCacheFactory,
                          DefaultConfigurableCacheFactory::CacheInfo::View vCacheInfo,
                          XmlElement::View vXmlBackScheme)
                : m_hCacheFactory(self(), hCacheFactory), f_vCacheInfo(self(), vCacheInfo), f_vXmlBackScheme(self(), vXmlBackScheme)
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        ViewCacheSupplier(const ViewCacheSupplier&);

    // ----- Supplier interface -----------------------------------------

    public:
        virtual Object::Holder get() const
            {
            return m_hCacheFactory->configureCache(f_vCacheInfo, cast<XmlElement::View>(f_vXmlBackScheme->clone()));
            }

        virtual Object::Holder get()
            {
            return m_hCacheFactory->configureCache(f_vCacheInfo, cast<XmlElement::View>(f_vXmlBackScheme->clone()));
            }

    // ----- data members -----------------------------------------------

    protected:
        mutable FinalHandle<DefaultConfigurableCacheFactory> m_hCacheFactory;
        FinalView<DefaultConfigurableCacheFactory::CacheInfo> f_vCacheInfo;
        FinalView<XmlElement> f_vXmlBackScheme;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

DefaultConfigurableCacheFactory::DefaultConfigurableCacheFactory(
        String::View vsFile)
        : f_vContext(self()),
          f_vXmlConfig(self()),
          f_hStoreCache(self()),
          f_hStoreService(self()),
          f_hThreadGroup(self(), ThreadGroup::create(
                  COH_TO_STRING("coherence - " << vsFile)))
    {
    if (NULL != vsFile)
        {
        setConfig(XmlHelper::loadFile(vsFile, "cache configuration"));
        }
    }

// ----- DefaultConfigurableCacheFactory interface --------------------------

DefaultConfigurableCacheFactory::CacheInfo::View
    DefaultConfigurableCacheFactory::findSchemeMapping(
        String::View vsCacheName)
        {
        XmlElement::View vXmlDefaultMatch = NULL;
        XmlElement::View vXmlPrefixMatch  = NULL;
        XmlElement::View vXmlExactMatch   = NULL;
        String::View     vsSuffix;

        for (Iterator::Handle hIter = f_vXmlConfig->getSafeElement
                ("caching-scheme-mapping")->getElements("cache-mapping");
                hIter->hasNext();)
            {
            XmlElement::View vXmlMapping = cast<XmlElement::View>(hIter->next());

            String::View vsName = vXmlMapping->getSafeElement("cache-name")->
                    getString();
            if (vsName->equals(vsCacheName))
                {
                vXmlExactMatch = vXmlMapping;
                break;
                }
            else if (vsName->equals("*"))
                {
                vXmlDefaultMatch = vXmlMapping;
                }
            else
                {
                size32_t cchPrefix = vsName->indexOf('*');
                if (cchPrefix != String::npos)
                    {
                    String::View vsPrefix = vsName->substring(0, cchPrefix);
                    if (vsCacheName->startsWith(vsPrefix))
                        {
                        if (cchPrefix != vsName->length() - 1)
                            {
                            COH_THROW_STREAM (IllegalArgumentException,
                                "Invalid wildcard pattern:\n" << vXmlMapping);
                            }
                        vXmlPrefixMatch = vXmlMapping;
                        vsSuffix        = vsCacheName->substring(cchPrefix);
                        }
                    }
                }
            }

        XmlElement::View vXmlMatch;

        if (NULL != vXmlExactMatch)
            {
            vXmlMatch = vXmlExactMatch;
            vsSuffix  = StringHelper::getEmptyString();
            }
        else if (NULL != vXmlPrefixMatch)
            {
            vXmlMatch = vXmlPrefixMatch;
            }
        else
            {
            vXmlMatch = vXmlDefaultMatch;
            vsSuffix  = vsCacheName;
            }

        if (NULL == vXmlMatch)
            {
            COH_THROW_STREAM (IllegalArgumentException,
                    "No scheme for cache: \"" << vsCacheName << '"');
            }

        String::View vsScheme = vXmlMatch->getSafeElement("scheme-name")->getString();
        Map::Handle  hMapAttr = HashMap::create();
        for (Iterator::Handle hIter = vXmlMatch->getSafeElement("init-params")->
                getElements("init-param"); hIter->hasNext();)
            {
            XmlElement::View vXmlParam = cast<XmlElement::View>(hIter->next());
            String::View     vsName    = vXmlParam->getSafeElement("param-name" )->getString();
            String::View     vsValue   = vXmlParam->getSafeElement("param-value")->getString();

            if (vsName->length() != 0)
                {
                size32_t ofReplace = vsValue->indexOf('*');
                if (ofReplace != String::npos && vsSuffix != NULL)
                    {
                    vsValue = COH_TO_STRING(
                            vsValue->substring(0, ofReplace) <<
                            vsSuffix << vsValue->substring(ofReplace + 1));
                    }
                hMapAttr->put(vsName, vsValue);
                }
            }

        return CacheInfo::create(vsCacheName, vsScheme, hMapAttr);
        }

XmlElement::View DefaultConfigurableCacheFactory::
    resolveScheme(CacheInfo::View vInfo)
    {
    XmlElement::Handle hXmlScheme = findScheme(vInfo->getSchemeName());

    vInfo->replaceAttributes(hXmlScheme);

    return resolveScheme(hXmlScheme, vInfo, false, true);
    }

DefaultConfigurableCacheFactory::SchemeType DefaultConfigurableCacheFactory::
    translateSchemeType(String::View vsScheme)
    {
    return vsScheme->equals("local-scheme")             ? scheme_local
         : vsScheme->equals("class-scheme")             ? scheme_class
         : vsScheme->equals("near-scheme")              ? scheme_near
         : vsScheme->equals("remote-cache-scheme")      ? scheme_remote_cache
         : vsScheme->equals("remote-invocation-scheme") ? scheme_remote_invocation
         : vsScheme->equals("view-scheme")              ? scheme_view
         :                                                scheme_unknown;
    }


// ----- accessors ----------------------------------------------------------

OperationalContext::View DefaultConfigurableCacheFactory::ensureOperationalContext()
    {
    OperationalContext::View vContext = f_vContext;
    if (vContext == NULL)
        {
        COH_SYNCHRONIZED(this)
            {
            vContext = f_vContext;
            if (vContext == NULL)
                {
                vContext = DefaultOperationalContext::create();
                initialize(f_vContext, vContext);
                }
            }
        }
    return vContext;
    }

void DefaultConfigurableCacheFactory::setOperationalContext(
        OperationalContext::View vContext)
    {
    if (f_vContext != NULL)
        {
        COH_THROW (IllegalStateException::
                create("OperationalContext has already been set"));
        }
    initialize(f_vContext, vContext);
    }

XmlDocument::Handle DefaultConfigurableCacheFactory::getDefaultCacheConfig()
    {
    // Support the Coherence system property being either "coherence.cacheconfig" or "coherence.cache.config".
    // For camel case environment variable, must continue to support "TangosolCoherenceCacheConfig".
    String::View vsFile = System::getProperty("coherence.cacheconfig",
                          System::getProperty("coherence.cache.config", "coherence-cache-config.xml"));

    std::ifstream in(vsFile->getCString(), std::ifstream::in);
    if (in.fail() && vsFile->equals("coherence-cache-config.xml"))
        {
        // no cache config specified or found in path, use a subset of the default java cache config, i.e. extend applicable part
        return XmlHelper::loadXml(COH_TO_STRING("\
<cache-config> \
  <caching-scheme-mapping> \
    <cache-mapping> \
      <cache-name>*</cache-name> \
      <scheme-name>" << System::getProperty("coherence.profile", "near") << "-remote</scheme-name> \
    </cache-mapping> \
  </caching-scheme-mapping> \
  <caching-schemes> \
    <!-- near caching scheme for extend clients --> \
    <near-scheme> \
      <scheme-name>near-remote</scheme-name> \
      <front-scheme> \
        <local-scheme> \
          <high-units>{front-limit-entries 10000}</high-units> \
        </local-scheme> \
      </front-scheme> \
      <back-scheme> \
        <remote-cache-scheme> \
          <scheme-ref>thin-remote</scheme-ref> \
        </remote-cache-scheme> \
      </back-scheme> \
    </near-scheme> \
    <!-- remote caching scheme for accessing the proxy from extend clients --> \
    <remote-cache-scheme> \
      <scheme-name>thin-remote</scheme-name> \
      <service-name>RemoteCache</service-name> \
      <proxy-service-name>Proxy</proxy-service-name> \
      <initiator-config> " << // TODO: remove initiator-config once we can infer from operational config
        "<tcp-initiator> \
          <name-service-addresses> \
            <socket-address> \
              <address>" << System::getProperty("coherence.address", "127.0.0.1") << "</address> \
              <port>" << System::getProperty("coherence.clusterport", "7574") << "</port> \
            </socket-address> \
          </name-service-addresses> \
        </tcp-initiator> \
      </initiator-config> \
    </remote-cache-scheme> \
  </caching-schemes> \
</cache-config>"));
        }

    return XmlHelper::loadFile(vsFile, "cache configuration");
    }

Object::Handle DefaultConfigurableCacheFactory::ensureStoreCache()
    {
    Object::Handle hStoreCache = f_hStoreCache;
    if (hStoreCache == NULL)
        {
        COH_SYNCHRONIZED(this)
            {
            hStoreCache = f_hStoreCache;
            if (hStoreCache == NULL)
                {
                hStoreCache =
                        ScopedReferenceStore::create(ensureOperationalContext());
                initialize(f_hStoreCache, hStoreCache);
                }
            }
        }
    return hStoreCache;
    }

Object::Handle DefaultConfigurableCacheFactory::ensureStoreService()
    {
    Object::Handle hStoreService = f_hStoreService;
    if (hStoreService == NULL)
        {
        COH_SYNCHRONIZED(this)
            {
            hStoreService = f_hStoreService;
            if (hStoreService == NULL)
                {
                hStoreService =
                        ScopedReferenceStore::create(ensureOperationalContext());
                initialize(f_hStoreService, hStoreService);
                }
            }
        }
    return hStoreService;
    }


// ----- protected ----------------------------------------------------------

XmlElement::Handle DefaultConfigurableCacheFactory::resolveScheme(
        XmlElement::View vXmlScheme, CacheInfo::View vInfo, bool fChild,
        bool fRequired)
    {
    if (fChild)
        {
        XmlElement::Handle hXmlChild = NULL;
        for (Iterator::Handle hIter = vXmlScheme->getElementList()->iterator();
                hIter->hasNext();)
            {
            XmlElement::View vXml = cast<XmlElement::View>(hIter->next());

            if (vXml->getName()->endsWith("-scheme"))
                {
                if (NULL == hXmlChild)
                    {
                    hXmlChild = cast<XmlElement::Handle>(vXml->clone());
                    }
                else
                    {
                    COH_THROW_STREAM (IllegalArgumentException,
                        "Scheme contains more then one child scheme:\n"
                            << vXmlScheme);
                    }
                }
            }

        if (NULL == hXmlChild)
            {
            if (fRequired)
                {
                String::View vsName = vXmlScheme->getName();
                if (vXmlScheme == vXmlScheme->getParent()->getElement(vsName))
                    {
                    COH_THROW_STREAM (IllegalArgumentException,
                        "Child scheme is missing at:\n" << vXmlScheme);
                    }
                else
                    {
                    COH_THROW_STREAM (IllegalArgumentException,
                        "Element \"" << vsName << "\" is missing at:\n" <<
                            vXmlScheme->getParent());
                    }
                }
            return NULL;
            }
        vXmlScheme = hXmlChild;
        }

    String::View vsRefName = vXmlScheme->getSafeElement("scheme-ref")->getString();
    if (vsRefName->length() == 0)
        {
        return cast<XmlElement::Handle>(vXmlScheme->clone());
        }

    XmlElement::Handle hXmlBase = findScheme(vsRefName);

    if (!vXmlScheme->getName()->equals(hXmlBase->getName()))
        {
        COH_THROW_STREAM (IllegalArgumentException,
            "Reference does not match the scheme type: scheme=\n" <<
                vXmlScheme << "\nbase=" << hXmlBase);
        }
    if (vXmlScheme->equals(hXmlBase))
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "Circular reference in scheme:\n" << vXmlScheme);
        }

    if (NULL != vInfo)
        {
        vInfo->replaceAttributes(hXmlBase);
        }

    XmlElement::Handle hXmlResolve = resolveScheme(hXmlBase, vInfo, false, false);

    for (Iterator::Handle hIter = vXmlScheme->getElementList()->iterator();
            hIter->hasNext();)
        {
        XmlHelper::replaceElement(hXmlResolve, cast<XmlElement::View>(hIter->next()));
        }
    return hXmlResolve;
    }

XmlElement::Handle DefaultConfigurableCacheFactory::
    findScheme(String::View vsSchemeName)
    {
    if (NULL != vsSchemeName)
        {
        for (Iterator::Handle hIter = f_vXmlConfig->getSafeElement("caching-schemes")->
                getElementList()->iterator(); hIter->hasNext();)
            {
            XmlElement::View vXml = cast<XmlElement::View>(hIter->next());

            if (vXml->getSafeElement("scheme-name")->getString()->equals(vsSchemeName))
                {
                return cast<XmlElement::Handle>(vXml->clone());
                }
            }
        }

    COH_THROW_STREAM (IllegalArgumentException, "Missing scheme: \"" <<
            vsSchemeName << '"');
    }

XmlElement::Handle DefaultConfigurableCacheFactory::
    findServiceScheme(String::View vsServiceName)
    {
    if (NULL != vsServiceName)
        {
        for (Iterator::Handle hIter = f_vXmlConfig->getSafeElement("caching-schemes")->
                getElementList()->iterator(); hIter->hasNext();)
            {
            XmlElement::View vXml = cast<XmlElement::View>(hIter->next());

            if (vXml->getSafeElement("service-name")->getString()->equals(vsServiceName))
                {
                return cast<XmlElement::Handle>(vXml->clone());
                }
            }
        }

    COH_THROW_STREAM (IllegalArgumentException,
            "Missing scheme for service: \"" << vsServiceName << '"');
    }

NamedCache::Handle
DefaultConfigurableCacheFactory::configureCache
        (CacheInfo::View vInfo, XmlElement::View vXmlScheme)
    {
    String::View       vsSchemeType = vXmlScheme->getName();
    NamedCache::Handle hCache;

    switch (translateSchemeType(vsSchemeType))
        {
        case scheme_class:
            hCache = instantiateCustomCache(vInfo, vXmlScheme);
            break;

        case scheme_local:
            hCache = instantiateLocalCache(vInfo, vXmlScheme);
            break;

        case scheme_remote_cache:
            {
            hCache = ensureRemoteCache(vInfo, vXmlScheme);
            XmlElement::View vXmlBundling = vXmlScheme->getElement("operation-bundling");
            if (NULL != vXmlBundling)
                {
                hCache = instantiateBundlingNamedCache(hCache, vXmlBundling);
                }
            }
            break;

        case scheme_near:
            hCache = ensureNearCache(vInfo, vXmlScheme);
            break;

        case scheme_view:
            hCache = ensureCacheView(vInfo, vXmlScheme);
            break;

        default:
            COH_THROW_STREAM (UnsupportedOperationException,
                    "configureCache: " << vsSchemeType);
        }

    verifyMapListener(vInfo, hCache, vXmlScheme, NULL);

    return hCache;
    }

void DefaultConfigurableCacheFactory::releaseCache(NamedCache::Handle hCache,
        bool fDestroy)
    {
    String::View                  vsCacheName = hCache->getCacheName();
    ScopedReferenceStore::Handle  hStoreCache =
            cast<ScopedReferenceStore::Handle>(ensureStoreCache());

    COH_SYNCHRONIZED (hStoreCache)
        {
        if (hStoreCache->getCache(vsCacheName) == hCache)
            {
            // remove the mapping
            hStoreCache->remove(vsCacheName);

            // free the resources; this is done under lock to prevent
            // other threads from obtaining access to any common back
            // cache during the cleanup process

            // release associated resources if it is a NearCache
            if (fDestroy)
                {
                hCache->destroy();
                }
            if (hCache->isActive() && instanceof<NearCache::Handle>(hCache))
                {
                // release both the NearCache and its back
                NamedCache::Handle hCacheBack =
                    cast<NearCache::Handle>(hCache)->getBackCache();

                // the back must be handled last as the NearCache
                // will reference it during its own cleanup
                hCache->release();
                hCacheBack->release();
                }
            else
                {
                // allow cache to release internal resources
                hCache->release();
                }
            }
        else if (hCache->isActive())
            {
            // active, but not managed by this factory
            COH_THROW_STREAM (IllegalArgumentException,
                    "The cache " << vsCacheName << " was created using a"
                    " different factory; that same factory should be used to"
                    " release the cache.");
            }
        }
    }

Service::Handle DefaultConfigurableCacheFactory::
    configureService(XmlElement::View vXmlScheme)
    {
    XmlElement::Handle hXmlScheme = resolveScheme(vXmlScheme, NULL, false, false);

    String::View  vsSchemeType  = vXmlScheme->getName();
    String::View  vsServiceName = vXmlScheme->getSafeElement("service-name")->getString();
    String::View  vsServiceType;
    int           nSchemeType   = translateSchemeType(vsSchemeType);

    SafeService::Handle hService;
    switch (nSchemeType)
        {
        case scheme_remote_cache:
            hService = SafeCacheService::create();
            hService->setServiceType(ServiceInfo::remote_cache);
            break;

       case scheme_remote_invocation:
            hService = SafeInvocationService::create();
            hService->setServiceType(ServiceInfo::remote_invocation);
            break;

        default:
            COH_THROW_STREAM (UnsupportedOperationException, "ensureService: "
                    << vsSchemeType);
        }

    if (vsServiceName->length() == 0)
        {
        vsServiceName = vsServiceType;
        }

    // configure service
    hService->setOperationalContext(ensureOperationalContext());
    hService->setSubject(SecurityHelper::getCurrentSubject());
    hService->setServiceName(vsServiceName);
    hService->setThreadGroup(f_hThreadGroup);

    // merge the standard service config parameters
    XmlElement::Handle hXmlConfig    = cast<XmlElement::Handle>(vXmlScheme->clone());
    List::Handle       hListStandard = hXmlConfig->getElementList();
    for (size32_t i = 0, c = hListStandard->size(); i < c; i++)
        {
        XmlElement::View vXmlParamStandard = cast<XmlElement::View>(hListStandard->get(i));
        String::View     vsParamName       = vXmlParamStandard->getName();
        XmlElement::View vXmlParam         = vXmlScheme->getElement(vsParamName);

        if (NULL != vXmlParam && !XmlHelper::isEmpty(vXmlParam))
            {
            hListStandard->set(i, vXmlParam->clone());
            }
        }

    // resolve nested serializers for remote services
    switch (nSchemeType)
        {
        case scheme_remote_cache:
        case scheme_remote_invocation:
            resolveSerializer(hXmlConfig->ensureElement("initiator-config"));
            break;
        }

    hService->configure(hXmlConfig);
    hService->start();
    return hService;
    }

CacheMap::Handle DefaultConfigurableCacheFactory::configureBackingMap(
        CacheInfo::View vInfo, XmlElement::View vXmlScheme)
    {
    String::View vsSchemeType = vXmlScheme->getName();
    CacheMap::Handle  hMap         = NULL;

    switch (translateSchemeType(vsSchemeType))
        {
        case scheme_local:
            hMap  = instantiateLocalCache(vInfo, vXmlScheme);
            break;

        case scheme_near:
            {
            XmlElement::View xmlBack = resolveScheme(
                vXmlScheme->getSafeElement("back-scheme"), vInfo, true, true);
            return configureBackingMap(vInfo, xmlBack);
            }

        case scheme_remote_cache:
            hMap  = configureCache(vInfo, vXmlScheme);
            break;

        case scheme_class:
            hMap  = cast<CacheMap::Handle>(instantiateMap(vInfo, vXmlScheme));
            break;

        default:
            COH_THROW_STREAM (UnsupportedOperationException,
                    "configureBackingMap: " << vsSchemeType);
        }
    return hMap;
    }

NamedCache::Handle DefaultConfigurableCacheFactory::instantiateCustomCache
    (CacheInfo::View vInfo, XmlElement::View vXmlScheme)
    {
    return cast<NamedCache::Handle>(instantiateAny(vInfo, vXmlScheme));
    }

NamedCache::Handle DefaultConfigurableCacheFactory::instantiateLocalCache
    (CacheInfo::View vInfo, XmlElement::View vXmlLocal)
    {
    int32_t cHighUnits         = (int32_t) StringHelper::parseMemorySize(vXmlLocal->getSafeElement("high-units")->getString("0"));
    int32_t cLowUnits          = (int32_t) StringHelper::parseMemorySize(vXmlLocal->getSafeElement("low-units" )->getString("0"));
    int32_t cExpiryDelayMillis = (int32_t) StringHelper::parseTime(vXmlLocal->getSafeElement("expiry-delay")->getString("0"), StringHelper::unit_s);
    int32_t cFlushDelayMillis  = (int32_t) StringHelper::parseTime(vXmlLocal->getSafeElement("flush-delay" )->getString("0"), StringHelper::unit_s);

    // check and default all of the Cache options
    if (cHighUnits <= 0)
        {
        cHighUnits = 0x7FFFFFFF;
        }
    if (cLowUnits <= 0)
        {
        cLowUnits = (int32_t) (cHighUnits * 0.75);
        }
    if (cExpiryDelayMillis < 0)
        {
        cExpiryDelayMillis = 0;
        }
    if (cFlushDelayMillis < 0)
        {
        cFlushDelayMillis = 0;
        }

    // As of Coherence 3.0, if the local cache has been configured with a
    // positive expiry-delay, but does not have a flush-delay,
    // the value of the flush-delay will be set to the default value
    if (cExpiryDelayMillis > 0 && cFlushDelayMillis == 0)
        {
        cFlushDelayMillis = 60000;
        }

    // configure and return the LocalCache
    LocalNamedCache::Handle hCache;
    String::View vsSubclass = vXmlLocal->getSafeElement("class-name")->getString();
    if (vsSubclass->length() == 0)
        {
        hCache = LocalNamedCache::create();
        }
    else
        {
        // TODO: aoparams?
        Class::View vClz = SystemClassLoader::getInstance()->loadByName(vsSubclass);
        hCache = cast<LocalNamedCache::Handle>(vClz->newInstance());

        XmlElement::View vXmlParams = vXmlLocal->getElement("init-params");
        if (NULL != vXmlParams)
            {
            XmlConfigurable::Handle hTarget = cast<XmlConfigurable::Handle>(hCache);
            if (NULL != hTarget)
                {
                XmlElement::Handle hXmlConfig = SimpleElement::create("config");

                XmlHelper::transformInitParams(hXmlConfig, vXmlParams);
                hTarget->setConfig(hXmlConfig);
                }
            }
        }

    hCache->setCacheName(vInfo->getCacheName());
    hCache->setHighUnits(cHighUnits);
    hCache->setExpiryDelay(cExpiryDelayMillis);
    hCache->setLowUnits(cLowUnits);
    hCache->setFlushDelay(cFlushDelayMillis);

    XmlElement::View vXmlEviction = vXmlLocal->getElement("eviction-policy");
    if (NULL != vXmlEviction)
        {
        String::View                       vsEvictionType = vXmlEviction->getString();
        int32_t nEvictionType  = -1;
        if (StringHelper::compare(vsEvictionType, "HYBRID", false) == 0)
            {
            nEvictionType = EvictionPolicy::eviction_policy_hybrid;
            }
        else if (StringHelper::compare(vsEvictionType, "LRU", false) == 0)
            {
            nEvictionType = EvictionPolicy::eviction_policy_lru;
            }
        else if (StringHelper::compare(vsEvictionType, "LFU", false) == 0)
            {
            nEvictionType = EvictionPolicy::eviction_policy_lfu;
            }

        if (nEvictionType >= 0)
            {
            hCache->setEvictionType(EvictionPolicy::EvictionPolicyType(nEvictionType));
            }
        else
            {
            XmlElement::View vXmlClass = vXmlEviction->getElement("class-scheme");
            if (NULL != vXmlClass)
                {
                try
                    {
                    hCache->setEvictionPolicy(cast<EvictionPolicy::Handle>(
                        instantiateAny(vInfo, vXmlClass)));
                    }
                catch (Exception::View e)
                    {
                    COH_THROW_STREAM (IllegalArgumentException,
                            "Unknown eviction policy:\n" << vXmlClass);
                    }
                }
            }
        }

    XmlElement::View vXmlCalculator = vXmlLocal->getElement("unit-calculator");
    if (vXmlCalculator != NULL)
        {
        String::View vsCalculatorType = vXmlCalculator->getString();
        int32_t      nCalculatorType = -1;
        if (StringHelper::compare(vsCalculatorType, "FIXED", false) == 0)
            {
            nCalculatorType = LocalNamedCache::unit_calculator_fixed;
            }

        if (nCalculatorType >= 0)
            {
            hCache->setUnitCalculatorType(LocalNamedCache::UnitCalculatorType(nCalculatorType));
            }
        else
            {
            XmlElement::View vXmlClass = vXmlCalculator->getElement("class-scheme");
            if (NULL != vXmlClass)
                {
                try
                    {
                    hCache->setUnitCalculator(
                            cast<UnitCalculator::Handle>(
                                    instantiateAny(vInfo, vXmlClass)));
                    }
                catch (Exception::View e)
                    {
                    COH_THROW_STREAM (IllegalArgumentException,
                            "Unknown unit calculator:\n" <<  vXmlClass);
                    }
                }
            }
        }

    XmlElement::Handle  hXmlStore = resolveScheme(vXmlLocal->getSafeElement("cachestore-scheme"), vInfo, false, false);
    CacheLoader::Handle hStore    = instantiateCacheStore(vInfo, hXmlStore);
    if (NULL != hStore)
        {
        hCache->setCacheLoader(hStore);
        }

    if (vXmlLocal->getSafeElement("pre-load")->getBoolean())
        {
        try
            {
            hCache->loadAll();
            }
        catch (Exception::View e)
            {
            COH_LOG(
                "An exception occurred while pre-loading the \""
                << vInfo->getCacheName() << "\" cache:"
                << '\n' << e
                << "\nThe following configuration was used for the \""
                << vInfo->getCacheName() << "\" cache:"
                << '\n' << vXmlLocal
                << "\n(The exception has been logged and will be ignored.)",
                Logger::level_warning);
            }
        }
    return hCache;
    }

MapListener::Handle DefaultConfigurableCacheFactory::instantiateMapListener
    (CacheInfo::View vInfo, XmlElement::View vXmlClass)
    {
    try
        {
        return cast<MapListener::Handle>(instantiateAny(vInfo, vXmlClass));
        }
    catch (ClassCastException::View vEx)
        {
        COH_THROW_STREAM (IllegalArgumentException, "Not a listener:\n" << vXmlClass);
        }
    }

NamedCache::Handle DefaultConfigurableCacheFactory::ensureRemoteCache
    (CacheInfo::View vInfo, XmlElement::View vXmlScheme)
    {
    try
        {
        CacheService::Handle hService = cast<CacheService::Handle>(
                ensureService(vXmlScheme->getSafeElement("service-name")->
                getString()));
        return hService->ensureCache(vInfo->getCacheName());
        }
    catch (ClassCastException::View vEx)
        {
        COH_THROW_STREAM (IllegalArgumentException, "Invalid scheme:\n" << vXmlScheme);
        }
    }

NamedCache::Handle DefaultConfigurableCacheFactory::ensureNearCache
    (CacheInfo::View vInfo, XmlElement::View vXmlScheme)
    {
    NamedCache::Handle hCache     = NULL;
    XmlElement::Handle hXmlFront  = resolveScheme(vXmlScheme->getSafeElement(
            "front-scheme"), vInfo, true, true);
    XmlElement::Handle hXmlBack   = resolveScheme(vXmlScheme->getSafeElement(
            "back-scheme"), vInfo, true, true);
    CacheMap::Handle   hMapFront  = configureBackingMap(vInfo, hXmlFront);
    NamedCache::Handle hCacheBack = configureCache(vInfo, hXmlBack);
    String::View       vsStrategy = vXmlScheme->getSafeElement(
            "invalidation-strategy")->getString("auto");
    String::View       vsSubclass = vXmlScheme->getSafeElement(
            "class-name")->getString();

    CachingMap::InvalidationStrategy strategy =
              vsStrategy->equals("none")    ? CachingMap::listen_none
            : vsStrategy->equals("present") ? CachingMap::listen_present
            : vsStrategy->equals("all")     ? CachingMap::listen_all
            : vsStrategy->equals("logical") ? CachingMap::listen_logical
            :                                 CachingMap::listen_auto;

    if (vsSubclass->length() == 0)
        {
        hCache = NearCache::create(hMapFront, hCacheBack, strategy);
        }
    else
        {
        Class::View vClz = SystemClassLoader::getInstance()->
                loadByName(vsSubclass);
        hCache = cast<NamedCache::Handle>(vClz->newInstance());

        XmlElement::View vXmlParams = vXmlScheme->getElement("init-params");
        if (NULL != vXmlParams)
            {
            XmlConfigurable::Handle hTarget =
                    cast<XmlConfigurable::Handle>(hCache, false);
            if (NULL != hTarget)
                {
                XmlElement::Handle hXmlConfig =
                        SimpleElement::create("config");

                XmlHelper::transformInitParams(hXmlConfig, vXmlParams);
                hTarget->setConfig(hXmlConfig);
                }
            }
        }
    return hCache;
    }

NamedCache::Handle DefaultConfigurableCacheFactory::ensureCacheView
        (CacheInfo::View vInfo, XmlElement::View vXmlScheme)
    {
    XmlElement::View       vXmlViewFilter     = vXmlScheme->getElement("view-filter");
    XmlElement::View       vXmlTransformer    = vXmlScheme->getElement("transformer");
    XmlElement::View       vXmlListener       = vXmlScheme->getElement("listener");
    XmlElement::Handle     hXmlBack           = resolveScheme(vXmlScheme->getSafeElement("back-scheme"), vInfo, true, true);
    bool                   fReadOnly          = vXmlScheme->getSafeElement("read-only")->getBoolean();
    int64_t                cReconnectInterval = StringHelper::parseTime(vXmlScheme->getSafeElement("reconnect-interval")->getString("0"), StringHelper::unit_ms);
    Filter::Handle         hFilter            = AlwaysFilter::getInstance();
    ValueExtractor::Handle hTransformer       = NULL;
    MapListener::Handle    hListener          = NULL;

    if (vXmlViewFilter != NULL)
        {
        hFilter = cast<Filter::Handle>(instantiateAny(vInfo, vXmlViewFilter->getSafeElement("class-scheme")));
        }

    if (vXmlTransformer != NULL)
        {
        hTransformer = cast<ValueExtractor::Handle>(instantiateAny(vInfo, vXmlTransformer->getSafeElement("class-scheme")));
        }

    if (vXmlListener != NULL)
        {
        hListener = cast<MapListener::Handle>(instantiateAny(vInfo, vXmlListener->getSafeElement("class-scheme")));
        }

    Supplier::Handle hCacheSupplier = ViewCacheSupplier::create(this, vInfo, cast<XmlElement::View>(hXmlBack->clone()));
    ContinuousQueryCache::Handle hQueryCache = ContinuousQueryCache::create((Supplier::View) hCacheSupplier,
                                                                            hFilter,
                                                                            true,
                                                                            hListener,
                                                                            hTransformer);
    hQueryCache->setCacheNameSupplier(ViewCacheNameSupplier::create(hQueryCache));
    hQueryCache->setReconnectInterval(cReconnectInterval);
    hQueryCache->setReadOnly(hTransformer != NULL || fReadOnly);
    return hQueryCache;
    }

BundlingNamedCache::Handle DefaultConfigurableCacheFactory::instantiateBundlingNamedCache
        (NamedCache::Handle hCache, XmlElement::View vXmlBundling)
    {
    BundlingNamedCache::Handle hCacheBundle = BundlingNamedCache::create(hCache);
    for (Iterator::Handle hIter = vXmlBundling->getElements("bundle-config");
            hIter->hasNext();)
        {
        XmlElement::View vXmlBundle  = cast<XmlElement::View>(hIter->next());
        String::View     vsOperation = vXmlBundle->getSafeElement("operation-name")->getString("all");
        int              cBundle     = convertInt(vXmlBundle->getSafeElement("preferred-size"));

        if (vsOperation->equals("all"))
            {
            initializeBundler(hCacheBundle->ensureGetBundler(cBundle), vXmlBundle);
            initializeBundler(hCacheBundle->ensurePutBundler(cBundle), vXmlBundle);
            initializeBundler(hCacheBundle->ensureRemoveBundler(cBundle), vXmlBundle);
            }
        else if (vsOperation->equals("get"))
            {
            initializeBundler(hCacheBundle->ensureGetBundler(cBundle), vXmlBundle);
            }
        else if (vsOperation->equals("put"))
            {
            initializeBundler(hCacheBundle->ensurePutBundler(cBundle), vXmlBundle);
            }
        else if (vsOperation->equals("remove"))
            {
            initializeBundler(hCacheBundle->ensureRemoveBundler(cBundle), vXmlBundle);
            }
        else
            {
            COH_THROW_STREAM(IllegalArgumentException,
                    "Invalid \"operation-name\" element:\n" << vXmlBundle);
            }
        }

    return hCacheBundle;
    }

void DefaultConfigurableCacheFactory::initializeBundler(
        AbstractBundler::Handle hBundler, XmlElement::View vXmlBundle)
    {
    if (NULL != hBundler)
        {
        hBundler->setThreadThreshold(
                convertInt(vXmlBundle->getSafeElement("thread-threshold"), 4));
        hBundler->setDelayMillis(
                convertInt(vXmlBundle->getSafeElement("delay-millis"), 1));
        hBundler->setAllowAutoAdjust(
                vXmlBundle->getSafeElement("auto-adjust")->getBoolean(false));
        }
    }

CacheLoader::Handle DefaultConfigurableCacheFactory::
    instantiateCacheStore(CacheInfo::View vInfo, XmlElement::View vXmlStore)
    {
    XmlElement::Handle hXmlStore = resolveScheme(vXmlStore, vInfo, true, false);
    if (NULL == hXmlStore || XmlHelper::isEmpty(hXmlStore))
        {
        return NULL;
        }

    String::View vsSchemeType = hXmlStore->getName();
    try
        {
        switch (translateSchemeType(vsSchemeType))
            {
            case scheme_class:
                return cast<CacheLoader::Handle>(instantiateAny(
                    vInfo, hXmlStore));

            case scheme_remote_cache:
                return cast<CacheLoader::Handle>(configureCache(
                        vInfo, hXmlStore));

            default:
                COH_THROW_STREAM (UnsupportedOperationException,
                    "instantiateCacheStore: " << vsSchemeType);
            }
        }
    catch (ClassCastException::View)
        {
        COH_THROW_STREAM (IllegalArgumentException,
            "Not a CacheLoader:\n" << hXmlStore);
        }
    }

Object::Handle DefaultConfigurableCacheFactory::
    instantiateAny(CacheInfo::View /*vInfo*/, XmlElement::View vXmlClass)
    {
    if (translateSchemeType(vXmlClass->getName()) != scheme_class)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "Invalid class definition: " << vXmlClass);
        }

    return XmlHelper::createInstance(vXmlClass);
    }

Map::Handle DefaultConfigurableCacheFactory::instantiateMap(
        CacheInfo::View vInfo, XmlElement::View vXmlClass)
    {
    try
        {
        return cast<Map::Handle>(instantiateAny(vInfo, vXmlClass));
        }
    catch (ClassCastException::View)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "Not a map:\n" << vXmlClass);
        }
    }

int32_t DefaultConfigurableCacheFactory::convertInt(XmlValue::View vXmlValue) const
    {
    return convertInt(vXmlValue, 0);
    }

int32_t DefaultConfigurableCacheFactory::convertInt(XmlValue::View vXmlValue, int32_t nDefault) const
    {
    try
        {
        String::View    vsValue = vXmlValue->getString();
        Integer32::View vI      = cast<Integer32::View>(XmlHelper::convert(vsValue, XmlValue::type_int));

        return vI == NULL ? nDefault : vI->getValue();
        }
    catch (RuntimeException::View e)
        {
        reportConversionError(vXmlValue, "int32_t", COH_TO_STRING(nDefault), e);
        return nDefault;
        }
    }

void DefaultConfigurableCacheFactory::reportConversionError(XmlValue::View vXmlValue,
        String::View vsType, String::View vsDefault, RuntimeException::View e) const
    {
    COH_LOG("Error converting " << vXmlValue << " to " << vsType <<
            "; proceeding with default value of "<< vsDefault << "\n" << e,
            Logger::level_warning);
    }

void DefaultConfigurableCacheFactory::resolveSerializer(XmlElement::Handle hXmlConfig) const
    {
    // check if the "serializer" element is missing or empty
    XmlElement::View vXmlSerializer = hXmlConfig->getElement("serializer");
    if (vXmlSerializer == NULL || XmlHelper::isEmpty(vXmlSerializer))
        {
        // remove an empty serializer element from the service config
        if (vXmlSerializer != NULL)
            {
            XmlHelper::removeElement(hXmlConfig, "serializer");
            }

        // apply the default serializer (if specified)
        vXmlSerializer = getConfig()->findElement("defaults/serializer");
        if (vXmlSerializer != NULL)
            {
            // Need to clone the serializer element, otherwise
            // RemoteService::doConfigure() will fail on an ensureElement()
            // on the initiator-config.
            hXmlConfig->getElementList()->add(vXmlSerializer->clone());
            }
        }
    }

void DefaultConfigurableCacheFactory::verifyMapListener(CacheInfo::View vInfo, Map::Handle hMap,
        XmlElement::View vXmlScheme, Map::Handle hMapListeners)
    {
    // CONSIDER: allow to configure filter-based listener and replace
    //      mapListeners values with MapListenerSupport objects
    XmlElement::View vXmlListener = vXmlScheme->getSafeElement("listener");
    XmlElement::View vXmlClass    = resolveScheme(vXmlListener, vInfo, true, false);
    if (vXmlClass != NULL)
        {
        String::View vsTier = vXmlScheme->getSafeAttribute("tier")->getString();
        if (vsTier->length() > 0)
            {
            // The "tier" is a synthetic attribute only injected for the top
            // level scheme definition. A listener could be defined at this
            // level in two cases:
            // (a) standard top level cache definition
            // (b) local cache using the same scheme for both front and
            //     back tier declaration
            // To cover a very esoteric case when one desires to listen
            // to the actual LocalCache instance that serves as a
            // backing map for a "local-scheme" we support an undocumented
            // "target" attribute with a valid value of "backing-map"
            bool fBackOnly = vXmlListener->getSafeAttribute("target")->getString()->equals("backing-map");
            if (vsTier->equals("front") && fBackOnly)
                {
                // the backing-map listener is requested
                return;
                }
            if (vsTier->equals("back") && !fBackOnly)
                {
                // prevent local cache registering a listener twice:
                // once for the front tier and the second time for the back
                return;
                }
            }

        MapListener::Handle hListener = instantiateMapListener(vInfo, vXmlClass);
        try
            {
            (cast<ObservableMap::Handle>(hMap))->addMapListener(hListener);
            if (hMapListeners != NULL)
                {
                hMapListeners->put(hMap, hListener);
                }
            }
        catch (ClassCastException::View)
            {
            COH_THROW_STREAM(IllegalArgumentException,
                "Map is not observable: " << Class::getClassName(hMap));
            }
        }
    }

// ----- ConfigurableCacheFactory interface ---------------------------------

NamedCache::Handle
DefaultConfigurableCacheFactory::ensureCache(String::View vsCacheName)
    {
    COH_ENSURE_PARAM(vsCacheName);

    ScopedReferenceStore::Handle hStoreCache =
            cast<ScopedReferenceStore::Handle>(ensureStoreCache());
    NamedCache::Handle           hCache      = cast<NamedCache::Handle>
            (hStoreCache->getCache(vsCacheName));

    if (NULL == hCache || !hCache->isActive())
        {
        CacheInfo::View  vInfoCache = findSchemeMapping(vsCacheName);
        XmlElement::View vXmlScheme = resolveScheme(vInfoCache);

        COH_SYNCHRONIZED (hStoreCache)
            {
            hCache = hStoreCache->getCache(vsCacheName);
            if (NULL == hCache || !hCache->isActive())
                {
                hCache = configureCache(vInfoCache, vXmlScheme);
                hStoreCache->putCache(hCache);
                }
            }
        }
    return hCache;
    }

void DefaultConfigurableCacheFactory::destroyCache(NamedCache::Handle hCache)
    {
    releaseCache(hCache, /*fDestroy*/ true);
    }

void DefaultConfigurableCacheFactory::releaseCache(NamedCache::Handle hCache)
    {
    releaseCache(hCache, /*fDestroy*/ false);
    }

Service::Handle
DefaultConfigurableCacheFactory::ensureService(String::View vsServiceName)
    {
    ScopedReferenceStore::Handle hStoreService =
            cast<ScopedReferenceStore::Handle>(ensureStoreService());
    Service::Handle              hService      = cast<Service::Handle>
            (hStoreService->getService(vsServiceName));

    if (NULL == hService || !hService->isRunning())
        {
        COH_SYNCHRONIZED (hStoreService)
            {
            hService = hStoreService->getService(vsServiceName);
            if (NULL == hService || !hService->isRunning())
                {
                hService = configureService(findServiceScheme(vsServiceName));
                hStoreService->putService(hService, vsServiceName,
                        hService->getInfo()->getServiceType());
                }
            }
        }
    return hService;
    }

void DefaultConfigurableCacheFactory::shutdown()
    {
    // release all caches, we must do all so that we can be sure we've hit
    // the local ones which don't have any associated service
    for (Iterator::Handle hIter = cast<ScopedReferenceStore::Handle>(ensureStoreCache())
            ->getAllCaches()->iterator();
            hIter->hasNext(); )
        {
        cast<NamedCache::Handle>(hIter->next())->release();
        }

    // shutdown all services
    for (Iterator::Handle hIter = cast<ScopedReferenceStore::Handle>(ensureStoreService())
            ->getAllServices()->iterator();
            hIter->hasNext(); )
        {
        cast<Service::Handle>(hIter->next())->shutdown();
        }

    // join all corresponding threads; this is necessary to ensure that all
    // coherence threads have terminated so that we may exit the process
    // without segfaulting
    f_hThreadGroup->join();
    f_hThreadGroup->destroy();
    }


// ----- XmlConfigurable interface ------------------------------------------

XmlElement::View DefaultConfigurableCacheFactory::getConfig() const
    {
    return f_vXmlConfig;
    }


void DefaultConfigurableCacheFactory::setConfig(XmlElement::View vXml)
    {
    XmlElement::View vXmlCache = f_vXmlConfig;
    if (NULL != vXmlCache)
        {
        COH_THROW (UnsupportedOperationException::create("already configured"));
        }

    if (NULL != vXml)
        {
        // COH-23599 - load system property overrides
        XmlElement::Handle hXml = cast<XmlElement::Handle>(vXml->clone());
        XmlHelper::replaceSystemProperties(hXml, "system-property");
        vXml = hXml;
        }

    initialize(f_vXmlConfig, vXml);
    }

// ----- nested class: CacheInfo --------------------------------------------

// ----- constructors ---------------------------------------------------

DefaultConfigurableCacheFactory::CacheInfo::CacheInfo
    (String::View vsCacheName, String::View vsSchemeName, Map::View vMapAttribute)
        : f_vsCacheName(self(), vsCacheName),
          f_vsSchemeName(self(), vsSchemeName),
          f_vMapAttribute(self(), vMapAttribute)
    {
    }

// ----- accessors ------------------------------------------------------

String::View DefaultConfigurableCacheFactory::CacheInfo::getCacheName() const
    {
    return f_vsCacheName;
    }

String::View DefaultConfigurableCacheFactory::CacheInfo::getSchemeName() const
    {
    return f_vsSchemeName;
    }

Map::View DefaultConfigurableCacheFactory::CacheInfo::getAttributes() const
    {
    return f_vMapAttribute;
    }

// ----- helpers --------------------------------------------------------

void DefaultConfigurableCacheFactory::CacheInfo::
    replaceAttributes(XmlElement::Handle hXml) const
    {
    for (Iterator::Handle hIter = hXml->getElementList()->iterator();
         hIter->hasNext(); )
        {
        XmlElement::Handle hXmlChild = cast<XmlElement::Handle>(hIter->next());
        if (!hXmlChild->isEmpty())
            {
            String::View vsText    = hXmlChild->getString();
            size32_t     ofStart   = vsText->indexOf('{');
            size32_t     ofEnd     = String::npos;
            bool         fReplace  = false;
            String::View vsTextNew = "";

            while (ofStart != String::npos)
                {
                vsTextNew = COH_TO_STRING(vsTextNew << vsText->substring(
                        ofEnd == String::npos ? 0 : ofEnd + 1,
                        ofStart));

                ofEnd = vsText->indexOf('}', ofStart);
                if (ofEnd == String::npos)
                    {
                    COH_LOG("Invalid attribute format: " << vsText,
                            Logger::level_error);
                    fReplace = false;
                    break;
                    }

                String::View vsAttribute = vsText->substring
                        (ofStart, ofEnd + 1); // "{name value}"
                String::View vsAttrName  = vsText->substring
                        (ofStart + 1, ofEnd); // "name value"
                String::View vsDefault;
                size32_t     ofDefault  = vsAttrName->indexOf(' ');
                if (ofDefault != String::npos)
                    {
                    vsDefault  =
                            vsAttrName->substring(ofDefault + 1)->trim();
                    vsAttrName = vsAttrName->substring(0, ofDefault);
                    }
                String::View vsValue = vsAttribute->equals("{cache-name}")
                        ? getCacheName()
                        : (String::View) cast<String::View>(
                                getAttributes()->get(vsAttrName));

                if (NULL == vsValue)
                    {
                    if (NULL == vsDefault)
                        {
                        if (!vsAttribute->equals("{class-loader}")
                         && !vsAttribute->equals("{manager-context}")
                         && !vsAttribute->equals("{scheme-ref}")
                         && !vsAttribute->equals("{cache-ref}"))
                            {
                            COH_LOG("Missing parameter definition: "
                                    << vsAttribute << " for cache \""
                                    << getCacheName() << '"',
                                    Logger::level_warning);
                            }
                        fReplace = false;
                        break;
                        }
                    else
                        {
                        vsValue = vsDefault;
                        }
                    }

                vsTextNew = COH_TO_STRING(vsTextNew << vsValue);
                fReplace = true;
                ofStart  = vsText->indexOf('{', ofEnd);
                }

            if (fReplace)
                {
                vsTextNew = COH_TO_STRING(vsTextNew << vsText->substring(ofEnd + 1));
                hXmlChild->setString(vsTextNew);
                }
            }
        replaceAttributes(hXmlChild);
        }
    }

DefaultConfigurableCacheFactory::CacheInfo::Handle
DefaultConfigurableCacheFactory::CacheInfo::getSyntheticInfo(String::View vsSuffix) const
    {
    return CacheInfo::create(COH_TO_STRING(getCacheName() << vsSuffix),
            String::null_string, getAttributes());
    }

COH_CLOSE_NAMESPACE2
