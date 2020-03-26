/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/native/NativeMutex.hpp"

using namespace coherence::lang;
using namespace coherence::native;
using namespace std;

/**
* Test Suite for the Boolean object.
*/
class NativeNativeMutexTest : public CxxTest::TestSuite
    {
    public:

        void testCreateDestroy()
            {
            NativeMutex* pNativeMutex = NativeMutex::create();
            TS_ASSERT(pNativeMutex != NULL);
            delete pNativeMutex;
            }

        void testLock()
            {
            COH_AUTO_PTR<NativeMutex> apNativeMutex(NativeMutex::create());
            apNativeMutex->lock();
            apNativeMutex->unlock();
            }

        void testTryLock()
            {
            COH_AUTO_PTR<NativeMutex> apNativeMutex(NativeMutex::create());
            TS_ASSERT(apNativeMutex->tryLock());
            apNativeMutex->unlock();
            }
    };
