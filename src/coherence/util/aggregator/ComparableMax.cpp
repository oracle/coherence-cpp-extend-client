/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/ComparableMax.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(24, ComparableMax);

using coherence::util::comparator::SafeComparator;


// ----- constructors -------------------------------------------------------

ComparableMax::ComparableMax()
    {
    }

ComparableMax::ComparableMax(ValueExtractor::View vExtractor,
        Comparator::View vComparator)
    : super(vExtractor, vComparator)
    {
    }

ComparableMax::ComparableMax(String::View vsMethod, Comparator::View vComparator)
    : super(vsMethod, vComparator)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void ComparableMax::process(Object::View v, bool /*fFinal*/)
    {
    if (v != NULL)
        {
        Object::View vResult = m_vResult;
        if (vResult == NULL ||
              SafeComparator::compareSafe(f_vComparator, vResult, v) < 0)
          {
          m_vResult = v;
          }
        m_count++;
        }
    }

COH_CLOSE_NAMESPACE3
