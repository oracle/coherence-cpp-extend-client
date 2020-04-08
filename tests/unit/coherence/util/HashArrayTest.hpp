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

#include "private/coherence/util/HashArray.hpp"

#include <sstream>
#include <iostream>

using namespace coherence::run::xml;
using namespace std;

using coherence::util::LongArray;
using coherence::util::LongArrayIterator;
using coherence::util::Iterator;
using coherence::util::HashArray;

/**
* Test Suite for HashArray
*/
class HashArrayTest : public CxxTest::TestSuite,
    public class_spec<HashArrayTest>
    {
    public:
        void testSetGet()
            {
            HashArray::Handle hHashArray = HashArray::create();
            Object::Handle h = Object::create();
            hHashArray->set(longVal, h);
            TS_ASSERT(hHashArray->get(longVal) == h);
            Object::Handle h2 = Object::create();
            hHashArray->set(longVal, h2);
            TS_ASSERT(hHashArray->get(longVal) == h2);
            TS_ASSERT(hHashArray->get(0) == NULL);
            }

        void testTestExists()
            {
            HashArray::Handle hHashArray = HashArray::create();
            Object::Handle h = Object::create();
            TS_ASSERT(!hHashArray->exists(longVal));
            hHashArray->set(longVal, h);
            TS_ASSERT(hHashArray->exists(longVal));
            }

        void testRemove()
            {
            HashArray::Handle hHashArray = HashArray::create();
            Object::Handle h = Object::create();
            hHashArray->set(longVal, h);
            hHashArray->set(123, Object::create());
            hHashArray->set(999999, Object::create());
            hHashArray->set(0, Object::create());
            TS_ASSERT(hHashArray->exists(longVal));
            TS_ASSERT(hHashArray->remove(longVal) == h);
            TS_ASSERT(!hHashArray->exists(longVal));
            TS_ASSERT(hHashArray->remove(9) == NULL);
            }

        void testClear()
            {
            HeapAnalyzer::ensureHeap(); // prime the analyzer
            HeapAnalyzer::Snapshot::View vSnap = HeapAnalyzer::ensureHeap();
                {
                HashArray::Handle hHashArray = HashArray::create();
                Object::Handle h = Object::create();
                hHashArray->set(45678, h);
                hHashArray->set(123, Object::create());
                hHashArray->set(9999999, Object::create());
                hHashArray->set(0, Object::create());
                TS_ASSERT(hHashArray->getSize() == 4);
                hHashArray->clear();
                TS_ASSERT(hHashArray->getSize() == 0);
                TS_ASSERT(!hHashArray->exists(45678));
                }
            HeapAnalyzer::ensureHeap(vSnap);
            }

        void testGetSize()
            {
            HashArray::Handle hHashArray = HashArray::create();
            TS_ASSERT(hHashArray->getSize() == 0);
            Object::Handle h = Object::create();
            hHashArray->set(45678, h);
            hHashArray->set(123, Object::create());
            hHashArray->set(9999999, Object::create());
            hHashArray->set(0, Object::create());
            TS_ASSERT(hHashArray->getSize() == 4);
            hHashArray->remove(123);
            TS_ASSERT(hHashArray->getSize() == 3);
            }

        void testAdd()
            {
            HashArray::Handle hHashArray = HashArray::create();
            TS_ASSERT(hHashArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            hHashArray->set(longVal, h);
            TS_ASSERT(hHashArray->getSize() == 1);
            int64_t i = hHashArray->add(h2);
            TS_ASSERT(i == longVal + 1);
            TS_ASSERT(h2 == hHashArray->get(i));
            TS_ASSERT(hHashArray->getSize() == 2);
            LongArrayIterator::Handle hIter = hHashArray->iterator();
            TS_ASSERT(h == hIter->next());
            TS_ASSERT(h2 == hIter->next());
            }

        void testEmptyIterator()
            {
            HashArray::Handle hHashArray = HashArray::create();
            TS_ASSERT(hHashArray->getSize() == 0);
            TS_ASSERT(hHashArray->iterator()->hasNext() == false);
            }

        void testIterator()
            {
            HashArray::Handle hHashArray = HashArray::create();
            TS_ASSERT(hHashArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            Object::Handle h4 = Object::create();

            hHashArray->set(longVal2, h);
            hHashArray->set(123, h2);
            hHashArray->set(99999, h3);
            hHashArray->set(longVal, h4);

            size32_t cEntries = 0;
            for (LongArrayIterator::Handle hIter = hHashArray->iterator();
                 hIter->hasNext(); )
                {
                Object::View v = hIter->next();
                switch (hIter->getIndex())
                    {
                    case longVal2:
                        TS_ASSERT(v == h);
                        TS_ASSERT(hIter->getValue() == h);
                        h = NULL;
                        break;

                    case 123:
                        TS_ASSERT(v == h2);
                        TS_ASSERT(hIter->getValue() == h2);
                        h2 = NULL;
                        break;

                    case 99999:
                        TS_ASSERT(v == h3);
                        TS_ASSERT(hIter->getValue() == h3);
                        h3 = NULL;
                        break;

                    case longVal:
                        TS_ASSERT(v == h4);
                        TS_ASSERT(hIter->getValue() == h4);
                        h4 = NULL;
                        break;

                    default:
                        TS_ASSERT(false);
                    }

                ++cEntries;
                }

            TS_ASSERT(cEntries == 4);
            }

        void testIteratorRemove()
            {
            HashArray::Handle hHashArray = HashArray::create();
            TS_ASSERT(hHashArray->getSize() == 0);
            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            Object::Handle h4 = Object::create();

            hHashArray->set(longVal2, h);
            hHashArray->set(123, h2);
            hHashArray->set(99999, h3);
            hHashArray->set(longVal, h4);

            size32_t cEntries = 0;
            for (LongArrayIterator::Handle hIter = hHashArray->iterator();
                 hIter->hasNext(); )
                {
                Object::View v = hIter->next();
                switch (hIter->getIndex())
                    {
                    case longVal2:
                        TS_ASSERT(v == h);
                        TS_ASSERT(hIter->getValue() == h);
                        h = NULL;
                        break;

                    case 123:
                        TS_ASSERT(v == h2);
                        TS_ASSERT(hIter->getValue() == h2);
                        h2 = NULL;
                        break;

                    case 99999:
                        TS_ASSERT(v == h3);
                        TS_ASSERT(hIter->getValue() == h3);
                        h3 = NULL;
                        break;

                    case longVal:
                        TS_ASSERT(v == h4);
                        TS_ASSERT(hIter->getValue() == h4);
                        h4 = NULL;
                        break;

                    default:
                        TS_ASSERT(false);
                    }

                hIter->remove();
                ++cEntries;
                }

            TS_ASSERT(cEntries == 4);
            TS_ASSERT(hHashArray->isEmpty());
            }

        void testConstIterator()
            {
            HashArray::Handle hHashArray = HashArray::create();
            TS_ASSERT(hHashArray->getSize() == 0);
            Object::Handle h = Object::create();

            hHashArray->set(longVal2, h);

            HashArray::View vHashArray = hHashArray;
            LongArrayIterator::Handle hIter = vHashArray->iterator();
            TS_ASSERT(hIter->hasNext());
            hIter->next();
            try
                {
                hIter->remove();
                TS_ASSERT(false);
                }
            catch (UnsupportedOperationException::View e)
                {
                }
            }

        void testContains()
            {
            HashArray::Handle hHashArray = HashArray::create();
            Object::Handle h = Object::create();
            hHashArray->set(1, Object::create());
            hHashArray->set(2, Object::create());
            hHashArray->set(longVal, h);
            TS_ASSERT(hHashArray->contains(h));
            TS_ASSERT(!hHashArray->contains(Object::create()));
            }

        void testIsEmpty()
            {
            HashArray::Handle hHashArray = HashArray::create();
            TS_ASSERT(hHashArray->isEmpty());
            hHashArray->set(1, Object::create());
            TS_ASSERT(!hHashArray->isEmpty());
            hHashArray->remove(1);
            TS_ASSERT(hHashArray->isEmpty());
            }

        void testSizeOf()
            {
            HashArray::Handle hHashArray = HashArray::create();
            size64_t cbShallow = hHashArray->sizeOf(false);
            size64_t cbEmpty = hHashArray->sizeOf(true);

            TS_ASSERT(cbShallow <= cbEmpty);

            TS_ASSERT(hHashArray->getSize() == 0);

            Object::Handle h = Object::create();
            Object::Handle h2 = Object::create();
            Object::Handle h3 = Object::create();
            Object::Handle h4 = Object::create();

            size64_t cbLast = hHashArray->sizeOf(true);
            hHashArray->set(longVal2, h);
            TS_ASSERT(cbLast < hHashArray->sizeOf(true));
            cbLast = hHashArray->sizeOf(true);

            hHashArray->set(123, h2);
            TS_ASSERT(cbLast < hHashArray->sizeOf(true));
            cbLast = hHashArray->sizeOf(true);

            hHashArray->set(99999, h3);
            TS_ASSERT(cbLast < hHashArray->sizeOf(true));
            cbLast = hHashArray->sizeOf(true);

            hHashArray->set(longVal, h4);
            TS_ASSERT(cbLast < hHashArray->sizeOf(true));
            cbLast = hHashArray->sizeOf(true);

            TS_ASSERT(cbShallow = hHashArray->sizeOf(false));
            }

        static const int64_t longVal  = ((int64_t(0x7FFFFFFF) << 32) | 0xFFFFFFFA);
        static const int64_t longVal2 = ((int64_t(0x7FFFFFFF) << 32) | 0xFFFFFFFB);
        static const int64_t longVal3 = ((int64_t(0x7FFFFFFF) << 32) | 0xFFFFFFFC);
    };
