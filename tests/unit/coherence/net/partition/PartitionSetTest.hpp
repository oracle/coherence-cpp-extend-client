/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/net/partition/PartitionSet.hpp"

using coherence::net::partition::PartitionSet;

class PartitionSetTest : public CxxTest::TestSuite
    {
    public:
        /**
         * Extend PartitionSet to have a usable constructor for testing PartitionSet.
         */
        class TestPartitionSet : public class_spec<TestPartitionSet,
                extends<PartitionSet> >
            {
            friend class factory<TestPartitionSet>;

            protected:
                /**
                 * Construct an empty partition set with a given count.
                 *
                 * @param cPartitions  the partition count
                 */
                TestPartitionSet(int32_t cPartitions)
                  : super()
                    {
                    m_cPartitions = cPartitions;
                    initialize(f_halBits, Array<int64_t>::create((int32_t) ((uint32_t) (cPartitions + 63) >> 6)));
                    m_lTailMask = (int64_t) (uint64_t(-1) >> (64 - (cPartitions & 63))),
                    m_cMarked = 0;
                    }
            };

        void testAdd()
            {
            PartitionSet::Handle hSetTest = TestPartitionSet::create(default_size);

            TS_ASSERT_THROWS(hSetTest->add(-1), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hSetTest->add(default_size), IndexOutOfBoundsException::View);

            TS_ASSERT(hSetTest->add(7));
            TS_ASSERT_EQUALS(7, hSetTest->next(0));
            TS_ASSERT_EQUALS(7, hSetTest->next(7));
            TS_ASSERT_EQUALS(-1, hSetTest->next(8));
            TS_ASSERT(!hSetTest->add(7));
            TS_ASSERT_EQUALS(7, hSetTest->next(0));
            TS_ASSERT_EQUALS(7, hSetTest->next(7));
            TS_ASSERT_EQUALS(-1, hSetTest->next(8));

            TS_ASSERT(hSetTest->add(9));
            TS_ASSERT_EQUALS(7, hSetTest->next(0));
            TS_ASSERT_EQUALS(7, hSetTest->next(7));
            TS_ASSERT_EQUALS(9, hSetTest->next(8));
            TS_ASSERT_EQUALS(9, hSetTest->next(9));
            TS_ASSERT_EQUALS(-1, hSetTest->next(10));
            TS_ASSERT(!hSetTest->add(9));
            TS_ASSERT_EQUALS(7, hSetTest->next(0));
            TS_ASSERT_EQUALS(7, hSetTest->next(7));
            TS_ASSERT_EQUALS(9, hSetTest->next(8));
            TS_ASSERT_EQUALS(9, hSetTest->next(9));
            TS_ASSERT_EQUALS(-1, hSetTest->next(10));

            PartitionSet::Handle hSetTest2 = TestPartitionSet::create(default_size);

            hSetTest = TestPartitionSet::create(default_size);

            TS_ASSERT(hSetTest->add(hSetTest2)); // REVIEW
            TS_ASSERT_EQUALS(-1, hSetTest->next(0));

            hSetTest2->add(1);

            TS_ASSERT(hSetTest->add(hSetTest2));
            TS_ASSERT_EQUALS(1, hSetTest->next(0));
            TS_ASSERT_EQUALS(1, hSetTest->next(1));
            TS_ASSERT_EQUALS(-1, hSetTest->next(2));

            hSetTest  = TestPartitionSet::create(default_size);
            hSetTest2 = TestPartitionSet::create(default_size);

            hSetTest->add(1);
            hSetTest2->add(1);

            TS_ASSERT(!hSetTest->add(hSetTest2));
            TS_ASSERT_EQUALS(1, hSetTest->next(0));
            TS_ASSERT_EQUALS(1, hSetTest->next(1));
            TS_ASSERT_EQUALS(-1, hSetTest->next(2));

            hSetTest  = TestPartitionSet::create(default_size);
            hSetTest2 = TestPartitionSet::create(default_size);

            hSetTest->add(1);
            hSetTest2->add(8);
            hSetTest2->add(16);

            TS_ASSERT(hSetTest->add(hSetTest2));
            TS_ASSERT_EQUALS(1, hSetTest->next(0));
            TS_ASSERT_EQUALS(1, hSetTest->next(1));
            TS_ASSERT_EQUALS(8, hSetTest->next(2));
            TS_ASSERT_EQUALS(8, hSetTest->next(8));
            TS_ASSERT_EQUALS(16, hSetTest->next(9));
            TS_ASSERT_EQUALS(16, hSetTest->next(16));
            TS_ASSERT_EQUALS(-1, hSetTest->next(17));

            hSetTest  = TestPartitionSet::create(default_size);
            hSetTest2 = TestPartitionSet::create(default_size);

            hSetTest->add(1);
            hSetTest2->add(1);
            hSetTest2->add(16);

            TS_ASSERT(!hSetTest->add(hSetTest2));
            TS_ASSERT_EQUALS(1, hSetTest->next(0));
            TS_ASSERT_EQUALS(1, hSetTest->next(1));
            TS_ASSERT_EQUALS(16, hSetTest->next(2));
            TS_ASSERT_EQUALS(16, hSetTest->next(16));
            TS_ASSERT_EQUALS(-1, hSetTest->next(17));
            }

        void testBug23083658()
            {
            PartitionSet::Handle hSetTest = TestPartitionSet::create(256);

            TS_ASSERT(hSetTest->add(255));
            TS_ASSERT(hSetTest->add(1));

            TS_ASSERT_EQUALS(1, hSetTest->next(0));
            TS_ASSERT_EQUALS(1, hSetTest->next(1));
            TS_ASSERT_EQUALS(255, hSetTest->next(2));
            TS_ASSERT_EQUALS(255, hSetTest->next(255));
            TS_ASSERT_EQUALS(-1, hSetTest->next(256));
            }

        void testNext()
            {
            PartitionSet::Handle hSetTest = TestPartitionSet::create(default_size);

            TS_ASSERT_THROWS(hSetTest->next(default_size + 1), IndexOutOfBoundsException::View);

            TS_ASSERT_EQUALS(-1, hSetTest->next(0));
            TS_ASSERT_EQUALS(-1, hSetTest->next(default_size));
            }

    private:
        /**
         * The default PartitionSet size.
         */
        static const int32_t default_size;
    };

const int32_t PartitionSetTest::default_size = 17;
