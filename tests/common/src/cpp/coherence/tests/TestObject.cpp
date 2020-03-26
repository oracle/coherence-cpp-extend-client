/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/TestObject.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)


// ----- constructors ---------------------------------------------------

TestObject::TestObject(int32_t Id, String::View vsName) 
        : m_ID(Id), m_vsName(self(), vsName)
    {
    }

TestObject::TestObject()
        : m_ID(0), m_vsName(self())
    {
    }


// ---- PortableObject Interface -----------------------------------------

void TestObject::readExternal(PofReader::Handle hReader)
    {
    m_ID     = hReader->readInt32(0);
    m_vsName = hReader->readString(1);
    }

void TestObject::writeExternal(PofWriter::Handle hWriter) const
    {
    hWriter->writeInt32(0, m_ID);
    hWriter->writeString(1, m_vsName);
    }

// ----- Object ---------------------------------------------------------

bool TestObject::equals(Object::View that) const
    {
    if(instanceof<TestObject::View>(that))
        {
        TestObject::View vThat = cast<TestObject::View>(that);

        if (getId() == vThat->getId() && getName()->equals(vThat->getName()))
            {
            return true;
            }
        else
            {
            return false;
            }
        }
    else
        {
        return false;
        }
    }

size32_t TestObject::hashCode() const
    {
    return (size32_t) getName()->hashCode();
    }

// -----  accessors ----------------------------------------

int32_t TestObject::getId() const
    {
    return m_ID;
    }

void TestObject::setId(int32_t ID)
    {
    m_ID = ID;
    }

String::View TestObject:: getName() const
    {
    return m_vsName;
    }

void TestObject::setName(String::View vsName)
    {
    m_vsName = vsName;
    }

COH_REGISTER_PORTABLE_CLASS(3011, TestObject);

COH_CLOSE_NAMESPACE2
