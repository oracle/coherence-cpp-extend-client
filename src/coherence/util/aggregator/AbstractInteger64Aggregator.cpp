/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/AbstractInteger64Aggregator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


// ----- constructors -------------------------------------------------------

AbstractInteger64Aggregator::AbstractInteger64Aggregator()
    : super()
    {
    }

AbstractInteger64Aggregator::AbstractInteger64Aggregator(
            ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

AbstractInteger64Aggregator::AbstractInteger64Aggregator(
            String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void AbstractInteger64Aggregator::init(bool /* fFinal */)
    {
    m_count = 0;
    }

Object::Holder AbstractInteger64Aggregator::finalizeResult(
        bool /* fFinal */)
    {
    return m_count == 0 ? (Integer64::Handle) NULL :
            Integer64::valueOf(m_lResult);
    }

COH_CLOSE_NAMESPACE3
