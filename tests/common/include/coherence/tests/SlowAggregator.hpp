/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SLOW_AGGREGATOR_HPP
#define COH_SLOW_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/aggregator/Float64Average.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using namespace coherence::lang;

using coherence::util::ValueExtractor;
using coherence::util::aggregator::Float64Average;
using coherence::util::extractor::IdentityExtractor;

/**
 * Test aggregator that takes 10 seconds
 * to execute on the server.
 */
class SlowAggregator
        : public class_spec<SlowAggregator,
                extends<Float64Average> >
    {
    protected:
        friend class factory<SlowAggregator>;

    public:
        /**
        * Constructor - for storing cache name
        */
        SlowAggregator()
            : super((ValueExtractor::View) IdentityExtractor::create())
            {
            };

    };

COH_REGISTER_PORTABLE_CLASS(3013, SlowAggregator);

COH_CLOSE_NAMESPACE2

#endif // COH_SLOW_AGGREGATOR_HPP
