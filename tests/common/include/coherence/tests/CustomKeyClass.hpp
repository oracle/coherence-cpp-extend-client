/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CUSTOMKEYCLASS_HPP
#define COH_CUSTOMKEYCLASS_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using namespace coherence::lang;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* A custom key class used by the Coherence*Extend tests.
*
* @author phf  2011.09.06
*/
class CustomKeyClass : public class_spec<CustomKeyClass,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<CustomKeyClass>;

    // ----- constructors -----------------------------------------------

    public:
        /**
        * Default constructor. Needed for PortableObject.
        */
        CustomKeyClass();

        /**
        * Construct a CustomKeyClass object from an Object.
        *
        * @param v  an object
        */
        CustomKeyClass(Object::View v);


    // ----- PortableObject interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;

    // ----- Object interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members -----------------------------------------------

    protected:
        /**
        * An object.
        */
        FinalView<Object> f_v;
    };

COH_CLOSE_NAMESPACE2

#endif

