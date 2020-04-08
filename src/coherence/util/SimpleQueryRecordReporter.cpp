/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SimpleQueryRecordReporter.hpp"

#include "coherence/util/LinkedList.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


namespace
    {
    // ----- constants ------------------------------------------------------

    /**
    * The largest possible value of type size32_t.
    */
    const size32_t npos = size32_t(-1);


    // ----- local file helpers ---------------------------------------------

    /**
    * Return the report divider string.
    *
    * @return the report divider string
    */
    static String::Handle getReportDivider()
        {
        static String::Handle hs = "======================================================================================\n";
        return hs;
        }
    COH_STATIC_INIT(getReportDivider());

    /**
    * Return the report explain header.
    *
    * @return the explain header
    */
    static String::Handle getReportExplainHeader()
        {
        static String::Handle hs = "\nExplain Plan\nName                                Index      Cost\n";
        return hs;
        }
    COH_STATIC_INIT(getReportExplainHeader());

    /**
    * Return the report trace header.
    *
    * @return the trace header
    */
    static String::Handle getReportTraceHeader()
        {
        static String::Handle hs = "\nTrace\nName                                Index      Effectiveness        Duration\n";
        return hs;
        }
    COH_STATIC_INIT(getReportTraceHeader());

    /**
    * Return the report index lookup header.
    *
    * @return the index lookup header
    */
    static String::Handle getReportIndexLookupHeader()
        {
        static String::Handle hs = "\nIndex Lookups\nIndex Description                              Extractor            Ordered\n";
        return hs;
        }
    COH_STATIC_INIT(getReportIndexLookupHeader());

    /**
    * Return the no-index string.
    *
    * @return the no-index string
    */
    static String::Handle getReportNoIndex()
        {
        static String::Handle hs = "No index found";
        return hs;
        }
    COH_STATIC_INIT(getReportNoIndex());

    /**
    * Return the report no-info string.
    *
    * @return the no-info string
    */
    static String::Handle getReportNoInfo()
        {
        static String::Handle hs = "----";
        return hs;
        }
    COH_STATIC_INIT(getReportNoInfo());

    /**
    * Pad the given string with the given fill character to make it the given
    * width.  If the given string is longer than the given width, it will be
    * truncated.
    *
    * @param vs      the string to be padded
    * @param nWidth  the width
    * @param cFill   the fill character
    *
    * @return the padded string
    */
    static String::View pad(String::View vs, int32_t nWidth, char cFill)
        {
        int32_t        nLen = vs->length();
        const char*    ach  = vs->getCString();
        String::Handle hs   = String::create("");

        for (int32_t i = 0; i < nWidth; ++i)
            {
            if (i < nLen)
                {
                hs = COH_TO_STRING(hs << ach[i]);
                }
            else
                {
                hs = COH_TO_STRING(hs << cFill);
                }
            }
           return hs;
        }
    } // end of anonymous namespace


// ----- helper methods -----------------------------------------------------

String::View SimpleQueryRecordReporter::report(QueryRecord::View vRecord)
    {
    String::Handle hs = String::create("");

    List::Handle hListIndexLookups = LinkedList::create();

    List::View vListRecords = vRecord->getResults();

    bool fReportPartition = vListRecords->size() > 1;

    for (size32_t i = 0; i < vListRecords->size(); ++i)
        {
        hs = COH_TO_STRING(hs << reportResult(
                cast<QueryRecord::PartialResult::View>(vListRecords->get(i)),
                vRecord->getType(), hListIndexLookups, fReportPartition));
        }

    hs = COH_TO_STRING(hs << reportIndexLookUps(hListIndexLookups));

    return hs;
    }

String::View SimpleQueryRecordReporter::reportResult(
        QueryRecord::PartialResult::View vResult,
        QueryRecorder::RecordType type,
        List::Handle hListIndexLookups,
        bool fReportPartition)
    {
    String::Handle hs;

    if (type == QueryRecorder::trace)
        {
        hs = getReportTraceHeader();
        }
    else
        {
        hs = getReportExplainHeader();
        }

    hs = COH_TO_STRING(hs << getReportDivider());

    for (Iterator::Handle hIter = vResult->getSteps()->iterator(); hIter->hasNext();)
        {
        hs = COH_TO_STRING(hs << reportStep(
                cast<QueryRecord::PartialResult::Step::View>(hIter->next()),
                type, hListIndexLookups, 0) << "\n");
        }

    hs = COH_TO_STRING(hs << "\n");

    if (fReportPartition)
        {
        hs = COH_TO_STRING(hs << vResult->getPartitions() << "\n\n");
        }

    return hs;
    }

