/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Integer64Sum.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(38, Integer64Sum);


// ----- constructors -------------------------------------------------------

Integer64Sum::Integer64Sum()
    : super()
    {
    }

Integer64Sum::Integer64Sum(ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

Integer64Sum::Integer64Sum(String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void Integer64Sum::init(bool fFinal)
    {
    AbstractInteger64Aggregator::init(fFinal);
    m_lResult = 0;
    }

void Integer64Sum::process(Object::View v, bool /* fFinal */)
    {
    if (v != NULL)
          {
          m_lResult += cast<Number::View>(v)->getInt64Value();
          m_count++;
          }
    }

COH_CLOSE_NAMESPACE3
