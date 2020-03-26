/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/InetAddress.hpp"

#include "coherence/net/UnknownHostException.hpp"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"

#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/native/NativeInetHelper.hpp"

#include <sstream>

using coherence::native::NativeInetHelper;
using coherence::util::Collection;
using coherence::util::Iterator;

COH_OPEN_NAMESPACE2(coherence,net)


// ----- local file helpers -------------------------------------------------

namespace
    {
    /**
    * Format a raw network address in human readable format.
    *
    * @return the formatted address
    */
    String::View formatAddress(Array<octet_t>::View vaIP)
        {
        size32_t cParts = NULL == vaIP ? 0 : vaIP->length;

        switch (cParts)
             {
             case 4: // IPv4 format
                 return COH_TO_STRING(std::dec << std::noshowbase
                     << (int32_t) vaIP[0] << '.'
                     << (int32_t) vaIP[1] << '.'
                     << (int32_t) vaIP[2] << '.'
                     << (int32_t) vaIP[3]);

             case 16: // IPv6 format
                 {
                 String::View vs = "";
                 for (size32_t i = 0; i < cParts; i += 2)
                     {
                     uint16_t nHex  = (vaIP[i]   & 0xFF) << 8;
                              nHex |= (vaIP[i+1] & 0xFF);

                     if (i != 0)
                         {
                         vs = COH_TO_STRING(vs << ':');
                         }
                     vs = COH_TO_STRING(vs << std::hex << std::noshowbase << (int32_t) nHex);
                     }

                 return vs;
                 }

             default: // unknown format
                 return COH_TO_STRING(std::oct << std::showbase << vaIP);
             }
        }
    }


// ----- constructors -------------------------------------------------------

InetAddress::InetAddress(String::View vsHost, Array<octet_t>::View vaIP)
    : f_vsHostName(self(), NULL == vsHost ? formatAddress(vaIP) : vsHost),
      f_vaIP(self(), immutable_view<Array<octet_t> >(vaIP))
    {
    }

InetAddress::InetAddress(const InetAddress& that)
    : super(that), f_vsHostName(self(), that.f_vsHostName),
      f_vaIP(self(), immutable_view<Array<octet_t> >(that.f_vaIP))
    {
    }


// ----- InetAddress interface ----------------------------------------------

InetAddress::View InetAddress::getLocalHost()
    {
    String::View vsName = NativeInetHelper::getLocalHostName();
    if (NULL == vsName)
        {
        vsName = "localhost";
        }
    return getByName(vsName);
    }

InetAddress::View InetAddress::getByName(String::View vsHost)
    {
    Array<octet_t>::View vaIP = NativeInetHelper::getRawAddress(vsHost);

    if (NULL == vaIP)
        {
        COH_THROW (UnknownHostException::create(vsHost));
        }

    return InetAddress::create(vsHost, vaIP);
    }

ObjectArray::Handle InetAddress::getAllByName(String::View vsHost)
    {
    if (vsHost == NULL || vsHost->length() == 0)
        {
        ObjectArray::Handle hoaRet = ObjectArray::create(1);
        hoaRet[0] = getLoopbackAddress();
        return hoaRet;
        }

    Collection::View vColIP = NativeInetHelper::getAllRawAddresses(vsHost);

    if (NULL == vColIP || vColIP->isEmpty())
        {
        COH_THROW (UnknownHostException::create(vsHost));
        }

    ObjectArray::Handle hoaRet = ObjectArray::create(vColIP->size());
    size32_t i = 0;
    for (Iterator::Handle hIter = vColIP->iterator(); hIter->hasNext(); )
        {
        hoaRet[i++] = InetAddress::create(vsHost, cast<Array<octet_t>::View>(hIter->next()));
        }
    return hoaRet;
    }

InetAddress::View InetAddress::getLoopbackAddress()
    {
    static InetAddress::View vLoopback;
    if (vLoopback == NULL)
        {
        Array<octet_t>::Handle haLoopback = Array<octet_t>::create(4);
        haLoopback[0] = 0x7f;
        haLoopback[1] = 0x00;
        haLoopback[2] = 0x00;
        haLoopback[3] = 0x01;
        vLoopback = InetAddress::create("localhost", haLoopback);
        }
    return vLoopback;
    }

String::View InetAddress::getHostAddress() const
    {
    return formatAddress(getAddress());
    }

String::View InetAddress::getHostName() const
    {
    return f_vsHostName;
    }

Array<octet_t>::View InetAddress::getAddress() const
    {
    return f_vaIP;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> InetAddress::toString() const
    {
    return COH_TO_STRING(getHostName() << '/' << getHostAddress());
    }

bool InetAddress::equals(Object::View that) const
    {
    InetAddress::View vThat = cast<InetAddress::View>(that, false);
    return this == vThat ||
        (vThat != NULL && Object::equals(getAddress(), vThat->getAddress()));
    }

size32_t InetAddress::hashCode() const
    {
    return Object::hashCode(getAddress());
    }

COH_CLOSE_NAMESPACE2
