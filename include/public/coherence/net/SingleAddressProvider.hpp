/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SINGLE_ADDRESS_PROVIDER_HPP
#define COH_SINGLE_ADDRESS_PROVIDER_HPP

#include "coherence/lang.ns"

#include "coherence/net/AddressProvider.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

/**
* SocketAddressProvider wrapper for a single address dynamically added to
* this provider.
*
* @author Wei Lin 2012.06.21
*
* @since Coherence 12.1.2
*/
class COH_EXPORT  SingleAddressProvider
    : public class_spec<SingleAddressProvider,
        extends<Object>,
        implements<AddressProvider> >
    {
    friend class factory<SingleAddressProvider>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Set the InetSocketAddress for this AddressProvider.
        *
        * @param address  the address which will be returned by this
        *                 InetSocketAddress
        */
        SingleAddressProvider(InetSocketAddress::View address);

    // ----- AddressProvider interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void accept();

        /**
        * {@inheritDoc}
        */
        virtual InetSocketAddress::View getNextAddress();

        /**
        * {@inheritDoc}
        */
        virtual void reject(Exception::Holder oheCause);

    // ----- Object methods -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The InetSocketAddress returned by this AddressProvider.
        */
        MemberView<InetSocketAddress> m_vAddress;

        /**
         * Whether the InetSocketAddress has already been returned.
         */
        bool m_fAddressGiven;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SINGLE_ADDRESS_PROVIDER_HPP

