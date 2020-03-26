/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_QUERY_RECORD_HPP
#define COH_SIMPLE_QUERY_RECORD_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/QueryRecord.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Simple QueryRecord implementation.
*
* @since Coherence 3.7.1
*
* @author tb 2011.05.26
*/
class COH_EXPORT SimpleQueryRecord
    : public class_spec<SimpleQueryRecord,
        extends<Object>,
        implements<PortableObject, QueryRecord> >
    {
    friend class factory<SimpleQueryRecord>;

    // ----- Constructors ---------------------------------------------------

    protected:
        /**
        * Construct a SimpleQueryRecord.
        */
        SimpleQueryRecord();

        /**
        * Construct a SimpleQueryRecord from the given collection of partial
        * results.
        *
        * @param type         the record type
        * @param vColResults  the collection of partial results
        */
        SimpleQueryRecord(QueryRecorder::RecordType type,
                Collection::View vColResults);


    // ----- QueryRecord interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual QueryRecorder::RecordType getType() const;

        /**
        * {@inheritDoc}
        */
        virtual List::View getResults() const;


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Merge the partial results from the associated record.  Matching
        * partial results are merged into a single result for the report.
        *
        * @param vColResults  the collection of partial results
        */
        virtual void mergeResults(Collection::View vColResults);


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


    // ----- inner class: PartialResult -------------------------------------

    public:
        /**
        * Simple QueryRecord::PartialResult implementation.
        */
        class COH_EXPORT PartialResult
            : public class_spec<PartialResult,
                extends<Object>,
                implements<PortableObject, QueryRecord::PartialResult> >
            {
            friend class factory<PartialResult>;

            // ----- handle definitions (needed for nested classes) -----

             public:
                 typedef this_spec::Handle Handle;
                 typedef this_spec::View   View;
                 typedef this_spec::Holder Holder;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a PartialResult
                */
                PartialResult();

                /**
                * Copy constructor for a Result.
                *
                * @param vThat  the result to copy
                */
                PartialResult(QueryRecord::PartialResult::View vThat);

            // ----- PartialResult interface ----------------------------

            public:

                /**
                * {@inheritDoc}
                */
                virtual List::View getSteps() const;

                /**
                * {@inheritDoc}
                */
                virtual PartitionSet::Handle getPartitions();

                /**
                * {@inheritDoc}
                */
                virtual PartitionSet::View getPartitions() const;


            // ----- helper methods -------------------------------------

            public:
                /**
                * Merge the given result with this one.
                *
                * @param vResult  the result to merge
                */
                virtual void merge(QueryRecord::PartialResult::View vResult);

                /**
                * Determine whether or not the given result is capable of
                * being placed in one-to-one correspondence with this result.
                * Results are matching if their owned lists of steps have the
                * same size, and all pairs of steps in the two lists are
                * matching.
                *
                * @param vResult  the result to be checked
                *
                * @return true iff the given result matches with this result
                */
                virtual bool isMatching(
                        QueryRecord::PartialResult::View vResult) const;

            // ----- PortableObject interface ---------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void readExternal(PofReader::Handle hIn);

                /**
                * {@inheritDoc}
                */
                virtual void writeExternal(PofWriter::Handle hOut) const;

                // ----- inner class: Step ------------------------------

                /**
                * Simple QueryRecord::PartialResult::Step implementation.
                */
                class COH_EXPORT Step
                    : public class_spec<Step,
                        extends<Object>,
                        implements<PortableObject,
                                QueryRecord::PartialResult::Step> >
                    {
                    friend class factory<Step>;

                    // ----- handle definitions (needed for nested classes)

                     public:
                         typedef this_spec::Handle Handle;
                         typedef this_spec::View   View;
                         typedef this_spec::Holder Holder;

                    // ----- constructors -------------------------------

                    protected:
                        /**
                        * Constructor for a Step.
                        */
                        Step();

                        /**
                        * Copy constructor for a Step.
                        *
                        * @param vThat  the step to copy
                        */
                        Step(QueryRecord::PartialResult::Step::View vThat);

                    // ----- Step interface -----------------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual String::View getFilterDescription() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual Set::View getIndexLookupRecords() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual size32_t getEfficiency() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual size32_t getPreFilterKeySetSize() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual size32_t getPostFilterKeySetSize() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual int64_t getDuration() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual List::View getSteps() const;

                    // ----- helper methods -----------------------------

                    public:
                        /**
                        * Merge the given step with this one.  This method
                        * assumes that the given step matches with this one.
                        *
                        * @param vStep  the step to merge
                        */
                        void merge(QueryRecord::PartialResult::Step::View
                                vStep);

                        /**
                        * Determine whether or not the given step is capable
                        * of being placed in one-to-one correspondence with
                        * this step.  Steps are defined to be matching if both
                        * steps have equivalent name, index lookup records and
                        * owned lists of sub-steps.
                        *
                        * @param vStep  the step to check
                        *
                        * @return true iff the given step matches with this
                        *         step
                        */
                        bool isMatching(QueryRecord::PartialResult::Step::View
                                vStep) const;

                    // ----- PortableObject interface -------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual void readExternal(PofReader::Handle hIn);

                        /**
                        * {@inheritDoc}
                        */
                        virtual void writeExternal(PofWriter::Handle hOut)
                                const;

                    // ----- data members -------------------------------

                    protected:
                        /**
                        * The filter description.
                        */
                        FinalView<String> f_vsFilter;

                        /**
                        * The estimated cost.
                        */
                        size32_t m_nEfficiency;

                        /**
                        * The pre-execution key set size.
                        */
                        size32_t m_nSizeIn;

                        /**
                        * The post-execution key set size.
                        */
                        size32_t m_nSizeOut;

                        /**
                        * The execution time in milliseconds.
                        */
                        int64_t m_cMillis;

                        /**
                        * The set of index lookup records.
                        */
                        FinalHandle<Set> f_hSetIndexLookupRecords;

                        /**
                        * The list of child steps.
                        */
                        FinalHandle<List> f_hListSubSteps;
                    };

                // ----- inner class: IndexLookupRecord -----------------

                /**
                * Simple QueryRecord::PartialResult::IndexLookupRecord
                * implementation.
                */
                class COH_EXPORT IndexLookupRecord
                    : public class_spec<IndexLookupRecord,
                        extends<Object>,
                        implements<PortableObject,
                                QueryRecord::PartialResult::IndexLookupRecord> >
                    {
                    friend class factory<IndexLookupRecord>;
                    friend class Step;

                    // ----- handle definitions (needed for nested classes)

                     public:
                         typedef this_spec::Handle Handle;
                         typedef this_spec::View   View;
                         typedef this_spec::Holder Holder;

                    // ----- constructors -------------------------------

                    protected:
                        /**
                        * Construct an IndexLookupRecord.
                        */
                        IndexLookupRecord();

                        /**
                        * Copy constructor for an IndexLookupRecord.
                        *
                        * @param vThat  the IndexLookupRecord to copy
                        */
                        IndexLookupRecord(
                                QueryRecord::PartialResult::IndexLookupRecord::View
                                vThat);

                        /**
                        * Construct an IndexLookupRecord.
                        *
                        * @param vsExtractor  the extractor description
                        * @param vsIndex      the index description
                        * @param fOrdered     indicates whether or not the
                        *                     associated index is ordered
                        */
                        IndexLookupRecord(String::View vsExtractor,
                                String::View vsIndex,
                                bool fOrdered,
                                int64_t cBytes,
                                int32_t cDistinctValues,
                                String::View vsIndexDef);

                    // ----- IndexLookupRecord interface ----------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual String::View getExtractorDescription() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual String::View getIndexDescription() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool isOrdered() const;

                        /**
                        * Returns index memory usage in bytes.
                        *
                        * @return index memory usage in bytes; -1 if there is no index
                        */
                        int64_t getMemoryUsage() const;

                        /**
                        * Return index content map size.
                        *
                        * @return index content map size; -1 if there is no index
                        */
                        int32_t getSize() const;

                        /**
                        * Returns the index definition.
                        *
                        * @return the index definition; null if there is no index
                        */
                        virtual String::View getIndexDef() const;

                    // ----- PortableObject interface -------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual void readExternal(PofReader::Handle hIn);

                        /**
                        * {@inheritDoc}
                        */
                        virtual void writeExternal(PofWriter::Handle hOut)
                                const;

                    // ----- Object interface ---------------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual bool equals(Object::View v) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual size32_t hashCode() const;

                    // ----- helper methods ------------------------------

                    protected:
                        /**
                        * Build an index description for this index.
                        *
                        * @return an index description for this index if there is an index definition;
                        *         null otherwise
                        */
                        virtual String::View buildIndexDescription();

                        /**
                        * Parses an index description into it's definition, footprint,
                        * and map size.
                        *
                        * @param vsIndex  the index description
                        */
                        virtual void parseIndexDescription(String::View vsIndex);

                    // ----- data members --------------------------------

                    protected:
                        /**
                        * The extractor description.
                        */
                        FinalView<String> f_vsExtractor;

                        /**
                        * The index description.
                        */
                        MemberView<String> m_vsIndex;

                        /**
                        * Indicates whether or not the associated index is
                        * ordered.
                        */
                        bool m_fOrdered;

                        /**
                        * The index footprint in bytes.
                        */
                        int64_t m_cBytes;

                        /**
                        * The index content map size.
                        */
                        int32_t m_cDistinctValues;

                        /**
                        * The index type description.
                        */
                        MemberView<String> m_vsIndexDef;
                    };

                // ----- data members ----------------------------------------

                protected:
                    /**
                    * The map of steps.
                    */
                    FinalHandle<List> f_hListSteps;

                    /**
                    * The partitions.
                    */
                    FinalHandle<PartitionSet> f_hPartMask;
            };


    // ----- data members ----------------------------------------------------

    protected:
        /**
        * This record type.
        */
        enum QueryRecorder::RecordType m_type;

        /**
        * The list of partial results.
        */
        FinalHandle<List> f_hListResults;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_QUERY_RECORD_HPP
