/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/native/NativeRecursiveMutex.hpp"

using namespace coherence::lang;
using namespace coherence::native;
using namespace std;

/**
* Test Suite for the Boolean object.
*/
class NativeRecursiveMutexTest : public CxxTest::TestSuite
    {
    public:

        void testCreateDestroy()
            {
            NativeRecursiveMutex* pNativeRecursiveMutex = NativeRecursiveMutex::create();
            TS_ASSERT(pNativeRecursiveMutex != NULL);
            delete pNativeRecursiveMutex;
            }

        void testLock()
            {
            COH_AUTO_PTR<NativeRecursiveMutex> apNativeRecursiveMutex(NativeRecursiveMutex::create());
            apNativeRecursiveMutex->lock();
            apNativeRecursiveMutex->unlock();
            }

        void testTryLock()
            {
            COH_AUTO_PTR<NativeRecursiveMutex> apNativeRecursiveMutex(NativeRecursiveMutex::create());
            TS_ASSERT(apNativeRecursiveMutex->tryLock());
            apNativeRecursiveMutex->unlock();
            }

        void testDoubleLock()
            {
            COH_AUTO_PTR<NativeRecursiveMutex> apNativeRecursiveMutex(NativeRecursiveMutex::create());
            TS_ASSERT(apNativeRecursiveMutex->tryLock());
            TS_ASSERT(apNativeRecursiveMutex->tryLock());
            apNativeRecursiveMutex->unlock();
            apNativeRecursiveMutex->unlock();
            }
    };
