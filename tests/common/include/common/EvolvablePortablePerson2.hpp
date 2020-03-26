/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVOLVABLE_PORTABLE_PERSON2_HPP
#define COH_EVOLVABLE_PORTABLE_PERSON2_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "common/Address.hpp"
#include "common/EvolvablePortablePerson.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Version 2 of EvolvablePortablePerson used for testing
*
* @author lh  2011.07.15
*/
class EvolvablePortablePerson2
    : public class_spec<EvolvablePortablePerson2,
        extends<EvolvablePortablePerson> >
    {
    friend class factory<EvolvablePortablePerson2>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new EvolvablePortablePerson2
        */
        EvolvablePortablePerson2(String::View vsName = String::null_string,
                int32_t nAge = 0)
            : super(vsName, nAge),
              m_vsNationality(self(), String::null_string), m_vPOB(self(), NULL)
            {
            }


    // ----- Evolvable interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getImplVersion() const
            {
            return 2;
            }


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn)
            {
            super::readExternal(hIn);
            setNationality(hIn->readString(5));
            setPOB(cast<Address::View>(hIn->readObject(6)));
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
            hOut->writeString(5, getNationality());
            hOut->writeObject(6, getPOB());
            }


    // ----- accessors ------------------------------------------------------

    public:
        virtual String::View getNationality() const
            {
            return m_vsNationality;
            }

        virtual void setNationality(String::View vsNationality)
            {
            m_vsNationality = vsNationality;
            }

        virtual Address::View getPOB() const
            {
            return m_vPOB;
            }

        virtual void setPOB(Address::View vPOB)
            {
            m_vPOB = vPOB;
            }


    // ----- data members ---------------------------------------------------

    protected:
        MemberView<String>  m_vsNationality;
        MemberView<Address> m_vPOB;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EVOLVABLE_PORTABLE_PERSON2_HPP
