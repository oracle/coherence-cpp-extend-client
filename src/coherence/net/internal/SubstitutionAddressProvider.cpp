/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/internal/SubstitutionAddressProvider.hpp"

COH_OPEN_NAMESPACE3(coherence,net,internal)

SubstitutionAddressProvider::SubstitutionAddressProvider(AddressProvider::Handle hDelegate, int32_t nPort)
    : f_hDelegate(self(), hDelegate), f_nPort(nPort)
    {
    }

InetSocketAddress::View SubstitutionAddressProvider::getNextAddress()
    {
    InetSocketAddress::View vAddress = f_hDelegate->getNextAddress();
    if (vAddress != NULL && f_nPort != 0 && vAddress->getPort() == 0)
        {
        vAddress = InetSocketAddress::create(vAddress->getAddress(), f_nPort);
        }

    return vAddress;
    }

void SubstitutionAddressProvider::accept()
    {
    f_hDelegate->accept();
    }

void SubstitutionAddressProvider::reject(Exception::Holder oheCause)
    {
    f_hDelegate->reject(oheCause);
    }

COH_OPEN_NAMESPACE_ANON(SubstitutionAddressProvider)

class SubFactory
    : public class_spec<SubFactory, extends<Object>, implements<AddressProviderFactory> >
    {
    friend class factory<SubFactory>;

    private:
        SubFactory(AddressProviderFactory::Handle hDelegate, int32_t nPort)
            : f_hDelegate(self(), hDelegate), f_nPort(nPort)
        {
        }

    public:
        AddressProvider::Handle createAddressProvider(ClassLoader::View vLoader)
            {
            return SubstitutionAddressProvider::create(f_hDelegate->createAddressProvider(vLoader), f_nPort);
            }

    private:
        FinalHandle<AddressProviderFactory> f_hDelegate;
        const int32_t f_nPort;
    };

COH_CLOSE_NAMESPACE_ANON

AddressProviderFactory::Handle SubstitutionAddressProvider::createFactory(AddressProviderFactory::Handle hDelegate, int32_t nPort)
    {
    return SubFactory::create(hDelegate, nPort);
    }


COH_CLOSE_NAMESPACE3
