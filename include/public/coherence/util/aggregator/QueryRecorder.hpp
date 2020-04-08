/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_QUERY_RECORDER_HPP
#define COH_QUERY_RECORDER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::InvocableMap;


/**
* This parallel aggregator used to produce a QueryRecord object that contains
* an estimated or actual cost of the query execution for a given filter.
*
* For example, the following code will return a QueryPlan, containing the
* estimated query cost and corresponding execution steps.
*
* <pre>
*   QueryRecorder::Handle hAgent  = QueryRecorder::create(QueryRecorder::explain);
*   QueryRecord::View     vRecord = cast<QueryRecord::View>(hCache->aggregate(filter, agent));
* </pre>
*
* @since Coherence 3.7.1
*
* @author tb 2011.05.26
*/
class COH_EXPORT QueryRecorder
    : public class_spec<QueryRecorder,
        extends<Object>,
        implements<PortableObject, InvocableMap::ParallelAwareAggregator> >
    {
    friend class factory<QueryRecorder>;

    // ----- RecordType enum ------------------------------------------------

    public:
        /**
        * RecordType enum specifies whether the {@link QueryRecorder} should
        * be used to produce a QueryRecord object that contains an estimated
        * or an actual cost of the query execution.
        */
        enum RecordType
            {
            /**
             * Produce a QueryRecord object that contains an estimated cost of
             * the query execution.
             */
            explain,

            /**
             * Produce a QueryRecord object that contains the actual cost of
             * the query execution.
             */
            trace
            };


    // ----- Constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        QueryRecorder();

        /**
        * Construct a QueryRecorder.
        *
        * @param type  the type for this aggregator
        */
        QueryRecorder(QueryRecorder::RecordType type);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Get the record type for this query recorder.
        *
        * @return the record type enum
        */
        virtual QueryRecorder::RecordType getType() const;


    // ----- EntryAggregator interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Set::View vSetEntries);


    // ----- ParallelAwareAggregator interface ------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual InvocableMap::EntryAggregator::Handle getParallelAggregator();

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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * This aggregator record type.
        */
        enum RecordType m_type;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_QUERY_RECORDER_HPP
