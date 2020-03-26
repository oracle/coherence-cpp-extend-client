/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NUMBER_MULTIPLIER_HPP
#define COH_NUMBER_MULTIPLIER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/processor/PropertyProcessor.hpp"
#include "coherence/util/ValueManipulator.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::InvocableMap;
using coherence::util::ValueManipulator;


/**
* The NumberMultiplier entry processor is used to multiply a property value
* of a Number type.  Supported types are: Byte, Short, Integer, Long,
* Float, Double, BigInteger and BigDecimal.
*
* @author tb  2008.04.28
*/
class COH_EXPORT NumberMultiplier
    : public class_spec<NumberMultiplier,
        extends<PropertyProcessor> >
    {
    friend class factory<NumberMultiplier>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an NumberMultiplier processor.
        */
        NumberMultiplier();

        /**
        * Construct an NumberMultiplier processor that will increment a
        * property value by a specified amount, returning either the old or
        * the new value as specified. The type of the vNumFactor parameter
        * will dictate the type of the original and the new value.
        *
        * @param vManipulator       the ValueManipulator; could be null
        * @param vNumFactor         the Number representing the magnitude and
        *                           sign of the increment
        * @param fPostFactor  pass  true to return the value as it was
        *                           before it was incremented, or pass false
        *                           to return the value as it is after it is
        *                           incremented
        */
        NumberMultiplier(ValueManipulator::View vManipulator,
                Number::View vNumFactor, bool fPostFactor);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;


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


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number to multiply by.
        */
        FinalView<Number> f_vNumFactor;

        /**
        * Whether to return the value before it was multiplied
        * ("post-factor") or after it is multiplied ("pre-factor").
        */
        bool m_fPostFactor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_NUMBER_MULTIPLIER_HPP
