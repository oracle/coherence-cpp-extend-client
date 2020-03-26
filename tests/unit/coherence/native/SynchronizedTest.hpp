/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/lang/SynchronizedBlock.hpp"

#include "private/coherence/native/NativeMutex.hpp"
#include "private/coherence/native/NativeRecursiveMutex.hpp"

using namespace coherence::lang;
using namespace coherence::native;
using namespace std;

/**
* Test Suite for the SynchronizedBlock object.
*/
class SynchronizedTest : public CxxTest::TestSuite
    {
    public:

        void testBlock()
            {
            COH_AUTO_PTR<NativeMutex> apMutex(NativeMutex::create());

            apMutex.get()->lock();
            apMutex.get()->unlock();
            TS_ASSERT(apMutex->tryLock()); // it got unlocked
            apMutex->unlock();
            }

        void testDoubleBlock()
            {
            COH_AUTO_PTR<NativeMutex> apMutex(NativeMutex::create());

            apMutex.get()->lock();
            apMutex.get()->unlock();
            TS_ASSERT(apMutex->tryLock()); // it got unlocked
            apMutex->unlock();

            apMutex.get()->lock();
            apMutex.get()->unlock();
            TS_ASSERT(apMutex->tryLock()); // it got unlocked
            apMutex->unlock();
            }

        void testRecursiveBlock()
            {
            COH_AUTO_PTR<NativeMutex> apMutex(NativeRecursiveMutex::create());

            apMutex.get()->lock();
            // we can't actually test if it is locked
            apMutex.get()->lock();
            // still can't test
            apMutex.get()->unlock();
            apMutex.get()->unlock();
            }
    };
