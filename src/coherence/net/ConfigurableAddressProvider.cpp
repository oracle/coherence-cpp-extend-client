/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/ConfigurableAddressProvider.hpp"

#include "coherence/net/InetAddress.hpp"
#include "coherence/net/UnknownHostException.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Random.hpp"

#include "private/coherence/net/InetAddressHelper.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::util::ArrayList;
using coherence::util::Collections;
using coherence::net::InetAddress;
using coherence::net::InetAddressHelper;
using coherence::run::xml::XmlHelper;
using coherence::util::Iterator;
using coherence::util::List;
using coherence::util::Random;

COH_OPEN_NAMESPACE_ANON(ConfigurableAddressProvider)

/**
 * An Iterator over addresses for a given hostname.
 */
class COH_EXPORT InetSocketAddressIterator
    : public class_spec<InetSocketAddressIterator,
    extends<Object>,
    implements<Iterator> >
    {
    friend class factory<InetSocketAddressIterator>;

    // ----- constructors -------------------------------------------

    protected:
        /**
         * Construct an instance of InetSocketAddressIterator based on the
         * specified host and port.
         *
         * @param vsHost  the hostname
         * @param nPort   the port number
         */
        InetSocketAddressIterator(String::View vsHost, int32_t nPort)
            : f_voaAddr(self()), m_iAddr(0), m_nPort(nPort)
            {
            if (vsHost != NULL && vsHost->equals("localhost"))
                {
                ObjectArray::Handle hoa = ObjectArray::create(1);
                hoa[0] = InetAddressHelper::getLocalHost();
                initialize(f_voaAddr, hoa);
                }
            else
                {
                initialize(f_voaAddr, Random::randomize(InetAddress::getAllByName(vsHost)));
                }
            }

    // ----- Iterator interface -------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual bool hasNext() const
            {
            return m_iAddr < f_voaAddr->length;
            }

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder next()
            {
            if (hasNext())
                {
                ObjectArray::View voaAddr = f_voaAddr;
                return InetSocketAddress::create(cast<InetAddress::View>(voaAddr[m_iAddr++]), m_nPort);
                }
            COH_THROW (NoSuchElementException::create());
            }

    // ----- data members -------------------------------------------

    protected:
        FinalView<ObjectArray> f_voaAddr;
        size32_t               m_iAddr;
        uint16_t               m_nPort;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors --------------------------------------------------------

ConfigurableAddressProvider::ConfigurableAddressProvider(XmlElement::View vXml,
        bool fSafe)
    : f_vListHolders(self()), m_hIterAddr(self()), m_iLast(List::npos), m_fSafe(fSafe)
    {
    configure(vXml);
    }

// ----- AddressProvider interface -------------------------------------------

InetSocketAddress::View ConfigurableAddressProvider::getNextAddress()
    {
    List::View vList =  f_vListHolders;
    size32_t   cItems = vList->size();

    if (cItems == 0)
        {
        return NULL;
        }

    Iterator::Handle        hIterAddr = m_hIterAddr;
    size32_t                iLast     = m_iLast;
    bool                    fSafe     = m_fSafe;
    AddressHolder::Handle   hHolder   = NULL;
    InetSocketAddress::View vAddress;

    do
        {
        while (hIterAddr == NULL || !hIterAddr->hasNext())
            {
            // select next configured address
            iLast = m_iLast = (iLast + 1) % cItems;

            hHolder = cast<AddressHolder::Handle>(f_vListHolders->get(iLast));

            if (hHolder->isPending())
                {
                reset();
                return NULL;
                }

            hHolder->setPending(true);
            hIterAddr = m_hIterAddr = resolveAddress(hHolder->getHost(), hHolder->getPort());
            }

        vAddress = cast<InetSocketAddress::View>(hIterAddr->next());

        // ensure the address can be resolved
        if (fSafe && vAddress->isUnresolved())
            {
            if (hHolder != NULL && !hHolder->isReported())
                {
                hHolder->setReported(true);
                COH_LOG("The ConfigurableAddressProvider is skipping the "
                        << "unresolvable address \"" << vAddress << "\".", 3);
                }
            vAddress = NULL;
            }
        }
    while (vAddress == NULL);

    return vAddress;
    }

void ConfigurableAddressProvider::accept()
    {
    reset(m_iLast);
    }

void ConfigurableAddressProvider::reject(Exception::Holder /*oheCause*/)
    {
    }

// ----- helpers -------------------------------------------------------------

void ConfigurableAddressProvider::reset()
    {
    reset(npos);
    }

void ConfigurableAddressProvider::reset(size32_t iLast)
    {
    // reset all holders
    for (size32_t i = 0, c = f_vListHolders->size(); i < c; i++)
        {
        cast<AddressHolder::Handle>(f_vListHolders->get(i))->setPending(false);
        }

    m_hIterAddr = NULL;
    m_iLast     = iLast;
    }

void ConfigurableAddressProvider::configure(XmlElement::View vXml)
    {
    List::Handle hList = ArrayList::create();

    for (Iterator::Handle hIter = vXml->getElementList()->iterator(); hIter->hasNext(); )
        {
        XmlElement::View vXmlAddr = cast<XmlElement::View>(hIter->next());
        String::View     vsAddr;
        int32_t          nPort;

        String::View vsName = vXmlAddr->getName();
        if (vsName == NULL)
            {
            continue;
            }
        if (vsName->equals("socket-address"))
            {
            vsAddr = vXmlAddr->getSafeElement("address")->getString()->trim();
            nPort  = vXmlAddr->getSafeElement("port")->getInt32();
            }
        else if (vsName->equals("host-address") || vsName->equals("address"))
            {
            vsAddr = vXmlAddr->getString()->trim();
            nPort  = 0;
            }
        else
            {
            continue;
            }

        if (vsAddr->isEmpty())
            {
            // ignore empty elements
            continue;
            }

        try
            {
            hList->add(AddressHolder::create(vsAddr, nPort)->validate());
            }
        catch (RuntimeException::View ve)
            {
            COH_THROW (RuntimeException::create(COH_TO_STRING("Invalid configuration element: " << vXmlAddr), ve));
            }
        }

    initialize(f_vListHolders, sortHolders(hList));
    }

List::Handle ConfigurableAddressProvider::sortHolders(List::Handle hList)
    {
    return Random::randomize(hList);
    }

Iterator::Handle ConfigurableAddressProvider::resolveAddress(String::View vsHost, int nPort) const
    {
    try
        {
        return InetSocketAddressIterator::create(vsHost, nPort);
        }
    catch (UnknownHostException::View /* ve */)
        {
        // return unresolved address
        return Collections::singleton(InetSocketAddress::create(vsHost, nPort))->iterator();
        }
    }

// ----- object methods ------------------------------------------------------

TypedHandle<const String> ConfigurableAddressProvider::toString() const
    {
    String::View vs = "[";

    for (Iterator::Handle hIter = f_vListHolders->iterator(); hIter->hasNext(); )
        {
        AddressHolder::Handle hHolder = cast<AddressHolder::Handle>(hIter->next());
        vs = COH_TO_STRING(vs << hHolder->getHost() << ':' << hHolder->getPort());

        if (hIter->hasNext())
            {
            vs = COH_TO_STRING(vs << ',');
            }
        }

    return COH_TO_STRING(vs << ']');
    }

// ----- inner class: AddressHolder ------------------------------------------

ConfigurableAddressProvider::AddressHolder::AddressHolder(String::View vsHost, int32_t nPort)
    : f_vsHost(self(), vsHost), m_nPort((uint16_t) nPort)
    {
    if (nPort < 0 || nPort > 0xFFFF)
        {
        COH_THROW_STREAM (IllegalArgumentException, "port " << nPort << " out of range of 0 to " << 0xFFFF);
        }
    }

ConfigurableAddressProvider::AddressHolder::Handle ConfigurableAddressProvider::AddressHolder::validate()
    {
    if (f_vsHost == NULL)
        {
        COH_THROW (IllegalArgumentException::create("host may not be NULL"));
        }

    return this;
    }

bool ConfigurableAddressProvider::AddressHolder::isPending() const
    {
    return m_fPending;
    }

void ConfigurableAddressProvider::AddressHolder::setPending(bool fPending)
    {
    m_fPending = fPending;
    }

bool ConfigurableAddressProvider::AddressHolder::isReported() const
    {
    return m_fReported;
    }

void ConfigurableAddressProvider::AddressHolder::setReported(bool fReported)
    {
    m_fReported = fReported;
    }

String::View ConfigurableAddressProvider::AddressHolder::getHost() const
    {
    return f_vsHost;
    }

uint16_t ConfigurableAddressProvider::AddressHolder::getPort() const
    {
    return m_nPort;
    }

COH_CLOSE_NAMESPACE2
