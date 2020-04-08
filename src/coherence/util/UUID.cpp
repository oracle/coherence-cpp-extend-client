/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/UUID.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/UnknownHostException.hpp"

#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::net::UnknownHostException;

COH_REGISTER_PORTABLE_CLASS(14, UUID);


COH_OPEN_NAMESPACE_ANON(UUID)

// ----- file local helpers -------------------------------------------------

/**
* Statically create an empty byte array.
*
* @return an empty byte array
*/
Array<octet_t>::View getEmptyByteArray()
    {
    static FinalView<Array<octet_t> > vab(System::common(),
            Array<octet_t>::create(0));
    return vab;
    }
COH_STATIC_INIT(getEmptyByteArray());

// ----- local class: InetAddressInfo -----------------------------------

/**
* Holds information about the InetAddress of the local machine.
*/
class InetAddressInfo
    : public class_spec<InetAddressInfo>
    {
    friend class factory<InetAddressInfo>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new InetAddressInfo instance.
        */
        InetAddressInfo()
            : m_fRealAddress(false), m_fIPv6(false),
              m_nAddr1(0), m_nAddr2(0), m_nAddr3(0), m_nAddr4(0)
            {
            InetAddress::View vInetAddr;
            try
                {
                vInetAddr = InetAddress::getLocalHost();
                }
            catch (...)
                {
                // do nothing
                }

            try
                {
                Array<octet_t>::View vabAddr = getEmptyByteArray();
                if (vInetAddr != NULL)
                    {
                    vabAddr = vInetAddr->getAddress();
                    }

                switch (vabAddr->length)
                    {
                    case 4:
                        {
                        int32_t nIP = (vabAddr[0] & 0xFF) << 24
                                    | (vabAddr[1] & 0xFF) << 16
                                    | (vabAddr[2] & 0xFF) <<  8
                                    | (vabAddr[3] & 0xFF);

                        if (nIP != 0x00000000 && nIP != 0x7F000001)
                            {
                            m_fRealAddress = true;
                            m_nAddr1       = nIP;
                            }
                        }
                        break;
                    case 16:
                        {
                        int32_t nIP1 = (vabAddr[ 0] & 0xFF) << 24
                                     | (vabAddr[ 1] & 0xFF) << 16
                                     | (vabAddr[ 2] & 0xFF) <<  8
                                     | (vabAddr[ 3] & 0xFF);
                        int32_t nIP2 = (vabAddr[ 4] & 0xFF) << 24
                                     | (vabAddr[ 5] & 0xFF) << 16
                                     | (vabAddr[ 6] & 0xFF) <<  8
                                     | (vabAddr[ 7] & 0xFF);
                        int32_t nIP3 = (vabAddr[ 8] & 0xFF) << 24
                                     | (vabAddr[ 9] & 0xFF) << 16
                                     | (vabAddr[10] & 0xFF) <<  8
                                     | (vabAddr[11] & 0xFF);
                        int32_t nIP4 = (vabAddr[12] & 0xFF) << 24
                                     | (vabAddr[13] & 0xFF) << 16
                                     | (vabAddr[14] & 0xFF) <<  8
                                     | (vabAddr[15] & 0xFF);

                        if (!(nIP1 == 0x00000000 &&  nIP2 == 0x00000000   &&
                              nIP3 == 0x00000000 && (nIP4 == 0x00000000   ||
                                                     nIP4 == 0x00000001)) &&
                            !((vabAddr[0] & 0xFF) ==
                              0xFE && (vabAddr[1] & 0xC0) == 0x80))
                            {
                            m_fRealAddress = true;
                            m_fIPv6        = true;
                            m_nAddr1       = nIP1;
                            m_nAddr2       = nIP2;
                            m_nAddr3       = nIP3;
                            m_nAddr4       = nIP4;
                            }
                        }
                        break;
                    }
                }
            catch (...)
                {
                //do nothing
                }
        }

    // ----- InetAddressInfo interface ------------------------------

    public:
        /**
        * @return true if using an IPv6 address
        */
        bool isIPv6() const
            {
            return m_fIPv6;
            }

        /**
        * @return true if using a real address
        */
        bool isRealAddress() const
            {
            return m_fRealAddress;
            }

        /**
        * @return the first four bytes of the IP address
        */
        int32_t getAddr1() const
            {
            return m_nAddr1;
            }

        /**
        * @return the second four bytes of the IP address
        */
        int32_t getAddr2() const
            {
            return m_nAddr2;
            }

        /**
        * @return the third four bytes of the IP address
        */
        int32_t getAddr3() const
            {
            return m_nAddr3;
            }

        /**
        * @return the fourth four bytes of the IP address
        */
        int32_t getAddr4() const
            {
            return m_nAddr4;
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * This is true if the host's IP address is a real IP address.
        * This is false if this class could not get an IP address (or one
        * that is not a loopback/localhost address) from the VM.
        */
        bool m_fRealAddress;

        /**
        * This is true if the host's address style is known and is IPv6.
        */
        bool m_fIPv6;

        /**
        * The first four bytes of the IP address of the host.
        */
        int32_t m_nAddr1;

        /**
        * The second four bytes of the IP address of the host.
        */
        int32_t m_nAddr2;

        /**
        * The third four bytes of the IP address of the host.
        */
        int32_t m_nAddr3;

        /**
        * The fourth four bytes of the IP address of the host.
        */
        int32_t m_nAddr4;
    };

// ----- static initialization ------------------------------------------

/**
* Static lock used as a mutex for updating the timestamp and count.
*
* @return a mutex for updating the timestamp and count
*/
Object::View getLock()
    {
    static FinalView<Object> vLock(System::common(),
            Object::create());
    return vLock;
    }
COH_STATIC_INIT(getLock());

/**
* Statically initialize count to a random number.
*
* @return the previous count value incremented by one
*/
int32_t getNextCount()
    {
    static int32_t c = Random::getInstance()->nextInt32();
    return ++c;
    }
COH_STATIC_INIT(getNextCount());

/**
* Statically set the host information
*
* @return the host information
*/
InetAddressInfo::View getInetAddressInfo()
    {
    static FinalView<InetAddressInfo> vInetAddrInfo(
            System::common(), InetAddressInfo::create());
    return vInetAddrInfo;
    }
COH_STATIC_INIT(getInetAddressInfo());

/**
* Performs an unsigned right shift on a 32 bit int, left filling
* with 0.
*
* @param n      the number to operate on
* @param cBits  the number of bits to shift
*
* @return the result of the shift
*/
int64_t unsignedRightShift(int64_t n, int32_t cBits)
    {
    return uint64_t(n) >> cBits;
    }

/**
* Performs an unsigned right shift on a 64 bit int, left filling
* with 0.
*
* @param n      the number to operate on
* @param cBits  the number of bits to shift
*
* @return the result of the shift
*/
int32_t unsignedRightShift(int32_t n, int32_t cBits)
    {
    return uint32_t(n) >> cBits;
    }

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

UUID::UUID()
    : m_lDateTime(0), m_nAddr1(0), m_nAddr2(0), m_nAddr3(0), m_nAddr4(0),
      m_nPort(0), m_nCount(0), m_nHash(self(), 0, /* fMutable */ true)
    {
    }

UUID::UUID(int64_t lDateTime, InetAddress::View vAddr,
        int32_t nPort, int32_t nCount)
    : m_lDateTime(lDateTime), m_nAddr1(0), m_nAddr2(0), m_nAddr3(0),
      m_nAddr4(0), m_nCount(nCount), m_nHash(self(), 0, /* fMutable */ true)
    {
    if (NULL != vAddr)
        {
        initFromAddress(vAddr->getAddress(), nPort);
        }
    initHashcode();
    }

UUID::UUID(int64_t lDateTime, Array<octet_t>::View vabIP, int32_t nPort,
        int32_t nCount)
    : m_lDateTime(lDateTime), m_nAddr1(0), m_nAddr2(0), m_nAddr3(0),
      m_nAddr4(0), m_nCount(nCount), m_nHash(self(), 0, /* fMutable */ true)
    {
    initFromAddress(vabIP, nPort);
    initHashcode();
    }

UUID::UUID(String::View vs)
    : m_nHash(self(), 0, /* fMutable */ true)
    {
    initFromBytes(StringHelper::parseHex(vs));
    initHashcode();
    }

UUID::UUID(Array<octet_t>::View vab)
    : m_nHash(self(), 0, /* fMutable */ true)
    {
    initFromBytes(vab);
    initHashcode();
    }


// ----- UUID interface -----------------------------------------------------

bool UUID::isGenerated() const
    {
    ensureConstructed();
    return (m_nPort & mask_generated) != 0;
    }

int64_t UUID::getTimestamp() const
    {
    ensureConstructed();
    return m_lDateTime;
    }

bool UUID::isAddressIncluded() const
    {
    ensureConstructed();
    return (m_nPort & mask_realaddr) != 0;
    }

Array<octet_t>::View UUID::getAddress() const
    {
    ensureConstructed();
    Array<octet_t>::View vab;

    switch (m_nPort & (mask_realaddr | mask_ipv6addr))
        {
        case mask_realaddr | mask_ipv6addr:
            {
            int32_t                nAddr1 = m_nAddr1;
            int32_t                nAddr2 = m_nAddr2;
            int32_t                nAddr3 = m_nAddr3;
            int32_t                nAddr4 = m_nAddr4;
            Array<octet_t>::Handle hab    = Array<octet_t>::create(16);

            hab[ 0] = (octet_t) unsignedRightShift(nAddr1, 24);
            hab[ 1] = (octet_t) unsignedRightShift(nAddr1, 16);
            hab[ 2] = (octet_t) unsignedRightShift(nAddr1,  8);
            hab[ 3] = (octet_t) nAddr1;
            hab[ 4] = (octet_t) unsignedRightShift(nAddr2, 24);
            hab[ 5] = (octet_t) unsignedRightShift(nAddr2, 16);
            hab[ 6] = (octet_t) unsignedRightShift(nAddr2,  8);
            hab[ 7] = (octet_t) nAddr2;
            hab[ 8] = (octet_t) unsignedRightShift(nAddr3, 24);
            hab[ 9] = (octet_t) unsignedRightShift(nAddr3, 16);
            hab[10] = (octet_t) unsignedRightShift(nAddr3,  8);
            hab[11] = (octet_t) nAddr3;
            hab[12] = (octet_t) unsignedRightShift(nAddr4, 24);
            hab[13] = (octet_t) unsignedRightShift(nAddr4, 16);
            hab[14] = (octet_t) unsignedRightShift(nAddr4,  8);
            hab[15] = (octet_t) nAddr4;

            vab = hab;
            }
            break;

        case mask_realaddr:
            {
            int32_t                nAddr1 = m_nAddr1;
            Array<octet_t>::Handle hab    = Array<octet_t>::create(4);

            hab[0] = (octet_t) unsignedRightShift(nAddr1, 24);
            hab[1] = (octet_t) unsignedRightShift(nAddr1, 16);
            hab[2] = (octet_t) unsignedRightShift(nAddr1,  8);
            hab[3] = (octet_t) nAddr1;

            vab = hab;
            }
            break;

        case 0:
            vab = getEmptyByteArray();
            break;

        default:
            COH_THROW (IllegalStateException::create());
        }
    return vab;
    }

int32_t UUID::getPort() const
    {
    ensureConstructed();
    return m_nPort & ~mask_allflags;
    }

int32_t UUID::getCount() const
    {
    ensureConstructed();
    return m_nCount;
    }

Array<octet_t>::View UUID::toByteArray() const
    {
    ensureConstructed();
    Array<octet_t>::Handle hab       = Array<octet_t>::create(32);
    int64_t                lDateTime = m_lDateTime;
    int32_t                nAddr1    = m_nAddr1;
    int32_t                nAddr2    = m_nAddr2;
    int32_t                nAddr3    = m_nAddr3;
    int32_t                nAddr4    = m_nAddr4;
    int32_t                nPort     = m_nPort;
    int32_t                nCount    = m_nCount;

    hab[ 0] = (octet_t) unsignedRightShift(lDateTime, 56);
    hab[ 1] = (octet_t) unsignedRightShift(lDateTime, 48);
    hab[ 2] = (octet_t) unsignedRightShift(lDateTime, 40);
    hab[ 3] = (octet_t) unsignedRightShift(lDateTime, 32);
    hab[ 4] = (octet_t) unsignedRightShift(lDateTime, 24);
    hab[ 5] = (octet_t) unsignedRightShift(lDateTime, 16);
    hab[ 6] = (octet_t) unsignedRightShift(lDateTime,  8);
    hab[ 7] = (octet_t) lDateTime;
    hab[ 8] = (octet_t) unsignedRightShift(nAddr1, 24);
    hab[ 9] = (octet_t) unsignedRightShift(nAddr1, 16);
    hab[10] = (octet_t) unsignedRightShift(nAddr1, 8);
    hab[11] = (octet_t) nAddr1;
    hab[12] = (octet_t) unsignedRightShift(nAddr2, 24);
    hab[13] = (octet_t) unsignedRightShift(nAddr2, 16);
    hab[14] = (octet_t) unsignedRightShift(nAddr2,  8);
    hab[15] = (octet_t) nAddr2;
    hab[16] = (octet_t) unsignedRightShift(nAddr3, 24);
    hab[17] = (octet_t) unsignedRightShift(nAddr3, 16);
    hab[18] = (octet_t) unsignedRightShift(nAddr3,  8);
    hab[19] = (octet_t) nAddr3;
    hab[20] = (octet_t) unsignedRightShift(nAddr4, 24);
    hab[21] = (octet_t) unsignedRightShift(nAddr4, 16);
    hab[22] = (octet_t) unsignedRightShift(nAddr4,  8);
    hab[23] = (octet_t) nAddr4;
    hab[24] = (octet_t) unsignedRightShift(nPort, 24);
    hab[25] = (octet_t) unsignedRightShift(nPort, 16);
    hab[26] = (octet_t) unsignedRightShift(nPort,  8);
    hab[27] = (octet_t) nPort;
    hab[28] = (octet_t) unsignedRightShift(nCount, 24);
    hab[29] = (octet_t) unsignedRightShift(nCount, 16);
    hab[30] = (octet_t) unsignedRightShift(nCount,  8);
    hab[31] = (octet_t) nCount;

    return hab;
    }


// ----- Comparable interface -----------------------------------------------

int32_t UUID::compareTo(Object::View v) const
    {
    ensureConstructed();

    int32_t    nResult = 0;
    UUID::View vThat   = cast<UUID::View>(v);

    if (this != vThat)
        {
        if (m_lDateTime != vThat->m_lDateTime)
            {
            nResult = m_lDateTime < vThat->m_lDateTime ? -1 : 1;
            }
        else if (m_nAddr1 != vThat->m_nAddr1)
            {
            nResult = m_nAddr1 < vThat->m_nAddr1 ? -1 : 1;
            }
        else if (m_nAddr2 != vThat->m_nAddr2)
            {
            nResult = m_nAddr2 < vThat->m_nAddr2 ? -1 : 1;
            }
        else if (m_nAddr3 != vThat->m_nAddr3)
            {
            nResult = m_nAddr3 < vThat->m_nAddr3 ? -1 : 1;
            }
        else if (m_nAddr4 != vThat->m_nAddr4)
            {
            nResult = m_nAddr4 < vThat->m_nAddr4 ? -1 : 1;
            }
        else if (m_nPort != vThat->m_nPort)
            {
            nResult = m_nPort < vThat->m_nPort ? -1 : 1;
            }
        else if (m_nCount != vThat->m_nCount)
            {
            nResult = m_nCount < vThat->m_nCount ? -1 : 1;
            }
        }
        return nResult;
    }


// ----- PortableObject interface -------------------------------------------

void UUID::readExternal(PofReader::Handle hIn)
    {
    // note: this public method must not call ensureConstructed()
    if (m_nHash != 0)
        {
        // an attempt was made to change a UUID -- which is an immutable
        // object -- by deserializing into it!
        COH_THROW (IllegalStateException::create("UUID can't be modified"));
        }

    m_lDateTime = hIn->readInt64(0);
    m_nAddr1    = hIn->readInt32(1);
    m_nAddr2    = hIn->readInt32(2);
    m_nAddr3    = hIn->readInt32(3);
    m_nAddr4    = hIn->readInt32(4);
    m_nPort     = hIn->readInt32(5);
    m_nCount    = hIn->readInt32(6);

    initHashcode();
    }

void UUID::writeExternal(PofWriter::Handle hOut) const
    {
    ensureConstructed();

    hOut->writeInt64(0, m_lDateTime);
    hOut->writeInt32(1, m_nAddr1);
    hOut->writeInt32(2, m_nAddr2);
    hOut->writeInt32(3, m_nAddr3);
    hOut->writeInt32(4, m_nAddr4);
    hOut->writeInt32(5, m_nPort);
    hOut->writeInt32(6, m_nCount);
    }


//----- Object interface ----------------------------------------------------

TypedHandle<const String> UUID::toString() const
    {
    return COH_TO_STRING(StringHelper::toHexEscape(toByteArray()));
    }

bool UUID::equals(Object::View v) const
    {
    ensureConstructed();

    UUID::View vThat = cast<UUID::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }
    else if (((UUID::View) this) == vThat)
        {
        return true;
        }

    return m_nHash     == vThat->m_nHash     &&
           m_lDateTime == vThat->m_lDateTime &&
           m_nAddr1    == vThat->m_nAddr1    &&
           m_nAddr2    == vThat->m_nAddr2    &&
           m_nAddr3    == vThat->m_nAddr3    &&
           m_nAddr4    == vThat->m_nAddr4    &&
           m_nPort     == vThat->m_nPort     &&
           m_nCount    == vThat->m_nCount;
    }

