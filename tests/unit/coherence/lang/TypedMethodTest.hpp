/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashMap.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"
#include "coherence/lang/TypedMethod.hpp"
#include "coherence/lang/Size32.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for the TypedMethod object.
*/
class TypedMethodSuite : public CxxTest::TestSuite
    {
    public:
        void testInvoke()
        {
//        Class::Handle hClass = TypedBarrenClass<Reference>::create()
//                ->declare(COH_SETTER(Reference, set, Object::Holder));


        Class::Handle hClass = TypedBarrenClass<Map>::create()
            ->declare(COH_METHOD0(BoxHandle<const Size32>,   Map::View, size))
            ->declare(COH_METHOD1(BoxHandle<const Boolean>,  Map::View, containsKey, Object::View))
            ->declare(COH_METHOD2(Object::Holder, Map::Handle, put, Object::View, Object::Holder))
            ->declare(COH_METHOD0(void, Map::Handle, clear))
            ->declare(COH_METHOD1(Object::Holder, Map::View,   get, Object::View))
            ->declare(COH_METHOD1(Object::Holder, Map::Handle, get, Object::View));

        Map::Handle hMap = HashMap::create();
        String::Handle vsKey1 = "key1";
        String::Handle vsValue1 = "value1";
        String::Handle vsKey2 = "key2";
        String::Handle vsValue2 = "value2";

        hMap->put(vsKey1, vsValue1);
        hMap->put(vsKey2, vsValue2);

        ObjectArray::Handle haArgs1 = ObjectArray::create(1);
        ObjectArray::Handle haArgs2 = ObjectArray::create(2);

        TS_ASSERT(cast<Size32::View>(hClass->getDeclaredMethod("size")->invoke(hMap))->getValue() == hMap->size());

        haArgs1[0] = vsKey1;
        TS_ASSERT(cast<Boolean::View>(hClass->getDeclaredMethod("containsKey", haArgs1)->invoke(hMap, haArgs1))->getValue() == hMap->containsKey(vsKey1));

        TS_ASSERT(hClass->getDeclaredMethod("get", haArgs1)->invoke(hMap, haArgs1)->equals(hMap->get(vsKey1)));
        TS_ASSERT(instanceof<Object::View>(hClass->getDeclaredMethod("get", haArgs1, Method::modifier_const)->invoke(hMap, haArgs1)));
        TS_ASSERT(instanceof<Object::Handle>(hClass->getDeclaredMethod("get", haArgs1, Method::modifier_mutable)->invoke(hMap, haArgs1)));

        haArgs2[0] = vsKey1;
        haArgs2[1] = vsValue2;
        TS_ASSERT(hClass->getDeclaredMethod("put", haArgs2)->invoke(hMap, haArgs2)->equals(vsValue1));
        TS_ASSERT(hMap->get(vsKey1)->equals(vsValue2));

        hClass->getDeclaredMethod("clear")->invoke(hMap);
        TS_ASSERT(hMap->isEmpty());
        }
    };
