/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Integer64Min.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include <algorithm>
#include <limits>

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(37, Integer64Min);


// ----- constructors -------------------------------------------------------

Integer64Min::Integer64Min()
    : super()
    {
    }

Integer64Min::Integer64Min(ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

Integer64Min::Integer64Min(String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void Integer64Min::init(bool fFinal)
    {
    AbstractInteger64Aggregator::init(fFinal);
    m_lResult = Integer64::max_value;
    }

void Integer64Min::process(Object::View v, bool /* fFinal */)
    {
    if (v != NULL)
          {
          m_lResult = std::min(m_lResult,
                      cast<Number::View>(v)->getInt64Value());
          m_count++;
          }
    }

COH_CLOSE_NAMESPACE3
