/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/native/NativeCondition.hpp"
#include "private/coherence/native/NativeThread.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace coherence::native;
using namespace std;

class NativeRunnable
    : public class_spec<NativeRunnable,
        extends<Object>,
        implements<Runnable> >
    {
    friend class factory<NativeRunnable>;

    protected:
        NativeRunnable(NativeCondition* pCond)
            : m_pCond(pCond)
            {
            }

    public:
        virtual void run()
            {
            NativeCondition* pCond = m_pCond;
            pCond->lock();
            pCond->wait(10); // delay
            pCond->notify();
            pCond->unlock();
            }

    private:
        NativeCondition* m_pCond;
    };

/**
* Test Suite for the NativeThread object.
*/
class NativeThreadTest : public CxxTest::TestSuite
    {
    public:

        void testStart()
            {
            auto_ptr<NativeCondition> pCondition(NativeCondition::create());

            pCondition->lock();
                {
                NativeThread::create(NativeRunnable::create(pCondition.get()));
                // thread created, waiting for signal
                pCondition->wait(10000);
                // thread creation confirmed
                }
            pCondition->unlock();
            }
    };

