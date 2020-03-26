/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/LongArrayIterator.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/SparseArray.hpp"

#include <sstream>
#include <iostream>

using namespace coherence::run::xml;
using namespace std;

using coherence::util::LongArray;
using coherence::util::LongArrayIterator;
using coherence::util::Iterator;
using coherence::util::SparseArray;

/**
* Test Suite for SparseArray
*/
class SparseArrayTest : public CxxTest::TestSuite,
    public class_spec<SparseArrayTest>
    {
    public:
        void testSetGet()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            Object::Handle h = Object::create();
            hSparseArray->set(longVal, h);
            TS_ASSERT(hSparseArray->get(longVal) == h);
            Object::Handle h2 = Object::create();
            hSparseArray->set(longVal, h2);
            TS_ASSERT(hSparseArray->get(longVal) == h2);
            TS_ASSERT(hSparseArray->get(0) == NULL);
            }

        void testTestExists()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            Object::Handle h = Object::create();
            TS_ASSERT(!hSparseArray->exists(longVal));
            hSparseArray->set(longVal, h);
            TS_ASSERT(hSparseArray->exists(longVal));
            }

        void testRemove()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            Object::Handle h = Object::create();
            hSparseArray->set(longVal, h);
            hSparseArray->set(123, Object::create());
            hSparseArray->set(999999, Object::create());
            hSparseArray->set(0, Object::create());
            TS_ASSERT(hSparseArray->exists(longVal));
            TS_ASSERT(hSparseArray->remove(longVal) == h);
            TS_ASSERT(!hSparseArray->exists(longVal));
            TS_ASSERT(hSparseArray->remove(9) == NULL);
            }

        void testClear()
            {
            HeapAnalyzer::Snapshot::View vSnap = HeapAnalyzer::ensureHeap();
                {
                SparseArray::Handle hSparseArray = SparseArray::create();
                Object::Handle h = Object::create();
                hSparseArray->set(45678, h);
                hSparseArray->set(123, Object::create());
                hSparseArray->set(9999999, Object::create());
                hSparseArray->set(0, Object::create());
                TS_ASSERT(hSparseArray->getSize() == 4);
                hSparseArray->clear();
                TS_ASSERT(hSparseArray->getSize() == 0);
                TS_ASSERT(!hSparseArray->exists(45678));
                }
            HeapAnalyzer::ensureHeap(vSnap);
            }

        void testGetSize()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->getSize() == 0);
            Object::Handle h = Object::create();
            hSparseArray->set(45678, h);
            hSparseArray->set(123, Object::create());
            hSparseArray->set(9999999, Object::create());
            hSparseArray->set(0, Object::create());
            TS_ASSERT(hSparseArray->getSize() == 4);
            hSparseArray->remove(123);
            TS_ASSERT(hSparseArray->getSize() == 3);
            }

        void testAdd()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            hSparseArray->set(longVal, h);
            TS_ASSERT(hSparseArray->getSize() == 1);
            int64_t i = hSparseArray->add(h2);
            TS_ASSERT(i == longVal + 1);
            TS_ASSERT(h2 == hSparseArray->get(i));
            TS_ASSERT(hSparseArray->getSize() == 2);
            LongArrayIterator::Handle hIter = hSparseArray->iterator();
            TS_ASSERT(h == hIter->next());
            TS_ASSERT(h2 == hIter->next());
            }

        void testIterator()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            Object::Handle h4 = Object::create();
            Object::Handle h5 = Object::create();

            hSparseArray->set(longVal2, h);
            hSparseArray->set(123, h2);
            hSparseArray->set(99999, h3);
            hSparseArray->set(longVal, h4);

            LongArrayIterator::Handle hIter = hSparseArray->iterator();
            TS_ASSERT(hIter->next() == h2);
            TS_ASSERT(hIter->getIndex() == 123);
            TS_ASSERT(hIter->getValue() == h2);
            hIter->setValue(h5);
            TS_ASSERT(hSparseArray->get(123) == h5);
            TS_ASSERT(hIter->hasNext());
            TS_ASSERT(hIter->next() == h3);
            hIter->remove();
            TS_ASSERT(hSparseArray->get(99999) == NULL);
            TS_ASSERT(hIter->hasNext());
            TS_ASSERT(hIter->next() == h4);
            TS_ASSERT(hIter->next() == h);
            TS_ASSERT(!hIter->hasNext());
            }

        void testIndexedIterator()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            Object::Handle h4 = Object::create();
            Object::Handle h5 = Object::create();

            hSparseArray->set(longVal2, h);
            hSparseArray->set(123, h2);
            hSparseArray->set(99999, h3);
            hSparseArray->set(longVal, h4);

            LongArrayIterator::Handle hIter = hSparseArray->iterator(longVal);
            TS_ASSERT(hIter->hasNext());
            TS_ASSERT(hIter->next() == h4);
            TS_ASSERT(hIter->next() == h);
            TS_ASSERT(!hIter->hasNext());
            }

        void testConstIterator()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            Object::Handle h4 = Object::create();
            Object::Handle h5 = Object::create();

            hSparseArray->set(longVal2, h);
            hSparseArray->set(123, h2);
            hSparseArray->set(99999, h3);
            hSparseArray->set(longVal, h4);

            SparseArray::View vSparseArray = hSparseArray;
            LongArrayIterator::Handle hIter = vSparseArray->iterator();
            TS_ASSERT(hIter->hasNext());
            TS_ASSERT(hIter->next() == h2);
            TS_ASSERT(hIter->getIndex() == 123);
            TS_ASSERT(hIter->getValue() == h2);
            TS_ASSERT_THROWS(hIter->setValue(h5),
                             UnsupportedOperationException::View);


            TS_ASSERT(hIter->hasNext());
            TS_ASSERT(hIter->next() == h3);
            TS_ASSERT_THROWS(hIter->remove(),
                             UnsupportedOperationException::View);

            TS_ASSERT(hIter->hasNext());
            TS_ASSERT(hIter->next() == h4);
            TS_ASSERT(hIter->next() == h);
            TS_ASSERT(!hIter->hasNext());
            }

        void testConstIndexedIterator()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            Object::Handle h4 = Object::create();
            Object::Handle h5 = Object::create();

            hSparseArray->set(longVal2, h);
            hSparseArray->set(123, h2);
            hSparseArray->set(99999, h3);
            hSparseArray->set(longVal, h4);

            SparseArray::View vSparseArray = hSparseArray;
            LongArrayIterator::Handle hIter = vSparseArray->iterator(longVal);
            TS_ASSERT(hIter->hasNext());
            TS_ASSERT(hIter->next() == h4);
            TS_ASSERT(hIter->next() == h);
            TS_ASSERT_THROWS(hIter->setValue(h5),
                             UnsupportedOperationException::View);
            TS_ASSERT_THROWS(hIter->remove(),
                             UnsupportedOperationException::View);

            TS_ASSERT(!hIter->hasNext());
            }

        void testGetFirstLastIndex()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();

            hSparseArray->set(longVal2, h);
            hSparseArray->set(longVal3, h2);
            hSparseArray->set(longVal, h3);

            TS_ASSERT(longVal == hSparseArray->getFirstIndex());
            TS_ASSERT(longVal3 == hSparseArray->getLastIndex());
            }

        void testContains()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            Object::Handle h = Object::create();
            hSparseArray->set(1, Object::create());
            hSparseArray->set(2, Object::create());
            hSparseArray->set(longVal, h);
            TS_ASSERT(hSparseArray->contains(h));
            TS_ASSERT(!hSparseArray->contains(Object::create()));
            }

        void testIsEmpty()
            {
            SparseArray::Handle hSparseArray = SparseArray::create();
            TS_ASSERT(hSparseArray->isEmpty());
            hSparseArray->set(1, Object::create());
            TS_ASSERT(!hSparseArray->isEmpty());
            hSparseArray->remove(1);
            TS_ASSERT(hSparseArray->isEmpty());
            }

        void testEquals()
            {
            SparseArray::Handle hArray  = SparseArray::create();
            SparseArray::Handle hArray2 = SparseArray::create();

            TS_ASSERT(hArray->equals(hArray2));
            TS_ASSERT(hArray2->equals(hArray));

            Object::Handle h = Object::create();
            hArray->set(longVal2, h);
            TS_ASSERT(!hArray->equals(hArray2));
            TS_ASSERT(!hArray2->equals(hArray));

            hArray2->set(longVal2, h);
            TS_ASSERT(hArray->equals(hArray2));
            TS_ASSERT(hArray2->equals(hArray));

            hArray->set(1, Object::create());
            TS_ASSERT(!hArray->equals(hArray2));
            TS_ASSERT(!hArray2->equals(hArray));
            hArray->remove(1);

            hArray2->remove(longVal2);
            TS_ASSERT(!hArray->equals(hArray2));
            TS_ASSERT(!hArray2->equals(hArray));

            hArray2->set(longVal, h);
            TS_ASSERT(!hArray->equals(hArray2));
            TS_ASSERT(!hArray2->equals(hArray));

            hArray->remove(longVal2);
            hArray->set(longVal, h);
            TS_ASSERT(hArray->equals(hArray2));
            TS_ASSERT(hArray2->equals(hArray));
            }

        void testClone()
            {
            SparseArray::Handle hArray = SparseArray::create();

            //clone empty sparse array
            SparseArray::Handle hArray2 = cast<SparseArray::Handle>(hArray->clone());

            TS_ASSERT(hArray != hArray2);
            TS_ASSERT(hArray->equals(hArray2));

            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();

            hArray->set(longVal2, h);
            hArray->set(longVal3, h2);
            hArray->set(longVal, h3);

            SparseArray::Handle hArray3 = cast<SparseArray::Handle>(hArray->clone());

            TS_ASSERT(hArray != hArray3);
            TS_ASSERT(hArray->equals(hArray3));
            }

        // simply ensure that toString returns without exception
        void testToString()
            {
            SparseArray::Handle hArray = SparseArray::create();
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            hArray->add(h);
            hArray->add(h2);
            hArray->add(h3);

            hArray->toString();
            }

        static const int64_t longVal  = ((int64_t(0x7FFFFFFF) << 32) | 0xFFFFFFFA);
        static const int64_t longVal2 = ((int64_t(0x7FFFFFFF) << 32) | 0xFFFFFFFB);
        static const int64_t longVal3 = ((int64_t(0x7FFFFFFF) << 32) | 0xFFFFFFFC);
    };
