/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PROCESSOR_PRINT_UUID_TIMESTAMP_HPP
#define COH_PROCESSOR_PRINT_UUID_TIMESTAMP_HPP

#include "coherence/lang.ns"

#include "coherence/util/processor/AbstractClusterProcessor.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

using coherence::util::processor::AbstractClusterProcessor;

/**
* Example AbstractClusterProcessor implementation that returns value's UUID timestamp.
*
* @author welin  2013.07.10
*/
class ProcessorPrintUUIDTimestamp : public class_spec<ProcessorPrintUUIDTimestamp,
        extends<AbstractClusterProcessor> >
    {
    friend class factory<ProcessorPrintUUIDTimestamp>;

    // ----- constructors -----------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        ProcessorPrintUUIDTimestamp();
    };

COH_CLOSE_NAMESPACE2

#endif //COH_PROCESSOR_PRINT_UUID_TIMESTAMP_HPP
