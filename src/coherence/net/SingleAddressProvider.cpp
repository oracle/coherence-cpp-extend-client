/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/SingleAddressProvider.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

// ----- constructors ---------------------------------------------------

SingleAddressProvider::SingleAddressProvider(InetSocketAddress::View vAddress)
    : m_vAddress(self(), vAddress), m_fAddressGiven(false)
    {
    }

// ----- AddressProvider interface --------------------------------------

InetSocketAddress::View SingleAddressProvider::getNextAddress()
    {
    return (m_fAddressGiven = !m_fAddressGiven) ? m_vAddress : (InetSocketAddress::View) NULL;
    }

void SingleAddressProvider::accept()
    {
    // no-op
    }

void SingleAddressProvider::reject(Exception::Holder /*oheCause*/)
    {
    // no-op
    }

// ----- object methods --------------------------------------------------------

TypedHandle<const String> SingleAddressProvider::toString() const
    {
    return COH_TO_STRING("Address=" << m_vAddress->getAddress() << ":" << m_vAddress->getPort());
    }

COH_CLOSE_NAMESPACE2