size32_t UUID::hashCode() const
    {
    ensureConstructed();
    return m_nHash;
    }


// ----- helper methods -----------------------------------------------------

void UUID::ensureConstructed() const
    {
    if (m_nHash == 0)
        {
        COH_SYNCHRONIZED (this)
            {
            if (m_nHash == 0)
                {
                // this UUID will be a "generated" UUID
                COH_SYNCHRONIZED(getLock())
                    {
                    m_lDateTime = System::safeTimeMillis();
                    m_nCount    = getNextCount();
                    }

                InetAddressInfo::View vInetAddr    = getInetAddressInfo();
                bool                  fRealAddress = vInetAddr->isRealAddress();
                bool                  fIPv6        = vInetAddr->isIPv6();
                Random::Handle        hRand        = Random::getInstance();

                // the "address" is either a 128-bit IPv6 address, a 32-bit IPv4
                // address with the balance random, or a 128-bit random
                if (fRealAddress)
                    {
                    if (fIPv6)
                        {
                        // 128-bit IPv6 address
                        m_nAddr1 = vInetAddr->getAddr1();
                        m_nAddr2 = vInetAddr->getAddr2();
                        m_nAddr3 = vInetAddr->getAddr3();
                        m_nAddr4 = vInetAddr->getAddr4();
                        }
                    else
                        {
                        // 32-bit IPv4 address; the rest is random
                        m_nAddr1 = vInetAddr->getAddr1();
                        m_nAddr2 = hRand->nextInt32();
                        m_nAddr3 = hRand->nextInt32();
                        m_nAddr4 = hRand->nextInt32();
                        }
                    }
                else
                    {
                    // 128-bit random value instead of an address
                    m_nAddr1 = hRand->nextInt32();
                    m_nAddr2 = hRand->nextInt32();
                    m_nAddr3 = hRand->nextInt32();
                    m_nAddr4 = hRand->nextInt32();
                    }

                // the "port" is mostly random data, except that the flags are
                // encoded into it
                m_nPort = mask_generated | (fRealAddress ? mask_realaddr : 0)
                        | (fIPv6 ?  mask_ipv6addr : 0)
                        | (hRand->nextInt32() & ~mask_allflags);

                initHashcode();
                }
            }
        }
    }

