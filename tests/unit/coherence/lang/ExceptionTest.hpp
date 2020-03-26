/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/lang/Exception.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/InterruptedIOException.hpp"
#include "private/coherence/net/SocketTimeoutException.hpp"
#include <iostream>
#include <sstream>

using namespace coherence::lang;
using coherence::io::InterruptedIOException;
using coherence::io::IOException;
using coherence::net::SocketTimeoutException;
using namespace std;

/**
* Test Suite for the Boolean object.
*/
class ExceptionTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test that we can throw and catch an exception
        */
        void testCohThrow()
            {
            try
                {
                COH_THROW(CloneNotSupportedException::create("ExceptionTest cannot be cloned"));
                }
            catch (CloneNotSupportedException::View)
                {
                //cout << "CloneNotSupported CAUGHT: " << tNPE << endl;
                }
            catch (const std::exception& e)
                {
                cout << "CAUGHT STD EXCEPTION!!!" << e.what() << endl;
                TS_FAIL("Caught an incorrect exception");
                }
            }

        /**
        * Test that we can catch base Exception::View
        */
        void testCohThrowCatchBaseException()
            {
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (Exception::View)
                {
                //cout << "Exception CAUGHT!!! " << tE.what();
                }
            catch (const std::exception& e)
                {
                cout << "CAUGHT STD EXCEPTION!!!" << e.what() << endl;
                TS_FAIL("Caught an incorrect exception");
                }
            }

        /**
        * Test that we can catch the standard exception
        */
        void testCohThrowCatchStdException()
            {
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (const std::exception&)
                {
                //cout << "CAUGHT STD EXCEPTION!!!" << e.what() << endl;
                }
            catch (...)
                {
                TS_FAIL("Did not catch std::exception!!");
                }
            }

        void testCohThrowCatchWrongException()
            {
            try
                {
                COH_THROW(NullPointerException::create());
                }
            catch (IOException::View)
                {
                TS_FAIL("Caught IOException when expecting NPE!");
                }
            catch (Exception::View)
                {
                //cout << "Proper exception behavior for this test: " << e << endl;
                }
            }

        /**
        * Test that we can call COH_THROW_STREAM
        */
        void testCohThrowToString()
            {
                try
                    {
                    COH_THROW_STREAM(IOException, "File not found: " << "fakeFileName.txt");
                    }
                catch (IOException::View)
                    {
                    //cout << "IOEXCEPTION CAUGHT: " << ioe << endl;
                    }
                catch (exception&)
                    {
                    TS_FAIL("Caught an incorrect exception");
                    }
            }

        void testHierarchy()
            {
            Exception::Handle hEx = SocketTimeoutException::create();
            size32_t cCaught = 0;
            try
                {
                try
                    {
                    try
                        {
                        try
                            {
                            try
                                {
                                try
                                    {
                                    COH_THROW (hEx);
                                    }
                                catch (const std::exception&)
                                    {
                                    ++cCaught;
                                    throw;
                                    }
                                }
                            catch (const std::ios_base::failure&)
                                {
                                ++cCaught;
                                throw;
                                }
                            }
                        catch (Exception::View vEx)
                            {
                            ++cCaught;
                            TS_ASSERT(hEx == vEx);
                            COH_THROW (vEx);
                            }
                        }
                    catch (IOException::View vEx)
                        {
                        ++cCaught;
                        TS_ASSERT(hEx == vEx);
                        COH_THROW (vEx);
                        }
                    }
                catch (InterruptedIOException::View vEx)
                    {
                    ++cCaught;
                    TS_ASSERT(hEx == vEx);
                    COH_THROW (vEx);
                    }
                }
            catch (SocketTimeoutException::View vEx)
                {
                ++cCaught;
                TS_ASSERT(hEx == vEx);
                }
            TS_ASSERT(cCaught == 6);
            }
    };
