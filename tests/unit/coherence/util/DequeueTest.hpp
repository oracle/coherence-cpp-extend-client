/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "private/coherence/util/Dequeue.hpp"

#include <sstream>
#include <stdio.h>
#include <time.h>


using namespace coherence::util;

/**
* Test Suite for the Dequeue object.
*/
class DequeueSuite : public CxxTest::TestSuite
        {
        public:


        /**
        * Test hasNext
        */
        void testHasNext()
        {
        Dequeue::Handle dq = Dequeue::create();

        TS_ASSERT(!dq->hasNext());

        dq->addElement(String::create("foo"));
        TS_ASSERT(dq->hasNext());

        dq->next();

        TS_ASSERT(!dq->hasNext());
        }


        /**
        * Test next
        */
        void testNext()
        {
        Dequeue::Handle dq = Dequeue::create();

        Object::View element1 = String::create("foo");
        Object::View element2 = String::create("bar");
        dq->addElement(element1);
        dq->addElement(element2);

        TS_ASSERT(dq->next() == element1);
        TS_ASSERT(dq->next() == element2);
        }


        /**
        * Test addElement
        */
        void testAddElement()
        {
        Dequeue::Handle dq = Dequeue::create();

        Object::View element1 = String::create("foo");
        Object::View element2 = String::create("bar");
        dq->addElement(element1);
        dq->addElement(element2);

        TS_ASSERT(dq->next() == element1);
        TS_ASSERT(dq->next() == element2);
        }

        /**
        * Test putBackElement
        */
        void testPutBackElement()
        {
        Dequeue::Handle dq = Dequeue::create();

        Object::View element1 = String::create("foo");
        Object::View element2 = String::create("bar");
        dq->addElement(element1);
        dq->addElement(element2);

        TS_ASSERT(dq->next() == element1);
        dq->putBackElement(element1);
        TS_ASSERT(dq->next() == element1);
        }

        /**
        * Test removeAllElements
        */
        void testRemoveAllElements()
        {
        Dequeue::Handle dq = Dequeue::create();

        Object::View element1 = String::create("foo");
        Object::View element2 = String::create("bar");
        dq->addElement(element1);
        dq->addElement(element2);

        TS_ASSERT(dq->size() == 2);
        dq->removeAllElements();
        TS_ASSERT(dq->size() == 0);
        }


        /**
        * Test size
        */
        void testSize()
        {
        Dequeue::Handle dq = Dequeue::create();
        TS_ASSERT(dq->size() == 0);

        Object::View element1 = String::create("foo");
        dq->addElement(element1);
        TS_ASSERT(dq->size() == 1);

        Object::View element2 = String::create("bar");
        dq->addElement(element2);
        TS_ASSERT(dq->size() == 2);

        Object::View element3 = String::create("monkey");
        dq->addElement(element3);
        TS_ASSERT(dq->size() == 3);

        dq->removeAllElements();
        TS_ASSERT(dq->size() == 0);
        }

        /**
        * Test isEmpty
        */
        void testIsEmpty()
        {
        Dequeue::Handle dq = Dequeue::create();

        TS_ASSERT(dq->isEmpty());

        dq->addElement(String::create("foo"));
        TS_ASSERT(!dq->isEmpty());

        dq->next();

        TS_ASSERT(dq->isEmpty());
        }

        /**
        * Test isFull
        */
        void testIsFull()
        {
        Dequeue::Handle dq = Dequeue::create();

        TS_ASSERT(!dq->isFull());

        size32_t cCap = dq->capacity();
        for (size32_t i = 0; i < cCap; ++i)
            {
            dq->addElement(String::create("foo"));
            }
        TS_ASSERT(dq->isFull());

        dq->next();

        TS_ASSERT(!dq->isFull());
        }

        /**
        * Test capacity
        */
        void testCapacity()
        {
        Dequeue::Handle dq = Dequeue::create();

        size32_t cCap = dq->capacity();

        TS_ASSERT(cCap == (Dequeue::default_max_elements - 1));

        // fill up the queue
        for (size32_t i = 0; i < cCap; ++i)
            {
            dq->addElement(String::create("foo"));
            }
        TS_ASSERT(dq->isFull());
        // add 1 more
        dq->addElement(String::create("foo"));

        cCap = dq->capacity();

        // make sure that the capacity has increased
        TS_ASSERT(cCap > (Dequeue::default_max_elements - 1));

        // repeat test but force the circular queue to wrap around
        dq = Dequeue::create();

        cCap = dq->capacity();

        TS_ASSERT(cCap == (Dequeue::default_max_elements - 1));

        // fill up the queue
        for (size32_t i = 0; i < cCap; ++i)
            {
            dq->addElement(String::create("foo"));
            }
        TS_ASSERT(dq->isFull());

        for (int32_t i = 0; i < 10; ++i)
            {
            dq->next();
            }

        for (int32_t i = 0; i < 10; ++i)
            {
            dq->addElement(String::create("foo"));
            }

        // add 1 more
        dq->addElement(String::create("foo"));

        cCap = dq->capacity();

        // make sure that the capacity has increased
        TS_ASSERT(cCap > (Dequeue::default_max_elements - 1));
        }

        /**
        * Test copy
        */
        void testCopy()
        {
        Dequeue::Handle dq = Dequeue::create();

        size32_t cCap = dq->capacity();

        TS_ASSERT(cCap == (Dequeue::default_max_elements - 1));

        // fill up the queue
        for (size32_t i = 0; i < cCap; ++i)
            {
            dq->addElement(String::create("foo"));
            }
        TS_ASSERT(dq->isFull());

        for (int32_t i = 0; i < 10; ++i)
            {
            dq->next();
            }

        for (int32_t i = 0; i < 10; ++i)
            {
            dq->addElement(String::create("foo"));
            }

        // add 1 more
        dq->addElement(String::create("foo"));

        cCap = dq->capacity();
        size32_t cSize = dq->size();

        Dequeue::Handle dq2 = Dequeue::create(dq);

        TS_ASSERT(cCap == dq2->capacity());
        TS_ASSERT(cSize == dq2->size());

        }

        };

