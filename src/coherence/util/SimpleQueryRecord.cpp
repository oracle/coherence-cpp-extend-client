/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SimpleQueryRecord.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/LinkedList.hpp"
#include "coherence/util/SimpleQueryRecordReporter.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_PORTABLE_CLASS(260, SimpleQueryRecord);

COH_REGISTER_NAMED_CLASS(PartialResult, TypedClass<SimpleQueryRecord::PartialResult>::create());
COH_REGISTER_NAMED_CLASS(Step, TypedClass<SimpleQueryRecord::PartialResult::Step>::create());
COH_REGISTER_NAMED_CLASS(IndexLookupRecord, TypedClass<SimpleQueryRecord::PartialResult::IndexLookupRecord>::create());

COH_REGISTER_ONLY_PORTABLE_CLASS(261, PartialResult);
COH_REGISTER_ONLY_PORTABLE_CLASS(262, Step);
COH_REGISTER_ONLY_PORTABLE_CLASS(263, IndexLookupRecord);


// ----- constants ------------------------------------------

namespace
    {
    String::View static getFootPrintName()
        {
        static FinalView<String>
            vFootPrintName(System::common(), String::create("Footprint="));
        return vFootPrintName;
        }
    COH_STATIC_INIT(getFootPrintName());

    /*
     * Map size name in the index description.
     */
    String::View static getSizeName()
        {
        static FinalView<String>
            vSizeName(System::common(), String::create(", Size="));
        return vSizeName;
        }
    COH_STATIC_INIT(getSizeName());
    }

// ----- constructors -------------------------------------------------------

SimpleQueryRecord::SimpleQueryRecord()
    : m_type(QueryRecorder::explain),
      f_hListResults(self(), cast<List::Handle>(LinkedList::create()))
    {
    }

SimpleQueryRecord::SimpleQueryRecord(QueryRecorder::RecordType type,
        Collection::View vColResults)
    : m_type(type), f_hListResults(self(), cast<List::Handle>(LinkedList::create()))
    {
    mergeResults(vColResults);
    }

SimpleQueryRecord::PartialResult::PartialResult()
    : f_hListSteps(self(), cast<List::Handle>(LinkedList::create())),
      f_hPartMask(self())
    {
    }

SimpleQueryRecord::PartialResult::PartialResult(QueryRecord::PartialResult::View vThat)
    : f_hListSteps(self(), cast<List::Handle>(LinkedList::create())),
      f_hPartMask(self())
    {
    List::Handle     hListSteps = f_hListSteps;
    Iterator::Handle hIter      = vThat->getSteps()->iterator();
    while (hIter->hasNext())
        {
        hListSteps->add(SimpleQueryRecord::PartialResult::Step::create(
                cast<QueryRecord::PartialResult::Step::View>(hIter->next())));
        }
    }

SimpleQueryRecord::PartialResult::Step::Step()
    : f_vsFilter(self()), f_hSetIndexLookupRecords(self(), HashSet::create()),
      f_hListSubSteps(self(), LinkedList::create())
    {
    }

SimpleQueryRecord::PartialResult::Step::Step(QueryRecord::PartialResult::Step::View vThat)
    : f_vsFilter(self(), vThat->getFilterDescription()),
      m_nEfficiency(vThat->getEfficiency()),
      m_nSizeIn(vThat->getPreFilterKeySetSize()),
      m_nSizeOut(vThat->getPostFilterKeySetSize()),
      m_cMillis(vThat->getDuration()),
      f_hSetIndexLookupRecords(self(), HashSet::create()),
      f_hListSubSteps(self(), LinkedList::create())
    {
    Set::View   vSetRecordsThat = vThat->getIndexLookupRecords();
    Set::Handle hSetRecordsThis = f_hSetIndexLookupRecords;

    Iterator::Handle hThatIter = vSetRecordsThat->iterator();
    while (hThatIter->hasNext())
        {
        hSetRecordsThis->add(
                SimpleQueryRecord::PartialResult::IndexLookupRecord::create(
                cast<QueryRecord::PartialResult::IndexLookupRecord::View>(
                        hThatIter->next())));
        }

    List::View   vListStepsThat = vThat->getSteps();
    List::Handle hListStepsThis = f_hListSubSteps;

    for (size32_t i = 0; i < vListStepsThat->size(); i++)
        {
        hListStepsThis->add(SimpleQueryRecord::PartialResult::Step::create(
                cast<QueryRecord::PartialResult::Step::View>(
                        vListStepsThat->get(i))));
        }
    }

