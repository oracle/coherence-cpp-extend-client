/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_QUERY_RECORD_HPP
#define COH_QUERY_RECORD_HPP

#include "coherence/lang.ns"

#include "coherence/net/partition/PartitionSet.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/aggregator/QueryRecorder.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::net::partition::PartitionSet;
using coherence::util::aggregator::QueryRecorder;

/**
* The QueryRecord object carries a record of the estimated or actual
* execution cost for a query operation.
*
* @since Coherence 3.7.1
*
* @author tb 2011.05.26
*/
class COH_EXPORT QueryRecord
    : public interface_spec<QueryRecord>
    {
    // ----- QueryRecord interface ------------------------------------------

    public:
        /**
        * Get the {@link RecordType type} that was specified when this query
        * record was created.
        *
        * @return the record type
        */
        virtual QueryRecorder::RecordType getType() const = 0;

        /**
        * Get the list of partial results for this query record.
        *
        * @return the list of results
        */
        virtual List::View getResults() const = 0;

        // ----- inner interface: PartialResult -------------------------

        /**
         * A QueryPlan.PartialResult is a partial query record that contains
         * recorded costs for a query operation.  Partial results are collected
         * in a query record by a {@link QueryRecorder}.
         */
        class COH_EXPORT PartialResult
            : public interface_spec<PartialResult>
            {
            public:

                /**
                * Get the list of steps for this query record partial result in the
                * order that they occurred.
                *
                * @return the list of steps
                */
                virtual List::View getSteps() const = 0;

                /**
                * Get the set of partitions associated with this partial result.
                *
                * @return the partition set
                */
                virtual PartitionSet::Handle getPartitions() = 0;

                /**
                * Get the set of partitions associated with this partial result.
                *
                * @return the partition set
                */
                virtual PartitionSet::View getPartitions() const = 0;

                // -----inner interface: Step ---------------------------

                /**
                * A QueryPlan.Step carries the recorded cost of evaluating a filter
                * as part of a query operation.  This cost may be the estimated or
                * actual execution cost depending on the
                * {@link QueryRecorder.RecordType type} of the
                * {@link QueryRecorder recorder} in use when the step was created.
                */
                class COH_EXPORT Step
                    : public interface_spec<Step>
                    {
                    public:

                        /**
                        * Get a description of the filter that was associated with this
                        * step during its creation.
                        *
                        * @return the description of the filter
                        */
                        virtual String::View getFilterDescription() const = 0;

                        /**
                        * Get the recorded information about the index lookups performed
                        * during filter evaluation as part of a query record.
                        *
                        * @return a set of {@link IndexLookupRecord}
                        */
                        virtual Set::View getIndexLookupRecords() const = 0;

                        /**
                        * Get the calculated cost of applying the filter as defined by
                        * {@link IndexAwareFilter#calculateEffectiveness(Map, Set)
                        * calculateEffectiveness}
                        *
                        * @return an effectiveness estimate of how well the associated
                        *         filter can use any applicable index
                        */
                        virtual size32_t getEfficiency() const = 0;

                        /**
                        * Get the size of the key set prior to evaluating the filter or
                        * applying an index.  This value can be used together with
                        * {@link #getPostFilterKeySetSize()} to calculate an actual
                        * effectiveness (reduction of the key set) for this filter step.
                        *
                        * @return the size of the key set prior to evaluating the filter
                        *         or applying an index
                        */
                        virtual size32_t getPreFilterKeySetSize() const = 0;

                        /**
                        * Get the size of the key set remaining after evaluating the
                        * filter or applying an index.  This value can be used together
                        * with {@link #getPreFilterKeySetSize()} to calculate an actual
                        * effectiveness (reduction of the key set) for this filter step.
                        *
                        * @return the size of the key set after evaluating the filter
                        *         or applying an index
                        */
                        virtual size32_t getPostFilterKeySetSize() const = 0;

                        /**
                        * Get the amount of time (in ms) spent evaluating the filter or
                        * applying an index for this query plan step.
                        *
                        * @return the number of milliseconds spent evaluating the filter
                        */
                        virtual int64_t getDuration() const = 0;

                        /**
                        * Return inner nested steps, may be null if not nested.
                        *
                        * @return the inner nested steps in the order they are applied
                        */
                        virtual List::View getSteps() const = 0;
                    };

                // ----- inner interface: IndexLookupRecord -------------

                /**
                * An IndexLookupRecord holds the recorded information about an index
                * lookup performed during filter evaluation as part of a query
                * record.
                *
                * An IndexLookupRecord is created each time that
                * {@link RecordableStep#recordExtractor(ValueExtractor)} is called on
                * a query record step.
                */
                class COH_EXPORT IndexLookupRecord
                    : public interface_spec<IndexLookupRecord>
                    {
                    public:
                        /**
                        * Get a description of the extractor that was used for the index
                        * lookup.
                        *
                        * @return the extractor description
                        */
                        virtual String::View getExtractorDescription() const = 0;

                        /**
                        * Get a description of the associated index.
                        *
                        * @return the index description; null if no index was found
                        *         for the associated extractor
                        */
                        virtual String::View getIndexDescription() const = 0;

                        /**
                        * Indicates whether or not the associated index is ordered.
                        *
                        * @return true if the associated index is ordered; false if the
                        *         index is not ordered or if no index was found for the
                        *         associated extractor
                        */
                        virtual bool isOrdered() const = 0;
                    };
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_QUERY_RECORD_HPP
