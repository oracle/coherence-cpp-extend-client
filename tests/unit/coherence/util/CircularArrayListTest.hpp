/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/CircularArrayList.hpp"

#include "private/coherence/util/logging/Logger.hpp"

using namespace coherence::lang;
using namespace std;

/**
* Test suite for the CircularArrayList object.
*
* 2008.02.08 nsa
*/
class CircularArrayListTest : public CxxTest::TestSuite
    {
    public:
        void testAddIndex()
            {
            CircularArrayList::Handle hList   = CircularArrayList::create();
            String::Handle     hValue  = String::create("Test Value");
            String::Handle     hValue2 = String::create("TV2");

            hList->add(hValue);
            hList->add(0, hValue2);

            TS_ASSERT(hList->size() == 2);
            TS_ASSERT(hList->get(0)->equals(hValue2));
            }

        void testAddAllIndex()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            for (int32_t x = 0; x < 10; ++x)
                {
                hList->add(COH_TO_STRING("List Value: " << x));
                }

            TS_ASSERT(hList->size() == 10);

            CircularArrayList::Handle hList2 = CircularArrayList::create();
            String::Handle hList2Value = String::create("hList2");
            hList2->add(hList2Value);
            hList2->addAll(0, hList);

            TS_ASSERT(hList2->size() == 11);
            TS_ASSERT(hList2->get(10) == hList2Value);
            }

        void testGet()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            String::Handle hValue = String::create("Test Value");

            hList->add(hValue);

            TS_ASSERT(hList->get(0)->equals(hValue));

            String::Handle hValue2 = String::create("Test Value 2");
            hList->add(hValue2);

            TS_ASSERT(hList->get(1)->equals(hValue2));
            }

        void testIndexOf()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            String::Handle findMe;
            String::Handle notFound = String::create("Can't Find Me");

            for (int32_t x = 0; x < 10; ++x)
                {
                String::Handle hString = COH_TO_STRING("List Value: " << x);

                if (x == 5)
                    {
                    findMe = hString;
                    }

                hList->add(hString);
                }

            TS_ASSERT(hList->indexOf(findMe) == 5);
            TS_ASSERT(hList->indexOf(notFound) == List::npos);
            }

        void testLastIndexOf()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            String::Handle hDup       = String::create("DuplicateString");
            String::Handle hNotFound  = String::create("Can't find me");
            for (int32_t x = 0; x < 10; ++x)
                {
                if (x % 2 == 0)
                    {
                    hList->add(hDup);
                    }
                else
                    {
                    hList->add(COH_TO_STRING("List Value: " << x));
                    }
                }

            TS_ASSERT(hList->lastIndexOf(hDup) == 8);
            TS_ASSERT(hList->lastIndexOf(hNotFound) == List::npos);
            }

        void testListIterator()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            TS_ASSERT_EQUALS(hList->listIterator()->hasNext(), false);
            TS_ASSERT_EQUALS(hList->listIterator()->hasPrevious(), false);
            int32_t x;
            for (x = 0; x < 10; ++x)
                {
                hList->add(Integer32::create(x));
                }
            TS_ASSERT_EQUALS(hList->size(), size32_t(10));

            ListIterator::Handle hIterator = hList->listIterator();
            TS_ASSERT_EQUALS(hList->listIterator()->hasPrevious(), false);

            x = 0;
            while (hIterator->hasNext())
                {
                Integer32::Handle hValue = cast<Integer32::Handle>(hIterator->next());
                TS_ASSERT_EQUALS(hValue->getValue(), x);
                x++;
                }
            TS_ASSERT_EQUALS(x, 10);

            while (hIterator->hasPrevious())
                {
                --x;
                Integer32::Handle hValue = cast<Integer32::Handle>(hIterator->previous());
                TS_ASSERT_EQUALS(hValue->getValue(), x);
                }
            TS_ASSERT(x == 0);

            while (hIterator->hasNext())
                {
                Integer32::Handle hValue = cast<Integer32::Handle>(hIterator->next());
                TS_ASSERT_EQUALS(hValue->getValue(), x);
                ++x;
                }
            TS_ASSERT(x == 10);
            }

        void testListIteratorIndex()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            int32_t x;
            for (x = 0; x < 10; ++x)
                {
                hList->add(Integer32::create(x));
                }

            x = 5;
            ListIterator::Handle hIterator = hList->listIterator(x);

            while (hIterator->hasNext())
                {
                Integer32::Handle hValue = cast<Integer32::Handle>(hIterator->next());
                TS_ASSERT_EQUALS(hValue->getValue(), x);
                ++x;
                }
            TS_ASSERT_EQUALS(x, 10);

            while (hIterator->hasPrevious())
                {
                --x;
                Integer32::Handle hValue = cast<Integer32::Handle>(hIterator->previous());
                TS_ASSERT_EQUALS(hValue->getValue(), x);
                }
            TS_ASSERT_EQUALS(x, 0);

            while (hIterator->hasNext())
                {
                Integer32::Handle hValue = cast<Integer32::Handle>(hIterator->next());
                TS_ASSERT_EQUALS(hValue->getValue(), x);
                ++x;
                }
            TS_ASSERT_EQUALS(x, 10);
            }

        void testListMuterator()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            TS_ASSERT_EQUALS(hList->listIterator()->hasNext(), false);
            TS_ASSERT_EQUALS(hList->listIterator()->hasPrevious(), false);
            size32_t x;

            ListMuterator::Handle hMiter = hList->listIterator();
            for (x = 0; x < 10; ++x)
                {
                hMiter->add(Integer32::create(x));
                }
            TS_ASSERT_EQUALS(hList->size(), x);

            while (hMiter->hasPrevious())
                {
                --x;
                Integer32::Handle hValue = cast<Integer32::Handle>(hMiter->previous());
                TS_ASSERT_EQUALS(hValue->getValue(), (int32_t)x);
                hMiter->remove();
                TS_ASSERT(hMiter->hasNext() == false)
                }
            TS_ASSERT_EQUALS(x, 0u);
            TS_ASSERT_EQUALS(hList->size(), x);

            for (x = 0; x < 5; ++x)
                {
                hMiter->add(Integer32::create(x * 2));
                }
            TS_ASSERT_EQUALS(hList->size(), x);

            hMiter = hList->listIterator();
            for (x = 0; x < 10; x += 2)
                {
                hMiter->next();
                hMiter->add(Integer32::create(x + 1));
                }
            TS_ASSERT_EQUALS(hList->size(), x);

            while (hMiter->hasPrevious())
                {
                --x;
                Integer32::Handle hValue = cast<Integer32::Handle>(hMiter->previous());
                TS_ASSERT_EQUALS(hValue->getValue(), (int32_t)x);
                }

            size32_t iNextExpected = 0;
            for (x = 0; x < 10; x += 2)
                {
                TS_ASSERT_EQUALS(hMiter->nextIndex(), iNextExpected);
                Integer32::Handle hValue = cast<Integer32::Handle>(hMiter->next());
                ++iNextExpected;

                TS_ASSERT_EQUALS(hValue->getValue(), (int32_t)x);
                TS_ASSERT_EQUALS(hMiter->previousIndex(), iNextExpected - 1);

                hMiter->remove();
                --iNextExpected;
                hMiter->next();
                ++iNextExpected;
                }
            TS_ASSERT_EQUALS(hList->size(), x/2);

            hMiter = hList->listIterator();
            for (x = 1; x < 10; x += 2)
                {
                Integer32::Handle hValue = cast<Integer32::Handle>(hMiter->next());
                TS_ASSERT_EQUALS(hValue->getValue(), (int32_t)x);
                hMiter->set(Integer32::create(x + 1));
                }

            hMiter = hList->listIterator();
            for (x = 1; x < 10; x += 2)
                {
                Integer32::Handle hValue = cast<Integer32::Handle>(hMiter->next());
                TS_ASSERT_EQUALS(hValue->getValue(), (int32_t)(x + 1));
                }

            // test next/prev toggle
            hMiter = hList->listIterator();
            size32_t     iNext = hMiter->nextIndex();
            Object::View vNext = hMiter->next();
            size32_t     iPrev = hMiter->previousIndex();
            Object::View vPrev = hMiter->previous();
            TS_ASSERT_EQUALS(vNext, vPrev);
            TS_ASSERT_EQUALS(iNext, iPrev);
            iNext = hMiter->nextIndex();
            vNext = hMiter->next();
            TS_ASSERT_EQUALS(vNext, vPrev);
            TS_ASSERT_EQUALS(iNext, iPrev);
            }

        void testRemoveIndex()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            String::Handle s1 = String::create("test1");
            String::Handle s2 = String::create("test2");

            hList->add(s1);
            hList->add(s2);

            TS_ASSERT(hList->remove(1)->equals(s2));
            TS_ASSERT(hList->size() == 1u);
            }

        void testSetIndex()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            String::Handle s1 = String::create("test1");
            String::Handle s2 = String::create("test2");
            String::Handle s3 = String::create("test3");

            hList->add(s1);
            hList->add(s2);

            TS_ASSERT(hList->get(1)->equals(s2));
            hList->set(1, s3);
            TS_ASSERT(hList->get(1)->equals(s3));
            TS_ASSERT(hList->size() == 2u);
            }

        void testSubList()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            for (int32_t x = 0; x < 10; ++x)
                {
                hList->add(COH_TO_STRING("List Value: " << x));
                }

            List::Handle hSub = hList->subList(1, 6);
            TS_ASSERT(hSub->size() == 5u);

            hSub->add(String::create("New Sub Value"));
            TS_ASSERT(hSub->size() == 6u);
            TS_ASSERT(hList->size() == 11u);

            Iterator::Handle hIter = hSub->iterator();
            while (hIter->hasNext())
                {
                TS_ASSERT(hList->contains(hIter->next()));
                }
            }

        void testSize()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            for (int32_t x = 0; x < 10; ++x)
                {
                hList->add(COH_TO_STRING("List Value: " << x));
                }

            TS_ASSERT(hList->size() == 10u);
            }

        void testIterator()
            {
            CircularArrayList::Handle hList = CircularArrayList::create();

            int32_t x;
            for (x = 0; x < 10; ++x)
                {
                hList->add(COH_TO_STRING("List Value: " << x));
                }

            Iterator::Handle hIterator = hList->iterator();

            x = 0;
            while (hIterator->hasNext())
                {
                String::Handle hString = cast<String::Handle>(hIterator->next());
                x++;
                }
            TS_ASSERT(x == 10);
            }

        void testAdd()
           {
           CircularArrayList::Handle hList = CircularArrayList::create();
           String::Handle     hValue = String::create("Test Value");

           hList->add(hValue);

           TS_ASSERT(hList->size() == 1u);
           TS_ASSERT(hList->get(0)->equals(hValue));
           }

       void testAddAll()
           {
           CircularArrayList::Handle hList = CircularArrayList::create();

           for (int32_t x = 0; x < 10; ++x)
               {
               hList->add(COH_TO_STRING("List Value: " << x));
               }

           TS_ASSERT(hList->size() == 10u);

           CircularArrayList::Handle hList2 = CircularArrayList::create();
           String::Handle hList2Value = String::create("hList2");
           hList2->add(hList2Value);
           hList2->addAll(hList);

           TS_ASSERT(hList2->size() == 11u);
           TS_ASSERT(hList2->get(10) == hList->get(hList->size() - 1));
           }

       void testRemove()
           {
           String::Handle h1 = String::create("h1");
           String::Handle h2 = String::create("h2");

           CircularArrayList::Handle hList = CircularArrayList::create();
           hList->add(h1);
           hList->add(h2);

           TS_ASSERT(hList->size() == 2u);

           hList->remove(h2);
           TS_ASSERT(hList->size() == 1u);
           }

       void testRemoveAll()
           {
           CircularArrayList::Handle hList = CircularArrayList::create();

           for (int32_t x = 0; x < 10; ++x)
               {
               hList->add(COH_TO_STRING("List Value: " << x));
               }

           TS_ASSERT(hList->size() == 10);

           CircularArrayList::Handle hList2   = CircularArrayList::create();
           String::Handle hList2Value  = String::create("hList2");
           String::Handle hList2Valueb = String::create("hList2b");
           hList2->add(hList2Value);
           hList2->add(hList2Valueb);

           hList->addAll(hList2);

           TS_ASSERT(hList->size() == 12u);

           hList->removeAll(hList2);
           TS_ASSERT(hList->size() == 10u);
           }

       void testRetainAll()
           {
           CircularArrayList::Handle hList = CircularArrayList::create();

           for (int32_t x = 0; x < 10; ++x)
               {
               hList->add(COH_TO_STRING("List Value: " << x));
               }

           TS_ASSERT(hList->size() == 10u);

           CircularArrayList::Handle hList2   = CircularArrayList::create();
           String::Handle hList2Value  = String::create("hList2");
           String::Handle hList2Valueb = String::create("hList2b");
           hList2->add(hList2Value);
           hList2->add(hList2Valueb);

           hList->addAll(hList2);

           TS_ASSERT(hList->size() == 12u);

           hList->retainAll(hList2);
           TS_ASSERT(hList->size() == 2u);
           }

       void testClear()
           {
           CircularArrayList::Handle hList = CircularArrayList::create();

           for (int32_t x = 0; x < 10; ++x)
               {
               hList->add(COH_TO_STRING("List Value: " << x));
               }

           TS_ASSERT(hList->size() == 10u);
           hList->clear();
           TS_ASSERT(hList->size() == 0u);

           hList->add(String::create("foo"));
           TS_ASSERT(hList->size() == 1u);
           }

       void testClone()
           {
           CircularArrayList::Handle hList = CircularArrayList::create();
           hList->add(String::create("test"));
           hList->add(String::create("test2"));

           CircularArrayList::View vList = hList;
           CircularArrayList::Handle hListClone = cast<CircularArrayList::Handle>(vList->clone());
           TS_ASSERT(hListClone->size() == 2u);
           TS_ASSERT(cast<String::View>(hListClone->get(0))->equals("test"));
           TS_ASSERT(cast<String::View>(hListClone->get(1))->equals("test2"));
           }

       void testCleanup()
           {
           HeapAnalyzer::Snapshot::View vSnap = HeapAnalyzer::ensureHeap();
           CircularArrayList::Handle           hList = CircularArrayList::create();
           for (int32_t i = 0; i < 10; ++i)
               {
               hList->add(Integer32::create(i));
               }
           hList = NULL;
           HeapAnalyzer::ensureHeap(vSnap);
           }

       void testBigList()
           {
           HeapAnalyzer::Snapshot::View vSnap = HeapAnalyzer::ensureHeap();
           CircularArrayList::Handle           hList = CircularArrayList::create();
           for (int32_t i = 0; i < 10000; ++i)
               {
               hList->add(Integer32::create(i));
               }
           MemberHandle<List> hEscape(System::common(), hList);
           int32_t n = 0;
           for (Iterator::Handle hIter = hList->iterator(); hIter->hasNext(); )
               {
               Integer32::Handle hN = cast<Integer32::Handle>(hIter->next());
               TS_ASSERT(hN->getValue() == n++);
               TS_ASSERT(hN->_isEscaped());
               }
           TS_ASSERT(n == 10000);
           hEscape = NULL;
           hList->_isEscaped(); // unescape
           hList = NULL; // delete
           HeapAnalyzer::ensureHeap(vSnap);
           }

     };
