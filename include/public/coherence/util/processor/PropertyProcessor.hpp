/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PROPERTY_PROCESSOR_HPP
#define COH_PROPERTY_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/ValueManipulator.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::Describable;
using coherence::util::InvocableMap;
using coherence::util::ValueManipulator;


/**
* PropertyProcessor is a base class for EntryProcessor implementations that
* depend on a ValueManipulator.
*
* @author tb  2008.04.28
*/
class COH_EXPORT PropertyProcessor
    : public abstract_spec<PropertyProcessor,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the ExternalizableLite interface).
        */
        PropertyProcessor();

        /**
        * Construct a PropertyProcessor based for the specified
        * ValueManipulator.
        *
        * @param vManipulator  a ValueManipulator; could be null
        */
        PropertyProcessor(ValueManipulator::View vManipulator);


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
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Get the property value from the passed Entry object.
        *
        * @param vEntry  the Entry object
        *
        * @return the property value
        */
        virtual Object::View get(InvocableMap::Entry::View vEntry) const;

        /**
        * Set the property value into the passed Entry object.
        *
        * @param hEntry  the Entry object
        * @param vValue a new property value
        */
        virtual void set(InvocableMap::Entry::Handle hEntry,
                Object::View vValue) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The property value manipulator.
        */
        FinalView<ValueManipulator> f_vManipulator;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PROPERTY_PROCESSOR_HPP
