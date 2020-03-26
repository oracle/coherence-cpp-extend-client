/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TESTOBJECT_HPP
#define COH_TESTOBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* A simple test object with minimal members
*
* @since Coherence 3.7.1.12
* @author par 2/3/2014
*/
class TestObject 
    : public class_spec<TestObject,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<TestObject>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        TestObject();

    public:
        /**
        * Create a new TestObject object.
        *
        * @param Id     the object id
        * @param sName  the object name
        */
        TestObject(int32_t Id, String::View vsName); 


    // ---- PortableObject Interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hReader);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hWriter) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
	virtual size32_t hashCode() const;


    // -----  accessors ----------------------------------------

    public:
        /**
        * Determine the id of the object.
        *
        * @return the object's id
        */
        int32_t getId() const;

        /**
        * Configure the object id.
        *
        * @param nID  the object's id
        */
        void setId(int32_t ID);

        /**
        * Determine the name of the object.
        *
        * @return the object's name
        */
        String::View getName() const;

        /**
        * Configure the name of the object.
        *
        * @param vsName  the object's name
        */
        void setName(String::View vsName);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The object's id.
        */
        int32_t m_ID;

        /**
        * The object's name.
        */
        MemberView<String> m_vsName;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TESTOBJECT_HPP
