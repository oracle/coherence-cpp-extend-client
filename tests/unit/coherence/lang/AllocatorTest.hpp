/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "private/coherence/lang/Allocator.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::lang::Allocator;


/**
* Test suite for the Allocator class.
*
* Generates allocations when the heapLogging flag
* has been set.
*/
class AllocatorTest : public CxxTest::TestSuite
    {
    public:

        /**
        * Test creation.
        */
        void testAllocation()
            {
            bool fHeapLogging =
                    Boolean::parse(System::getProperty("tangosol.coherence.heap.logging"));
            if (fHeapLogging)
                {
                for (int32_t i = 0; i < 50000; i++)
                    {
                    void* ab1 = Allocator::allocate(1);
                    Allocator::release(ab1);
                    }
                }
            }
    };
