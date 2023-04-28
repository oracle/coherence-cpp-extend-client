/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_PERSON_HPP
#define COH_PORTABLE_PERSON_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "common/Address.hpp"
#include "common/TestPerson.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Extends TestPerson, implements a PortableObject.
*
* @author lh  2011.07.14
*/
class PortablePerson
    : public class_spec<PortablePerson,
        extends<TestPerson>,
        implements<PortableObject> >
    {
    friend class factory<PortablePerson>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PortablePerson
        */
        PortablePerson(String::View vsName = String::null_string,
                int32_t nAge = 0)
            : super(vsName, nAge),
              m_vAddress(self(), NULL),
              m_vSpouse(self(), NULL), m_vaChildren(self(), NULL)
            {
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn)
            {
            setName(hIn->readString(0));
            setAge(hIn->readInt32(1));
            setAddress(cast<Address::View>(hIn->readObject(2)));
            setSpouse(cast<PortablePerson::View>(hIn->readObject(3)));
            setChildren(hIn->readObjectArray(4));
            }

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            hOut->writeString(0, getName());
            hOut->writeInt32(1, getAge());
            hOut->writeObject(2, getAddress());
            hOut->writeObject(3, getSpouse());
            hOut->writeObjectArray(4, getChildren());
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

            PortablePerson::View vThat = cast<PortablePerson::View>(v, false);
            if (NULL != vThat)
                {
                return Object::equals(this->getName(),    vThat->getName())
                    && this->getAge()                  == vThat->getAge()
                    && Object::equals(this->getAddress(), vThat->getAddress())
                    && Object::equals(this->getSpouse(),  vThat->getSpouse());
                }
            return false;
            }


    // ----- accessors ------------------------------------------------------

    public:
        virtual Address::View getAddress() const
            {
            return m_vAddress;
            }

        virtual void setAddress(Address::View vAddress)
            {
            m_vAddress = vAddress;
            }

        virtual PortablePerson::View getSpouse() const
            {
            return m_vSpouse;
            }

        virtual void setSpouse(PortablePerson::View vSpouse)
            {
            m_vSpouse = vSpouse;
            }

        virtual ObjectArray::View getChildren() const
            {
            return m_vaChildren;
            }

        virtual void setChildren(ObjectArray::View vaChildren)
            {
            m_vaChildren = vaChildren;
            }


    // ----- data members ---------------------------------------------------

    protected:
        MemberView<Address>        m_vAddress;
        MemberView<PortablePerson> m_vSpouse;
        MemberView<ObjectArray>    m_vaChildren;
    };
COH_REGISTER_PORTABLE_CLASS(1034, PortablePerson);

COH_CLOSE_NAMESPACE2

#endif // COH_PORTABLE_PERSON_HPP
