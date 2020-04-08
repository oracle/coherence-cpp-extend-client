/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/CacheFactory.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/DefaultOperationalContext.hpp"

#include "private/coherence/net/Console.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <fstream>

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::IOException;
using coherence::run::xml::XmlHelper;
using coherence::util::logging::Logger;


COH_REGISTER_CLASS((TypedExecutableClass<CacheFactory,
        TypedBarrenClass<CacheFactory> >::create()));


COH_OPEN_NAMESPACE_ANON(CacheFactory)

/**
* CacheFactoryImpl is a helper class for CacheFactory.
*
* This class stores a handle to a ConfigurableCacheFactory (CCF) and
* manages the Logger.  This class is used by CacheFactory's static
* methods to guarantee that there is always at most one CCF
* used and/or returned by the CacheFactory methods.
*
* @author jh, phf  2010.11.01
*/
class CacheFactoryImpl
    : public class_spec<CacheFactoryImpl>
    {
    friend class factory<CacheFactoryImpl>;

    // ----- constructor ------------------------------------------------

    protected:
        /**
         * Create a new CacheFactoryImpl
         */
        CacheFactoryImpl()
            : m_hCacheFactory(self())
            {
            }


    // ----- CacheFactoryImplInterface ----------------------------------
    public:
        /**
        * Return the CCF.  If not yet initialized, create, configure and
        * return a DefaultConfigurableCacheFactory
        *
        * @return the CCF
        */
        virtual ConfigurableCacheFactory::Handle getCacheFactory()
            {
            ConfigurableCacheFactory::Handle hFactory = m_hCacheFactory;
            if (NULL == hFactory)
                {
                COH_SYNCHRONIZED (this)
                    {
                    hFactory = m_hCacheFactory;
                    if (NULL == hFactory)
                        {
                        OperationalContext::View vContext =
                                DefaultOperationalContext::create();
                        initLogger(vContext);
                        configure(cast<XmlElement::View>(DefaultConfigurableCacheFactory::
                                getDefaultCacheConfig()), vContext);
                        }
                    hFactory = m_hCacheFactory;
                    }
                }

            return hFactory;
            }

        /**
        * Configure the CacheFactoryImpl using the supplied
        * XML Elements for the DefaultConfigurableCacheFactory and
        * DefaultOperationalContext
        *
        * @param vXmlCache      an XML element corresponding
        *                       to cache-config.dtd
        * @param vXmlCoherence  (optional) an XML element corresponding
        *                       to coherence.dtd
        */
        virtual void configure(XmlElement::View vXmlCache,
                XmlElement::View vXmlCoherence = NULL)
            {
            OperationalContext::View vContext =
                    DefaultOperationalContext::create(vXmlCoherence);
            initLogger(vContext);
            configure(vXmlCache, vContext);
            }

        /**
        * Configure the CacheFactoryImpl using the supplied
        * XML Elements for the DefaultConfigurableCacheFactory and
        * DefaultOperationalContext
        *
        * @param vXmlCache  an XML element corresponding to
        *                       cache-config.dtd
        * @param vContext   (optional) an XML element corresponding
        *                       to coherence.dtd
        */
        virtual void configure(XmlElement::View vXmlCache,
                OperationalContext::View vContext)
            {
            ConfigurableCacheFactory::Handle hFactory =
                    DefaultConfigurableCacheFactory::create();
            cast<DefaultConfigurableCacheFactory::Handle>(hFactory)
                    ->setOperationalContext(vContext);
            hFactory->setConfig(vXmlCache);
            configure(hFactory, vContext, false);
            }

        /**
        * Configure the CacheFactoryImpl using the supplied
        * ConfigurableCacheFactory and OperationalContext.
        */
        virtual void configure(ConfigurableCacheFactory::Handle hFactory,
                OperationalContext::View vContext, bool fInitLogger = true)
            {
            COH_SYNCHRONIZED (this)
                {
                if (NULL != vContext && fInitLogger)
                    {
                    initLogger(vContext);
                    }

                ConfigurableCacheFactory::Handle hFactoryOld =
                        m_hCacheFactory;
                if (NULL != hFactoryOld)
                    {
                    hFactoryOld->shutdown();
                    }
                m_hCacheFactory = hFactory;
                }
            }

        /**
        * System shutdown.
        *
        * Shut down the ConfigurableCacheFactory and Logger
        */
        virtual void shutdown()
            {
            COH_SYNCHRONIZED (this)
                {
                ConfigurableCacheFactory::Handle hFactory =
                        m_hCacheFactory;

                if (NULL != hFactory)
                    {
                    hFactory->shutdown();
                    }
                Logger::getLogger()->shutdown();
                m_hCacheFactory = NULL;
                }
            }


    // ----- helper methods ---------------------------------------------

    protected:
        /**
        * Initialize the Logger.
        *
        * @param vContext  an OperationalContext to initialize the Logger
        *                  with
        */
        virtual void initLogger(OperationalContext::View vContext)
            {
            Logger::Handle hLogger = Logger::getLogger();
            // Switch to a new logger
            hLogger->shutdown();
            hLogger = Logger::getLogger();
            hLogger->configure(vContext);
            hLogger->start();

            // output the product startup banner
            COH_LOG('\n'
                    << hLogger->getProduct()
                    << " Version "
                    << hLogger->getVersion()
                    << " Build "
                    << COH_SYMB_TO_STRING(COH_BUILD_INFO)
                    << '\n'
                    << ' ' << hLogger->getEdition() << ' '
                    << COH_SYMB_TO_STRING(COH_PLATFORM) << ' '
                    << COH_SYMB_TO_STRING(COH_BUILD_TYPE) << ' '
                    << COH_SYMB_TO_STRING(COH_CC)
                    << " build\n"
                    << COH_COPYRIGHT << '\n',
                    Logger::level_internal);
            }


    // ----- data members -----------------------------------------------

    protected:
        /**
        * The ConfigurableCacheFactory.
        */
        MemberHandle<ConfigurableCacheFactory> m_hCacheFactory;
    };

