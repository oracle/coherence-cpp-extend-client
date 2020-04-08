/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

using namespace coherence::lang;

COH_OPEN_NAMESPACE(system_class_loader_test)
 

class EchoTest
    : public class_spec<EchoTest>
    {
    friend class factory<EchoTest>;
 
    public:
        static void main(ObjectArray::View vasArg)
            {
            for (size32_t i = 0, c = vasArg->length; i < c; ++i)
                {
                std::cout << vasArg[i] << std::endl;
                }
            }
    };

COH_REGISTER_EXECUTABLE_CLASS(EchoTest);
 
COH_CLOSE_NAMESPACE


/**
* Test Suite for the SystemClassLoader object.
*/
class SystemClassLoaderSuite : public CxxTest::TestSuite
    {
    public:

        /**
        * Test that the registration of an executable class
        * succeeded.
        */
        void testRegisterExecutableClassMacro()
            {
            Class::View vClass =
                SystemClassLoader::getInstance()->loadByName
                    ("system_class_loader_test::EchoTest");

            TS_ASSERT(vClass != NULL);
            }

    };
