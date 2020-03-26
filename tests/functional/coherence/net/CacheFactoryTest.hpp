/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/WrapperStreamFactory.hpp"
#include "coherence/io/pof/SimplePofContext.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/CacheService.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/Member.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/run/xml/XmlConfigurable.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/tests/IncrementorInvocable.hpp"
#include "coherence/tests/POFObjectInvocable.hpp"
#include "coherence/util/CompositeKey.hpp"
#include "coherence/util/Map.hpp"

#include "common/PortablePerson.hpp"
#include "common/TestUtils.hpp"

#include "private/coherence/component/net/extend/RemoteInvocationService.hpp"
#include "private/coherence/component/util/SafeInvocationService.hpp"
#include "private/coherence/component/util/SafeService.hpp"
#include "private/coherence/net/LocalMember.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::component::util::SafeService;
using coherence::io::IOException;
using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::io::WrapperStreamFactory;
using coherence::io::pof::SimplePofContext;
using coherence::net::CacheFactory;
using coherence::net::CacheService;
using coherence::net::ConfigurableCacheFactory;
using coherence::net::InvocationService;
using coherence::net::LocalMember;
using coherence::net::Member;
using coherence::net::NamedCache;
using coherence::net::ServiceInfo;
using coherence::run::xml::XmlConfigurable;
using coherence::run::xml::XmlElement;
using coherence::tests::IncrementorInvocable;
using coherence::tests::POFObjectInvocable;
using coherence::util::CompositeKey;
using coherence::util::Map;
using coherence::component::net::extend::RemoteInvocationService;
using coherence::component::util::SafeInvocationService;

using common::test::PortablePerson;

class NullNetworkFilter
    : public class_spec<NullNetworkFilter,
        extends<Object>,
        implements<WrapperStreamFactory, XmlConfigurable> >
    {
    friend class factory<NullNetworkFilter>;

    public:
       virtual ReadBuffer::BufferInput::Handle getInputStream(
               ReadBuffer::BufferInput::Handle hIn) const
               {
               s_fis = true;
               return hIn;
               }

       virtual WriteBuffer::BufferOutput::Handle getOutputStream(
               WriteBuffer::BufferOutput::Handle hOut) const
               {
               s_fos = true;
               return hOut;
               }

       virtual void setConfig(XmlElement::View vXmlConfig)
           {
           COH_ENSURE(vXmlConfig->getSafeElement("dummy1")->getString()->equals("value1"));
           COH_ENSURE(vXmlConfig->getSafeElement("dummy2")->getString()->equals("value2"));

           s_fcfg = true;
           }

       virtual XmlElement::View getConfig() const
           {
           return NULL;
           }

       static void ensureUsed()
            {
            TS_ASSERT(s_fcfg);
            TS_ASSERT(s_fis);
            TS_ASSERT(s_fos);
            }

    private:
        static bool s_fcfg;
        static bool s_fis;
        static bool s_fos;
    };
COH_REGISTER_TYPED_CLASS(NullNetworkFilter);

bool NullNetworkFilter::s_fcfg = false;
bool NullNetworkFilter::s_fis  = false;
bool NullNetworkFilter::s_fos  = false;

