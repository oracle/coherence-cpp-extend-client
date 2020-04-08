/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OPERATIONAL_CONTEXT_HPP
#define COH_OPERATIONAL_CONTEXT_HPP

#include "coherence/lang.ns"

#include "coherence/net/Member.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/security/IdentityAsserter.hpp"
#include "coherence/security/IdentityTransformer.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::run::xml::XmlElement;
using coherence::security::IdentityAsserter;
using coherence::security::IdentityTransformer;
using coherence::util::Map;


/**
* OperationalContext is an interface for providing Oracle Coherence
* operational configuration.
*
* @author phf  2010.10.28
*
* @since Coherence 3.7
*/
class COH_EXPORT OperationalContext
    : public interface_spec<OperationalContext>
    {
    // ----- OperationalContext interface -----------------------------------

    public:
        /**
        * Get the product edition.
        *
        * @return the product edition
        */
        virtual int32_t getEdition() const = 0;

        /**
        * Get the product edition in a formatted string.
        *
        * @return the product edition in a formatted string
        */
        virtual String::View getEditionName() const = 0;

        /**
        * Return a Member object representing this process.
        *
        * @return the local Member
        */
        virtual Member::View getLocalMember() const = 0;

        /**
        * Return a Map of network filter factories.
        *
        * @return a Map of WrapperStreamFactory instances keyed by filter
        *         name
        */
        virtual Map::View getFilterMap() const = 0;

        /**
        * Return a Map of serializer factories.
        *
        * @return a Map of SerializerFactory instances keyed by serializer
        *         name
        *
        * @since Coherence 12.1.2
        */
        virtual Map::View getSerializerMap() const = 0;

        /**
        * Return a Map of address provider factories.
        *
        * @return a Map of AddressProviderFactory instances keyed by address
        *         provider name
        *
        * @since Coherence 12.1.2
        */
        virtual Map::View getAddressProviderMap() const = 0;

        /**
        * Return an IdentityAsserter that can be used to establish a user's
        * identity.
        *
        * @return the IdentityAsserter
        */
        virtual IdentityAsserter::View getIdentityAsserter() const = 0;

        /**
        * Return an IdentityTransformer that can be used to transform a
        * Subject into an identity assertion.
        *
        * @return the IdentityTransformer
        */
        virtual IdentityTransformer::View getIdentityTransformer() const = 0;

        /**
        * Indicates if Subject scoping is enabled.
        *
        * @return true if subject scoping is enabled
        */
        virtual bool isSubjectScopingEnabled() const = 0;

        /**
        * Return the logging severity level.
        *
        * @return the logging severity level
        */
        virtual int32_t getLogLevel() const = 0;

        /**
        * Return the maximum number of characters for a logger daemon to
        * queue before truncating.
        *
        * @return the maximum number of characters for a logger daemon
        *         to queue before truncating
        */
        virtual int32_t getLogCharacterLimit() const = 0;

        /**
        * Return the log message format.
        *
        * @return the log message format
        */
        virtual String::View getLogMessageFormat() const = 0;

        /**
        * Return the destination for log messages.
        *
        * @return the destination for log messages
        */
        virtual String::View getLogDestination() const = 0;

        /**
         * Return the TTL for multicast based discovery.
         *
         * @return the TTL
         *
         * @since 12.2.1
         */
        virtual int32_t getDiscoveryTimeToLive() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OPERATIONAL_CONTEXT_HPP
