/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DISTINCT_VALUES_HPP
#define COH_DISTINCT_VALUES_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/AbstractAggregator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* Return the set of unique values extracted from a set of entries in a Map.
* If the set of entries is empty, an empty set is returned.
*
* This aggregator could be used in combination with
* coherence::util::extractor::MultiExtractor allowing
* to collect all unique combinations (tuples) of a given set of attributes.
*
* The DistinctValues aggregator covers a simple case of a more generic
* aggregation pattern implemented by the GroupAggregator, which in
* addition to collecting all distinct values or tuples, runs an aggregation
* against each distinct entry set (group).
*
* @author djl  2008.05.12
*/
class COH_EXPORT DistinctValues
    : public class_spec<DistinctValues,
        extends<AbstractAggregator> >
    {
    friend class factory<DistinctValues>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        DistinctValues();

        /**
        * Construct a DistinceValues aggregator.
        *
        * @param vExtractor  the extractor that provides a value in the form
        *                    of any object
        */
        DistinctValues(ValueExtractor::View vExtractor);


    // ----- AbstractAggregator Interface  ----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void init(bool fFinal);

        /**
        * {@inheritDoc}
        */
        virtual void process(Object::View vO, bool fFinal);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder finalizeResult(bool fFinal);


    // ----- internal helpers -----------------------------------------------

    protected:
        /**
        * Return a set that can be used to store distinct values, creating it
        * if one has not already been created.
        *
        * @return a set that can be used to store distinct values
        */
        virtual Set::Handle ensureSet();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The resulting set of distinct values.
        */
        MemberHandle<Set> m_hSet;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_DISTINCT_VALUES_HPP