void UUID::initHashcode() const
    {
    int32_t nDateTimeHighOrder = (int32_t) unsignedRightShift(m_lDateTime, 32);
    int32_t nHash = nDateTimeHighOrder
                  ^ (int32_t) m_lDateTime  // low order 32-bits
                  ^ m_nAddr1
                  ^ m_nAddr2
                  ^ m_nAddr3
                  ^ m_nAddr4
                  ^ m_nPort
                  ^ m_nCount;

    m_nHash = nHash != 0 ? nHash : 2147483647;
    }

void UUID::initFromAddress(Array<octet_t>::View vabIP, int32_t nPort)
    {
    bool fAddr = false;
    bool fIPv6 = false;

    if (vabIP != NULL)
        {
        switch(vabIP->length)
            {
            case 16:
                m_nAddr4 = (vabIP[12] & 0xFF) << 24
                         | (vabIP[13] & 0xFF) << 16
                         | (vabIP[14] & 0xFF) <<  8
                         | (vabIP[15] & 0xFF);
                m_nAddr3 = (vabIP[ 8] & 0xFF) << 24
                         | (vabIP[ 9] & 0xFF) << 16
                         | (vabIP[10] & 0xFF) <<  8
                         | (vabIP[11] & 0xFF);
                m_nAddr2 = (vabIP[ 4] & 0xFF) << 24
                         | (vabIP[ 5] & 0xFF) << 16
                         | (vabIP[ 6] & 0xFF) <<  8
                         | (vabIP[ 7] & 0xFF);
                fIPv6 = true;
                // fall through

            case 4:
                m_nAddr1 = (vabIP[ 0] & 0xFF) << 24
                         | (vabIP[ 1] & 0xFF) << 16
                         | (vabIP[ 2] & 0xFF) <<  8
                         | (vabIP[ 3] & 0xFF);
                fAddr = true;
                break;

            case 0:
                break;

            default:
                COH_THROW_STREAM(IllegalArgumentException,
                        "Unsupported IP address length: " << vabIP->length);
            }
        }
    m_nPort = (fAddr ? mask_realaddr : 0) | (fIPv6 ? mask_ipv6addr : 0)
            | (nPort & ~mask_allflags);
    }