String::View SimpleQueryRecordReporter::reportIndexLookUps(
        List::View vListIndexLookups)
    {
    String::Handle hs = COH_TO_STRING( getReportIndexLookupHeader() <<
            getReportDivider());

    Iterator::Handle hIter = vListIndexLookups->iterator();
    int32_t i = 0;

    while (hIter->hasNext())
        {
        hs = COH_TO_STRING(hs << reportIndexLookupRecord(i++,
                cast<QueryRecord::PartialResult::IndexLookupRecord::View>(hIter->next())) <<
                "\n");
        }

    hs = COH_TO_STRING(hs << "\n");

    return hs;
    }

String::View SimpleQueryRecordReporter::reportStep(
        QueryRecord::PartialResult::Step::View vStep,
        QueryRecorder::RecordType type,
        List::Handle hListIndexLookups,
        int32_t nLevel)
    {
        String::Handle hs = String::create("");
        for (int i = 0; i < nLevel; ++i)
            {
            hs = COH_TO_STRING(hs << "  ");
            }
        hs = COH_TO_STRING(hs << vStep->getFilterDescription());

        String::View vsIndex = String::create("");
        for (Iterator::Handle hIter = vStep->getIndexLookupRecords()->iterator();
                hIter->hasNext();)
            {
            QueryRecord::PartialResult::IndexLookupRecord::View vRecord =
                    cast<QueryRecord::PartialResult::IndexLookupRecord::View>(hIter->next());

            int32_t nIndex = hListIndexLookups->indexOf(vRecord);
            if (nIndex == -1)
                {
                nIndex = hListIndexLookups->size();

                hListIndexLookups->add(vRecord);
                }

            String::View vsSep = vsIndex->length() > 0 ? COH_TO_STRING(",") : COH_TO_STRING("");

            vsIndex = COH_TO_STRING(vsIndex << vsSep << nIndex);
            }
        vsIndex = vsIndex->length() > 0 ? COH_TO_STRING(vsIndex) : getReportNoInfo();

        if (type == QueryRecorder::trace)
            {
            String::View vsSizeIn   = vStep->getPreFilterKeySetSize() == npos ? getReportNoInfo() : COH_TO_STRING(vStep->getPreFilterKeySetSize());
            String::View vsSizeOut  = vStep->getPostFilterKeySetSize() == npos ? getReportNoInfo() : COH_TO_STRING(vStep->getPostFilterKeySetSize());
            String::View vsDuration = vStep->getDuration() >= 0 ? COH_TO_STRING(vStep->getDuration()) : getReportNoInfo();

            int nEff = vStep->getPreFilterKeySetSize() == 0 ? 0 :
                    (vStep->getPreFilterKeySetSize() - vStep->getPostFilterKeySetSize()) * 100 / vStep->getPreFilterKeySetSize();

            String::View vsEff = COH_TO_STRING( vsSizeIn << "|" << vsSizeOut <<  "(" << nEff << "%)");

            hs = COH_TO_STRING( pad(hs, 35, ' ') << ' ' << pad(vsIndex, 10, ' ') << ' ' << pad(vsEff, 20, ' ') << ' ' << pad(vsDuration, 10, ' '));
            }
        else
            {
            String::View vsCost = vStep->getEfficiency() == npos ? getReportNoInfo() : COH_TO_STRING(vStep->getEfficiency());

            hs = COH_TO_STRING( pad(hs, 35, ' ') << ' ' << pad(vsIndex, 10, ' ') << ' ' << pad(vsCost, 10, ' '));
            }

        for (Iterator::Handle hIter = vStep->getSteps()->iterator(); hIter->hasNext();)
            {
            hs = COH_TO_STRING(hs << "\n" << reportStep(cast<QueryRecord::PartialResult::Step::View>(hIter->next()),
                    type, hListIndexLookups, nLevel + 1));
            }

        return hs;
    }

String::View SimpleQueryRecordReporter::reportIndexLookupRecord(
        int32_t nIndexLookupId,
        QueryRecord::PartialResult::IndexLookupRecord::View vRecord)
    {
    String::View vsIndexDesc = vRecord->getIndexDescription();

    if (vsIndexDesc == NULL)
        {
        vsIndexDesc = getReportNoIndex();
        }

    String::View vsIndexLookupId = COH_TO_STRING(nIndexLookupId);
    String::View vsIndexOrdered  = vRecord->isOrdered() ? COH_TO_STRING("true") : COH_TO_STRING("false");

    String::Handle hs = COH_TO_STRING( pad(vsIndexLookupId, 5, ' ') <<
            ' ' << pad(vsIndexDesc, 40, ' ') <<
            ' ' << pad(vRecord->getExtractorDescription(), 20, ' ') <<
            ' ' << vsIndexOrdered);

    return hs;
    }

COH_CLOSE_NAMESPACE2
