/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NUMBER_INCREMENTOR_HPP
#define COH_NUMBER_INCREMENTOR_HPP

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
* The NumberIncrementor entry processor is used to increment a property value
* of a Number type.
*
* @author tb  2008.04.28
*/
class COH_EXPORT NumberIncrementor
    : public class_spec<NumberIncrementor,
        extends<PropertyProcessor> >
    {
    friend class factory<NumberIncrementor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an NumberIncrementor processor.
        */
        NumberIncrementor();

        /**
        * Construct an NumberIncrementor processor that will increment a
        * property value by a specified amount, returning either the old or
        * the new value as specified. The type of the numInc parameter will
        * dictate the type of the original and the new value.
        *
        * @param vManipulator    the ValueManipulator; could be NULL
        * @param vNumInc         the Number representing the magnitude and
        *                        sign of the increment
        * @param fPostIncrement  pass true to return the value as it was
        *                        before it was incremented, or pass false to
        *                        return the value as it is after it is
        *                        incremented
        */
        NumberIncrementor(ValueManipulator::View vManipulator,
                Number::View vNumInc, bool fPostIncrement);


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


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The Integer16 value of 0.
        */
        static Number::View getInteger16Zero();

        /**
        * The Integer32 value of 0.
        */
        static Number::View getInteger32Zero();

        /**
        * The Integer64 value of 0.
        */
        static Number::View getInteger64Zero();

        /**
        * The Float32 value of 0.
        */
        static Number::View getFloat32Zero();

        /**
        * The Float64 value of 0.
        */
        static Number::View getFloat64Zero();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number to increment by.
        */
        FinalView<Number> f_vNumInc;

        /**
        * Whether to return the value before it was incremented
        * ("post-increment") or after it is incremented ("pre-increment").
        */
        bool m_fPostInc;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_NUMBER_INCREMENTOR_HPP
