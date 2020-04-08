/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/TypedExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::TypedExtractor;
using coherence::util::filter::EqualsFilter;

/**
* Test Suite for  TypedExtractor.
*/
class TypedExtractorSuite : public CxxTest::TestSuite
    {
    public:
         /**
        * Test TypedExtractor
        */
        void testTypedExtractor()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(123), Integer32::create(456));
            Collection::View vValues = ((Map::View) hMap)->values();

            ReflectionExtractor::View vExt =
                COH_TYPED_EXTRACTOR(Collection::View, Map, values);
            TS_ASSERT(vExt->getMethodName()->equals("values"));

            EqualsFilter::Handle hFilter = EqualsFilter::create(
                    vExt, vValues);
            TS_ASSERT(hFilter->evaluate(hMap));
            try
                {
                hFilter->evaluate(Integer32::create(123));

                TS_ASSERT(false);
                }
            catch (ClassCastException::View e)
                {
                }
            }
    };
