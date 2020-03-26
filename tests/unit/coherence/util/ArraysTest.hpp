/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/Arrays.hpp"
#include "private/coherence/util/StringHelper.hpp"


using namespace coherence::lang;
using namespace coherence::util;



class ArraysTestStringComparator
    : public class_spec<ArraysTestStringComparator,
        extends<Object>,
        implements<Comparator> >
{
friend class factory<ArraysTestStringComparator>;

 virtual int32_t compare(Object::View v1, Object::View v2) const
     {
     return StringHelper::compare(cast<String::View>(v1), cast<String::View>(v2), true);
     }
};

/**
* Test Suite for the Collections object.
*/
class ArraysTest : public CxxTest::TestSuite
        {
        public:
        /**
        * Test copy
        */
        void testCopy()
            {
            ObjectArray::Handle hArr  = ObjectArray::create(8);
            ObjectArray::Handle hArr2 = ObjectArray::create(8);

            hArr[0] = String::create("AAAA");
            hArr[1] = String::create("BBBB");
            hArr[2] = String::create("CCCC");
            hArr[3] = String::create("DDDD");
            hArr[4] = String::create("EEEE");
            hArr[5] = String::create("FFFF");
            hArr[6] = String::create("GGGG");
            hArr[7] = String::create("HHHH");

            Arrays::copy(hArr, 0, hArr2, 0, hArr->length);

            TS_ASSERT(hArr2[0]->equals(String::create("AAAA")));
            TS_ASSERT(hArr2[1]->equals(String::create("BBBB")));
            TS_ASSERT(hArr2[2]->equals(String::create("CCCC")));
            TS_ASSERT(hArr2[3]->equals(String::create("DDDD")));
            TS_ASSERT(hArr2[4]->equals(String::create("EEEE")));
            TS_ASSERT(hArr2[5]->equals(String::create("FFFF")));
            TS_ASSERT(hArr2[6]->equals(String::create("GGGG")));
            TS_ASSERT(hArr2[7]->equals(String::create("HHHH")));
            }

        /**
        * Test copy
        */
        void testCopyPartial()
            {
            ObjectArray::Handle hArr  = ObjectArray::create(8);
            ObjectArray::Handle hArr2 = ObjectArray::create(4);

            hArr[0] = String::create("AAAA");
            hArr[1] = String::create("BBBB");
            hArr[2] = String::create("CCCC");
            hArr[3] = String::create("DDDD");
            hArr[4] = String::create("EEEE");
            hArr[5] = String::create("FFFF");
            hArr[6] = String::create("GGGG");
            hArr[7] = String::create("HHHH");

            Arrays::copy(hArr, 2, hArr2, 0, 4);

            TS_ASSERT(hArr2[0]->equals(String::create("CCCC")));
            TS_ASSERT(hArr2[1]->equals(String::create("DDDD")));
            TS_ASSERT(hArr2[2]->equals(String::create("EEEE")));
            TS_ASSERT(hArr2[3]->equals(String::create("FFFF")));
            }

        void testReverse()
            {
            // empty array
            Arrays::reverse(ObjectArray::create(0));

            // singleton
            ObjectArray::Handle hao = ObjectArray::create(1);
            hao[0] = Integer32::create(1);
            Arrays::reverse(hao);
            TS_ASSERT_EQUALS(cast<Integer32::View>(hao[0])->getInt32Value(), int32_t(1));

            hao = ObjectArray::create(1000);
            for (size32_t i = 0; i < hao->length; ++i)
                {
                hao[i] = Integer32::create(i);
                }

            Arrays::reverse(hao);

            for (size32_t i = 0; i < hao->length; ++i)
                {
                TS_ASSERT_EQUALS(cast<Integer32::View>(hao[i])->getInt32Value(), int32_t((hao->length - 1) - i));
                }
            }

        /**
        * Test sort
        */
        void testSort()
            {
            ObjectArray::Handle hArr = ObjectArray::create(8);

            hArr[0] = String::create("EEEE");
            hArr[1] = String::create("BBBB");
            hArr[2] = String::create("FFFF");
            hArr[3] = String::create("DDDD");
            hArr[4] = String::create("GGGG");
            hArr[5] = String::create("AAAA");
            hArr[6] = String::create("HHHH");
            hArr[7] = String::create("CCCC");

            Arrays::sort(hArr);

            TS_ASSERT(hArr[0]->equals(String::create("AAAA")));
            TS_ASSERT(hArr[1]->equals(String::create("BBBB")));
            TS_ASSERT(hArr[2]->equals(String::create("CCCC")));
            TS_ASSERT(hArr[3]->equals(String::create("DDDD")));
            TS_ASSERT(hArr[4]->equals(String::create("EEEE")));
            TS_ASSERT(hArr[5]->equals(String::create("FFFF")));
            TS_ASSERT(hArr[6]->equals(String::create("GGGG")));
            TS_ASSERT(hArr[7]->equals(String::create("HHHH")));
            }

        /**
        * Test sort with a comparator
        */
        void testSortWithComparator()
            {
            ArraysTestStringComparator::Handle hComparator =
                    ArraysTestStringComparator::create();

            ObjectArray::Handle hArr = ObjectArray::create(8);

            hArr[0] = String::create("EEEE");
            hArr[1] = String::create("BBBB");
            hArr[2] = String::create("FFFF");
            hArr[3] = String::create("DDDD");
            hArr[4] = String::create("GGGG");
            hArr[5] = String::create("AAAA");
            hArr[6] = String::create("HHHH");
            hArr[7] = String::create("CCCC");

            Arrays::sort(hArr, hComparator);

            TS_ASSERT(hArr[0]->equals(String::create("AAAA")));
            TS_ASSERT(hArr[1]->equals(String::create("BBBB")));
            TS_ASSERT(hArr[2]->equals(String::create("CCCC")));
            TS_ASSERT(hArr[3]->equals(String::create("DDDD")));
            TS_ASSERT(hArr[4]->equals(String::create("EEEE")));
            TS_ASSERT(hArr[5]->equals(String::create("FFFF")));
            TS_ASSERT(hArr[6]->equals(String::create("GGGG")));
            TS_ASSERT(hArr[7]->equals(String::create("HHHH")));
            }

        void testSwap()
            {
            ObjectArray::Handle hao = ObjectArray::create(10);

            for (size32_t i = 0; i < hao->length; ++i)
                {
                hao[i] = Integer32::create(i);
                }

            Arrays::swap(hao, 0, 1);

            TS_ASSERT_EQUALS(cast<Integer32::View>(hao[0])->getInt32Value(), 1);
            TS_ASSERT_EQUALS(cast<Integer32::View>(hao[1])->getInt32Value(), 0);

            TS_ASSERT_THROWS(Arrays::swap(hao,  0, 10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(Arrays::swap(hao, 10,  0), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(Arrays::swap(hao, -1, -1), IndexOutOfBoundsException::View);
            }

         /**
         * Test sort
         */
         void testBinarySearch()
             {
             ObjectArray::Handle hArr = ObjectArray::create(8);

             hArr[0] = String::create("AAAA");
             hArr[1] = String::create("BBBB");
             hArr[2] = String::create("CCCC");
             hArr[3] = String::create("DDDD");
             hArr[4] = String::create("FFFF");
             hArr[5] = String::create("GGGG");
             hArr[6] = String::create("HHHH");
             hArr[7] = String::create("IIII");

             int32_t nIndex = Arrays::binarySearch(hArr, 0, hArr->length,
                     String::create("CCCC"));

             TS_ASSERT(nIndex == 2);

             nIndex = Arrays::binarySearch(hArr, 0, hArr->length,
                     String::create("EEEE"));

             TS_ASSERT(nIndex == -5);
             }

         /**
         * Test sort
         */
         void testBinarySearchWithComparator()
             {
             ArraysTestStringComparator::Handle hComparator =
                     ArraysTestStringComparator::create();

             ObjectArray::Handle hArr = ObjectArray::create(8);

             hArr[0] = String::create("AAAA");
             hArr[1] = String::create("BBBB");
             hArr[2] = String::create("CCCC");
             hArr[3] = String::create("DDDD");
             hArr[4] = String::create("FFFF");
             hArr[5] = String::create("GGGG");
             hArr[6] = String::create("HHHH");
             hArr[7] = String::create("IIII");

             int32_t nIndex = Arrays::binarySearch(hArr, 0, hArr->length,
                     String::create("CCCC"), hComparator);

             TS_ASSERT(nIndex == 2);

             nIndex = Arrays::binarySearch(hArr, 0, hArr->length,
                     String::create("EEEE"), hComparator);

             TS_ASSERT(nIndex == -5);
             }
        };