SimpleQueryRecord::PartialResult::IndexLookupRecord::IndexLookupRecord()
    :f_vsExtractor(self()), m_vsIndex(self(), NULL), m_cBytes(-1), m_cDistinctValues(-1),
     m_vsIndexDef(self(), NULL)
    {
    }

SimpleQueryRecord::PartialResult::IndexLookupRecord::IndexLookupRecord(
        QueryRecord::PartialResult::IndexLookupRecord::View vThat)
    : f_vsExtractor(self(), vThat->getExtractorDescription()),
      m_vsIndex(self(), vThat->getIndexDescription()),
      m_fOrdered(vThat->isOrdered()),
      m_cBytes(cast<SimpleQueryRecord::PartialResult::IndexLookupRecord::View>(vThat)->getMemoryUsage()),
      m_cDistinctValues(cast<SimpleQueryRecord::PartialResult::IndexLookupRecord::View>(vThat)->getSize()),
      m_vsIndexDef(self(), cast<SimpleQueryRecord::PartialResult::IndexLookupRecord::View>(vThat)->getIndexDef())
    {
    }

SimpleQueryRecord::PartialResult::IndexLookupRecord::IndexLookupRecord(
        String::View vsExtractor,
        String::View vsIndex,
        bool fOrdered,
        int64_t cBytes,
        int32_t cDistinctValues,
        String::View vsIndexDef)
    : f_vsExtractor(self(), vsExtractor), m_vsIndex(self(), vsIndex),
      m_fOrdered(fOrdered), m_cBytes(cBytes), m_cDistinctValues(cDistinctValues),
      m_vsIndexDef(self(), vsIndexDef)
    {
    }


// ----- QueryRecord interface ----------------------------------------------

QueryRecorder::RecordType SimpleQueryRecord::getType() const
    {
    return m_type;
    }

List::View SimpleQueryRecord::getResults() const
    {
    return f_hListResults;
    }


// ----- QueryRecord::PartialResult interface -------------------------------

List::View SimpleQueryRecord::PartialResult::getSteps() const
    {
    return f_hListSteps;
    }

PartitionSet::Handle SimpleQueryRecord::PartialResult::getPartitions()
    {
    return f_hPartMask;
    }

PartitionSet::View SimpleQueryRecord::PartialResult::getPartitions() const
    {
    return f_hPartMask;
    }

// ----- QueryRecord::PartialResult::Step interface -------------------------

String::View SimpleQueryRecord::PartialResult::Step::getFilterDescription() const
    {
    return f_vsFilter;
    }

Set::View SimpleQueryRecord::PartialResult::Step::getIndexLookupRecords() const
    {
    return f_hSetIndexLookupRecords;
    }

size32_t SimpleQueryRecord::PartialResult::Step::getEfficiency() const
    {
    return m_nEfficiency;
    }

size32_t SimpleQueryRecord::PartialResult::Step::getPreFilterKeySetSize() const
    {
    return m_nSizeIn;
    }

size32_t SimpleQueryRecord::PartialResult::Step::getPostFilterKeySetSize() const
    {
    return m_nSizeOut;
    }

int64_t SimpleQueryRecord::PartialResult::Step::getDuration() const
    {
    return m_cMillis;
    }

List::View SimpleQueryRecord::PartialResult::Step::getSteps() const
    {
    return f_hListSubSteps;
    }


// ----- QueryRecord::PartialResult::IndexLookupRecord interface ------------

String::View SimpleQueryRecord::PartialResult::IndexLookupRecord::getExtractorDescription() const
    {
    return f_vsExtractor;
    }

