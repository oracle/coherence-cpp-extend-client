/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/AtomicCounter.hpp"

using namespace coherence::lang;

using coherence::util::AtomicCounter;

/**
* Test Suite for the ObjectArray object.
*/
class ObjectArrayTest : public CxxTest::TestSuite
    {
    public:

    void testAllocation()
        {
        ObjectArray::Handle hArr = ObjectArray::create(1);
        TS_ASSERT(hArr[0] == NULL);
        }

    void testIndex()
        {
        ObjectArray::Handle hArr = ObjectArray::create(3);
        for (size32_t i = 0, c = hArr->length; i < c; ++i)
            {
            hArr[i] = Integer32::create(i);
            }

        ObjectArray::View vArr = hArr;
        for (size32_t i = 0, c = hArr->length; i < c; ++i)
            {
            TS_ASSERT(vArr[i] == vArr[i]);
            }

        try
            {
            Object::View v = hArr[123];
            TS_ASSERT(false);
            }
        catch (IndexOutOfBoundsException::View)
            {
            //std::cerr << e << std::endl;
            }
        }

    void testAssignment()
        {
        size32_t            cElements = 10;
        ObjectArray::Handle hArr      = ObjectArray::create(cElements);

        for (size32_t i = 0; i < cElements; ++i)
            {
            hArr[i] = Integer32::create(i);
            }

        for (size32_t i = 0, c = hArr->length; i < c; ++i)
            {
            TS_ASSERT(cast<Integer32::Handle>(hArr[i])->getValue() == (int32_t) i);
            }
        }

    void testArrayCopy()
        {
        size32_t            cElements = 10;
        ObjectArray::Handle hArr      = ObjectArray::create(cElements);

        for (size32_t i = 0; i < cElements; ++i)
            {
            hArr[i] = Integer32::create(i);
            }

        ObjectArray::Handle hArr2 = ObjectArray::copy(hArr, 0, ObjectArray::create(hArr->length));

        for (size32_t i = 0, c = hArr2->length; i < c; ++i)
            {
            TS_ASSERT(hArr2[i] == hArr[i]); // ensure shallow copy
            }
        }

    void testArrayCopyOverlap()
        {
        size32_t            cElements = 10;
        ObjectArray::Handle hArr      = ObjectArray::create(cElements);

        for (size32_t i = 0; i < cElements; ++i)
            {
            hArr[i] = Integer32::create(i);
            }

        ObjectArray::copy(hArr, 2, hArr, 0); // shift values to the left

        TS_ASSERT(cast<Integer32::View>(hArr[0])->getValue() == 2);
        TS_ASSERT(cast<Integer32::View>(hArr[1])->getValue() == 3);
        TS_ASSERT(cast<Integer32::View>(hArr[2])->getValue() == 4);
        TS_ASSERT(cast<Integer32::View>(hArr[3])->getValue() == 5);
        TS_ASSERT(cast<Integer32::View>(hArr[4])->getValue() == 6);
        TS_ASSERT(cast<Integer32::View>(hArr[5])->getValue() == 7);
        TS_ASSERT(cast<Integer32::View>(hArr[6])->getValue() == 8);
        TS_ASSERT(cast<Integer32::View>(hArr[7])->getValue() == 9);

        TS_ASSERT(cast<Integer32::View>(hArr[8])->getValue() == 8); // unmodified
        TS_ASSERT(cast<Integer32::View>(hArr[9])->getValue() == 9); // unmodified
        }

    void testClone()
        {
        size32_t            cElements = 10;
        ObjectArray::Handle hArr      = ObjectArray::create(cElements);

        for (size32_t i = 0; i < cElements; ++i)
            {
            hArr[i] = COH_TO_STRING(i);
            }

        ObjectArray::Handle hArr2 = cast<ObjectArray::Handle>(hArr->clone());

        for (size32_t i = 0, c = hArr->length; i < c; ++i)
            {
            TS_ASSERT(cast<String::Handle>(hArr2[i])->equals(
                    COH_TO_STRING(i)));
            TS_ASSERT(hArr2[i] != hArr[i]); // ensure deep clone
            }
        }

    void testEquality()
        {
        size32_t            cElements = 10;
        ObjectArray::Handle hArr      = ObjectArray::create(cElements);

        for (size32_t i = 0; i < cElements; ++i)
            {
            hArr[i] = String::create("test");
            }

        ObjectArray::Handle hArr2 = cast<ObjectArray::Handle>(hArr->clone()); // deep copy

        TS_ASSERT(hArr2->equals(hArr));
        TS_ASSERT(hArr2->hashCode() == hArr->hashCode());
        }

    void testImmutability()
        {
        Object::Handle hA;
        Object::Handle hB;
            {
            hA = AtomicCounter::create(123);
            hB = AtomicCounter::create(456);
            }
        ObjectArray::Handle hArr = ObjectArray::create(2);
        TS_ASSERT(hArr->isImmutable() == false);
        hArr[0] = (Object::View) hA;
        hArr[1] = (Object::View) hB;
        TS_ASSERT(hArr->isImmutable() == false);
        ObjectArray::View vArr;
            {
            vArr = cast<ObjectArray::View>(hArr->clone());
            }
        TS_ASSERT(vArr->isImmutable() == true);
        vArr = hArr;
        hArr = NULL;
        TS_ASSERT(vArr->isImmutable() == false);
        hA = hB = NULL;
        TS_ASSERT(vArr->isImmutable() == true);
        }
    };

