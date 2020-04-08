/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

using namespace coherence::lang;


/**
* Test Suite for BackTraceTest
*
* Tests Bug21155867.  The BackTrace
* methods are not all thread-safe.
*
* To reproduce the issue, increase the threads to 20,
* and the iterations to 1000.
*/
class BackTraceTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test Dbg thread safety
        */
        void testThreads()
            {
            Object::View v = Object::create();

            Runnable::Handle hTask = 
                    DoExceptions::create(v, 20);

            s_fStart = false;

            Thread::Handle ahThreads[THREADS];
            String::View   vsName;
            for (int32_t i = 0; i < THREADS; i++)
                {
                ahThreads[i] = Thread::create(hTask, COH_TO_STRING(getPREFIX() << i));
                String::View str = COH_TO_STRING(getPREFIX() << i);
                ahThreads[i]->start();
                }

            COH_SYNCHRONIZED (v)
                {
                s_fStart = true;
                v->notifyAll();
                }

            for (int32_t i = 0; i < THREADS; i++)
                {
                ahThreads[i]->join();
                }
            }

    //----- Inner Class DoExceptions

    class DoExceptions
        : public class_spec<DoExceptions,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<DoExceptions>;

        protected:
            /**
            * Constructor
            */
            DoExceptions(Object::View v, int32_t nIterations)
                : f_vMonitor(self(), v),
                  m_nIterations(nIterations)
            {
            }

        // ----- Runnable interface ---------------------------------------------

        public:
            void run()
                {
                COH_SYNCHRONIZED (f_vMonitor)
                    {
                    while (!s_fStart)
                        {
                            f_vMonitor->wait();
                        }
                    }
 
                for (int i = 0; i < m_nIterations; i++ )
                    {
                    try 
                        {
                        COH_THROW(NullPointerException::create());
                        }
                    catch (NullPointerException::View /* vNPE */)
                        {
                        }
                    }
                }


        // ----- data members ---------------------------------------------------

        private:
            FinalView<Object>  f_vMonitor;     
            int32_t            m_nIterations;
        };

    // ----- class local constants -------------------------------------------

    public:   
        static String::View getPREFIX()
            {
            return String::create("Thread-");
            }

    // ----- fields and constants -------------------------------------------

    public:
        static const int32_t THREADS  = 5;

        static Volatile<bool> s_fStart;
    };


Volatile<bool> BackTraceTest::s_fStart(System::common());
