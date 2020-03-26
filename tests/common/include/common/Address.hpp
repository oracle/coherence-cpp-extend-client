/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ADDRESS_HPP
#define COH_ADDRESS_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Address is a place where a person is located.
*
* The type implements PortableObject for efficient cross-platform
* serialization.
*
* @author lh  2011.07.15
*/
class Address
    : public class_spec<Address,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<Address>;

    protected:
        Address()
            : m_vsStreet(self(), String::null_string),
              m_vsCity(self(), String::null_string),
              m_vsState(self(), String::null_string),
              m_vsZip(self(), String::null_string)
            {
            }

        /**
        * Construct an Address.
        *
        * @param vsStreet  the street address
        * @param vsCity    city name
        * @param vsState   State name
        * @param vsZip     Zip (postal) code
        */
        Address(String::View vsStreet, String::View vsCity,
                String::View vsState, String::View vsZip)
            : m_vsStreet(self(), vsStreet), m_vsCity(self(), vsCity),
              m_vsState(self(), vsState), m_vsZip(self(), vsZip)
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn)
            {
            setStreet(hIn->readString(0));
            setCity(hIn->readString(1));
            setState(hIn->readString(2));
            setZipCode(hIn->readString(3));
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeString(0, getStreet());
            hOut->writeString(1, getCity());
            hOut->writeString(2, getState());
            hOut->writeString(3, getZipCode());
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        bool equals(Object::View v) const
            {
            if (this == v)
                {
                return true;
                }

            Address::View vThat = cast<Address::View>(v, false);
            if (NULL != vThat)
                {
                return Object::equals(this->getStreet(),  vThat->getStreet())
                    && Object::equals(this->getCity(),    vThat->getCity())
                    && Object::equals(this->getState(),   vThat->getState())
                    && Object::equals(this->getZipCode(), vThat->getZipCode());
                }
            return false;
            }


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the street address.
        *
        * @return the street address
        */
        String::View getStreet() const
            {
            return m_vsStreet;
            }

        /**
        * Set the street address.
        *
        * @param sStreet  the street address
        */
        void setStreet(String::View vsStreet)
            {
            m_vsStreet = vsStreet;
            }

        /**
        * Return the city name.
        *
        * @return the city name
        */
        String::View getCity() const
            {
            return m_vsCity;
            }

        /**
        * Set the city name.
        *
        * @param sCity  the city name
        */
        void setCity(String::View vsCity)
            {
            m_vsCity = vsCity;
            }

        /**
        * Return the state or territory name.
        *
        * @return the state or territory name
        */
        String::View getState() const
            {
            return m_vsState;
            }

        /**
        * Set the State or Province name.
        *
        * @param sState  the State or Province name
        */
        void setState(String::View vsState)
             {
             m_vsState = vsState;
             }

        /**
        * Return the Zip code.
        *
        * @return the Zip code
        */
        String::View getZipCode() const
            {
            return m_vsZip;
            }

        /**
        * Set the Zip code.
        *
        * @param sZip  the Zip code
        */
        void setZipCode(String::View vsZip)
            {
            m_vsZip = vsZip;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Street address.
        */
        MemberView<String> m_vsStreet;

        /**
        * City.
        */
        MemberView<String> m_vsCity;

        /**
        * State or Province.
        */
        MemberView<String> m_vsState;

        /**
        * Zip or other postal code.
        */
        MemberView<String> m_vsZip;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ADDRESS_HPP