/**
* Return the singleton CacheFactoryImpl.
*
* @return the CacheFactoryImpl
*/
CacheFactoryImpl::Handle getCacheFactoryImpl()
    {
    static FinalHandle<CacheFactoryImpl> hSingleton(System::common(),
            CacheFactoryImpl::create());
    return hSingleton;
    }
COH_STATIC_INIT(getCacheFactoryImpl());

COH_CLOSE_NAMESPACE_ANON

// -------- CacheFactory interface ------------------------------------------

NamedCache::Handle CacheFactory::getCache(String::View vsName)
    {
    return getConfigurableCacheFactory()->ensureCache(vsName);
    }

void CacheFactory::destroyCache(NamedCache::Handle hCache)
    {
    getConfigurableCacheFactory()->destroyCache(hCache);
    }

void CacheFactory::releaseCache(NamedCache::Handle hCache)
    {
    getConfigurableCacheFactory()->releaseCache(hCache);
    }

Service::Handle CacheFactory::getService(String::View vsName)
    {
    return getConfigurableCacheFactory()->ensureService(vsName);
    }

void CacheFactory::shutdown()
    {
    getCacheFactoryImpl()->shutdown();
    }

void CacheFactory::configure(XmlElement::View vXmlCache, XmlElement::View vXmlCoherence)
    {
    getCacheFactoryImpl()->configure(vXmlCache, vXmlCoherence);
    }

XmlElement::Handle CacheFactory::loadXml(String::View vsXml)
    {
    XmlElement::Handle hXml = XmlHelper::loadXml(vsXml);

    XmlHelper::replaceSystemProperties(hXml, "system-property");
    // TODO: override file

    return hXml;
    }

XmlElement::Handle CacheFactory::loadXmlFile(String::View vsFile)
    {
    std::ifstream in(vsFile->getCString(), std::ifstream::in);
    if (in.fail())
        {
        COH_THROW_STREAM (IOException, "Error opening file: \""
                << vsFile << "\"");
        }
    return loadXml(in);
    }

ConfigurableCacheFactory::Handle CacheFactory::getConfigurableCacheFactory()
    {
    return getCacheFactoryImpl()->getCacheFactory();
    }

void CacheFactory::setConfigurableCacheFactory(
        ConfigurableCacheFactory::Handle hFactory,
        OperationalContext::View vContext)
    {
    if (NULL == vContext && instanceof<DefaultConfigurableCacheFactory::Handle>(hFactory))
        {
        vContext = cast<DefaultConfigurableCacheFactory::Handle>(hFactory)
                ->ensureOperationalContext();
        }
    getCacheFactoryImpl()->configure(hFactory, vContext);
    }

void CacheFactory::main(ObjectArray::View vasArg)
    {
    Console::main(vasArg);
    }

COH_CLOSE_NAMESPACE2
