/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashMap.hpp"

#include "coherence/stl/adapter_map.hpp"

using namespace coherence::lang;
using namespace coherence::util;
using namespace coherence::stl;

/**
* Test Suite for adapter_map.
*/
class AdapterMapTest : public CxxTest::TestSuite
    {
    public:
        void testPutGet()
            {
            adapter_map<String::View, Integer32::View> m(HashMap::create());
            String::View    vsKey = "hello";
            Integer32::View vnVal = Integer32::valueOf(123);

            m[vsKey] = vnVal;
            TS_ASSERT(m[vsKey] == vnVal);
            }

        void testBoxing()
            {
            adapter_map<String::View, BoxHandle<Integer32 const> > m(HashMap::create());
            m["hello"] = 123;
            int32_t val = m["hello"];
            TS_ASSERT(123 == val);
            }
    };
