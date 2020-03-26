/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PARALLEL_COMPOSITE_AGGREGATOR_HPP
#define COH_PARALLEL_COMPOSITE_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/CompositeAggregator.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* ParallelCompositeAggregator provides an ability to execute a collection of
* parallel aware aggregators against the same subset of the entries in an
* InvocableMap, resulting in a list of corresponding aggregation results. The
* size of the returned list will always be equal to the length of the
* aggregators' array.
*
* Unlike many other concrete EntryAggregator implementations that are
* constructed directly, instances of ParallelCompositeAggregator should only
* becreated indirectly by using the factory method of CompositeAggregator.
*
* @deprecated as of Coherence 12.2.1
*
* @author djl  2008.05.16
*/
class COH_EXPORT ParallelCompositeAggregator
    : public class_spec<ParallelCompositeAggregator,
        extends<CompositeAggregator>,
        implements<InvocableMap::ParallelAwareAggregator> >
    {
    friend class factory<ParallelCompositeAggregator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject
        * interface).
        */
        ParallelCompositeAggregator();

        /**
        * Construct a ParallelCompositeAggregator based on a
        * specified
        * coherence::util::InvocableMap::EntryAggregator array.
        *
        * @param vaAggregator  an array of ParallelAwareAggregator
        *                      objects; may not be null
        */
        ParallelCompositeAggregator(ObjectArray::View vaAggregator);

    private:
        /**
        * Blocked copy constructor.
        */
        ParallelCompositeAggregator(const ParallelCompositeAggregator&);


    // ----- InvocableMap::ParallelAwareAggregator interface ----------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual InvocableMap::EntryAggregator::Handle
                getParallelAggregator();

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregateResults(
                Collection::View vCollResults);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PARALLEL_COMPOSITE_AGGREGATOR_HPP
