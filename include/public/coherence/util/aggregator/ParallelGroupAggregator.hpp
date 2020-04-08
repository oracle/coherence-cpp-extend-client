/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PARALLEL_GROUP_AGGREGATOR_HPP
#define COH_PARALLEL_GROUP_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/GroupAggregator.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* The ParallelGroupAggregator provides an ability to split a subset of
* entries in an InvocableMap into a collection of non-intersecting subsets
* and then aggregate them separately and independently. The splitting
* (grouping) is performed using the results of the underlying ValueExtractor
* in such a way that two entries will belong to the same group if and only if
* the result of the corresponding ValueExtractor#extract extract call
* produces the same value or tuple (list of values). After the entries are
* split into the groups, the underlying aggregator is applied separately to
* each group. The result of the aggregation by the GroupAggregator is a Map
* that has distinct values (or tuples) as keys and results of the individual
* aggregation as values. Additionally, those results could be further reduced
* using an optional Filter object.
*
* Informally speaking, this aggregator is analogous to the SQL "group by" and
* "having" clauses. Note that the "having" Filter is applied independently on
* each server against the partial aggregation results; this generally implies
* that data affinity is required to ensure that all required data used to
* generate a given result exists within a single cache partition.
* In other words, the "group by" predicate should not span multiple
* partitions if the "having" clause is used.
*
* The ParallelGroupAggregator is somewhat similar to the DistinctValues
* aggregator, which returns back a list of distinct values (tuples) without
* performing any additional aggregation work.
*
* Unlike many other concrete EntryAggregator implementations that are
* constructed directly, instances of ParallelGroupAggregator should only be
* created indirectly by using the factory method of GroupAggregator.
*
* @deprecated as of Coherence 12.2.1
*
* @author djl  2008.05.16
*/

class COH_EXPORT ParallelGroupAggregator
    : public class_spec<ParallelGroupAggregator,
        extends<GroupAggregator>,
        implements<InvocableMap::ParallelAwareAggregator> >
    {
    friend class factory<ParallelGroupAggregator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject
        * interface).
        */
        ParallelGroupAggregator();

        /**
        * Create an instance of GroupAggregator based on a specified
        * extractor and anInvocableMap::EntryAggregator
        * and a result evaluation filter.
        * <br>
        * If the specified aggregator is an instance of
        * coherence::util::InvocableMap::ParallelAwareAggregator, then a
        * parallel-aware instance of the GroupAggregator will be created.
        * Otherwise, the resulting GroupAggregator will not be
        * parallel-aware and could be ill-suited for aggregations run
        * against large partitioned caches.
        *
        * @param vExtractor   a ValueExtractor that will be used to
        *                     split a set of InvocableMap entries into
        *                     distinct
        *                     groups
        * @param hAggregator  an underlying EntryAggregator
        * @param vFilter      an optional Filter object used to filter
        *                     out results of individual group
        *                     aggregation results
        */
        ParallelGroupAggregator(ValueExtractor::View vExtractor,
                InvocableMap::ParallelAwareAggregator::Handle hAggregator,
                Filter::View vFilter);

    private:
        /**
        * Blocked copy constructor.
        */
        ParallelGroupAggregator(const ParallelGroupAggregator&);


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

#endif // COH_PARALLEL_GROUP_AGGREGATOR_HPP
