/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/TopNAggregator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/Arrays.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/ConverterCollections.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"


COH_OPEN_NAMESPACE3(coherence,util,aggregator)


COH_REGISTER_PORTABLE_CLASS(252, TopNAggregator);

COH_REGISTER_NAMED_CLASS(TopNPartialResult, TypedClass<TopNAggregator::PartialResult>::create());
COH_REGISTER_ONLY_PORTABLE_CLASS(253, TopNPartialResult);

using coherence::util::comparator::SafeComparator;

COH_OPEN_NAMESPACE_ANON(TopNAggregator)

// ----- local class: ExtractingConverter ---------------------------

/**
* A value-extracting converter.
*/
class COH_EXPORT ExtractingConverter
    : public class_spec<ExtractingConverter,
        extends<Object>,
        implements<Converter> >
    {
    friend class factory<ExtractingConverter>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new ExtractingConverter.
        *
        * @param vExtractor  the value extractor
        */
        ExtractingConverter(ValueExtractor::View vExtractor)
                : f_vExtractor(self(), vExtractor)
            {
            }

    // ----- Converter interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        Object::Holder convert(Object::Holder oh) const
            {
            return cast<InvocableMap::Entry::Holder>(oh)->extract(f_vExtractor);
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * The value extractor.
        */
        FinalView<ValueExtractor> f_vExtractor;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

TopNAggregator::TopNAggregator()
        : m_fParallel(false), f_vExtractor(self()), f_vComparator(self()), m_cResults(0)
    {
    }

TopNAggregator::TopNAggregator(ValueExtractor::View vExtractor, Comparator::View vComparator,
                               int32_t cResults)
        : m_fParallel(false), f_vExtractor(self(), vExtractor),
          f_vComparator(self(), vComparator == NULL
                        ? (Comparator::View) SafeComparator::getInstance() : vComparator),
          m_cResults(cResults)
    {
    }


// ----- ParallelAwareAggregator Interface  ---------------------------------

InvocableMap::EntryAggregator::Handle TopNAggregator::getParallelAggregator()
    {
    m_fParallel = true;
    return this;
    }

Object::Holder TopNAggregator::aggregateResults(Collection::View vColResults)
    {
    return finalizeResult(cast<TopNAggregator::PartialResult::Handle>(
                                  aggregatePartialResults(vColResults)));
    }

Object::Holder TopNAggregator::aggregate(Set::View vSetEntries)
    {
    TopNAggregator::PartialResult::Handle hResultPartial =
            TopNAggregator::PartialResult::create(f_vComparator);

    addToResult(ConverterCollections::ConverterIterator::create(
                        vSetEntries->iterator(), ExtractingConverter::create(f_vExtractor)),
                hResultPartial);

    return m_fParallel ? (Object::Holder) hResultPartial : (Object::Holder) finalizeResult(hResultPartial);
    }


// ----- PartialResultAggregator methods ------------------------------------

Object::Holder TopNAggregator::aggregatePartialResults(Collection::View vColPartialResults)
    {
    TopNAggregator::PartialResult::Handle hResultMerged =
            TopNAggregator::PartialResult::create(f_vComparator);
    size32_t cMaxSize = (size32_t) m_cResults;

    for (Iterator::Handle hIter = vColPartialResults->iterator(); hIter->hasNext(); )
        {
        // if we have already accumulated N "merged" results, use the fact
        // that the partial results are sorted; we don't need to consider
        // any elements that are lesser-than the first element in the
        // merged results bag so far

        TopNAggregator::PartialResult::View vResultPartial =
                cast<TopNAggregator::PartialResult::View>(hIter->next());

        Iterator::Handle hIterValues = hResultMerged->size() >= cMaxSize
                                ? vResultPartial->tailBag(hResultMerged->first())->iterator()
                                : vResultPartial->iterator();

        addToResult(hIterValues, hResultMerged);
        }

    return hResultMerged;
    }


// ----- internal methods ---------------------------------------------------

void TopNAggregator::addToResult(Iterator::Handle hIterValues,
                                 TopNAggregator::PartialResult::Handle hResult)
    {
    int32_t        cMaxSize   = m_cResults;
    int32_t        cCurSize   = hResult->size();
    Object::Holder ohElemFirst = NULL;

    while (hIterValues->hasNext())
        {
        Object::Holder ohValue = hIterValues->next();
        if (ohValue == NULL)
            {
            continue;
            }

        if (cCurSize < cMaxSize)
            {
            hResult->add(ohValue);
            }
        else
            {
            if (ohElemFirst == NULL)
                {
                ohElemFirst = hResult->first();
                }
            if (f_vComparator->compare(ohValue, ohElemFirst) > 0)
                {
                hResult->add(ohValue);
                hResult->removeFirst();
                ohElemFirst = NULL;
                }
            }

        ++cCurSize;
        }
    }

ObjectArray::Handle TopNAggregator::finalizeResult(TopNAggregator::PartialResult::View vResult)
    {
    ObjectArray::Handle hao = vResult->toArray();
    Arrays::reverse(hao);
    return hao;
    }


// ----- PortableObject methods ---------------------------------------------

void TopNAggregator::readExternal(PofReader::Handle hIn)
    {
    m_fParallel = hIn->readBoolean(0);
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObject(1)));
    initialize(f_vComparator, cast<Comparator::View>(hIn->readObject(2)));
    m_cResults = hIn->readInt32(3);
    }

void TopNAggregator::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeBoolean(0, m_fParallel);
    hOut->writeObject (1, f_vExtractor);
    hOut->writeObject (2, f_vComparator);
    hOut->writeInt32  (3, m_cResults);
    }


// ----- inner class: PartialResult -----------------------------------------

TopNAggregator::PartialResult::PartialResult()
    : super()
    {
    }

TopNAggregator::PartialResult::PartialResult(Comparator::View vComparator)
    : super(vComparator)
    {
    }

// ----- PortableObject methods -------------------------------------

void TopNAggregator::PartialResult::readExternal(PofReader::Handle hIn)
    {
    m_vComparator = cast<Comparator::View>(hIn->readObject(0));
    m_hMap        = instantiateInternalMap(m_vComparator);

    hIn->readCollection(1, this);
    }

void TopNAggregator::PartialResult::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, m_vComparator);
    hOut->writeCollection(1, this);
    }

COH_CLOSE_NAMESPACE3
