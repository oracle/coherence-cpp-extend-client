/*
 * Copyright (c) 2000, 2021, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/WrapperStreamFactory.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/DefaultOperationalContext.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/security/DefaultIdentityAsserter.hpp"
#include "coherence/security/DefaultIdentityTransformer.hpp"
#include "coherence/security/IdentityAsserter.hpp"
#include "coherence/security/IdentityTransformer.hpp"

#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <sstream>

using namespace coherence::lang;
using namespace std;

using coherence::io::WrapperStreamFactory;
using coherence::net::CacheFactory;
using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::DefaultOperationalContext;
using coherence::net::OperationalContext;
using coherence::run::xml::SimpleElement;
using coherence::run::xml::XmlElement;
using coherence::security::DefaultIdentityAsserter;
using coherence::security::DefaultIdentityTransformer;
using coherence::security::IdentityAsserter;
using coherence::security::IdentityTransformer;
using coherence::util::logging::Logger;

COH_OPEN_NAMESPACE2(test,operationalcontext)


/**
* Test IdentityAsserter
*
* Used to verify that the DefaultOperationalContext will load an
* IdentityAsserter from config.
*/
class TestIdentityAsserter
    : public class_spec<TestIdentityAsserter,
         extends<Object>,
         implements<IdentityAsserter> >
    {
    friend class factory<TestIdentityAsserter>;

    // ------ IdentityAsserter interface ------------------------------------

    public:
        virtual Subject::View assertIdentity(Object::View /*vToken*/,
                coherence::net::Service::View /*vService*/) const
            {
            return NULL;
            }
    };
COH_REGISTER_TYPED_CLASS(TestIdentityAsserter);

/**
* Test IdentityTransformer
*
* Used to verify that the DefaultOperationalContext will load an
* IdentityTransformer from config.
*/
class TestIdentityTransformer
    : public class_spec<TestIdentityTransformer,
         extends<Object>,
         implements<IdentityTransformer> >
    {
    friend class factory<TestIdentityTransformer>;

    // ------ IdentityTransformer interface ---------------------------------

    public:
        virtual Object::View transformIdentity(Subject::View /*vSubject*/,
                coherence::net::Service::View /*vService*/) const
            {
            return NULL;
            }
    };
COH_REGISTER_TYPED_CLASS(TestIdentityTransformer);

/**
* Test Network Filter
*
* Used to verify that the DefaultOperationalContext will load a
* filter from config.
*/
class TestNetworkFilter
   : public class_spec<TestNetworkFilter,
       extends<Object>,
       implements<WrapperStreamFactory, XmlConfigurable> >
   {
   friend class factory<TestNetworkFilter>;

   // ------ WrapperStreamFactory interface ---------------------------------

    public:
       virtual ReadBuffer::BufferInput::Handle getInputStream(
               ReadBuffer::BufferInput::Handle hIn) const
               {
               return hIn;
               }

       virtual WriteBuffer::BufferOutput::Handle getOutputStream(
               WriteBuffer::BufferOutput::Handle hOut) const
               {
               return hOut;
               }


   // ------ XmlConfigurable interface --------------------------------------

   public:
       virtual void setConfig(XmlElement::View /*vXmlConfig*/)
           {
           }

       virtual XmlElement::View getConfig() const
           {
           return NULL;
           }
    };
COH_REGISTER_TYPED_CLASS(TestNetworkFilter);

COH_CLOSE_NAMESPACE2


