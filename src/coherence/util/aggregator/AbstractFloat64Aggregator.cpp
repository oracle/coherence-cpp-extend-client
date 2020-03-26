/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/AbstractFloat64Aggregator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


// ----- constructors -------------------------------------------------------

AbstractFloat64Aggregator::AbstractFloat64Aggregator()
    : super()
    {
    }

AbstractFloat64Aggregator::AbstractFloat64Aggregator(
            ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

AbstractFloat64Aggregator::AbstractFloat64Aggregator(
            String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void AbstractFloat64Aggregator::init(bool /* fFinal */)
    {
    m_count = 0;
    }

Object::Holder AbstractFloat64Aggregator::finalizeResult(
        bool /* fFinal */)
    {
    return m_count == 0 ? (Float64::Handle) NULL :
        Float64::valueOf(m_dflResult);
    }

COH_CLOSE_NAMESPACE3
