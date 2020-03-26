/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GROUP_AGGREGATOR_HPP
#define COH_GROUP_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* The GroupAggregator provides an ability to split a subset of entries in an
* InvocableMap into a collection of non-intersecting subsets and then
* aggregate them separately and independently. The splitting (grouping) is
* performed using the results of the underlying ValueExtractor in such a way
* that two entries will belong to the same group if and only if the result of
* the corresponding ValueExtractor#extract extract call produces the
* same value or tuple (list of values). After the entries are split into the
* groups, the underlying aggregator is applied separately to each group. The
* result of the aggregation by the GroupAggregator is a Map that has distinct
* values (or tuples) as keys and results of the individual aggregation as
* values. Additionally, those results could be further reduced using an
* optional Filter object.
*
* Informally speaking, this aggregator is analogous to the SQL "group by" and
* "having" clauses. Note that the "having" Filter is applied independently on
* each server against the partial aggregation results; this generally implies
* that data affinity is required to ensure that all required data used to
* generate a given result exists within a single cache partition.
* In other words, the "group by" predicate should not span multiple
* partitions if the "having" clause is used.
*
* The GroupAggregator is somewhat similar to the DistinctValues
* aggregator, which returns back a list of distinct values (tuples) without
* performing any additional aggregation work.
*
* <b>Unlike many other concrete EntryAggregator implementations that are
* constructed directly, instances of GroupAggregator should only be created
* using one of the factory methods:</b>
* #create(ValueExtractor, InvocableMap::EntryAggregator)
* #create(vExtractor, hAggregator),
* #create(ValueExtractor, InvocableMap::EntryAggregator, Filter),
*
* @author djl  2008.05.16
*/
class COH_EXPORT GroupAggregator
    : public class_spec<GroupAggregator,
        extends<Object>,
        implements<PortableObject, InvocableMap::EntryAggregator> >
    {
    friend class factory<GroupAggregator>;

    // ----- factory methods ------------------------------------------------

    public:
        /**
         * Default constructor (necessary for the PortableObject interface).
         */
         static GroupAggregator::Handle create();

         /**
         * Create an instance of GroupAggregator based on a specified
         * extractor and an coherence::util::InvocableMap::EntryAggregator
         * and a result evaluation filter.
         *
         * @param vExtractor   a ValueExtractor that will be used to split a
         *                     set of InvocableMap entries into distinct
         *                     groups
         * @param hAggregator  an underlying EntryAggregator
         * @param vFilter      an optional Filter object used to filter out
         *                     results of individual group aggregation
         *                     results
         */
         static GroupAggregator::Handle create(
                 ValueExtractor::View vExtractor,
                 InvocableMap::EntryAggregator::Handle hAggregator,
                 Filter::View vFilter = NULL);


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        GroupAggregator();

        /**
        * @internal
        */
        GroupAggregator(ValueExtractor::View vExtractor,
                InvocableMap::EntryAggregator::Handle hAggregator,
                Filter::View vFilter);

    private:
        /**
        * Blocked copy constructor.
        */
        GroupAggregator(const GroupAggregator&);


    // ----- InvocableMap::EntryAggregator interface ------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Set::View vSetEntries);


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

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the underlying ValueExtractor.
        *
        * @return the underlying ValueExtractor
        */
        virtual ValueExtractor::View getExtractor() const;

        /**
        * Obtain the underlying EntryAggregator.
        *
        * @return the underlying EntryAggregator
        */
        virtual  InvocableMap::EntryAggregator::Handle getAggregator();

        /**
        * Obtain the underlying EntryAggregator.
        *
        * @return the underlying EntryAggregator
        */
        virtual  InvocableMap::EntryAggregator::View getAggregator() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying ValueExtractor.
        */
        FinalView<ValueExtractor> f_vExtractor;

        /**
        * The underlying EntryAggregator.
        */
        FinalHandle<InvocableMap::EntryAggregator> f_hAggregator;

        /**
        * The underlying Filter.
        */
        FinalView<Filter> f_vFilter;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_GROUP_AGGREGATOR_HPP
