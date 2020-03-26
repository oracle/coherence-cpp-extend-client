/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DEFAULT_OPERATIONAL_CONTEXT_HPP
#define COH_DEFAULT_OPERATIONAL_CONTEXT_HPP

#include "coherence/lang.ns"

#include "coherence/net/Member.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/run/xml/XmlDocument.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/security/IdentityAsserter.hpp"
#include "coherence/security/IdentityTransformer.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::run::xml::XmlDocument;
using coherence::run::xml::XmlElement;
using coherence::security::IdentityAsserter;
using coherence::security::IdentityTransformer;
using coherence::util::Map;


/**
* The DefaultOperationalContext provides an OperationalContext with
* information obtained from XML in coherence.dtd format, system properties
* (as set through environment variables) and default values.
*
* @author phf  2010.10.28
*
* @since Coherence 3.7
*/
class COH_EXPORT DefaultOperationalContext
    : public class_spec<DefaultOperationalContext,
        extends<Object>,
        implements<OperationalContext> >
    {
    friend class factory<DefaultOperationalContext>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new DefaultOperationalContext.
        *
        * @param vXmlCoherence  an XML element corresponding to coherence.dtd
        */
        DefaultOperationalContext(XmlElement::View vXmlCoherence = NULL);


    // ----- typedef: ProductEdition ----------------------------------------

    public:
        /**
        * Extend editions.
        */
        typedef enum
            {
            edition_dc  = 0,
            edition_rtc = 1
            } ProductEdition;


    // ----- DefaultOperationalContext interface ----------------------------

    public:
        /**
        * The default XML configuration used when one isn't explicitly passed
        * in the constructor for this class.
        *
        * @return the default XML configuration
        */
        static XmlDocument::Handle getDefaultOperationalConfig();


    // ----- OperationalContext interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getEdition() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getEditionName() const;

        /**
        * {@inheritDoc}
        */
        virtual Member::View getLocalMember() const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View getFilterMap() const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View getSerializerMap() const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View getAddressProviderMap() const;

        /**
        * {@inheritDoc}
        */
        virtual IdentityAsserter::View getIdentityAsserter() const;

        /**
        * {@inheritDoc}
        */
        virtual IdentityTransformer::View getIdentityTransformer() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isSubjectScopingEnabled() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getLogLevel() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getLogCharacterLimit() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getLogMessageFormat() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getLogDestination() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getDiscoveryTimeToLive() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The configured edition.
        */
        int32_t m_nEdition;

        /**
        * The configured edition in a formatted string for use by the Logger.
        */
        FinalView<String> f_vsEdition;

        /**
        * The configured Filter map.
        *
        * See OperationalContext::getFilterMap() for a description of the
        * keys and values.
        */
        FinalView<Map> f_vFilterMap;

        /**
        * The configured Serializer map.
        *
        * See OperationalContext::getSerializerMap() for a description of the
        * keys and values.
        */
        FinalView<Map> f_vSerializerMap;

        /**
        * The configured AddressProvider map.
        *
        * See OperationalContext::getAddressProviderMap() for a description of the
        * keys and values.
        */
        FinalView<Map> f_vAddressProviderMap;

        /**
        * The configured local member.
        */
        FinalView<Member> f_vMemberLocal;

        /**
        * The configured IdentityAsserter that validates a token in order to
        * establish a user's identity.
        */
        FinalView<IdentityAsserter> f_vAsserter;

        /**
        * The configured IdentityTransformer that transforms a Subject into
        * a token that asserts a user's identity.
        */
        FinalView<IdentityTransformer> f_vTransformer;

        /**
        * True iff subject scope configuration is set to true.
        */
        bool m_fSubjectScoped;

        /**
        * The configured logging severity level.
        */
        int32_t m_nLogLevel;

        /**
        * The the maximum number of characters for a logger daemon to queue
        * before truncating.
        */
        int32_t m_nLogCharacterLimit;

        /**
        * The configured log message format.
        */
        FinalView<String> f_vsLogMessageFormat;

        /**
        * The configured destination for log messages.
        */
        FinalView<String> f_vsLogDestination;

        /**
         * The multicast TTL for discovery.
         */
        int32_t m_nTtlDiscovery;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DEFAULT_OPERATIONAL_CONTEXT_HPP
