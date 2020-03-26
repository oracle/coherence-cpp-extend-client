/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TOP_N_AGGREGATOR_HPP
#define COH_TOP_N_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/SortedBag.hpp"
#include "coherence/util/ValueExtractor.hpp"


COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* TopNAggregator is a ParallelAwareAggregator that aggregates the top <i>N</i>
* extracted values into an array.  The extracted values must not be null, but
* do not need to be unique.
*
* @author rhl 2013.04.24
*/
class COH_EXPORT TopNAggregator
    : public class_spec<TopNAggregator,
        extends<Object>,
        implements<PortableObject, InvocableMap::ParallelAwareAggregator,
                   InvocableMap::ParallelAwareAggregator::PartialResultAggregator> >
    {
    friend class factory<TopNAggregator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        TopNAggregator();

        /**
        * Construct a TopNAggregator that will aggregate the top extracted values,
        * as determined by the specified comparator.
        *
        * @param vExtractor   the extractor
        * @param vComparator  the comparator for extracted values
        * @param cResults     the maximum number of results to return
        */
        TopNAggregator(ValueExtractor::View vExtractor, Comparator::View vComparator, int32_t cResults);

    private:
        /**
        * Blocked copy constructor.
        */
        TopNAggregator(const TopNAggregator&);


    // ----- inner class: PartialResult -------------------------------------

    public:
        /**
        * The sorted partial result.
        */
        class COH_EXPORT PartialResult
            : public class_spec<PartialResult,
                extends<SortedBag>,
                implements<PortableObject> >
        {
        friend class factory<PartialResult>;

        // ----- constructors -----------------------------------------------

        protected:
            /**
            * @internal
            */
           PartialResult();

            /**
            * @internal
            */
            PartialResult(Comparator::View vComparator);

        private:
            /**
            * Blocked copy constructor.
            */
            PartialResult(const PartialResult& that);


        // ----- PortableObject methods -------------------------------------

        public:
            /**
            * {@inheritDoc}
            */
            virtual void readExternal(PofReader::Handle hIn);

            /**
            * {@inheritDoc}
            */
            virtual void writeExternal(PofWriter::Handle hOut) const;
        };


    // ----- ParallelAwareAggregator Interface  -----------------------------

    public:
        /**
        * {@inheritDoc}
        */
        InvocableMap::EntryAggregator::Handle getParallelAggregator();

        /**
        * {@inheritDoc}
        */
        Object::Holder aggregateResults(Collection::View vColResults);

        /**
        * {@inheritDoc}
        */
        Object::Holder aggregate(Set::View vSetEntries);



    // ----- PartialResultAggregator methods --------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        Object::Holder aggregatePartialResults(Collection::View vColPartialResults);



    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Add the specified values to the result if they are within the top-N.
        *
        * @param vIterValues  the iterator of values to add
        * @param hResult      the result
        */
        void addToResult(Iterator::Handle hIterValues, PartialResult::Handle hResult);

        /**
        * Finalize the partial aggregation result.
        *
        * @param result  the partial result
        *
        * @return the final aggregation result
        */
        ObjectArray::Handle finalizeResult(PartialResult::View vResult);


    // ----- PortableObject methods -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- data members ---------------------------------------------------

    public:
        /**
        * True iff this aggregator is to be used in parallel.
        */
        bool m_fParallel;

        /**
        * The ValueExtractor used by this aggregator.
        */
        FinalView<ValueExtractor> f_vExtractor;

        /**
        * The Comparator used to order the extracted values.
        */
        FinalView<Comparator> f_vComparator;

        /**
        * The maximum number of results to include in the aggregation result.
        */
        int32_t m_cResults;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_TOP_N_AGGREGATOR_HPP