/**
* CacheFactory Test Suite.
*/
class CacheFactoryTest : public CxxTest::TestSuite
    {
    // ----- test fixtures --------------------------------------------------

    public:
        /**
         * Runs after each test
         */
        void tearDown()
            {
            CacheFactory::shutdown();
            }

    public:
        /**
        * Test getCache().
        */
        void testGetCache()
            {
            String::Handle hCacheName = "dist-extend";
            NamedCache::Handle hNamedCache = CacheFactory::getCache(hCacheName);

            TS_ASSERT(hNamedCache != NULL);
            TS_ASSERT(hCacheName->equals(hNamedCache->getCacheName()));
            TS_ASSERT(hNamedCache->isActive() == true);
            CacheFactory::releaseCache(hNamedCache);
            TS_ASSERT(hNamedCache->isActive() == false);
            TS_ASSERT(hNamedCache->getCacheService()->isRunning() == true);
            }

        /**
         * Test macro in cache-scheme.
         */
        void testMacroGetCache()
            {
            String::View   vsCacheConfig = String::create("defaultconfig/extend-cache-config.xml");
            String::Handle hCacheName    = "test-macro-cache";

            // test with inlined system property coherence.remote.client set to near
            System::setProperty("coherence.remote.client", "near");
            CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));

            NamedCache::Handle                               hNamedCache = CacheFactory::getCache(hCacheName);
            ConfigurableCacheFactory::Handle                 hccf        = CacheFactory::getConfigurableCacheFactory();
            DefaultConfigurableCacheFactory::Handle          dccf        = cast<DefaultConfigurableCacheFactory::Handle>(hccf);
            DefaultConfigurableCacheFactory::CacheInfo::View vci         = dccf->findSchemeMapping(hCacheName);

            std::cout << "MacroGetCache: cacheName=" << hCacheName << " scheme-name=" << vci->getSchemeName() <<
                                "system property coherence.remote.client=near" << std::endl;
            TS_ASSERT(vci != NULL);
            TS_ASSERT(vci->getCacheName()->equals(hCacheName));
            TS_ASSERT(vci->getSchemeName()->equals("near-remote"));
            TS_ASSERT(hNamedCache != NULL);
            TS_ASSERT(hCacheName->equals(hNamedCache->getCacheName()));
            TS_ASSERT(hNamedCache->isActive() == true);
            TS_ASSERT(hNamedCache->getCacheService()->isRunning() == true);
            TS_ASSERT(hNamedCache->getCacheService()->getInfo()->getServiceName()->equals("ExtendTcpCacheService"));
            CacheFactory::releaseCache(hNamedCache);
            TS_ASSERT(hNamedCache->isActive() == false);

            // test with inlined system property coherence.remote.client set to thin
            System::setProperty("coherence.remote.client", "thin");
            CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
            hNamedCache = CacheFactory::getCache(hCacheName);
            hccf        = CacheFactory::getConfigurableCacheFactory();
            dccf        = cast<DefaultConfigurableCacheFactory::Handle>(hccf);
            vci         = dccf->findSchemeMapping(hCacheName);
            std::cout << "MacroGetCache: cacheName=" << hCacheName << " scheme-name=" << vci->getSchemeName() <<
                    "system property coherence.remote.client=thin"  << std::endl;
            TS_ASSERT(vci != NULL);
            TS_ASSERT(vci->getCacheName()->equals(hCacheName));
            TS_ASSERT(vci->getSchemeName()->equals("thin-remote"));
            TS_ASSERT(hNamedCache != NULL);
            TS_ASSERT(hCacheName->equals(hNamedCache->getCacheName()));
            TS_ASSERT(hNamedCache->isActive() == true);
            TS_ASSERT(hNamedCache->getCacheService()->isRunning() == true);
            TS_ASSERT(hNamedCache->getCacheService()->getInfo()->getServiceType() == ServiceInfo::remote_cache);
            CacheFactory::releaseCache(hNamedCache);
            TS_ASSERT(hNamedCache->isActive() == false);

            // restore the default cache configuration
            setDefaultCacheConfigure();
            System::clearProperty("coherence.remote.client");
            }

        /**
        * Test remote invocation().
        */
        void testRemoteInvocation()
            {
            InvocationService::Handle hService = cast<InvocationService::Handle>(CacheFactory::getService("ExtendTcpInvocationService"));
            TS_ASSERT(hService != NULL);
            TS_ASSERT(instanceof<SafeInvocationService::View>(hService));
            SafeInvocationService::Handle hSafeService = cast<SafeInvocationService::Handle>(hService);
            TS_ASSERT(hSafeService->isRunning());
            TS_ASSERT(hSafeService->getServiceName()->equals("ExtendTcpInvocationService"));
            TS_ASSERT(hSafeService->getServiceType() == ServiceInfo::remote_invocation);
            TS_ASSERT(hSafeService->getService() != NULL);
            TS_ASSERT(instanceof<RemoteInvocationService::View>(hSafeService->getService()));
            RemoteInvocationService::View hRemoteService = cast<RemoteInvocationService::View>(hSafeService->getService());
            TS_ASSERT(hRemoteService->isRunning());
            TS_ASSERT(hRemoteService->getServiceName()->equals("ExtendTcpInvocationService"));
            TS_ASSERT(hRemoteService->getServiceType() == ServiceInfo::remote_invocation);

            IncrementorInvocable::Handle hInvocable = IncrementorInvocable::create(1);

            Member::View vMember = hSafeService->getOperationalContext()->getLocalMember();
            Map::View vMap = hSafeService->query(hInvocable, NULL);
            TS_ASSERT(vMap->size() == 1);

            Integer32::View vI = cast<Integer32::View>(vMap->get(vMember));
            TS_ASSERT(vI->getInt32Value() == 2);
            }

        void testMemberInvocation()
            {
            InvocationService::Handle hService =
                    cast<InvocationService::Handle>(
                            CacheFactory::getService(
                                    "ExtendTcpInvocationService"));

            LocalMember::Handle hMember = LocalMember::create();
            hMember->setSiteName(String::create("site1"));
            hMember->setRackName(String::create("rack1"));
            hMember->setMachineName(String::create("machine1"));
            hMember->setProcessName(String::create("process1"));
            hMember->setMemberName(String::create("member1"));
            hMember->setRoleName(String::create("role1"));
            POFObjectInvocable::Handle hInvocable = POFObjectInvocable::create(hMember);

            Member::View vMember =
                    cast<SafeService::View>(hService)->
                    getOperationalContext()->getLocalMember();
            Map::View vMap = hService->query(hInvocable, NULL);
            TS_ASSERT(vMap->size() == 1);

            Member::View vCopy = cast<Member::View>(vMap->get(vMember));
            TS_ASSERT(vCopy->getSiteName()->equals(hMember->getSiteName()));
            TS_ASSERT(vCopy->getRackName()->equals(hMember->getRackName()));
            TS_ASSERT(vCopy->getMachineName()->equals(hMember->getMachineName()));
            TS_ASSERT(vCopy->getProcessName()->equals(hMember->getProcessName()));
            TS_ASSERT(vCopy->getMemberName()->equals(hMember->getMemberName()));
            TS_ASSERT(vCopy->getRoleName()->equals(hMember->getRoleName()));
            }

        void testCompositeKeyInvocation()
            {
            InvocationService::Handle hService = cast<InvocationService::Handle>(CacheFactory::getService("ExtendTcpInvocationService"));

            CompositeKey::Handle hCompKey = CompositeKey::create(String::create("abc"),
                                                                 String::create("xyz"));
            POFObjectInvocable::Handle hInvocable = POFObjectInvocable::create(hCompKey);

            Member::View vMember =
                    cast<SafeService::View>(hService)->
                    getOperationalContext()->getLocalMember();
            Map::View vMap = hService->query(hInvocable, NULL);
            TS_ASSERT(vMap->size() == 1);

            CompositeKey::View vCopy = cast<CompositeKey::View>(vMap->get(vMember));
            TS_ASSERT(vCopy->equals(hCompKey));
            }

        /**
        * Test network filters
        */
        void testNetworkFilters()
            {
            // Inject NullNetworkFilter into operational config
            /*
            * <filters>
            *   <filter id="1">
            *     <filter-name>DummyFilter</filter-name>
            *     <filter-class>NullNetworkFilter</filter-class>
            *     <init-params>
            *       <init-param id="1">
            *         <param-name>dummy1</param-name>
            *         <param-value>value1</param-value>
            *       </init-param>
            *      <init-param id="2">
            *        <param-name>dummy2</param-name>
            *        <param-value>value2</param-value>
            *      </init-param>
            *    </init-params>
            *   </filter>
            * </filters>
            */
            String::View vsFile = System::getProperty(
                    "tangosol.coherence.override",
                    "tangosol-coherence-override.xml");
            XmlElement::Handle hContext = CacheFactory::loadXmlFile(vsFile);
            XmlElement::Handle hFilter = hContext
                    ->findElement("cluster-config")
                    ->addElement("filters")->addElement("filter");
            hFilter->addAttribute("id")->setInt32(1);
            hFilter->addElement("filter-name")
                    ->setString("DummyFilter");
            hFilter->addElement("filter-class")
                    ->setString("NullNetworkFilter");
            XmlElement::Handle hParam1 = hFilter->addElement("init-params")
                    ->addElement("init-param");
            XmlElement::Handle hParam2 = hFilter->findElement("init-params")
                    ->addElement("init-param");

            hParam1->addAttribute("id")->setInt32(1);
            hParam1->addElement("param-name")->setString("dummy1");
            hParam1->addElement("param-value")->setString("value1");

            hParam2->addAttribute("id")->setInt32(2);
            hParam2->addElement("param-name")->setString("dummy2");
            hParam2->addElement("param-value")->setString("value2");

            CacheFactory::configure(
                    CacheFactory::loadXmlFile(System::getProperty(
                            "tangosol.coherence.cacheconfig",
                            "coherence-cache-config.xml")),
                            hContext);
            NamedCache::Handle hCache = CacheFactory::getCache("filter-foo");
            hCache->put(String::create("sick"), String::create("tired"));
            TS_ASSERT(hCache->get(String::create("sick"))->equals(String::create("tired")));
            NullNetworkFilter::ensureUsed();
            }

        /**
        * Test name service.
        */
        void testNameService()
            {
            CacheService::Handle hService = cast<CacheService::Handle>(CacheFactory::getService("ExtendTcpCacheService4"));

            String::Handle hCacheName = "ephemeral-extend";
            NamedCache::Handle hNamedCache = CacheFactory::getCache(hCacheName);

            TS_ASSERT(hNamedCache != NULL);
            TS_ASSERT(hCacheName->equals(hNamedCache->getCacheName()));
            TS_ASSERT(hNamedCache->isActive());
            CacheFactory::releaseCache(hNamedCache);
            TS_ASSERT(!hNamedCache->isActive());
            TS_ASSERT(hNamedCache->getCacheService()->isRunning());
            }

        /**
        * Regression test for COH-8911.
        */
        void testCompositeKey()
            {
            CacheService::Handle hService    = cast<CacheService::Handle>
                    (CacheFactory::getService("ExtendTcpCacheService"));
            Serializer::View     vSerializer = hService->getSerializer();
            if (instanceof<SimplePofContext::View>(vSerializer))
                {
                cast<SimplePofContext::View>(vSerializer)->setReferenceEnabled(true);
                }

            NamedCache::Handle     hCache     = hService->ensureCache("dist-extend-direct");
            CompositeKey::Handle   hKey0      = CompositeKey::create(
                    PortablePerson::create(String::create("Joe Smith"), 30),
                    PortablePerson::create(String::create("Joe Smith"), 30));
            PortablePerson::Handle hPerson1   = PortablePerson::create("Joe Smith", 30);
            CompositeKey::Handle   hKey1      = CompositeKey::create(hPerson1, hPerson1);

            hCache->clear();
            hCache->put(hKey0, String::create("value0"));
            hCache->put(hKey1, String::create("value1"));

            TS_ASSERT(hCache->entrySet()->size() == 1);
            if (instanceof<SimplePofContext::View>(vSerializer))
                {
                cast<SimplePofContext::View>(vSerializer)->setReferenceEnabled(false);
                }
            hService->releaseCache(hCache);
            }

        void testInvalidConfig()
            {
            try
                {
                String::View vsCacheName = String::create("dist-default");
                String::View vsCacheConfig = String::create("defaultconfig/extend-cache-config-invalid.xml");

                CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
                TS_ASSERT(false);
                }
            catch (IOException::View ve)
                {
                // expected...
                TS_ASSERT(ve->getDescription()->startsWith("coherence::io::IOException: Error opening file:"));
                }
            }

        void testTypoConfig()
            {
            try
                {
                String::View vsCacheName = String::create("dist-default");
                String::View vsCacheConfig = String::create("defaultconfig/invalid-extend-cache-config.xml");

                CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
                TS_ASSERT(false);
                }
            catch (RuntimeException::View ve)
                {
                // expected...
                TS_ASSERT(ve->getDescription()->startsWith("coherence::lang::RuntimeException: Exception occurred during parsing: looking for id=request-timeout, found"));
                }
            }

        void testCacheSubstitutions()
            {
            String::View vsCacheName   = String::create("test-substitution");
            String::View vsCacheConfig = String::create("defaultconfig/extend-cache-config.xml");

            CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
            NamedCache::Handle hNamedCache = CacheFactory::getCache(vsCacheName);

            TS_ASSERT(hNamedCache != NULL);
            TS_ASSERT(vsCacheName->equals(hNamedCache->getCacheName()));
            TS_ASSERT(hNamedCache->isActive() == true);
            CacheFactory::releaseCache(hNamedCache);

            // restore the default cache configuration
            setDefaultCacheConfigure();
            }

        /**
        * COH-6301 Test clone() on XmlElement returned by CacheFactory::loadXml().
        */
        void testXmlDocClone()
            {
            String::View sCacheName = "dist-extend";
            String::View sAddress   = "127.0.0.1";
            int          nPort      = 2222;

            try {
                std::stringstream ss;
                ss  << "      <remote-cache-scheme>"
                    << "      <scheme-name>" << sCacheName << " - " << sAddress << ":" << nPort << "</scheme-name>"
                    << "      <service-name>" << sCacheName << " - " << sAddress << ":" << nPort << "</service-name>"
                    << "         <initiator-config>"
                    << "            <tcp-initiator>"
                    << "               <remote-addresses>"
                    << "                  <socket-address>"
                    << "                     <address>" << sAddress << "</address>"
                    << "                     <port>" << nPort << "</port>"
                    << "                  </socket-address>"
                    << "               </remote-addresses>"
                    << "            </tcp-initiator>"
                    << "            <connect-timeout>2s</connect-timeout>"
                    << "         </initiator-config>"
                    << "      </remote-cache-scheme>";

                XmlElement::View vXmlDoc = CacheFactory::loadXml(ss);
                TS_ASSERT(vXmlDoc->equals(vXmlDoc->clone()));
                }
            catch (Exception::View ve)
                {
                TS_FAIL(ve->getDescription());
                }
            }
    };
