/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/InetSocketAddress.hpp"

#include "coherence/net/InetAddress.hpp"
#include "coherence/net/UnknownHostException.hpp"

#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/native/NativeInetHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::native::NativeInetHelper;

class UnresolvedInetSocketAddress
    : public class_spec<UnresolvedInetSocketAddress,
        extends<InetSocketAddress> >
    {
    friend class factory<UnresolvedInetSocketAddress>;

    protected:
        UnresolvedInetSocketAddress(String::View vsHost, uint16_t nPort)
            : f_vsHost(self(), vsHost), m_nPort(nPort)
            {
            }

    // ----- InetSocketAddress interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual InetAddress::View getAddress() const
            {
            return NULL; // it is unresolved
            }

        /**
        * {@inheritDoc}
        */
        virtual uint16_t getPort() const
            {
            return m_nPort;
            }

        /**
        * {@inheritDoc}
        */
        virtual String::View getHostName() const
            {
            return f_vsHost;
            }

    // ----- SocketAddress interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isUnresolved() const
            {
            return true;
            }

        /**
        * {@inheritDoc}
        */
        virtual SocketAddress::View resolve() const
            {
            InetSocketAddress::View vAddr = InetSocketAddress::create(
                    InetAddress::getByName(getHostName()), getPort());
            if (vAddr->isUnresolved())
                {
                COH_THROW_STREAM (IOException, "Failed to resolve " << *this);
                }
            return vAddr;
            }

    // ----- Object interface -----------------------------------------------

    public:
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING(getHostName() << ':' << getPort());
            }

    // ----- data members ---------------------------------------------------

    protected:

        /**
        * The host.
        */
        FinalView<String> f_vsHost;

        /**
        * The port.
        */
        const uint16_t m_nPort;
    };

// ----- factory methods ----------------------------------------------------

InetSocketAddress::Handle InetSocketAddress::create(InetAddress::View vAddr,
        uint16_t nPort)
    {
    InetSocketAddress::Handle hAddr = NativeInetHelper::getSocketAddress(vAddr, nPort);
    if (NULL == hAddr)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "address " << vAddr << ", port " << nPort);
        }
    return hAddr;
    }

InetSocketAddress::Handle InetSocketAddress::create(String::View vsHost,
        uint16_t nPort, bool fResolve)
    {
    COH_ENSURE_PARAM(vsHost);
    if (fResolve)
        {
        try
            {
            return create(InetAddress::getByName(vsHost), nPort);
            }
        catch (UnknownHostException::View vEx) {} // fall through
        }
    // return an unresolved address
    return UnresolvedInetSocketAddress::create(vsHost, nPort);
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> InetSocketAddress::toString() const
    {
    return COH_TO_STRING(getAddress() << ':' << getPort());
    }

COH_CLOSE_NAMESPACE2
