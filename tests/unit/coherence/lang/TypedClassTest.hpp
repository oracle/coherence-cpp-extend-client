/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashMap.hpp"
#include "coherence/lang/TypedClass.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for the TypedClass object.
*/
class TypedClassSuite : public CxxTest::TestSuite
    {
    public:
        void testNewInstance()
        {
        Class::Handle hClass = TypedClass<HashMap>::create();
        HashMap::Handle hMap = cast<HashMap::Handle>(hClass->newInstance());
        TS_ASSERT(hMap != NULL);
        }
    };
