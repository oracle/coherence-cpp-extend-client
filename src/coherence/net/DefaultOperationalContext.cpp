/*
 * Copyright (c) 2000, 2021, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/DefaultOperationalContext.hpp"

#include "coherence/io/ConfigurableSerializerFactory.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/WrapperStreamFactory.hpp"
#include "coherence/net/ConfigurableAddressProviderFactory.hpp"
#include "coherence/net/InetAddress.hpp"
#include "coherence/security/DefaultIdentityAsserter.hpp"
#include "coherence/security/DefaultIdentityTransformer.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/SafeHashMap.hpp"

#include "private/coherence/io/pof/SystemPofContextSerializerFactory.hpp"
#include "private/coherence/native/NativePID.hpp"
#include "private/coherence/native/NativeUser.hpp"
#include "private/coherence/net/LocalMember.hpp"
#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/run/xml/SimpleParser.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"
#include "private/coherence/util/StringHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::ConfigurableSerializerFactory;
using coherence::io::IOException;
using coherence::io::WrapperStreamFactory;
using coherence::io::pof::SystemPofContextSerializerFactory;
using coherence::native::NativePID;
using coherence::native::NativeUser;
using coherence::run::xml::SimpleElement;
using coherence::run::xml::SimpleParser;
using coherence::run::xml::XmlHelper;
using coherence::security::DefaultIdentityAsserter;
using coherence::security::DefaultIdentityTransformer;
using coherence::util::Iterator;
using coherence::util::SafeHashMap;
using coherence::util::StringHelper;
using coherence::util::logging::Logger;


// ----- constructors -------------------------------------------------------

DefaultOperationalContext::DefaultOperationalContext(
        XmlElement::View vXmlCoherence)
        : f_vsEdition(self()),
          f_vFilterMap(self()),
          f_vSerializerMap(self()),
          f_vAddressProviderMap(self()),
          f_vMemberLocal(self()),
          f_vAsserter(self()),
          f_vTransformer(self()),
          f_vsLogMessageFormat(self()),
          f_vsLogDestination(self())
    {
    if (NULL == vXmlCoherence)
        {
        vXmlCoherence = getDefaultOperationalConfig();
        }

    // COH-23599 - load system property overrides
    XmlElement::Handle hXml = cast<XmlElement::Handle>(vXmlCoherence->clone());
    XmlHelper::replaceSystemProperties(hXml, "system-property");
    vXmlCoherence = hXml;

    if (!vXmlCoherence->getName()->equals("coherence"))
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "Unexpected root element" << vXmlCoherence);
        }

    // Edition
    String::View vsEdition =
            (vXmlCoherence->getSafeElement("license-config/edition-name")
                    ->getString(System::getProperty(
                            "coherence.edition", "RTC")))->trim();
    if (StringHelper::compare(vsEdition, "DC", false) == 0)
        {
        m_nEdition = DefaultOperationalContext::edition_dc;
        }
    else if (StringHelper::compare(vsEdition, "RTC", false) == 0)
        {
        m_nEdition = DefaultOperationalContext::edition_rtc;
        }
    else
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "Unknown Coherence Extend edition: \""
                << vsEdition << "\"");
        }
    initialize(f_vsEdition, vsEdition);

    // Logger configuration
    XmlElement::View vLoggerConfig =
            vXmlCoherence->getSafeElement("logging-config");

    // Need to call Logger::getDefaultLevel() to differentiate between
    // passing in a zero in the config versus the absence of the setting
    // in the config.
    String::View vsLogLevel = vLoggerConfig->getSafeElement("severity-level")
            ->getString(System::getProperty("coherence.log.level"));

    m_nLogLevel          = (vsLogLevel == NULL || vsLogLevel->length() == 0)
            ? Logger::getDefaultLevel() : Integer32::parse(vsLogLevel);
    m_nLogCharacterLimit = vLoggerConfig->getSafeElement("character-limit")
            ->getInt32(Integer32::parse(
                    System::getProperty("coherence.log.limit", "0")));
    initialize(f_vsLogMessageFormat,
            vLoggerConfig->getSafeElement("message-format")->getString());
    initialize(f_vsLogDestination,
            vLoggerConfig->getSafeElement("destination")
            ->getString(System::getProperty("coherence.log")));

    // Member
    LocalMember::Handle hMember    = LocalMember::create();
    XmlElement::View    vXmlMember =
            vXmlCoherence->getSafeElement("cluster-config/member-identity");

    String::View vsCluster = vXmlMember->getSafeElement("cluster-name")
                ->getString(System::getProperty("coherence.cluster"));

    if (vsCluster == NULL || vsCluster->isEmpty())
        {
        String::View vsUser = NativeUser::getUserName();
        if (vsUser == NULL || vsUser->isEmpty() || vsUser->equals("?")) // NIS will return ? if NIS server is down
            {
            COH_THROW(UnsupportedOperationException::create("unable to generate a default cluster name, user name is not available, explicit cluster name configuration is required"));
            }

        vsCluster = COH_TO_STRING(vsUser << "'s cluster");
        COH_LOG("The cluster name has not been configured, a value of \"" << vsCluster << "\" has been automatically generated", Logger::level_info);
        }

    String::View vsHost = InetAddress::getLocalHost()->getHostName();
    size32_t     iDelim = vsHost->indexOf('.');
    String::View vsMachine;
    String::View vsSite;

    if (iDelim == String::npos || !isalpha(vsHost->getCString()[0]))
        {
        vsMachine = vsHost;
        }
    else
        {
        vsMachine = vsHost->substring(0, iDelim);
        vsSite    = vsHost->substring(iDelim + 1);
        }

    hMember->setClusterName(vsCluster);
    hMember->setSiteName(vXmlMember->getSafeElement("site-name")
            ->getString(System::getProperty(
                    "coherence.site", vsSite)));
    hMember->setRackName(vXmlMember->getSafeElement("rack-name")
            ->getString(System::getProperty(
                    "coherence.rack")));
    hMember->setMachineName(vXmlMember->getSafeElement("machine-name")
            ->getString(System::getProperty(
                    "coherence.machine", vsMachine)));
    hMember->setProcessName(vXmlMember->getSafeElement("process-name")
            ->getString(System::getProperty(
                    "coherence.process",
                    COH_TO_STRING(NativePID::getPID()))));
    hMember->setMemberName(vXmlMember->getSafeElement("member-name")
            ->getString(System::getProperty(
                    "coherence.member")));
    hMember->setRoleName(vXmlMember->getSafeElement("role-name")
            ->getString(System::getProperty(
                    "coherence.role",
                    COH_TO_STRING("C++ " << vsEdition << " client"))));

    initialize(f_vMemberLocal, hMember);

    ClassLoader::View vLoader = SystemClassLoader::getInstance();

    // Filters
    Map::Handle       hFilterMap  = SafeHashMap::create();
    XmlElement::View  vXmlFilters =
            vXmlCoherence->getSafeElement("cluster-config/filters");
    for (Iterator::Handle hIter = vXmlFilters->getElements("filter");
            hIter->hasNext(); )
        {
        // Filter elements use filter-class instead of class-name so
        // need to convert before passing in to createInstance()
        XmlElement::View   vXmlFilter = cast<XmlElement::View>(hIter->next());
        XmlElement::View   vXmlParams =
                vXmlFilter->getElement("init-params");
        String::View       vsFilter   =
                vXmlFilter->getSafeElement("filter-name")->getString();

        COH_LOG("Declaring filter \"" << vsFilter << "\" = " << vXmlFilter, 7);

        XmlElement::Handle hXmlFilterClass = SimpleElement::create("instance");
        hXmlFilterClass->addElement("class-name")
                ->setString(vXmlFilter->getSafeElement("filter-class")
                        ->getString());
        if (vXmlParams != NULL)
            {
            hXmlFilterClass->addElement("init-params")->getElementList()->
                    addAll(vXmlParams->getElementList());
            }
        WrapperStreamFactory::Handle hWSFactory =
                cast<WrapperStreamFactory::Handle>(XmlHelper::createInstance(
                        hXmlFilterClass,
                        vLoader,
                        vLoader->loadByType(typeid(WrapperStreamFactory))));
        hFilterMap->put(vsFilter, hWSFactory);
        }
    initialize(f_vFilterMap, hFilterMap);

    // Serializers
    Map::Handle       hSerializerMap  = SafeHashMap::create();
    XmlElement::View  vXmlSerializers =
            vXmlCoherence->getSafeElement("cluster-config/serializers");
    for (Iterator::Handle hIter = vXmlSerializers->getElements("serializer");
            hIter->hasNext(); )
        {
        XmlElement::View  vXmlSerializer = cast<XmlElement::View>(hIter->next());
        String::View      vsSerializer   =
                vXmlSerializer->getAttribute("id")->getString();

        ConfigurableSerializerFactory::Handle hCSFactory = ConfigurableSerializerFactory::create();
        hCSFactory->setConfig(vXmlSerializer);
        hSerializerMap->put(vsSerializer, hCSFactory);
        }

    // Check that the well-known pof serializer is present
    String::View   vsSerializerName = String::create("pof");
    if (!hSerializerMap->containsKey(vsSerializerName))
        {
        hSerializerMap->put(vsSerializerName, SystemPofContextSerializerFactory::create());
        }
    initialize(f_vSerializerMap, hSerializerMap);

    // AddressProviders
    Map::Handle hAddressProviderMap  = SafeHashMap::create();
    XmlElement::View  vXmlAddressProviders =
            vXmlCoherence->getSafeElement("cluster-config/address-providers");
    for (Iterator::Handle hIter = vXmlAddressProviders->getElements("address-provider");
            hIter->hasNext(); )
        {
        XmlElement::View  vXmlAddressProvider = cast<XmlElement::View>(hIter->next());
        String::View      vsAddressProvider   =
                vXmlAddressProvider->getAttribute("id")->getString();

        COH_LOG("Declaring address provider \"" << vsAddressProvider << "\" = "
                << vXmlAddressProvider, 7);

        ConfigurableAddressProviderFactory::Handle hCAPFactory = ConfigurableAddressProviderFactory::create();
        hCAPFactory->setConfig(vXmlAddressProvider);
        hAddressProviderMap->put(vsAddressProvider, hCAPFactory);
        }
    initialize(f_vAddressProviderMap, hAddressProviderMap);

    // IdentityAsserter and IdentityTransformer
    IdentityAsserter::Handle    hAsserter    = NULL;
    IdentityTransformer::Handle hTransformer = NULL;

    XmlElement::View vXmlIdentityAsserter    =
            vXmlCoherence->findElement("security-config/identity-asserter");
    XmlElement::View vXmlIdentityTransformer =
            vXmlCoherence->findElement("security-config/identity-transformer");

    if (vXmlIdentityAsserter != NULL)
        {
        COH_LOG("Creating IdentityAsserter from configuration: " <<
                vXmlIdentityAsserter, 7);
        hAsserter = cast<IdentityAsserter::Handle>(
                XmlHelper::createInstance(vXmlIdentityAsserter,
                vLoader,
                vLoader->loadByType(typeid(IdentityAsserter))));
        }
    if (vXmlIdentityTransformer != NULL)
        {
        COH_LOG("Creating IdentityTransformer from configuration: " <<
                vXmlIdentityTransformer, 7);
        hTransformer = cast<IdentityTransformer::Handle>(
                XmlHelper::createInstance(vXmlIdentityTransformer,
                vLoader,
                vLoader->loadByType(typeid(IdentityTransformer))));
        }

    if (hAsserter == NULL)
        {
        COH_LOG("Creating DefaultIdentityAsserter", 7);
        hAsserter = DefaultIdentityAsserter::getInstance();
        }
    if (hTransformer == NULL)
        {
        COH_LOG("Creating DefaultIdentityTransformer", 7);
        hTransformer = DefaultIdentityTransformer::getInstance();
        }

    initialize(f_vAsserter, hAsserter);
    initialize(f_vTransformer, hTransformer);

    // Subject scope
    m_fSubjectScoped =
            vXmlCoherence->getSafeElement("security-config/subject-scope")
            ->getBoolean();

    m_nTtlDiscovery = vXmlCoherence->getSafeElement("multicast-listener/time-to-live")->getInt32(4);
    }


// ----- DefaultOperationalContext interface --------------------------------

XmlDocument::Handle DefaultOperationalContext::getDefaultOperationalConfig()
    {
    String::View vsDefault = "tangosol-coherence-override.xml";
    String::View vsFile    = System::getProperty(
            "coherence.override", vsDefault);
    try
        {
        return XmlHelper::loadFile(vsFile, "operational configuration");
        }
    catch (Exception::View)
        {
        if (vsFile->equals(vsDefault))
            {
            COH_LOG("Optional operational override \"" << vsFile
                    << "\" is not specified, using default", 5);
            }
        else
            {
            COH_LOG("Specified \"" << vsFile << "\" operational "
                    << "override not found, using default",
                    Logger::level_warning);
            }
        return SimpleParser::create()->parseXml("<coherence/>");
        }
    }


// ----- OperationalContext interface ---------------------------------------

bool DefaultOperationalContext::isSubjectScopingEnabled() const
    {
    return m_fSubjectScoped;
    }

int32_t DefaultOperationalContext::getEdition() const
    {
    return m_nEdition;
    }

String::View DefaultOperationalContext::getEditionName() const
    {
    return f_vsEdition;
    }

Member::View DefaultOperationalContext::getLocalMember() const
    {
    return f_vMemberLocal;
    }

Map::View DefaultOperationalContext::getFilterMap() const
    {
    return f_vFilterMap;
    }

Map::View DefaultOperationalContext::getSerializerMap() const
    {
    return f_vSerializerMap;
    }

Map::View DefaultOperationalContext::getAddressProviderMap() const
    {
    return f_vAddressProviderMap;
    }

IdentityAsserter::View DefaultOperationalContext::getIdentityAsserter() const
    {
    return f_vAsserter;
    }

IdentityTransformer::View DefaultOperationalContext::
    getIdentityTransformer() const
    {
    return f_vTransformer;
    }

int32_t DefaultOperationalContext::getLogLevel() const
    {
    return m_nLogLevel;
    }

int32_t DefaultOperationalContext::getLogCharacterLimit() const
    {
    return m_nLogCharacterLimit;
    }

String::View DefaultOperationalContext::getLogMessageFormat() const
    {
    return f_vsLogMessageFormat;
    }

String::View DefaultOperationalContext::getLogDestination() const
    {
    return f_vsLogDestination;
    }

int32_t DefaultOperationalContext::getDiscoveryTimeToLive() const
    {
    return m_nTtlDiscovery;
    }

COH_CLOSE_NAMESPACE2
