/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ADDRESS_PROVIDER_FACTORY_HPP
#define COH_ADDRESS_PROVIDER_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/net/AddressProvider.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

/**
* A factory for AddressProvider objects.
*
* @author wl  2012.04.04
* @since Coherence 12.1.2
*/
class COH_EXPORT AddressProviderFactory
    : public interface_spec<AddressProviderFactory >
    {
    public:
        /**
        * Create a new AddressProvider.
        *
        * @param vLoader  the optional ClassLoader with which to configure the
        *                 new AddressProvider.
        * @return an instance of the corresponding AddressProvider
        *         implementation.
        *
        */
        virtual AddressProvider::Handle createAddressProvider(ClassLoader::View vLoader = NULL) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ADDRESS_PROVIDER_FACTORY_HPP
