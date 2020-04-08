/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CHAINED_COMPARATOR_HPP
#define COH_CHAINED_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/QueryMap.hpp"
#include "coherence/util/comparator/EntryAwareComparator.hpp"
#include "coherence/util/comparator/QueryMapComparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
* Composite comparator implementation based on a collection of comparators.
* The comparators in the array assumed to be sorted according to their
* priorities; only in a case when the n-th comparator cannot determine the
* order of the passed objects:
* <pre>
*   vaComparator[n]->compare(o1, o2) == 0
* </pre>
* the (n+1)-th comparator will be applied to calculate the value.
*
* @author gm  2008.08.04
*/
class COH_EXPORT ChainedComparator
    : public class_spec<ChainedComparator,
        extends<Object>,
        implements<Comparator,
                   QueryMapComparator,
                   EntryAwareComparator,
                   PortableObject> >
    {
    friend class factory<ChainedComparator>;

    // ----- constructors/destructor ----------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ChainedComparator();

        /**
        * Construct a ChainedComparator.
        *
        * @param vaComparator  the comparator array
        */
        ChainedComparator(ObjectArray::View vaComparator);


    // ----- Comparator interface -------------------------------------------

    public:
        /**
        * Compares its two arguments for order.  Returns a negative integer,
        * zero, or a positive integer as the first argument is less than, equal
        * to, or greater than the second.
        *
        * @param vO1  the first object to be compared
        * @param vO2  the second object to be compared
        *
        * @return a negative integer, zero, or a positive integer as the first
        *         argument is less than, equal to, or greater than the second
        *
        * @throws ClassCastException if the arguments' types prevent them from
        *          being compared by this Comparator.
        */
        virtual int32_t compare(Object::View vO1, Object::View vO2) const;


    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
        * Compare two entries using the underlying comparator.This implementation 
        * simply passes on this invocation to the wrapped
        * Comparator objects if they too implement this interface, or invokes
        * their default compare method passing the values extracted from the
        * passed entries.
        */
        virtual int32_t compareEntries(QueryMap::Entry::View vEntry1,
                    QueryMap::Entry::View vEntry2) const;


    // ----- EntryAwareComparator interface ---------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isKeyComparator() const;

        /**
        * Check whether the specified comparator expects to compare keys
        * or values.
        *
        * @param vComparator  a Comparator to check
        *
        * @return true if the comparator expects keys; false otherwise
        */
        static bool isKeyComparator(Comparator::View vComparator);


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

    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the underlying Comparator array.
        *
        * @return the Comparator array
        */
        virtual ObjectArray::View getComparators() const;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * An instance of the ChainedComparator.
        */
        static ChainedComparator::Handle getInstance();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The wrapped Comparator array. Could be null.
        */
        FinalView<ObjectArray> f_vaComparator;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CHAINED_COMPARATOR_HPP
