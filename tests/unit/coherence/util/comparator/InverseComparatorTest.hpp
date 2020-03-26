/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/comparator/InverseComparator.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "common/TestClasses.hpp"

using namespace coherence::lang;

using coherence::util::comparator::InverseComparator;
using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::KeyExtractor;
using coherence::util::extractor::ReflectionExtractor;
using common::test::TestQueryMapEntry;

/**
* Test Suite for InverseComparator.
*/
class InverseComparatorSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test InverseComparator
        */
        void testInverseComparator()
            {
			InverseComparator::View vIc1 = InverseComparator::create();
			InverseComparator::View vIc2 = InverseComparator::create();
			TS_ASSERT(vIc1 != NULL);
			TS_ASSERT(vIc1->equals(vIc2));
			
			InverseComparator::View vIc3 = InverseComparator::create(IdentityExtractor::getInstance());
			TS_ASSERT(vIc1->equals(vIc3) == false);
			TS_ASSERT(vIc3->getComparator() != NULL);
			TS_ASSERT(vIc3->getComparator() == IdentityExtractor::getInstance());
			
			TS_ASSERT(vIc1->compare(NULL, NULL) == 0);
			TS_ASSERT(vIc1->compare(Object::create(), NULL) == -1);
			TS_ASSERT(vIc1->compare(NULL, Object::create()) == 1);
			
			try 
				{
				vIc1->compare(Object::create(), Object::create());
				}
			catch(Exception::View vex)
				{
				TS_ASSERT(instanceof<ClassCastException::View>(vex));
				}
				
			SafeComparator::View vSc = SafeComparator::create(IdentityExtractor::getInstance());	
			TS_ASSERT(vIc1->compare(Integer32::create(10), Integer32::create(100)) == 1);
			TS_ASSERT(vIc3->compare(Integer32::create(10), Integer32::create(100)) == -vSc->compare(Integer32::create(10), Integer32::create(100)));
			
			TestQueryMapEntry::View vEntry1 = TestQueryMapEntry::create(String::create("k2"), String::create("aaa"));
            TestQueryMapEntry::View vEntry2 = TestQueryMapEntry::create(String::create("k1"), String::create("zzz"));
			
			vIc3 = InverseComparator::create(IdentityExtractor::getInstance());
			vSc = SafeComparator::create(IdentityExtractor::getInstance());	
			
			TS_ASSERT(vIc3->compareEntries(vEntry2, vEntry1) == 1);
			TS_ASSERT(vIc3->compareEntries(vEntry2, vEntry1) == -vSc->compareEntries(vEntry2, vEntry1));
			
			//TODO: uncomment when ReflectionExtractor::extract implementation is complete
			//InverseComparator::View vIc4 = InverseComparator::create(KeyExtractor::create(ReflectionExtractor::create(String::create("Key"))));
			//TS_ASSERT(vIc4->compare(vEntry1, vEntry2) == -1);			
			}
    };
