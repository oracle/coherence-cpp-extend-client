/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_AGGREGATOR_STUB_HPP
#define COH_AGGREGATOR_STUB_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/PriorityAggregator.hpp"

using coherence::util::aggregator::PriorityAggregator;

COH_OPEN_NAMESPACE (test)

class AggregatorStub
        : public class_spec<AggregatorStub,
                extends<PriorityAggregator> >
    {
    friend class factory<AggregatorStub>;

    protected:

        AggregatorStub()
            {
            m_fRunCanceledCalled = false;
            }

    public:
        void runCanceled(bool fAbandoned)
            {
            m_fRunCanceledCalled = true;
            m_fAbandoned         = fAbandoned;
            }

        bool wasRunCanceledCalled()
            {
            return m_fRunCanceledCalled;
            }

    private:
        bool m_fRunCanceledCalled;

        bool m_fAbandoned;
    };

COH_CLOSE_NAMESPACE // test

#endif // COH_AGGREGATOR_STUB_HPP
