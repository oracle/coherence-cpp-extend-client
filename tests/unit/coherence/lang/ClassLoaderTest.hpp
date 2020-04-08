/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashSet.hpp"

using namespace coherence::lang;
using namespace coherence::util;

namespace class_loader_test
    {
    class RegisteredClass
        : public class_spec<RegisteredClass>
            {
            friend class factory<RegisteredClass>;
            };

    COH_REGISTER_TYPED_CLASS(RegisteredClass);
    }

/**
* Test Suite for the ClassLoader object.
*/
class ClassLoaderSuite : public CxxTest::TestSuite
    {
    public:
        void testByName()
            {
            Class::View vClass =
                SystemClassLoader::getInstance()->loadByName
                    ("class_loader_test::RegisteredClass");

            TS_ASSERT(vClass != NULL);
            }

        void testByType()
            {
            Class::View vClass =
                SystemClassLoader::getInstance()->loadByType
                    (typeid(class_loader_test::RegisteredClass));

            TS_ASSERT(vClass != NULL);
            }
    };
