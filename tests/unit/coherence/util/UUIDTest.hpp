/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/UUID.hpp"

#include "coherence/lang.ns"

#include "coherence/util/UUID.hpp"

#include <iostream>
#include <sstream>

using namespace coherence::lang;
using namespace std;

using coherence::util::UUID;


/**
* UUID test suite
*/
class UUIDTest : public CxxTest::TestSuite
    {
    public:
        // ----- TestCase inner interface -----------------------------------

        /*
         * TestCase interface used to control the threads which
         * run the test.
         */
        class TestCase
            : public interface_spec<TestCase>
            {
            public:
                /**
                 * Counter that determines whether all threads are
                 * running and finished.
                 */
                virtual AtomicCounter::Handle getAtomicCounter() = 0;

                /**
                 * Method that runs the thread starting and
                 * stopping
                 */
                virtual void run() = 0;

                /**
                * Shutdown the test, and stop all Runner threads.
                */
                virtual void shutdown() = 0;

                /**
                 * Method that detects that all threads have completed.
                 */
                virtual void threadIsFinished() = 0;
            };

    protected:
        // ----- Runner inner class -----------------------------------------

        /**
        * Base TestCase runner thread.
        */
        class Runner
            : public abstract_spec<Runner,
                extends <Object>,
                implements<Runnable> >
            {
            friend class factory<Runner>;

            // ----- constructors -------------------------------------------

            protected:
                /**
                * Create a new Runner thread that will perform the test.
                *
                */
               Runner()
                   : m_hTest(self(), NULL)
                    {
                    }

            public:
                /**
                * Set the test case.
                *
                * @param test  the test case
                */
                void setTestCase(TestCase::Handle hTest)
                    {
                    m_hTest = hTest;
                    }

                /**
                * Perform the test.
                */
                virtual void doTest() = 0;


            // ----- Runnable methods ---------------------------------------

            public:
                /**
                * Verify that the thread is ready and run the test.
                */
                virtual void run()
                    {
                    bool fFirst = true;
                    while (true)
                        {
                        COH_SYNCHRONIZED (m_hTest)
                            {
                            if (fFirst)
                                {
                                // signal that this test thread is "ready" to be notified
                                m_hTest->getAtomicCounter()->increment();
                                fFirst = false;
                                }
                            try
                                {
                                // wait for all the other threads to start
                                m_hTest->wait(0);
                                }
                            catch (InterruptedException::View vIe)
                                {
                                Thread::currentThread()->interrupt();
                                break;
                                }
                            }

                        doTest();

                        m_hTest->threadIsFinished();
                        }
                    }


            // ----- data members -------------------------------------------

            protected:
                /**
                * Test case to be run.
                */
                MemberHandle<TestCase> m_hTest;
            };

        // ----- UUIDRunner inner class -------------------------------------

        /**
        * Thread that performs toString operation against
        * a UUID object.  UUID::toString causes a lazy
        * initialization of the UUID by whichever thread
        * gets there first.
        */
        class UUIDRunner
            : public class_spec<UUIDRunner,
                extends <Runner> >
            {
            friend class factory<UUIDRunner>;

            protected:
                /**
                * Create a new UUIDRunner.
                */
                UUIDRunner()
                    : m_hId(self(), NULL),
                      m_vsUUID(self(), String::null_string)
                    {
                    }

                /**
                * Initialize with the UUID.
                *
                * @param hId  the UUID to be initialized
                */
                UUIDRunner(UUID::Handle hId)
                    : m_hId(self(), hId),
                      m_vsUUID(self(), String::null_string)
                    {
                    }

                /*
                * Performs the test; the toString method causes the
                * UUID to be initialized by whichever thread gets
                * access first.
                */
                void doTest()
                    {
                    UUID::Handle hId = m_hId;
                    m_vsUUID = hId->toString();
                    }

            public:
                /**
                 * Accessor for the string initialized by the test.
                 */
                String::View getString()
                    {
                    return m_vsUUID;
                    }

            // ----- data members -------------------------------------------

            private:
                /**
                * The UUID to initialize.
                */
                MemberHandle<UUID> m_hId;

                /**
                * The UUID as string.
                */
                MemberView<String> m_vsUUID;
            };

        // ----- TestCase inner class ---------------------------------------

        /**
        * The TestCase class.
        */
        class UUIDTestCase
            : public class_spec<UUIDTestCase,
                extends<Object>,
                implements<TestCase> >
            {
            friend class factory<UUIDTestCase>;

            protected:
                /**
                * Construct a new TestCase with the specified name to be run by the
                * specified array of Runner threads.
                *
                * @param vsTestName  the name of the TestCase
                * @param haThread    the array of Thread
                * @param haRunner    the array of Runner run by the threads
                */
                UUIDTestCase(String::View vsTestName,
                             ObjectArray::Handle haThread,
                             ObjectArray::Handle haRunner)
                    : m_hAtomicThreads(self(), 0),
                      m_haThread(self(), haThread),
                      m_haRunner(self(), haRunner),
                      m_vsTestName(self(), vsTestName)
                    {

                    m_hAtomicThreads = AtomicCounter::create(0);

                    for (size32_t i = 0; i < haRunner->length; i++)
                        {
                        UUIDRunner::Handle hRunner = cast<UUIDRunner::Handle>(haRunner[i]);
                        hRunner->setTestCase(this);

                        Thread::Handle hThread = cast<Thread::Handle>(haThread[i]);
                        hThread->start();
                        }
                    }

            public:
                /**
                * Accessor for thread counter.
                */
                AtomicCounter::Handle getAtomicCounter()
                    {
                    return m_hAtomicThreads;
                    }

                /**
                * Run the TestCase on the specified UUID
                */
                void run()
                    {
                    ObjectArray::Handle   haThread       = m_haThread;
                    AtomicCounter::Handle hAtomicThreads = m_hAtomicThreads;

                    while (hAtomicThreads->getCount() < haThread->length)
                        {
                        // COH-9751: wait for all runner threads to start so we don't miss
                        // a notification
                        Thread::sleep(100L);
                        }

                        // ready... set...  go!
                    COH_SYNCHRONIZED(this)
                        {
                        notifyAll();

                        try
                            {
                            /* wait for all the test threads to finish */
                            wait(0);
                            }
                        catch (InterruptedException::View e)
                            {
                            Thread::currentThread()->interrupt();
                            throw e;
                            }
                        }
                    }

                /**
                * Shutdown the test, and stop all Runner threads.
                */
                void shutdown()
                    {
                    ObjectArray::Handle haThread = m_haThread;
                    for (size32_t i = 0; i < haThread->length; i++)
                        {
                        cast<Thread::Handle>(haThread[i])->interrupt();
                        }
                    for (size32_t i = 0; i < haThread->length; i++)
                        {
                        cast<Thread::Handle>(haThread[i])->join();
                        }
                    }

                /**
                * Used by the Runner thread to notify the TestCase that it has
                * finished a TestCase execution.
                */
                void threadIsFinished()
                    {
                    if (m_hAtomicThreads->decrement() == 0)
                        {
                        /* last thread is finishing */
                        COH_SYNCHRONIZED (this)
                            {
                            notify();
                            }
                        }
                    }

            // ----- data members -------------------------------------------

            private:

                /**
                * The counter of active test threads.
                */
                MemberHandle<AtomicCounter> m_hAtomicThreads;

                /**
                * The array of test threads.
                */
                MemberHandle<ObjectArray>   m_haThread;

               /**
                * The array of test runners.
                */
                MemberHandle<ObjectArray>   m_haRunner;

                /**
                * The name of this test.
                */
                MemberView<String>          m_vsTestName;
            };

    // ----- unit tests -----------------------------------------------------

    public:
        void testConstructNoArgs()
            {
            int64_t l = System::safeTimeMillis();
            UUID::View v = UUID::create();
            TS_ASSERT(v->isGenerated());
            TS_ASSERT(v->getTimestamp() >= l);

            InetAddress::View vAddr = InetAddress::getLocalHost();
            if (!  (vAddr->getHostAddress()->equals("127.0.0.1") ||
                    vAddr->getHostAddress()->equals("0:0:0:0:0:0:0:1") ) )
                {
                TS_ASSERT(v->isAddressIncluded());
                TS_ASSERT(vAddr->getAddress()->length == v->getAddress()->length);
                for (size32_t i = 0; i < v->getAddress()->length; ++i)
                    {
                    TS_ASSERT(vAddr->getAddress()[i] ==
                            v->getAddress()[i]);
                    }
                }
            else
                {
                TS_ASSERT(!v->isAddressIncluded());
                }

            UUID::View v2 = UUID::create();
            int32_t c  = v->getCount();
            int32_t c2 = v2->getCount();
            TS_ASSERT(c + 1 == c2);
            }

        void testConstructWithArgs()
            {
            int64_t lTime = System::currentTimeMillis();
            InetAddress::View vAddr = InetAddress::getLocalHost();
            UUID::View v = UUID::create(lTime, vAddr, 12345, 67890);

            TS_ASSERT(!v->isGenerated());
            TS_ASSERT(v->isAddressIncluded());
            TS_ASSERT(lTime == v->getTimestamp());
            TS_ASSERT(12345 == v->getPort());
            TS_ASSERT(67890 == v->getCount());
            TS_ASSERT(vAddr->getAddress()->length == v->getAddress()->length);
            for (size32_t i = 0; i < v->getAddress()->length; ++i)
                {
                TS_ASSERT(vAddr->getAddress()[i] ==
                        v->getAddress()[i]);
                }
            }

        void testConstructWithArgs2()
            {
            int64_t lTime = System::currentTimeMillis();
            InetAddress::View vAddr = InetAddress::getLocalHost();
            UUID::View v = UUID::create(lTime, vAddr->getAddress(), 12345, 67890);

            TS_ASSERT(!v->isGenerated());
            TS_ASSERT(v->isAddressIncluded());
            TS_ASSERT(lTime == v->getTimestamp());
            TS_ASSERT(12345 == v->getPort());
            TS_ASSERT(67890 == v->getCount());
            TS_ASSERT(vAddr->getAddress()->length == v->getAddress()->length);
            for (size32_t i = 0; i < v->getAddress()->length; ++i)
                {
                TS_ASSERT(vAddr->getAddress()[i] ==
                        v->getAddress()[i]);
                }
            }

        void testConstructFromBytes()
            {
            UUID::View v = UUID::create();
            Array<octet_t>::View vab = v->toByteArray();

            UUID::View v2 = UUID::create(vab);
            TS_ASSERT(v->equals(v2));
            }

        void testConstructFromString()
            {
            UUID::View v = UUID::create();
            String::View vs = v->toString();
            UUID::View v2 = UUID::create(vs);
            TS_ASSERT(v->equals(v2));
            }

        void testEquals()
            {
            // check NULL arg
            UUID::View v = UUID::create();
            TS_ASSERT(!v->equals(NULL));

            // 2 generated UUID's should not be equal
            UUID::View v2 = UUID::create();
            TS_ASSERT(!v->equals(v2));
            TS_ASSERT(!v2->equals(v));

            // UUID should be equal to itself
            UUID::View v3 = UUID::View(v);
            TS_ASSERT(v->equals(v3));
            TS_ASSERT(v3->equals(v));

            // 2 UUID's with the same properties should be equal
            int64_t lTime = 234234234;
            InetAddress::View vAddr = InetAddress::getLocalHost();
            UUID::View v4 = UUID::create(lTime, vAddr, 12345, 67890);
            UUID::View v5 = UUID::create(lTime, vAddr, 12345, 67890);
            TS_ASSERT(v4->equals(v5));
            TS_ASSERT(v5->equals(v4));

            // create with inter address in byte array form
            UUID::View v6 = UUID::create(lTime, vAddr->getAddress(), 12345, 67890);
            TS_ASSERT(v6->equals(v5));
            TS_ASSERT(v5->equals(v6));

            // negative tests
            // different time
            UUID::View v7 = UUID::create(12121212, vAddr, 12345, 67890);
            TS_ASSERT(!v4->equals(v7));
            TS_ASSERT(!v7->equals(v4));

            // different port
            UUID::View v8 = UUID::create(lTime, vAddr, 9000, 67890);
            TS_ASSERT(!v4->equals(v8));
            TS_ASSERT(!v8->equals(v4));

            // different count
            UUID::View v9 = UUID::create(lTime, vAddr, 12345, 34343434);
            TS_ASSERT(!v4->equals(v9));
            TS_ASSERT(!v9->equals(v4));
            }

        void testHashCode()
            {
            UUID::View v = UUID::create();
            TS_ASSERT(v->hashCode() != 0);

            int64_t lTime = System::currentTimeMillis();
            InetAddress::View vAddr = InetAddress::getLocalHost();
            UUID::View v2 = UUID::create(lTime, vAddr, 12345, 67890);
            UUID::View v3 = UUID::create(lTime, vAddr, 12345, 67890);
            TS_ASSERT(v2->hashCode() == v3->hashCode());

            UUID::View v4 = UUID::create(lTime, vAddr->getAddress(), 12345, 67890);
            TS_ASSERT(v4->hashCode() == v3->hashCode());
            }

        void testCompareTo()
            {
            int64_t lTime = System::currentTimeMillis();
            InetAddress::View vAddr = InetAddress::getLocalHost();
            // identical UUID's
            UUID::View v  = UUID::create(lTime, vAddr, 12345, 67890);
            UUID::View v1  = UUID::create(lTime, vAddr, 12345, 67890);

            //port decremented
            UUID::View v2 = UUID::create(lTime, vAddr, 12344, 67890);
            // count incremented
            UUID::View v3 = UUID::create(lTime, vAddr, 12345, 67891);
            // time incremented
            UUID::View v4  = UUID::create(lTime + 1, vAddr, 12345, 67890);

            TS_ASSERT(v->compareTo(v1) == 0);
            TS_ASSERT(v1->compareTo(v2) == 1);
            TS_ASSERT(v1->compareTo(v3) == -1);
            TS_ASSERT(v1->compareTo(v4) == -1);
            }

        //TODO
        void XXXtestReadWriteExternal()
            {
            // once PofWriter/Reader are implemented:
            // uuid1->writeExternal(pofWriter)
            // uuid2(readExternal(pofReader)
            // uuid1.equals(uuid2)
            }

        //TODO
        void XXXtestReadExternalThrowsException()
            {
            }

        /**
         * Test UUID construction by multiple threads. Creates multiple
         * threads which contend to initialize a single UUID instance.
         *
         * The unfixed issue (COH-11868) can be reproduced with this unit test
         * in about 30% of executions of this unit test when 10,000 threads
         * are created, on a Windows machine.  (Linux could only create about
         * 1000 threads, so can't be used to reproduce this issue).
         */
        void testUUIDConflict()
            {
            ObjectArray::Handle haThread = ObjectArray::create(NUM_THREADS);
            ObjectArray::Handle haRunner = ObjectArray::create(NUM_THREADS);
            UUID::Handle hU = UUID::create();
            for (int32_t i = 0; i < NUM_THREADS; ++i)
                {
                haRunner[i] = UUIDRunner::create(hU);
                haThread[i] = Thread::create(cast<UUIDRunner::Handle>(haRunner[i]));
                (cast<Thread::Handle>(haThread[i]))->setName(COH_TO_STRING("UUIDRunner(" << i << ")"));
                }

            TestCase::Handle hTest = UUIDTestCase::create("testUUIDConflict", haThread, haRunner);

            struct Finally
                {
                Finally(TestCase::Handle _hTest)
                    : hTest(_hTest)
                    {
                    }

                ~Finally()
                    {
                    hTest->shutdown();
                    }

                TestCase::Handle hTest;
                } finally(hTest);

            hTest->run();

            TS_ASSERT(stringResult(haRunner) == true);
            }

    private:
        /**
         * Verify that the thread didn't collide while
         * initializing UUID.  The string returned by
         * each test case thread should be the same,
         * because they are all acting on the same
         * UUID instance.  If a thread is able to
         * reinitialize the UUID's data, the fix has failed.
         *
         * @param haRunner  Array of Runner tasks
         */
        bool stringResult(ObjectArray::Handle haRunner)
            {
            bool fResult = true;
            for (size32_t i = 0; i < haRunner->length - 1; i++)
                {
                String::View vStr1 = (cast<UUIDRunner::Handle>(haRunner[i]))->getString();
                String::View vStr2 = (cast<UUIDRunner::Handle>(haRunner[i+1]))->getString();
                if (!vStr1->equals(vStr2))
                    {
                    std::cout << "UUIDTest::testUUIDConflict; comparison failed: " << i << " vs " << (i+1) << std::endl;
                    fResult = false;
                    break;
                    }
                }

            return fResult;
            }

    // ----- data members ---------------------------------------------------

    public:
        /**
        * The number of concurrent test threads.
        */
        static const int32_t NUM_THREADS = 5;
    };
