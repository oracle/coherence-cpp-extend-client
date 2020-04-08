/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_THREADDUMPHANDLER_HPP
#define COH_NATIVE_THREADDUMPHANDLER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang.ns"

#include <iostream>
#include <sstream>

COH_OPEN_NAMESPACE2(coherence,native)

/**
* Thread dump handler
*
* @author dr  2008.04.05
*/
class COH_EXPORT NativeThreadDumpHandler
    {
    // ----- NativeThreadDumpHandler interface ------------------------------

    public:
        /**
        * Do a thread dump and log HeapAnalyzer (if enabled)
        */
        static void doThreadDump()
            {
            std::stringstream ss;
            Object::View vHeap = System::getHeapAnalyzer();
            if (NULL != vHeap)
                {
                ss << std::endl << "Heap Analysis\n" << vHeap << std::endl;
                }
            ss << std::endl << Thread::formatStacks();
            std::cerr << ss.str();
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_THREADDUMPHANDLER_HPP
