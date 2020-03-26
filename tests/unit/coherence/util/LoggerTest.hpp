/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/run/xml/SimpleElement.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/util/logging/Standard.hpp"

#include <iostream>
#include <ctime>

using namespace coherence::lang;
using namespace coherence::util::logging;
using namespace coherence::run::xml;

/**
* Test Suite for the Logger.
*/
class LoggerTest : public CxxTest::TestSuite
    {
    public:
        void setUp()
            {
            mteststream = new ostringstream;
            Logger::Handle hLogger = Logger::getLogger();
            if (hLogger->isStarted())
                {
                hLogger->shutdown();
                hLogger = Logger::getLogger();
                }
            nOrigLogLevel = hLogger->getLevel();
            cast<Standard::Handle>(hLogger->getLogOutput())->setOutputStream(mteststream);
            hLogger->setLevel(Logger::level_all);
            }

        void tearDown()
            {
            // Note we don't have to delete the stream because the output will
            // on destruction
            }

        void testCreate()
            {
            Logger::Handle hLogger = Logger::create();
            TS_ASSERT(Logger::daemon_initial == hLogger->getDaemonState());
            }

        void testIsEnabled()
            {
            Logger::Handle myLogger = Logger::create();
            myLogger->setLevel(3);
            TS_ASSERT(myLogger->isEnabled(1) == true);
            TS_ASSERT(myLogger->isEnabled(2) == true);
            TS_ASSERT(myLogger->isEnabled(3) == true);
            TS_ASSERT(myLogger->isEnabled(4) == false);
            TS_ASSERT(myLogger->isEnabled(5) == false);
            }

        void testLogLevel()
            {
            Logger::Handle hLogger = Logger::getLogger();

            streambuf* pOrigCerr = setupLogger();
            hLogger->setLevel(3);
            COH_LOG("This is a test", 4);
            resetLogger(pOrigCerr);

            TS_ASSERT(mteststream->str().find("This is a test") == string::npos);
            }

        void testMacroLoggerNotStarted()
            {
            // sanity check
            TS_ASSERT(! Logger::getLogger()->isStarted());

            streambuf* pOrigCerr = setupLogger();
            Logger::getLogger()->setLevel(Logger::level_all);

            COH_LOG("THIS IS A TEST OF THE SYSTEM LOGGER", 1);

            resetLogger(pOrigCerr);

            TS_ASSERT(mteststream->str().find(
                    "<Error> (thread=main): THIS IS A TEST OF THE SYSTEM LOGGER")
                    != string::npos);
            }

        void testMacroLoggerStarted()
            {
            Logger::getLogger()->start();
            Logger::getLogger()->setLevel(Logger::level_all);
            TS_ASSERT(Logger::getLogger()->isEnabled(1));
            TS_ASSERT(Logger::isEnabledSingleton(1));
            COH_LOG("THIS IS A TEST OF THE SYSTEM LOGGER", 1);
            TS_ASSERT(waitForMessage(
                    "<Error> (thread=main): THIS IS A TEST OF THE SYSTEM LOGGER"));
            }

        void testMacroLogExLoggerNotStarted()
            {
            streambuf* pOrigCerr = setupLogger();
            Logger::getLogger()->setLevel(1);
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (NullPointerException::View npe)
                {
                Logger::getLogger()->setLevel(Logger::level_all);
                COH_LOGEX((Exception::View)npe, 1);
                resetLogger(pOrigCerr);

                TS_ASSERT(waitForMessage("<Error>"));
                TS_ASSERT(waitForMessage("coherence::lang::NullPointerException"));
                }
            }

        void testMacroLogExLoggerStarted()
            {
            streambuf* pOrigCerr = setupLogger();
            Logger::getLogger()->setLevel(1);
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (NullPointerException::View npe)
                {
                Logger::getLogger()->start();
                Logger::getLogger()->setLevel(Logger::level_all);
                COH_LOGEX((Exception::View)npe, 1);
                resetLogger(pOrigCerr);

                TS_ASSERT(waitForMessage("<Error>"));
                TS_ASSERT(waitForMessage("coherence::lang::NullPointerException"));
                }
            }

        void testMacroLogExMsgLoggerNotStarted()
            {
            streambuf* pOrigCerr = setupLogger();
            Logger::getLogger()->setLevel(1);
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (NullPointerException::View npe)
                {
                Logger::getLogger()->setLevel(Logger::level_all);
                COH_LOGEXMSG((Exception::View) npe, "TESTMSG LEVEL " << 1, 1);
                resetLogger(pOrigCerr);
                TS_ASSERT(mteststream->str().find("<Error> (thread=main): TESTMSG LEVEL 1") != string::npos);
                TS_ASSERT(mteststream->str().find("coherence::lang::NullPointerException")  != string::npos);
                }
            }

        void testMacroLogExMsgLoggerStarted()
            {
            streambuf* pOrigCerr = setupLogger();
            Logger::getLogger()->setLevel(1);
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (NullPointerException::View npe)
                {
                Logger::getLogger()->start();
                Logger::getLogger()->setLevel(Logger::level_all);
                COH_LOGEXMSG((Exception::View)npe, "TESTMSG LEVEL " << 1, 1);
                resetLogger(pOrigCerr);

                TS_ASSERT(waitForMessage("<Error> (thread=main): TESTMSG LEVEL 1"));
                TS_ASSERT(waitForMessage("coherence::lang::NullPointerException"));
                }
            }

        void TODOtestLog()
            {
            Logger::Handle hLogger = Logger::getLogger();
            hLogger->getQueue()->flush();
            hLogger->log(1, "This is a test", NULL);
            TS_ASSERT(Logger::getLogger()->getQueue()->size() == 1);

            ObjectArray::View voaMsg = cast<ObjectArray::View>
                    (Logger::getLogger()->getQueue()->peekNoWait());

            TS_ASSERT(voaMsg[0] != NULL);
            TS_ASSERT(1 == cast<Integer32::View>(voaMsg[1])->getInt32Value());
            TS_ASSERT(Thread::currentThread() == cast<Thread::Handle>(voaMsg[2]));
            TS_ASSERT(NULL == voaMsg[3]);
            TS_ASSERT(cast<String::View>(voaMsg[4])->equals("This is a test"));

            TS_ASSERT(mteststream->str().find("<Error> (thread=main): This is a test\n") != string::npos);
            }

        void TODOtestLogExceptionNoMsg()
            {
            Logger::Handle hLogger = Logger::getLogger();
            hLogger->getQueue()->flush();
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (NullPointerException::View npe)
                {
                mteststream->str("");
                hLogger->log(1, (Exception::View) npe, NULL);

                ObjectArray::View voaMsg = cast<ObjectArray::View>
                    (hLogger->getQueue()->peekNoWait());
                TS_ASSERT(voaMsg[0] != NULL);
                TS_ASSERT(1 == cast<Integer32::View>(voaMsg[1])->getInt32Value());
                TS_ASSERT(Thread::currentThread() == cast<Thread::Handle>(voaMsg[2]));
                TS_ASSERT(npe == voaMsg[3]);
                TS_ASSERT(NULL == voaMsg[4]);

                TS_ASSERT(mteststream->str().find("<Error>") != string::npos);
                TS_ASSERT(mteststream->str().find("coherence::lang::NullPointerException") != string::npos);
                }
            }

        void TODOtestLogExceptionMsg()
            {
            Logger::Handle hLogger = Logger::getLogger();
            hLogger->getQueue()->flush();
            try
                {
                COH_THROW(UnsupportedOperationException::create("This test operation is unsupported"));
                }
            catch (UnsupportedOperationException::View usoe)
                {
                hLogger->log(1, (Exception::View) usoe, "Caught UnsupportedOperationException:", NULL);

                ObjectArray::View voaMsg = cast<ObjectArray::View>
                    (hLogger->getQueue()->peekNoWait());
                TS_ASSERT(voaMsg[0] != NULL);
                TS_ASSERT(1 == cast<Integer32::View>(voaMsg[1])->getInt32Value());
                TS_ASSERT(Thread::currentThread() == cast<Thread::Handle>(voaMsg[2]));
                TS_ASSERT(usoe == voaMsg[3]);
                TS_ASSERT(cast<String::View>(voaMsg[4])->equals("Caught UnsupportedOperationException:"));

                TS_ASSERT(mteststream->str().find(
                        "<Error> (thread=main): Caught UnsupportedOperationException:")
                        != string::npos);
                TS_ASSERT(mteststream->str().find(
                        "coherence::lang::UnsupportedOperationException: This test operation is unsupported")
                        != string::npos);
                }
            }

        void testLifecycle()
            {
            Logger::Handle hLogger = Logger::getLogger();
            TS_ASSERT(Logger::daemon_initial == hLogger->getDaemonState());

            Logger::getLogger()->start();
            TS_ASSERT(Logger::daemon_running == hLogger->getDaemonState());

            Logger::getLogger()->shutdown();
        //TODO: Not a safe assertion
            //TS_ASSERT(Logger::daemon_exited == hLogger->getDaemonState());
            TS_ASSERT(Logger::daemon_initial == Logger::getLogger()->getDaemonState());
            TS_ASSERT(hLogger != Logger::getLogger());
            }


    private:
        streambuf* setupLogger()
            {
            streambuf* pOrigCerr = cerr.rdbuf();
            streambuf* pstrErr   = mteststream->rdbuf();
            cerr.rdbuf(pstrErr);
            return pOrigCerr;
            }

        void resetLogger(streambuf* stream)
            {
            cerr.rdbuf(stream);
            Logger::getLogger()->setLevel(nOrigLogLevel);
            }

        bool waitForMessage(String::View vs)
            {
            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                   System::currentTimeMillis() - nInitialTime < 10000)
                {
                fResult = mteststream->str().find(vs->getCString()) != string::npos;
                Thread::sleep(10);
                }
            if (!fResult)
                {
                std::cerr << "stream(" << mteststream->str() << ")" << std::endl;
                }
            return fResult;
            }

        // ----- Data members -----------------------------------------------
        ostringstream* mteststream;
        int32_t nOrigLogLevel;
    };
