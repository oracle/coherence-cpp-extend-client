/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVERSE_COMPARATOR_HPP
#define COH_INVERSE_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/QueryMap.hpp"
#include "coherence/util/comparator/QueryMapComparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::comparator::SafeComparator;

/**
* Comparator that reverses the result of another comparator.
*
* @author gm  2008.08.05
*/
class COH_EXPORT InverseComparator
    : public class_spec<InverseComparator,
        extends<SafeComparator>,
        implements<QueryMapComparator> >
    {
    friend class factory<InverseComparator>;

    // ----- constructors/destructor ----------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        InverseComparator();

        /**
        * Construct a InverseComparator.
        *
        * @param vComparator  the comparator whose results are inverted by
        *                     this Comparator
        */
        InverseComparator(Comparator::View vComparator);

    private:
        /**
        * Blocked copy constructor.
        */
        InverseComparator(const InverseComparator&);


    // ----- Comparator interface -------------------------------------------

    public:
        /**
        * Use the wrapped Comparator to compare the two arguments for order and
        * negate the result.
        *
        * @param vO1  the first object to be compared
        * @param vO2  the second object to be compared
        *
        * @return a positive integer, zero, or a negative integer as the first
        *         argument is less than, equal to, or greater than the second
        *
        * @throws ClassCastException if the arguments' types prevent them from
        *         being compared by this Comparator.
        */
        virtual int32_t compare(Object::View vO1, Object::View vO2) const;

    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
        * Compare two entries using the underlying comparator and negate the
        * result.
        */
        virtual int32_t compareEntries(QueryMap::Entry::View vEntry1,
                    QueryMap::Entry::View vEntry2) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * An instance of the InverseComparator.
        */
        static InverseComparator::Handle getInstance();
    };

COH_CLOSE_NAMESPACE3

#endif // COH_INVERSE_COMPARATOR_HPP
