/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_PERSON_HPP
#define COH_SIMPLE_PERSON_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::List;

/**
* SimplePerson, a simple class, implements a PortableObject.
*
* @author par  2013.09.1426
* @since Coherence 12.1.3
*/
class SimplePerson
    : public class_spec<SimplePerson,
        extends<Object>,
        implements< PortableObject> >
    {
    friend class factory<SimplePerson>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor
        */
        SimplePerson();

    public:
        /**
        * Create a new SimplePerson
        */
        SimplePerson(String::View vsSSN, String::View vsFirstName,
                String::View vsLastName, int32_t nYear,
                String::View vsMotherId,
                List::View   vChildrenIds);


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
        bool equals(Object::View v) const;


    // ----- accessors ------------------------------------------------------

    public:
        virtual String::View getLastName() const;

        virtual void setLastName(String::View vsLastName);

    // ----- data members ---------------------------------------------------

    protected:
        MemberView<String>  m_vsSSN;
        MemberView<String>  m_vsFirstName;
        MemberView<String>  m_vsLastName;
        int32_t             m_nYear;
        MemberView<String>  m_vsMotherId;
        MemberView<List>    m_vaChildrenIds;

    // ----- POF index constants --------------------------------------------

    public:
        static const int32_t SSN        = 0;
        static const int32_t FIRST_NAME = 1;
        static const int32_t LAST_NAME  = 2;
        static const int32_t BIRTH_YEAR = 3;
        static const int32_t MOTHER_SSN = 4;
        static const int32_t CHILDREN   = 5;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_PERSON_HPP
