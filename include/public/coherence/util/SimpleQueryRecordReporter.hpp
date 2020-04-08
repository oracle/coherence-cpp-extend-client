/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_QUERY_RECORD_REPORTER_HPP
#define COH_SIMPLE_QUERY_RECORD_REPORTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/QueryRecord.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Simple query record reporter used to obtain a string representation of
* {@link QueryRecord} object.
*
* @since Coherence 3.7.1
*
* @author tb 2011.05.26
*/
class COH_EXPORT SimpleQueryRecordReporter
    : public class_spec<SimpleQueryRecordReporter,
        extends<Object> >
    {
    friend class factory<SimpleQueryRecordReporter>;

    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Return a report for the given query record.
        *
        * @param record  the record
        *
        * @return  a report for the given query record
        */
        static String::View report(QueryRecord::View vRecord);


    protected:
        /**
        * Report the given result.
        *
        * @param vResult            the result
        * @param type               the record type
        * @param hListIndexLookups  the list of lookup ids
        * @param fReportPartition   indicates whether or not to report
        *                           partitions
        *
        * @return a report for the given result
        */
        static String::View reportResult(
                QueryRecord::PartialResult::View vResult,
                QueryRecorder::RecordType type,
                List::Handle hListIndexLookups,
                bool fReportPartition);

        /**
        * Report the index look ups.
        *
        * @param vListIndexLookups  the list of lookup ids
        *
        * @return a report for the index look ups
        */
        static String::View reportIndexLookUps(List::View vListIndexLookups);

        /**
        * Report the given step.
        *
        * @param vStep              the step
        * @param type               the record type
        * @param hListIndexLookups  the list of lookup ids
        * @param nLevel             the indent level
        *
        * @return a report line for the given step
        */
        static String::View reportStep(
                QueryRecord::PartialResult::Step::View vStep,
                QueryRecorder::RecordType type,
                List::Handle hListIndexLookups,
                int32_t nLevel);

        /**
        * Report the given index lookup record with the given id.
        *
        * @param nIndexLookupId  the index lookup id
        * @param vRecord         the index lookup record
        *
        * @return a report line for the given index lookup
        */
        static String::View reportIndexLookupRecord(int32_t nIndexLookupId,
                QueryRecord::PartialResult::IndexLookupRecord::View vRecord);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_QUERY_RECORD_REPORTER_HPP
