/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRIORITY_AGGREGATOR_HPP
#define COH_PRIORITY_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/net/AbstractPriorityTask.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::AbstractPriorityTask;


/**
* PriorityAggregator is used to explicitly control the scheduling priority
* and timeouts for execution of EntryAggregator-based methods.
*
* For example, let's assume that there is an <i>Orders</i> cache that belongs
* to a partitioned cache service configured with a <i>request-timeout</i> and
* <i>task-timeout</i> of 5 seconds.
* Also assume that we are willing to wait longer for a particular
* aggregation request that scans the entire cache. Then we could override the
* default timeout values by using the PriorityAggregator as follows:
* <pre>
*   Float64Average::Handle aggrStandard =
*           Float64Average::create(ReflectionExtractor::create("getPrice"));
*   PriorityAggregator::Handle aggrPriority =
*           PriorityAggregator::create(aggrStandard);
*   aggrPriority->setExecutionTimeoutMillis(PriorityTask::timeout_none);
*   aggrPriority->setRequestTimeoutMillis(PriorityTask::timeout_none);
*   cacheOrders->aggregate(NULL, aggrPriority);
* </pre>
*
* This is an advanced feature which should be used judiciously.
*
* @author djl  2008.05.16
*/
class COH_EXPORT PriorityAggregator
    : public class_spec<PriorityAggregator,
        extends<AbstractPriorityTask>,
        implements<InvocableMap::ParallelAwareAggregator> >
    {
    friend class factory<PriorityAggregator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        PriorityAggregator();

        /**
        * Construct a PriorityAggregator.
        *
        * @param hAggregator  the aggregator wrapped by this
        *                     PriorityAggregator
        */
        PriorityAggregator(
                InvocableMap::ParallelAwareAggregator::Handle hAggregator);


    // ----- InvocableMap::EntryAggregator interface ------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Set::View vSetEntries);

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
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the underlying aggregator.
        *
        * @return the aggregator wrapped by this PriorityAggregator
        */
        virtual InvocableMap::ParallelAwareAggregator::Handle
                getAggregator();

        /**
        * Obtain the underlying aggregator.
        *
        * @return the aggregator wrapped by this PriorityAggregator
        */
        virtual InvocableMap::ParallelAwareAggregator::View
                 getAggregator() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The wrapped aggregator.
        */
        FinalHandle<InvocableMap::ParallelAwareAggregator> f_hAggregator;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PRIORITY_AGGREGATOR_HPP
