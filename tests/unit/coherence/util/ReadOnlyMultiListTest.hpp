/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/ReadOnlyMultiList.hpp"

using coherence::util::ReadOnlyArrayList;
using coherence::util::ReadOnlyMultiList;

class ReadOnlyMultiListSuite : public CxxTest::TestSuite
    {
    public:
        // ----- test methods -----------------------------------------------

        /**
        * Test a "small" sized {@link ReadOnlyMultiList}.
        */
        void testSmallArray()
            {
            ObjectArray::Handle aL = _makeArray(10);
            _test(_makeList(aL), aL);
            }

        /**
        * Test a "medium" sized {@link ReadOnlyMultiList}.
        */
        void mediumArray()
            {
            ObjectArray::Handle aL = _makeArray(1000);
            _test(_makeList(aL), aL);
            }

        /**
        * Test a "large" sized {@link ReadOnlyMultiList}.
        */
        void largeArray()
            {
            ObjectArray::Handle aL = _makeArray(10000);
            _test(_makeList(aL), aL);
            }


        // ----- internal test methods ------------------------------------------

        List::Handle _makeList(ObjectArray::Handle aL)
            {
            size32_t iBreak[] = {3, 45, 45, 123, 436, 684, 1027, 5326, 8132, 9999, Integer32::max_value};

            // compute number of sub-arrays to create
            size32_t ca = 1;
            size32_t cL = aL->length;
            for (size32_t i = 0; iBreak[i] < cL; ++i, ++ca);

            ObjectArray::Handle aaL = ObjectArray::create(ca);
            size32_t cpLast = 0;
            for (size32_t i = 0; i < ca; ++i)
                {
                size32_t cp = (i == ca - 1)
                    ? cL - cpLast
                    : iBreak[i] - cpLast;

                aaL[i] = ObjectArray::copy(aL, cpLast, ObjectArray::create(cp));
                cpLast += cp;
                }

            return ReadOnlyMultiList::create(aaL);
            }

        /**
        * Make an array of a given number of Integer64 elements
        *
        * @param c  the number of elements
        *
        * @return  the list
        */
        static ObjectArray::Handle _makeArray(size32_t c)
            {
            TS_ASSERT(c > 0);
            ObjectArray::Handle aL = ObjectArray::create(c);
            for (size32_t i = 0; i < c; ++i)
                {
                aL[i] = Integer64::create(i);
                }

            return aL;
            }

        /**
        * Run a variety of tests of the following methods on a new
        * ReadOnlyMultiList of the specified size:
        * <ul>
        *   <li>{@link ReadOnlyMultiList#size}</li>
        *   <li>{@link ReadOnlyMultiList#iterator}</li>
        *   <li>{@link ReadOnlyMultiList#get}</li>
        *   <li>{@link ReadOnlyMultiList#contains}</li>
        *   <li>{@link ReadOnlyMultiList#indexOf}</li>
        *   <li>{@link ReadOnlyMultiList#lastIndexOf}</li>
        *   <li>{@link ReadOnlyMultiList#toArray}</li>
        * </ul>
        *
        * @param list  the list to test
        * @param aL    the array to validate against
        */
        static void _test(List::Handle list, ObjectArray::Handle aL)
            {
            size32_t c = aL->length;

            // size
            TS_ASSERT(c == list->size());

            // iterator
            {
            size32_t i = 0;
            Iterator::Handle iter = list->iterator();
            for (; iter->hasNext(); )
                {
                Integer64::View L = cast<Integer64::View>(iter->next());
                TS_ASSERT(L == aL[i++]);
                }
            try
                {
                iter->next();
                TS_ASSERT(false);
                }
            catch (NoSuchElementException::View e)
                {
                // expected
                }
            }

            // get
            for (size32_t i = 0; i < c; ++i)
                {
                TS_ASSERT(list->get(i) == aL[i]);
                }

            // contains
            TS_ASSERT(!list->contains(NULL));                   // test NULL
            TS_ASSERT(!list->contains(Integer64::create(-1)));           // test !contains
            TS_ASSERT(list->contains(Integer64::create(0)) == (c != 0)); // test ->equals()
            for (size32_t i = 0; i < c; ++i)
                {
                TS_ASSERT(list->contains(aL[i]));
                }

            // indexOf
            TS_ASSERT(list->indexOf(NULL) == List::npos);                       // test NULL
            TS_ASSERT(list->indexOf(Integer64::create(-1)) == List::npos);               // test !contains
            TS_ASSERT(list->indexOf(Integer64::create(0)) == (c == 0 ? List::npos : 0)); // test ->equals()
            for (size32_t i = 0; i < c; ++i)
                {
                TS_ASSERT(list->indexOf(aL[i]) == i);
                }

            // lastIndexOf
            TS_ASSERT(list->lastIndexOf(NULL) == List::npos);                       // test NULL
            TS_ASSERT(list->lastIndexOf(Integer64::create(-1)) == List::npos);               // test !contains
            TS_ASSERT(list->lastIndexOf(Integer64::create(0)) == (c == 0 ? List::npos : 0)); // test ->equals()
            for (size32_t i = 0; i < c; ++i)
                {
                TS_ASSERT(list->lastIndexOf(aL[i]) == i);
                }

            // toArray
            {
            ObjectArray::Handle ao1 = list->toArray();
            ObjectArray::Handle aL2 = list->toArray(ObjectArray::create(0));
            ObjectArray::Handle aL3 = list->toArray(ObjectArray::create(list->isEmpty() ? 0 : list->size() - 1));
            ObjectArray::Handle aL4 = list->toArray(ObjectArray::create(list->size()));
            ObjectArray::Handle aL5 = list->toArray(ObjectArray::create(list->size() + 1));
            TS_ASSERT(aL->equals(ao1));
            TS_ASSERT(aL->equals(aL2));
            TS_ASSERT(aL->equals(aL3));
            TS_ASSERT(aL->equals(aL4));
            TS_ASSERT(!aL->equals(aL5));
            }

            if (c < 2)
                {
                return;
                }

            // test dups (changes "value index" logic)
            aL[c-1] = aL[0];
            list = ReadOnlyArrayList::create(aL);

            // contains
            TS_ASSERT(!list->contains(NULL));                   // test NULL
            TS_ASSERT(!list->contains(Integer64::create(-1)));           // test !contains
            TS_ASSERT(list->contains(Integer64::create(0)) == (c != 0)); // test ->equals()
            for (size32_t i = 0; i < c; ++i)
                {
                TS_ASSERT(list->contains(aL[i]));
                }

            // indexOf
            TS_ASSERT(list->indexOf(NULL) == List::npos);               // test NULL
            TS_ASSERT(list->indexOf(Integer64::create(-1)) == List::npos);       // test !contains
            TS_ASSERT(list->indexOf(Integer64::create(0)) == 0);         // test ->equals()
            for (size32_t i = 0; i < c-1; ++i)
                {
                TS_ASSERT(list->indexOf(aL[i]) == i);
                }

            // lastIndexOf
            TS_ASSERT(list->lastIndexOf(NULL) == List::npos);           // test NULL
            TS_ASSERT(list->lastIndexOf(Integer64::create(-1)) == List::npos);   // test !contains
            TS_ASSERT(list->lastIndexOf(Integer64::create(0)) == (c-1)); // test ->equals()
            for (size32_t i = 1; i < c; ++i)
                {
                TS_ASSERT(list->lastIndexOf(aL[i]) == i);
                }

            // subList
            List::Handle listSub = list->subList(1, list->size() - 1);
            TS_ASSERT(listSub->size() == list->size() - 2);
            Iterator::Handle hIterSub  = listSub->listIterator();
            Iterator::Handle hIterOrig = list->listIterator(1);
            size32_t cIters = 0;
            while (hIterSub->hasNext())
                {
                TS_ASSERT(hIterSub->next() == hIterOrig->next());
                ++cIters;
                }
            TS_ASSERT(cIters == listSub->size());
            }
    };
