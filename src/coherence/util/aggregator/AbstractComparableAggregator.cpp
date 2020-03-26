/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/AbstractComparableAggregator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


// ----- constructors -------------------------------------------------------

AbstractComparableAggregator::AbstractComparableAggregator()
    : m_count(0), m_vResult(self()), f_vComparator(self())
    {
    }

AbstractComparableAggregator::AbstractComparableAggregator(
            ValueExtractor::View vExtractor, Comparator::View vComparator)
    : super(vExtractor), m_count(0), m_vResult(self()),
      f_vComparator(self(), vComparator)
    {
    }

AbstractComparableAggregator::AbstractComparableAggregator(
            String::View vsMethod, Comparator::View vComparator)
    : super(vsMethod), m_count(0), m_vResult(self()),
      f_vComparator(self(), vComparator)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void AbstractComparableAggregator::init(bool /*fFinal*/)
    {
    m_count   = 0;
    m_vResult = NULL;
    }

Object::Holder AbstractComparableAggregator::finalizeResult(
        bool /*fFinal*/)
    {
    return m_count == 0 ? (Object::View) NULL : m_vResult;
    }


// ----- PortableObject interface -------------------------------------------

void AbstractComparableAggregator::readExternal(PofReader::Handle hIn)
    {
    super::readExternal(hIn);
    initialize(f_vComparator, cast<Comparator::View>(hIn->readObject(2)));
    }

void AbstractComparableAggregator:: writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);
    hOut->writeObject(2, f_vComparator);
    }

COH_CLOSE_NAMESPACE3