String::View SimpleQueryRecord::PartialResult::IndexLookupRecord::getIndexDescription() const
    {
    return m_vsIndex;
    }

bool SimpleQueryRecord::PartialResult::IndexLookupRecord::isOrdered() const
    {
    return m_fOrdered;
    }

int64_t SimpleQueryRecord::PartialResult::IndexLookupRecord::getMemoryUsage() const
    {
    return m_cBytes;
    }

int32_t SimpleQueryRecord::PartialResult::IndexLookupRecord::getSize() const
    {
    return m_cDistinctValues;
    }

String::View SimpleQueryRecord::PartialResult::IndexLookupRecord::getIndexDef() const
    {
    return m_vsIndexDef;
    }


// ----- helper methods -----------------------------------------------------

void SimpleQueryRecord::mergeResults(Collection::View vColResults)
    {
    List::Handle                     hListResults = f_hListResults;
    QueryRecord::PartialResult::View vResultThat;

    for (Iterator::Handle hIter = vColResults->iterator(); hIter->hasNext();)
        {
        vResultThat = cast<QueryRecord::PartialResult::View>(hIter->next());

        for (Iterator::Handle hThisIter = hListResults->iterator();
                hThisIter->hasNext();)
            {
            SimpleQueryRecord::PartialResult::Handle hResultThis =
                    cast<SimpleQueryRecord::PartialResult::Handle>(
                            hThisIter->next());

            if (hResultThis->isMatching(vResultThat))
                {
                hResultThis->merge(vResultThat);
                vResultThat = NULL;
                break;
                }
            }
        }

    if (NULL != vResultThat)
        {
        // no matching partial result found; create a new one
        hListResults->add(SimpleQueryRecord::PartialResult::create(
                vResultThat));
        }
    }

 void SimpleQueryRecord::PartialResult::merge(
         QueryRecord::PartialResult::View vResult)
     {
     getPartitions()->add(vResult->getPartitions());

     List::Handle hListStepsThis = f_hListSteps;

     List::View vListStepsThat = vResult->getSteps();

     for (size32_t i = 0; i < vListStepsThat->size(); i++)
         {
         QueryRecord::PartialResult::Step::View vStep =
                 cast<QueryRecord::PartialResult::Step::View>(
                         vListStepsThat->get(i));

         SimpleQueryRecord::PartialResult::Step::Handle hMergeStep =
                 cast<SimpleQueryRecord::PartialResult::Step::Handle>(
                         hListStepsThis->get(i));

         hMergeStep->merge(vStep);
         }
     }

 bool SimpleQueryRecord::PartialResult::isMatching(QueryRecord::PartialResult::View vResult) const
     {
     List::View vListStepsThis = f_hListSteps;

     List::View vListStepsThat = vResult->getSteps();

     if (vListStepsThat->size() != vListStepsThis->size())
         {
         return false;
         }

     for (size32_t i = 0; i < vListStepsThis->size(); i++)
         {
         SimpleQueryRecord::PartialResult::Step::View vStepThis =
                 cast<SimpleQueryRecord::PartialResult::Step::View>(vListStepsThis->get(i));

         SimpleQueryRecord::PartialResult::Step::View vStepThat =
             cast<SimpleQueryRecord::PartialResult::Step::View>(vListStepsThat->get(i));

         if (!vStepThis->isMatching(vStepThat))
             {
             return false;
             }
         }
     return true;
     }

 void SimpleQueryRecord::PartialResult::Step::merge(QueryRecord::PartialResult::Step::View vStep)
     {
     m_nSizeIn     += vStep->getPreFilterKeySetSize();
     m_nSizeOut    += vStep->getPostFilterKeySetSize();
     m_nEfficiency += vStep->getEfficiency();
     m_cMillis     += vStep->getDuration();

     Map::Handle hMapIndexRecords = HashMap::create();
     for (Iterator::Handle hIter = vStep->getIndexLookupRecords()->iterator(); hIter->hasNext();)
         {
         IndexLookupRecord::View vIndexRecord = cast<IndexLookupRecord::View>(hIter->next());
         hMapIndexRecords->put(vIndexRecord, vIndexRecord);
         }

     for (Iterator::Handle hIter = f_hSetIndexLookupRecords->iterator(); hIter->hasNext();)
         {
         IndexLookupRecord::Handle hIndexRecord     = cast<IndexLookupRecord::Handle>(hIter->next());
         IndexLookupRecord::View   vStepIndexRecord = cast<IndexLookupRecord::View>(hMapIndexRecords->get(hIndexRecord));

         if (NULL != vStepIndexRecord && NULL != vStepIndexRecord->getIndexDescription())
             {
             hIndexRecord->m_cBytes         += vStepIndexRecord->getMemoryUsage();
             hIndexRecord->m_cDistinctValues = std::max(hIndexRecord->getSize(), vStepIndexRecord->getSize());
             hIndexRecord->m_vsIndex         = hIndexRecord->buildIndexDescription();
             }
         }

     List::View   vListSteps     = vStep->getSteps();
     List::Handle hListStepsThis = f_hListSubSteps;

     for (size32_t i = 0; i < hListStepsThis->size(); i++)
         {
         SimpleQueryRecord::PartialResult::Step::Handle hStepThis =
                 cast<SimpleQueryRecord::PartialResult::Step::Handle>(hListStepsThis->get(i));

         SimpleQueryRecord::PartialResult::Step::View vStepThat =
                 cast<SimpleQueryRecord::PartialResult::Step::View>(vListSteps->get(i));

         hStepThis->merge(vStepThat);
         }
     }

 bool SimpleQueryRecord::PartialResult::Step::isMatching(QueryRecord::PartialResult::Step::View vStep) const
     {
     if (getFilterDescription()->equals(vStep->getFilterDescription()) &&
         getIndexLookupRecords()->equals(vStep->getIndexLookupRecords()))
         {
         List::View vListSteps     = vStep->getSteps();
         List::View vListStepsThis = f_hListSubSteps;

         if (vListStepsThis->size() == vListSteps->size())
             {
             for (size32_t i = 0; i < vListStepsThis->size(); i++)
                 {
                 SimpleQueryRecord::PartialResult::Step::View vStepThis =
                         cast<SimpleQueryRecord::PartialResult::Step::View>(vListStepsThis->get(i));

                 SimpleQueryRecord::PartialResult::Step::View vStepThat =
                         cast<SimpleQueryRecord::PartialResult::Step::View>(vListSteps->get(i));

                 if (!vStepThis->isMatching(vStepThat))
                     {
                     return false;
                     }
                 }

             return true;
             }
         }

     return false;
     }


 // ----- PortableObject interface ------------------------------------------

 void SimpleQueryRecord::readExternal(PofReader::Handle hIn)
     {
     m_type = (QueryRecorder::RecordType) hIn->readInt32(0);
     hIn->readCollection(1, f_hListResults);
     }

 void SimpleQueryRecord::writeExternal(PofWriter::Handle hOut) const
     {
     hOut->writeInt32(0, (int32_t) m_type);
     hOut->writeCollection(1, f_hListResults);
     }

 void SimpleQueryRecord::PartialResult::readExternal(PofReader::Handle hIn)
     {
     initialize(f_hPartMask, cast<PartitionSet::Handle>(hIn->readObject(0)));
     hIn->readCollection(1, f_hListSteps);
     }

 void SimpleQueryRecord::PartialResult::writeExternal(PofWriter::Handle hOut) const
     {
     hOut->writeObject(0, f_hPartMask);
     hOut->writeCollection(1, f_hListSteps);
     }

 void SimpleQueryRecord::PartialResult::Step::readExternal(PofReader::Handle hIn)
     {
     initialize(f_vsFilter, cast<String::View>(hIn->readObject(0)));

     m_nEfficiency = hIn->readInt32(1);
     m_nSizeIn     = hIn->readInt32(2);
     m_nSizeOut    = hIn->readInt32(3);
     m_cMillis     = hIn->readInt64(4);

     hIn->readCollection(5, f_hSetIndexLookupRecords);
     hIn->readCollection(6, f_hListSubSteps);
     }

 void SimpleQueryRecord::PartialResult::Step::writeExternal(PofWriter::Handle hOut) const
     {
     hOut->writeObject(    0, f_vsFilter);
     hOut->writeInt32(     1, m_nEfficiency);
     hOut->writeInt32(     2, m_nSizeIn);
     hOut->writeInt32(     3, m_nSizeOut);
     hOut->writeInt64(     4, m_cMillis);
     hOut->writeCollection(5, f_hSetIndexLookupRecords);
     hOut->writeCollection(6, f_hListSubSteps);
     }

 void SimpleQueryRecord::PartialResult::IndexLookupRecord::readExternal(PofReader::Handle hIn)
     {
     initialize(f_vsExtractor, cast<String::View>(hIn->readObject(0)));
     m_vsIndex = cast<String::View>(hIn->readObject(1));
     m_fOrdered = hIn->readBoolean(2);

     if (NULL != m_vsIndex)
         {
         parseIndexDescription(m_vsIndex);
         }
     }

 void SimpleQueryRecord::PartialResult::IndexLookupRecord::writeExternal(PofWriter::Handle hOut) const
     {
     hOut->writeObject(0, f_vsExtractor);
     hOut->writeObject(1, m_vsIndex);
     hOut->writeBoolean(2, m_fOrdered);
     }


