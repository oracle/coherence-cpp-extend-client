/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INET_ADDRESS_HPP
#define COH_INET_ADDRESS_HPP

#include "coherence/lang.ns"



COH_OPEN_NAMESPACE2(coherence,net)


/**
* InetAddress represents an IP address.
*
* @author mf  2008.03.17
*/
class COH_EXPORT InetAddress
    : public cloneable_spec<InetAddress>
    {
    friend class factory<InetAddress>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Manually construct an InetAddress from a network byte ordered array.
        *
        * It is recommended that the #getByName() method be used in place of
        * manual InetAddress construction.
        *
        * @param vsName  the name of the host
        * @param vaIP    the array representation of the host's IP address.
        */
        InetAddress(String::View vsHost, Array<octet_t>::View vaIP);

        /**
        * Copy Constructor.
        */
        InetAddress(const InetAddress&);


    // ----- InetAddress interface ------------------------------------------

    public:
        /**
        * Return the InetAddress for the local machine.
        *
        * @return the local InetAddress.
        */
        static InetAddress::View getLocalHost();

        /**
        * Return the InetAddress for the specified machine name.
        *
        * @param vHost  the name or IP of the host to resolve
        *
        * @return the InetAddress for the specified machine name
        */
        static InetAddress::View getByName(String::View vsHost);

        /**
         * Given the name of a host, returns an array of its IP addresses,
         * based on the configured name service on the system.
         *
         * If the host is NULL then an InetAddress representing an address
         * of the loopback interface is returned.
         *
         * @param vsHost  the name of the host, or NULL 
         *
         * @return an array of all the IP addresses for a given host name
         *
         * @exception  UnknownHostException  if no IP address for the
         *             vsHost could be found.
         *
         * @since 12.2.1
         */
        static ObjectArray::Handle getAllByName(String::View vsHost);

        /**
         * Returns the loopback address.
         *
         * @return  the InetAddress loopback instance
         *
         * @since 12.2.1
         */
        static InetAddress::View getLoopbackAddress();

        /**
        * Return the hostname associated with this address.
        *
        * @return the hostname associated with this address
        */
        virtual String::View getHostName() const;

        /**
        * Return the address in textual form.
        *
        * @return the address in textual form
        */
        virtual String::View getHostAddress() const;

        /**
        * Return the raw IP address, in network byte order, with the highest
        * order byte stored at index zero.
        *
        * @return the raw IP address
        */
        virtual Array<octet_t>::View getAddress() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View that) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ---- data members ----------------------------------------------------

    private:
        /**
        * The hostname or textual IP for the address.
        */
        FinalView<String> f_vsHostName;

        /**
        * The raw IP address.
        */
        FinalView<Array<octet_t> > f_vaIP;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INET_ADDRESS_HPP
