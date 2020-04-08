/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUBSTITUTION_ADDRESS_PROVIDER_HPP
#define COH_SUBSTITUTION_ADDRESS_PROVIDER_HPP

#include "coherence/lang.ns"

#include "coherence/net/AddressProvider.hpp"
#include "coherence/net/AddressProviderFactory.hpp"
#include "coherence/net/InetSocketAddress.hpp"

COH_OPEN_NAMESPACE3(coherence,net,internal)

using coherence::net::AddressProvider;
using coherence::net::AddressProviderFactory;
using coherence::net::InetSocketAddress;

/**
 * An AddressProvider which substitutes "unset" portions of an address.
 *
 * @author mf
 *
 * @since Coherence 12.2.1
 */
class COH_EXPORT SubstitutionAddressProvider
    : public class_spec<SubstitutionAddressProvider,
          extends<Object>,
          implements<AddressProvider> >
    {
    friend class factory<SubstitutionAddressProvider>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Return a new  SubstitutionAddressProvider
         *
         * @param delegate  the delegate provider
         * @param nPort     the value to replace, zero port values with
         */
        SubstitutionAddressProvider(AddressProvider::Handle hDelegate, int32_t nPort);

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


    // ----- factory helpers ------------------------------------------------

    public:
        /**
         * Return a AddressProviderFactory which substitutes the specified port for any zero ports.
         *
         * @param delegate  the delegate factory
         * @param nPort     the new port value
         *
         * @return the factory
         */
        static AddressProviderFactory::Handle createFactory(AddressProviderFactory::Handle hDelegate, int32_t nPort);


    // ----- data fields ----------------------------------------------------

    private:
        /**
         * The delegate provider.
         */
        FinalHandle<AddressProvider> f_hDelegate;

        /**
         * The port to replace zero port values with
         */
        const int32_t f_nPort;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SUBSTITUTION_ADDRESS_PROVIDER_HPP
