/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/util/SortedBag.hpp"

#include <iostream>

using coherence::util::SortedBag;

#define CREATE_ARRAY(id,n,ai) \
    ObjectArray::Handle id = ObjectArray::create(n); \
    for (int32_t i = 0; i < n; i++) \
        { \
        id[i] = Integer32::create(ai[i]); \
        }

#define INTEGER(n) Integer32::create(n)

/**
* Test Suite for SortedBag
*/
class SortedBagTest : public CxxTest::TestSuite,
      public class_spec<SortedBagTest>
    {
    public:
        void testEmpty()
            {
            SortedBag::Handle hBag = SortedBag::create();

            ensureEmpty(hBag);
            hBag->remove(Integer32::create(1));
            ensureEmpty(hBag);
            hBag->add(Integer32::create(1));
            hBag->remove(Integer32::create(1));
            ensureEmpty(hBag);
            }

        void testSingle()
            {
            ObjectArray::Handle hao1 = ObjectArray::create(1);
            hao1[0] = Integer32::create(1);

            SortedBag::Handle hBag = SortedBag::create();
            hBag->add(Integer32::create(1));


            checkBag(hBag, hao1);
            checkBag(hBag->headBag(INTEGER(2)), hao1);
            checkBag(hBag->subBag(INTEGER(0), INTEGER(10)), hao1);
            checkBag(hBag->tailBag(INTEGER(0)), hao1);
            checkBag(hBag->tailBag(INTEGER(1)), hao1);

            ensureEmpty(hBag->headBag(INTEGER(1)));
            ensureEmpty(hBag->subBag(INTEGER(1), INTEGER(1)));
            ensureEmpty(hBag->tailBag(INTEGER(2)));
            }

        void testMultipleUnique()
            {
            int32_t ai1[1] = {1};
            CREATE_ARRAY(hao1, 1,  ai1);

            int32_t ai15[2] = {1, 5};
            CREATE_ARRAY(hao15, 2,  ai15);

            int32_t ai5[1] = {5};
            CREATE_ARRAY(hao5, 1,  ai5);

            SortedBag::Handle hBag = SortedBag::create();
            hBag->add(INTEGER(1));
            hBag->add(INTEGER(5));

            checkBag(hBag, hao15);
            checkBag(hBag->tailBag(INTEGER(1)), hao15);
            checkBag(hBag->tailBag(INTEGER(2)), hao5);
            checkBag(hBag->headBag(INTEGER(5)), hao1);
            checkBag(hBag->headBag(INTEGER(6)), hao15);
            checkBag(hBag->subBag(INTEGER(1), INTEGER(6)), hao15);
            checkBag(hBag->subBag(INTEGER(1), INTEGER(5)), hao1);

            ensureEmpty(hBag->headBag(INTEGER(1)));
            ensureEmpty(hBag->headBag(INTEGER(1)));
            ensureEmpty(hBag->subBag(INTEGER(2), INTEGER(5)));
            ensureEmpty(hBag->tailBag(INTEGER(6)));
            }

        void testMultipleDuplicate()
            {
            int32_t ai111223445[9] = {1,1,1,2,2,3,4,4,5};
            CREATE_ARRAY(hao111223445, 9, ai111223445);

            int32_t ai11122344[8] = {1,1,1,2,2,3,4,4};
            CREATE_ARRAY(hao11122344, 8, ai11122344);

            int32_t ai111223[6] = {1,1,1,2,2,3};
            CREATE_ARRAY(hao111223, 6, ai111223);

            int32_t ai223[3] = {2,2,3};
            CREATE_ARRAY(hao223, 3, ai223);

            int32_t ai223445[6] = {2,2,3,4,4,5};
            CREATE_ARRAY(hao223445, 6, ai223445);

            SortedBag::Handle hBag = SortedBag::create();

            hBag->add(INTEGER(1));
            hBag->add(INTEGER(5));
            hBag->add(INTEGER(1));
            hBag->add(INTEGER(2));
            hBag->add(INTEGER(4));
            hBag->add(INTEGER(2));
            hBag->add(INTEGER(3));
            hBag->add(INTEGER(1));
            hBag->add(INTEGER(4));

            checkBag(hBag, hao111223445);
            checkBag(hBag->tailBag(INTEGER(1)), hao111223445);
            checkBag(hBag->tailBag(INTEGER(2)), hao223445);
            checkBag(hBag->headBag(INTEGER(5)), hao11122344);
            checkBag(hBag->headBag(INTEGER(6)), hao111223445);
            checkBag(hBag->subBag(INTEGER(1), INTEGER(6)), hao111223445);
            checkBag(hBag->subBag(INTEGER(1), INTEGER(5)), hao11122344);
            checkBag(hBag->subBag(INTEGER(1), INTEGER(4)), hao111223);
            checkBag(hBag->subBag(INTEGER(2), INTEGER(4)), hao223);

            ensureEmpty(hBag->headBag(INTEGER(0)));
            ensureEmpty(hBag->headBag(INTEGER(1)));
            ensureEmpty(hBag->subBag(INTEGER(6), INTEGER(10)));
            ensureEmpty(hBag->tailBag(INTEGER(6)));
            }

        void testMultipleDuplicateInverse()
            {
            int32_t ai544322111[9] = {5,4,4,3,2,2,1,1,1};
            CREATE_ARRAY(hao544322111, 9, ai544322111);

            int32_t ai54432211[8] = {5,4,4,3,2,2,1,1};
            CREATE_ARRAY(hao54432211, 8, ai54432211);

            int32_t ai5443211[7] = {5,4,4,3,2,1,1};
            CREATE_ARRAY(hao5443211, 7, ai5443211);

            int32_t ai111[3] = {1,1,1};
            CREATE_ARRAY(hao111, 3, ai111);

            int32_t ai22111[5] = {2,2,1,1,1};
            CREATE_ARRAY(hao22111, 5, ai22111);

            int32_t ai544322[6] = {5,4,4,3,2,2};
            CREATE_ARRAY(hao544322, 6, ai544322);

            int32_t ai5443[4] = {5,4,4,3};
            CREATE_ARRAY(hao5443, 4, ai5443);

            int32_t ai44322[5] = {4,4,3,2,2};
            CREATE_ARRAY(hao44322, 5, ai44322);

            int32_t ai443[3] = {4,4,3};
            CREATE_ARRAY(hao443, 3, ai443);

            int32_t ai544311[6] = {5,4,4,3,1,1};
            CREATE_ARRAY(hao544311, 6, ai544311);

            int32_t ai54411[5] = {5,4,4,1,1};
            CREATE_ARRAY(hao54411, 5, ai54411);

            int32_t ai5411[4] = {5,4,1,1};
            CREATE_ARRAY(hao5411, 4, ai5411);

            int32_t ai541[3] = {5,4,1};
            CREATE_ARRAY(hao541, 3, ai541);

            int32_t ai51[] = {5,1};
            CREATE_ARRAY(hao51, 2, ai51);

            SortedBag::Handle hBag = SortedBag::create(
                    InverseComparator::create(SafeComparator::getInstance()));

            hBag->add(INTEGER(1));
            hBag->add(INTEGER(5));
            hBag->add(INTEGER(1));
            hBag->add(INTEGER(2));
            hBag->add(INTEGER(4));
            hBag->add(INTEGER(2));
            hBag->add(INTEGER(3));
            hBag->add(INTEGER(1));
            hBag->add(INTEGER(4));

            checkBag(hBag, hao544322111);
            checkBag(hBag->tailBag(INTEGER(1)), hao111);
            checkBag(hBag->tailBag(INTEGER(2)), hao22111);
            checkBag(hBag->tailBag(INTEGER(5)), hao544322111);
            checkBag(hBag->tailBag(INTEGER(6)), hao544322111);
            checkBag(hBag->subBag(INTEGER(6), INTEGER(1)), hao544322);
            checkBag(hBag->subBag(INTEGER(5), INTEGER(1)), hao544322);
            checkBag(hBag->subBag(INTEGER(5), INTEGER(2)), hao5443);
            checkBag(hBag->subBag(INTEGER(4), INTEGER(1)), hao44322);
            checkBag(hBag->subBag(INTEGER(4), INTEGER(2)), hao443);
            checkBag(hBag->headBag(INTEGER(0)), hao544322111);
            checkBag(hBag->headBag(INTEGER(1)), hao544322);

            ensureEmpty(hBag->headBag(INTEGER(5)));
            ensureEmpty(hBag->headBag(INTEGER(6)));
            ensureEmpty(hBag->tailBag(INTEGER(0)));
            ensureEmpty(hBag->subBag(INTEGER(10), INTEGER(6)));

            hBag->remove(INTEGER(0));
            checkBag(hBag, hao544322111);
            hBag->remove(INTEGER(1));
            checkBag(hBag, hao54432211);
            hBag->remove(INTEGER(2));
            checkBag(hBag, hao5443211);
            hBag->remove(INTEGER(2));
            checkBag(hBag, hao544311);
            hBag->tailBag(INTEGER(4))->remove(INTEGER(3));
            checkBag(hBag, hao54411);
            hBag->tailBag(INTEGER(4))->remove(INTEGER(4));
            checkBag(hBag, hao5411);
            hBag->tailBag(INTEGER(3))->remove(INTEGER(1));
            checkBag(hBag, hao541);
            hBag->headBag(INTEGER(3))->remove(INTEGER(4));
            checkBag(hBag, hao51);
            }



    // ----- helper methods ------------------------------------------------

    protected:
        void ensureEmpty(SortedBag::Handle hBag)
            {
            TS_ASSERT(hBag->isEmpty());
            TS_ASSERT_EQUALS(0u, hBag->size());
            TS_ASSERT(!hBag->iterator()->hasNext());
            }

        void checkBag(SortedBag::Handle hBag, ObjectArray::Handle hao)
            {
            if (hao->length == 0)
                {
                ensureEmpty(hBag);
                }
            else
                {
                TS_ASSERT(!hBag->isEmpty());
                TS_ASSERT_EQUALS(hao->length, hBag->size());

                Iterator::Handle hIter = hBag->iterator();
                for (size32_t i = 0; i < hao->length; i++)
                    {
                    Object::View v = hao[i];

                    TS_ASSERT(hBag->contains(v));
                    TS_ASSERT(hIter->hasNext());
                    TS_ASSERT(Object::equals(v, hIter->next()));
                    }

                TS_ASSERT(!hIter->hasNext());
                }
            }
    };