// ----- Object interface ---------------------------------------------------

 TypedHandle<const String> SimpleQueryRecord::toString() const
    {
    return COH_TO_STRING(SimpleQueryRecordReporter::create()->report(this));
    }

bool SimpleQueryRecord::PartialResult::IndexLookupRecord::equals(Object::View v) const
    {
    SimpleQueryRecord::PartialResult::IndexLookupRecord::View that =
            cast<SimpleQueryRecord::PartialResult::IndexLookupRecord::View>(v, false);
    if (NULL == that)
        {
        return false;
        }

    // Note: IndexLookupRecords are considered equivalent based on
    //       the definition of the indices and not varying factors
    //       such as index footprint and size.
    return Class::typeEquals(this, that)
          && m_fOrdered == that->m_fOrdered
          && f_vsExtractor->equals(that->f_vsExtractor)
          && Object::equals(m_vsIndexDef, that->m_vsIndexDef);
    }

size32_t SimpleQueryRecord::PartialResult::IndexLookupRecord::hashCode() const
    {
    return (m_fOrdered ? 1 : 0 ) +
            Object::hashCode(m_vsIndexDef) + f_vsExtractor->hashCode();
    }


// ----- Helper methods ---------------------------------------------------

String::View SimpleQueryRecord::PartialResult::IndexLookupRecord::buildIndexDescription()
    {
    if (NULL == m_vsIndexDef)
        {
        return m_vsIndexDef;
        }

    String::View sFP = StringHelper::toMemorySizeString(m_cBytes, false);
    if (!sFP->endsWith("B"))
        {
        sFP = COH_TO_STRING(sFP << "B");
        }

    return COH_TO_STRING(m_vsIndexDef
                         << getFootPrintName()
                         << sFP
                         << getSizeName()
                         << m_cDistinctValues);
    }

void SimpleQueryRecord::PartialResult::IndexLookupRecord::parseIndexDescription(String::View vsIndex)
    {
    int32_t iStart = vsIndex->indexOf(getFootPrintName());

    if (iStart <= 0)
        {
        return;
        }

    m_vsIndexDef = vsIndex->substring(0, iStart);

    int iEnd = vsIndex->indexOf(',', iStart);
    m_cBytes = StringHelper::parseMemorySize(vsIndex->substring(iStart
               + getFootPrintName()->length(), iEnd));

    iStart            = vsIndex->indexOf(getSizeName());
    m_cDistinctValues = Integer32::parse(vsIndex->substring(iStart + getSizeName()->length()));
    }

COH_CLOSE_NAMESPACE2