void UUID::initFromBytes(Array<octet_t>::View vab)
    {
    COH_ENSURE_PARAM(vab);
    COH_ENSURE_PARAM_RELATION(vab->length, ==, 32);

    m_lDateTime = (int64_t) (vab[ 0] & 0xFF) << 56
                | (int64_t) (vab[ 1] & 0xFF) << 48
                | (int64_t) (vab[ 2] & 0xFF) << 40
                | (int64_t) (vab[ 3] & 0xFF) << 32
                | (int64_t) (vab[ 4] & 0xFF) << 24
                | (int64_t) (vab[ 5] & 0xFF) << 16
                | (int64_t) (vab[ 6] & 0xFF) <<  8
                | (int64_t) (vab[ 7] & 0xFF);
    m_nAddr1    = (int32_t) (vab[ 8] & 0xFF) << 24
                | (int32_t) (vab[ 9] & 0xFF) << 16
                | (int32_t) (vab[10] & 0xFF) <<  8
                | (int32_t) (vab[11] & 0xFF);
    m_nAddr2    = (int32_t) (vab[12] & 0xFF) << 24
                | (int32_t) (vab[13] & 0xFF) << 16
                | (int32_t) (vab[14] & 0xFF) <<  8
                | (int32_t) (vab[15] & 0xFF);
    m_nAddr3    = (int32_t) (vab[16] & 0xFF) << 24
                | (int32_t) (vab[17] & 0xFF) << 16
                | (int32_t) (vab[18] & 0xFF) <<  8
                | (int32_t) (vab[19] & 0xFF);
    m_nAddr4    = (int32_t) (vab[20] & 0xFF) << 24
                | (int32_t) (vab[21] & 0xFF) << 16
                | (int32_t) (vab[22] & 0xFF) <<  8
                | (int32_t) (vab[23] & 0xFF);
    m_nPort     = (int32_t) (vab[24] & 0xFF) << 24
                | (int32_t) (vab[25] & 0xFF) << 16
                | (int32_t) (vab[26] & 0xFF) <<  8
                | (int32_t) (vab[27] & 0xFF);
    m_nCount    = (int32_t) (vab[28] & 0xFF) << 24
                | (int32_t) (vab[29] & 0xFF) << 16
                | (int32_t) (vab[30] & 0xFF) <<  8
                | (int32_t) (vab[31] & 0xFF);
    }

COH_CLOSE_NAMESPACE2
