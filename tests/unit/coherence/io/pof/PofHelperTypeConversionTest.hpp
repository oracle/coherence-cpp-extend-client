/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/pof/PofConstants.hpp"
#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/HashMap.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::pof::PofConstants;
using coherence::io::pof::RawDate;
using coherence::util::ArrayList;
using coherence::util::Binary;
using coherence::util::HashMap;

#include <limits>

/**
* Test suite for the POF Helper type conversion methods.
*/
class PofHelperTypeConversionTest : public CxxTest::TestSuite
    {
    private:
        SystemPofContext::View        vCtx;

    public:
        /**
        * Test the POF Ids.
        */
        void testPofTypeId()
            {
            vCtx = SystemPofContext::getInstance();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            String::View vClassName = Class::getClassName(Boolean::create(true));
            TS_ASSERT(PofConstants::t_boolean == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_boolean == PofHelper::getPofTypeId(vLoader->loadByType(typeid(bool)), vCtx));

            vClassName = Class::getClassName(Octet::create(42));
            TS_ASSERT(PofConstants::t_octet == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_octet == PofHelper::getPofTypeId(vLoader->loadByType(typeid(octet_t)), vCtx));

            vClassName = Class::getClassName(Character16::create('a'));
            TS_ASSERT(PofConstants::t_char == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_char == PofHelper::getPofTypeId(vLoader->loadByType(typeid(wchar16_t)), vCtx));

            vClassName = Class::getClassName(Integer16::create(numeric_limits<int16_t>::min()));
            TS_ASSERT(PofConstants::t_int16 == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_int16 == PofHelper::getPofTypeId(vLoader->loadByType(typeid(int16_t)), vCtx));

            vClassName = Class::getClassName(Integer32::create(-1));
            TS_ASSERT(PofConstants::t_int32 == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_int32 == PofHelper::getPofTypeId(vLoader->loadByType(typeid(int32_t)), vCtx));

            vClassName = Class::getClassName(Integer64::create(numeric_limits<long>::max()));
            TS_ASSERT(PofConstants::t_int64 == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_int64 == PofHelper::getPofTypeId(vLoader->loadByType(typeid(int64_t)), vCtx));

            vClassName = Class::getClassName(Float32::create(3.14F));
            TS_ASSERT(PofConstants::t_float32 == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_float32 == PofHelper::getPofTypeId(vLoader->loadByType(typeid(float32_t)), vCtx));

            vClassName = Class::getClassName(Float64::create(3.14));
            TS_ASSERT(PofConstants::t_float64 == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            TS_ASSERT(PofConstants::t_float64 == PofHelper::getPofTypeId(vLoader->loadByType(typeid(float64_t)), vCtx));

            vClassName = Class::getClassName(RawDate::create(2008, 6, 5));
            TS_ASSERT(PofConstants::t_date == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));

            vClassName = Class::getClassName(String::create("this is a test"));
            TS_ASSERT(PofConstants::t_char_string == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));

            vClassName = Class::getClassName(Array<int32_t>::create(3));
            TS_ASSERT(PofConstants::t_uniform_array == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));

            vClassName = Class::getClassName(ObjectArray::create(5));
            TS_ASSERT(PofConstants::t_array == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));

            vClassName = Class::getClassName(ArrayList::create());
            TS_ASSERT(PofConstants::t_collection == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));

            vClassName = Class::getClassName(HashMap::create(2));
            TS_ASSERT(PofConstants::t_map == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));

            vClassName = Class::getClassName(Binary::create(Array<octet_t>::create(3), 0, 0));
            TS_ASSERT(PofConstants::t_octet_string == PofHelper::getPofTypeId(vLoader->loadByName(vClassName), vCtx));
            }
    };
