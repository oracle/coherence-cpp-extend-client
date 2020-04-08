/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Float64Max.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include <algorithm>
#include <limits>

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(31, Float64Max);


// ----- constructors -------------------------------------------------------

Float64Max::Float64Max()
    : super()
    {
    }

Float64Max::Float64Max(ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

Float64Max::Float64Max(String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void Float64Max::init(bool fFinal)
    {
    AbstractFloat64Aggregator::init(fFinal);
    m_dflResult = std::numeric_limits<float64_t>::min();
    }

void Float64Max::process(Object::View v, bool /* fFinal */)
    {
    if (v != NULL)
          {
          m_dflResult = std::max(m_dflResult,
                      cast<Number::View>(v)->getFloat64Value());
          m_count++;
          }
    }

COH_CLOSE_NAMESPACE3
