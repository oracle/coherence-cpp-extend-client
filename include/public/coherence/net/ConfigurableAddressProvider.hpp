/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONFIGURABLE_ADDRESS_PROVIDER_HPP
#define COH_CONFIGURABLE_ADDRESS_PROVIDER_HPP

#include "coherence/lang.ns"

#include "coherence/net/AddressProvider.hpp"

#include "coherence/run/xml/XmlElement.hpp"

#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::run::xml::XmlElement;
using coherence::util::Iterator;
using coherence::util::List;

/**
* ConfigurableAddressProvider is an implementation of the AddressProvider
* interface based on a static list of addresses configured in an XML element
* that contains one or more items in the following format:
* <pre>
* &lt;socket-address&gt;
* &nbsp;&nbsp;&lt;address&gt;...&lt;/address&gt;
* &nbsp;&nbsp;&lt;port&gt;...&lt;/port&gt;
* &lt;/socket-address&gt;
* </pre>
* The order of items in the configured list will be randomized to provide
* basic load balancing.
* This implementation is not thread safe.
*
* @author gg 2008-08-18
* @author gm 2008-08-25
* @since Coherence 3.4
*/
class COH_EXPORT ConfigurableAddressProvider
    : public class_spec<ConfigurableAddressProvider,
        extends<Object>,
        implements<AddressProvider> >
    {
    friend class factory<ConfigurableAddressProvider>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ConfigurableAddressProvider instance.
        */
        ConfigurableAddressProvider();

        /**
        * Construct an instance of ConfigurableAddressProvider based on the
        * specified XML element.
        *
        * @param vXml   the XML element that contains the configuration info
        * @param fSafe  true if the provider is skips unresolved addresses
        */
        ConfigurableAddressProvider(XmlElement::View vXml, bool fSafe = true);


    // ----- AddressProvider interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual InetSocketAddress::View getNextAddress();

        /**
        * {@inheritDoc}
        */
        virtual void accept();

        /**
        * {@inheritDoc}
        */
        virtual void reject(Exception::Holder oheCause);


    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Configure this ConfigurableAddressProvider based on the specified
        * XML.
        *
        * @param xmlConfig  the XML element that contains the configuration
        *                   info
        */
        virtual void configure(XmlElement::View vXml);

        /**
        * Make all addresses iterable, starting at the first address.
        */
        virtual void reset();

        /**
        * Make all addresses iterable, starting at the index after the specified
        * one.
        *
        * @param iLast  the index of the last address returned
        */
        virtual void reset(size32_t iLast);

        /**
         * Resolve an address and port.
         *
         * @param vsHost  the host
         * @param nPort   the port
         *
         * @return an iterator over the resolved InetSocketAddresses
         */
        virtual Iterator::Handle resolveAddress(String::View vsHost, int nPort) const;

        /**
        * Sort the holders in the order to be returned by the getNextAddress
        * method.  This implementation randomizes the holder lists for
        * simple load balancing.
        *
        * @param list  the original list retrieved from the configuration
        *
        * @return the re-ordered list
        */
        virtual List::Handle sortHolders(List::Handle hList);


    // ----- Object methods -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- inner class: AddressHolder -------------------------------------

    protected:
        /**
        * A stateful holder for an InetSocketAddress object.
        */
        class COH_EXPORT AddressHolder
            : public class_spec<AddressHolder>
            {
            friend class factory<AddressHolder>;
            friend class ConfigurableAddressProvider;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct an AddressHolder for the specified
                * InetSocketAddress.
                *
                * @param sHost  the hostname
                * @param nPort  the port number
                */
                AddressHolder(String::View vsHost, int32_t nPort);

                /**
                * Create a new AddressHolder instance.
                */
                AddressHolder();

            public:
                /**
                 * Throw IllegalArgumentException if any values are invalid.
                 *
                 * @return this
                 */
                AddressHolder::Handle validate();

            // ----- accessors ------------------------------------------

            protected:
                /**
                * Check whether or not the underlying address has been accepted.
                *
                * @return true iff the underlying address has not yet been
                *         accepted
                */
                virtual bool isPending() const;

                /**
                * Set or clear the "pending" flag.
                *
                * @param fPending  the flag value
                */
                virtual void setPending(bool fPending);

                /**
                * Check whether or not the underlying address has been reported
                * as unresolveable.
                *
                * @return true iff the underlying address has been reported as
                *         unresolveable
                */
                virtual bool isReported() const;

                /**
                * Set of clear the "reported" flag.
                *
                * @param fReported  the flag value
                */
                virtual void setReported(bool fReported);

            public:
                /**
                 * Return the host name.
                 *
                 * @return the host name
                 */
                virtual String::View getHost() const;

                /**
                 * Return the port number.
                 *
                 * @return the port number
                 */
                virtual uint16_t getPort() const;

            // ----- data members -------------------------------------------

            protected:
                /**
                * The configured address, either hostname or IP address.
                */
                FinalView<String> f_vsHost;

                /**
                * The configured port.
                */
                const uint16_t m_nPort;

                /**
                * A flag indicating that the underlying address has been
                * provided to a client, but has not yet been accepted.
                */
                bool m_fPending;

                /**
                * Specifies if this address has already been reported as
                * unresolved.
                */
                bool m_fReported;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A list of AddressHolder objects.
        */
        FinalView<List> f_vListHolders;

        /**
         * An address iterator for the previously resolved address.
         *
         * @since 12.2.1
         */
        MemberHandle<Iterator> m_hIterAddr;

        /**
        * Index of the last returned address.
        */
        size32_t m_iLast;

        /**
        * Specifies if the provider is only to return resolved addresses.
        */
        bool m_fSafe;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONFIGURABLE_ADDRESS_PROVIDER_HPP
