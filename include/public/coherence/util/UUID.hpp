/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UUID_HPP
#define COH_UUID_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Random.hpp"
#include "coherence/net/InetAddress.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::net::InetAddress;
using coherence::io::pof::PortableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* A UUID is a 256-bit identifier that, if it is generated, is statistically
* guaranteed to be unique.
*
* @author jh 2007.12.20
* @author js 2008.03.24
*/
class COH_EXPORT UUID
    : public class_spec<UUID,
        extends<Object>,
        implements<Comparable, PortableObject> >
    {
    friend class factory<UUID>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Generate a UUID.
        */
        UUID();

        /**
        * Build a UUID from its constituent members (advanced constructor).
        * <p/>
        * It is guaranteed that a generated UUID will never equal a built
        * UUID.
        *
        * @param lDateTime  the creation date/time millis portion of the UUID
        * @param vAddr      the InetAddress portion of the UUID
        * @param nPort      the port number portion of the UUID; a port
        *                   number is 16 bits, but up to 28 bits of data from
        *                   this value will be maintained by the UUID
        * @param nCount     the counter portion of the UUID
        */
        UUID(int64_t lDateTime, InetAddress::View vAddr,
                int32_t nPort, int32_t nCount);

        /**
        * Build a UUID from its constituent members (advanced constructor).
        * <p/>
        * It is guaranteed that a generated UUID will never equal a built
        * UUID.
        *
        * @param lDateTime  the creation date/time millis portion of the UUID
        * @param vabIP      the InetAddress portion of the UUID
        * @param nPort      the port number portion of the UUID; a port
        *                   number is 16 bits, but up to 28 bits of data from
        *                   this value will be maintained by the UUID
        * @param nCount     the counter portion of the UUID
        */
        UUID(int64_t lDateTime, Array<octet_t>::View vabIP, int32_t nPort,
                int32_t nCount);

        /**
        * Construct a UUID from a String.
        *
        * @param vs  a string as would be returned from UUID.toString()
        */
        UUID(String::View vs);

        /**
        * Construct a UUID from a byte array.
        *
        * @param vab  a byte array as would be returned from
        *             UUID.toByteArray()
        */
        UUID(Array<octet_t>::View vab);


    // ----- UUID interface -------------------------------------------------

    public:
        /**
        * True if the UUID was generated, and false if it was built. A
        * generated UUID is universally unique. Note that the port number is
        * random if the UUID is generated.
        *
        * @return true if the UUID was generated
        */
        virtual bool isGenerated() const;

        /**
        * Determine the date/time value for the UUID.
        *
        * @return date/time value in millis
        */
        virtual int64_t getTimestamp() const;

        /**
        * True if the IP address is a real IP address. This is only false
        * if two conditions are met: The UUID is generated, and it could not
        * get an IP address (or one that is not a loopback/localhost address).
        *
        * @return true if the UUID has IP address information
        */
        virtual bool isAddressIncluded() const;

        /**
        * Determine the internet address of the host that generated the UUID
        * instance.
        *
        * @return an array of bytes containing the IP address information;
        *         the array can be zero bytes (no address,) four bytes (IPv4)
        *         or 16 bytes (IPv6)
        */
        virtual Array<octet_t>::View getAddress() const;

        /**
        * Determine the port portion of the UUID. Note that the port is a
        * 28-bit value; the first nibble is always 0x0.
        *
        * @return the port portion of the UID
        */
        virtual int32_t getPort() const;

        /**
        * Determine the "counter" portion of the UUID that ensures that two
        * UUIDs generated at the same exact time by the same process are
        * unique.
        *
        * @return a number that helps to make the UUID unique
        */
        virtual int32_t getCount() const;

        /**
        * Convert the UUID to a byte array of 32 bytes.
        *
        * @return the UUID data as a byte array of 32 bytes
        */
        virtual Array<octet_t>::View toByteArray() const;


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- helper methods -------------------------------------------------

    private:
        /**
        * If this UUID is being used as a generated UUID but its fields have
        * not yet been initialized, this method ensures that the
        * initialization occurs. All public methods, except for
        * deserialization methods, must call this method to ensure that the
        * UUID is properly constructed.
        */
        void ensureConstructed() const;

        /**
        * Finish construction or deserialization. The UUID's internally
        * cached hashcode value is zero until construction is completed, or
        * until deserialization is completed, and never zero otherwise. Every
        * constructor, except for the deserialization constructor, must call
        * this method.
        */
        void initHashcode() const;

        /**
        * Finish construction by setting Addr1..4 from an IP address.
        */
        void initFromAddress(Array<octet_t>::View vabIP, int32_t nPort);

        /**
        * Finish construction from a byte array.
        */
        void initFromBytes(Array<octet_t>::View vab);

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * System date/time value that the UUID instance was generated.
        */
        mutable int64_t m_lDateTime;

        /**
        * Internet address of host that generated the UUID instance. For IPv4,
        * this contains the entire IP address. For IPv6, this contains only
        * the first four bytes of the address. If the UUID is auto-generated
        * and it could not obtain a real address, then this is a random
        * number.
        */
        mutable int32_t m_nAddr1;

        /**
        * The second four bytes of the IP address. For IPv6, this is the
        * second four bytes of the IP address. If the UUID is auto-generated
        * and it could not obtain a real address, then this is a random
        * number. Otherwise if the UUID is built (not generated) and the
        * address is IPv4, then this is zero.
        */
        mutable int32_t m_nAddr2;

        /**
        * The third four bytes of the IP address. For IPv6, this is the third
        * four bytes of the IP address. If the UUID is auto-generated and it
        * could not obtain a real address, then this is a random number.
        * Otherwise if the UUID is built (not generated) and the address is
        * IPv4, then this is zero.
        */
        mutable int32_t m_nAddr3;

        /**
        * The fourth four bytes of the IP address. For IPv6, this is the
        * fourth four bytes of the IP address. If the UUID is auto-generated
        * and it could not obtain a real address, then this is a random
        * number. Otherwise if the UUID is built (not generated) and the
        * address is IPv4, then this is zero.
        */
        mutable int32_t m_nAddr4;

        /**
        * The least significant two bytes of this value are the port number
        * if the UUID is built (not generated). Otherwise this is a random
        * number, with the exception of the most significant nibble. The most
        * significant nibble contains the flags of the UUID.
        */
        mutable int32_t m_nPort;

        /**
        * A rolling counter.
        */
        mutable int32_t m_nCount;

        /**
        * Cache the hash. Only zero pending deserialization or generation.
        */
        mutable Volatile<size32_t> m_nHash;


    // ----- constants ------------------------------------------------------

    protected:
        /**
        * A bit mask that represents the portion of the "port" value reserved
        * for bit flags.
        */
        static const int32_t mask_allflags = 0xF0000000;

        /**
        * The bit mask for the "is generated UUID" flag.
        */
        static const int32_t mask_generated = 1 << 31;

        /**
        * The bit mask for the "is a real IP address" flag.
        */
        static const int32_t mask_realaddr = 1 << 30;

        /**
        * The bit mask for the "is an IPv6 address" flag.
        */
        static const int32_t mask_ipv6addr = 1 << 29;

        /**
        * The one remaining bit for future use.
        */
        static const int32_t mask_unused = 1 << 28;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_UUID_HPP
