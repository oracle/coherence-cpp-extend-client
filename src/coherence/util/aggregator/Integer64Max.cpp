/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Integer64Max.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include <algorithm>
#include <limits>

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(36, Integer64Max);


// ----- constructors -------------------------------------------------------

Integer64Max::Integer64Max()
    : super()
    {
    }

Integer64Max::Integer64Max(ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

Integer64Max::Integer64Max(String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void Integer64Max::init(bool fFinal)
    {
    AbstractInteger64Aggregator::init(fFinal);
    m_lResult = Integer64::min_value;
    }

void Integer64Max::process(Object::View v, bool /* fFinal */)
    {
    if (v != NULL)
          {
          m_lResult = std::max(m_lResult,
                      cast<Number::View>(v)->getInt64Value());
          m_count++;
          }
    }

COH_CLOSE_NAMESPACE3
