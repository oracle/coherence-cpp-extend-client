/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/SimplePerson.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using coherence::util::ArrayList;
using coherence::util::ReadOnlyArrayList;


// ----- constructors ---------------------------------------------------

SimplePerson::SimplePerson()
        : m_vsSSN(self()),
          m_vsFirstName(self()),
          m_vsLastName(self()),
          m_nYear(0),
          m_vsMotherId(self()),
          m_vaChildrenIds(self())
    {
    }

SimplePerson::SimplePerson(String::View vsSSN, String::View vsFirstName,
        String::View vsLastName, int32_t nYear,
        String::View vsMotherId,
        List::View vaChildrenIds)
        : m_vsSSN(self(), vsSSN),
          m_vsFirstName(self(), vsFirstName),
          m_vsLastName(self(), vsLastName),
          m_nYear(nYear),
          m_vsMotherId(self(), vsMotherId),
          m_vaChildrenIds(self(), ArrayList::create(vaChildrenIds))
    {
    }


// ----- PortableObject interface ---------------------------------------

void SimplePerson::readExternal(PofReader::Handle hIn)
    {
    m_vsSSN         = hIn->readString(SSN);
    m_vsFirstName   = hIn->readString(FIRST_NAME);
    setLastName(hIn->readString(LAST_NAME));
    m_nYear         = hIn->readInt32(BIRTH_YEAR);
    m_vsMotherId    = hIn->readString(MOTHER_SSN);
    m_vaChildrenIds = cast<List::View>(hIn->readCollection(CHILDREN, ArrayList::create()));
    }

void SimplePerson::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeString(SSN,          m_vsSSN);
    hOut->writeString(FIRST_NAME,   m_vsFirstName);
    hOut->writeString(LAST_NAME,    getLastName());
    hOut->writeInt32(BIRTH_YEAR,    m_nYear);
    hOut->writeString(MOTHER_SSN,   m_vsMotherId);
    hOut->writeCollection(CHILDREN, m_vaChildrenIds);
    }


// ----- Object interface -----------------------------------------------

bool SimplePerson::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    if (instanceof<SimplePerson::View>(v))
        {
        SimplePerson::View vThat = cast<SimplePerson::View>(v);
        return Object::equals(this->m_vsSSN,       vThat->m_vsSSN)
                && Object::equals(this->m_vsFirstName, vThat->m_vsFirstName)
                && Object::equals(this->getLastName(), vThat->getLastName())
                && this->m_nYear                  ==   vThat->m_nYear
                && Object::equals(this->m_vsMotherId,  vThat->m_vsMotherId);
        }
        return false;
    }


// ----- accessors ------------------------------------------------------

String::View SimplePerson::getLastName() const
    {
    return m_vsLastName;
    }

void SimplePerson::setLastName(String::View vsLastName)
    {
    m_vsLastName = vsLastName;
    }

COH_REGISTER_PORTABLE_CLASS(3010, SimplePerson);

COH_CLOSE_NAMESPACE2

