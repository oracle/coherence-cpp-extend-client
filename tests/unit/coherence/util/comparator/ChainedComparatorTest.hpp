/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/comparator/ChainedComparator.hpp"
#include "coherence/util/comparator/InverseComparator.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "common/TestClasses.hpp"

using namespace coherence::lang;

using coherence::util::comparator::ChainedComparator;
using coherence::util::comparator::InverseComparator;
using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::IdentityExtractor;
using common::test::TestQueryMapEntry;

/**
* Test Suite for ChainedComparator.
*/
class ChainedComparatorSuite : public CxxTest::TestSuite
    {
    public:
		void testSafeComparator()
			{
			InverseComparator::Handle hSafe = InverseComparator::create(IdentityExtractor::getInstance());
			Comparator::View vCompa = hSafe->getComparator();
			TS_ASSERT(instanceof<IdentityExtractor::View>(vCompa));
			}

        /**
        * Test ChainedComparator
        */
        void testChainedComparator()
            {
            ObjectArray::Handle hoa = ObjectArray::create(2);
			hoa[0] = SafeComparator::create(IdentityExtractor::getInstance());
			hoa[1] = InverseComparator::create(IdentityExtractor::getInstance());

			ChainedComparator::Handle hcc1 = ChainedComparator::create(hoa);
			ChainedComparator::Handle hcc2 = ChainedComparator::create(hoa);
			TS_ASSERT(hcc1 != NULL);
			//TODO: uncomment when the implementation of equals is complete
            //TS_ASSERT(hcc1->equals(hcc2));

			TS_ASSERT(hcc1->getComparators() != NULL);
			TS_ASSERT(instanceof<SafeComparator::View>(hcc1->getComparators()[0]));
			TS_ASSERT(instanceof<InverseComparator::View>(hcc1->getComparators()[1]));
			TS_ASSERT(hcc1->compare(NULL, NULL) == 0);
			TS_ASSERT(hcc1->compare(Object::create(), NULL) == 1);
			TS_ASSERT(hcc1->compare(NULL, Object::create()) == -1);

			try
				{
				hcc1->compare(Object::create(), Object::create());
				}
			catch(Exception::View vex)
				{
				TS_ASSERT(instanceof<ClassCastException::View>(vex));
				}

			TS_ASSERT(hcc1->compare(Integer32::create(10), Integer32::create(100)) == -1);

			TestQueryMapEntry::View vEntry1 = TestQueryMapEntry::create(String::create("k2"), String::create("aaa"));
            TestQueryMapEntry::View vEntry2 = TestQueryMapEntry::create(String::create("k1"), String::create("zzz"));

			TS_ASSERT(hcc1->compareEntries(vEntry2, vEntry1) == -1);
            }
    };