/**
* DefaultOperationalContext test suite
*/
class DefaultOperationalContextTest : public CxxTest::TestSuite
    {
    public:

        void testEmptyConfiguration()
            {
            verifyDefaultConfig(DefaultOperationalContext::create(loadEmptyConfig()));
            CacheFactory::shutdown();
            }

        void testEmptyConfigurationThroughCacheFactory()
            {
            CacheFactory::configure(NULL, loadEmptyConfig());
            verifyDefaultConfig(
                cast<DefaultConfigurableCacheFactory::Handle>(
                    CacheFactory::getConfigurableCacheFactory())
                    ->ensureOperationalContext());
            TS_ASSERT(Logger::getLogger()->getLevel() == Logger::getDefaultLevel());
            CacheFactory::shutdown();
            }

        void testNullConfigurationThroughCacheFactory()
            {
            CacheFactory::configure(NULL);
            verifyDefaultConfig(
                cast<DefaultConfigurableCacheFactory::Handle>(
                    CacheFactory::getConfigurableCacheFactory())
                    ->ensureOperationalContext());
            TS_ASSERT(Logger::getLogger()->getLevel() == Logger::getDefaultLevel());
            CacheFactory::shutdown();
            }

        void testCustomConfig()
            {
            verifyCustomConfig(DefaultOperationalContext::
                    create(loadCustomConfig()));
            }

        void testCustomConfigurationThroughCacheFactory()
            {
            CacheFactory::configure(NULL, loadCustomConfig());
            verifyCustomConfig(
                cast<DefaultConfigurableCacheFactory::Handle>(
                    CacheFactory::getConfigurableCacheFactory())
                    ->ensureOperationalContext());
            TS_ASSERT(Logger::getLogger()->getLevel() == getCustomLogLevel());
            CacheFactory::shutdown();
            }

        /**
        * COH-23599
        */
        void testConfigThroughEnvironmentVariable()
            {
            String::View vsOverride = System::setProperty("coherence.override", "config/system-property-override.xml");

            System::setProperty("test.edition", "DC");

            DefaultOperationalContext::View vContext = DefaultOperationalContext::create();

            TS_ASSERT_EQUALS(DefaultOperationalContext::edition_dc, vContext->getEdition());

            // revert the "coherence.override" property back to its previous value (or clear it)
            if (NULL == vsOverride)
                {
                System::clearProperty("coherence.override");
                }
            else
                {
                System::setProperty("coherence.override", vsOverride);
                }
            }

        void verifyDefaultConfig(OperationalContext::View vContext)
            {
            TS_ASSERT(vContext->getEdition() == DefaultOperationalContext::edition_rtc);
            TS_ASSERT(vContext->getEditionName()->equals("RTC"));
            TS_ASSERT(vContext->getFilterMap()->size() == 0);
            TS_ASSERT(instanceof<DefaultIdentityAsserter::View>
                (vContext->getIdentityAsserter()));
            TS_ASSERT(instanceof<DefaultIdentityTransformer::View>
                (vContext->getIdentityTransformer()));
            TS_ASSERT(vContext->getLocalMember() != NULL);
            TS_ASSERT(vContext->getLocalMember()->getMachineName() != NULL);
            TS_ASSERT(vContext->getLocalMember()->getMemberName() == NULL);
            TS_ASSERT(vContext->getLocalMember()->getProcessName() != NULL);
            TS_ASSERT(vContext->getLocalMember()->getRackName() == NULL);
            TS_ASSERT(vContext->getLocalMember()->getRoleName()->equals("C++ RTC client"));
            // TS_ASSERT(vContext->getLocalMember()->getSiteName() ==
            //      NULL); - machine dependent
            TS_ASSERT(vContext->getLogLevel() == Logger::getDefaultLevel());
            TS_ASSERT(vContext->getLogCharacterLimit() == 0);
            TS_ASSERT(vContext->getLogDestination() == NULL);
            TS_ASSERT(vContext->getLogMessageFormat()->length() == 0);
            TS_ASSERT(!vContext->isSubjectScopingEnabled());
            }

        void verifyCustomConfig(OperationalContext::View vContext)
            {
            TS_ASSERT(vContext->getEdition() == DefaultOperationalContext::edition_dc);
            TS_ASSERT(vContext->getEditionName()->equals("DC"));
            TS_ASSERT(instanceof<test::operationalcontext::
                    TestNetworkFilter::View>(
                            vContext->getFilterMap()
                            ->get(String::create("TestNetworkFilter"))));
            TS_ASSERT(instanceof<test::operationalcontext::
                    TestIdentityAsserter::View>(
                            vContext->getIdentityAsserter()));
            TS_ASSERT(instanceof<test::operationalcontext::
                    TestIdentityTransformer::View>
                            (vContext->getIdentityTransformer()));
            TS_ASSERT(vContext->getLocalMember() != NULL);
            TS_ASSERT(vContext->getLocalMember()->getMachineName()
                    ->equals("test-machine"));
            TS_ASSERT(vContext->getLocalMember()->getMemberName()
                    ->equals("test-member"));
            TS_ASSERT(vContext->getLocalMember()->getProcessName()
                    ->equals("test-process"));
            TS_ASSERT(vContext->getLocalMember()->getRackName()
                    ->equals("test-rack"));
            TS_ASSERT(vContext->getLocalMember()->getRoleName()
                    ->equals("test-role"));
            TS_ASSERT(vContext->getLocalMember()->getSiteName()
                    ->equals("test-site"));
            TS_ASSERT(vContext->getLogCharacterLimit() == 123);
            TS_ASSERT(vContext->getLogMessageFormat()->equals("foo"));
            TS_ASSERT(vContext->getLogDestination()->equals("bar"));
            TS_ASSERT(vContext->isSubjectScopingEnabled());
            }

        XmlElement::View loadEmptyConfig()
            {
            return SimpleElement::create("coherence");
            }

        int32_t getCustomLogLevel()
            {
            return Logger::getDefaultLevel() + 1;
            }

        XmlElement::View loadCustomConfig()
            {
            stringstream ss;
            ss << "<coherence>"
               << "  <logging-config>"
               << "    <severity-level>"
               <<        getCustomLogLevel()
               << "    </severity-level>"
               << "    <character-limit>123</character-limit>"
               << "    <message-format>foo</message-format>"
               << "    <destination>bar</destination>"
               << "  </logging-config>"
               << "  <license-config>"
               << "    <edition-name>DC</edition-name>"
               << "  </license-config>"
               << "  <cluster-config>"
               << "    <filters>"
               << "      <filter>"
               << "        <filter-name>TestNetworkFilter</filter-name>"
               << "        <filter-class>test::operationalcontext::"
               << "TestNetworkFilter</filter-class>"
               << "        <init-params>"
               << "          <init-param>"
               << "            <param-name>dummy1</param-name>"
               << "            <param-value>value1</param-value>"
               << "          </init-param>"
               << "          <init-param>"
               << "            <param-name>dummy2</param-name>"
               << "            <param-value>value2</param-value>"
               << "          </init-param>"
               << "        </init-params>"
               << "      </filter>"
               << "    </filters>"
               << "    <member-identity>"
               << "      <site-name>test-site</site-name>"
               << "      <rack-name>test-rack</rack-name>"
               << "      <machine-name>test-machine</machine-name>"
               << "      <process-name>test-process</process-name>"
               << "      <member-name>test-member</member-name>"
               << "      <role-name>test-role</role-name>"
               << "    </member-identity>"
               << "  </cluster-config>"
               << "  <security-config>"
               << "    <identity-asserter>"
               << "      <instance>"
               << "        <class-name>test::operationalcontext::"
               << "TestIdentityAsserter</class-name>"
               << "      </instance>"
               << "    </identity-asserter>"
               << "    <identity-transformer>"
               << "      <instance>"
               << "        <class-name>test::operationalcontext::"
               << "TestIdentityTransformer</class-name>"
               << "      </instance>"
               << "    </identity-transformer>"
               << "    <subject-scope>true</subject-scope>"
               << "  </security-config>"
               << "</coherence>";
            return CacheFactory::loadXml(ss);
            }
    };
