/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/DeltaSet.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for DeltaSet.
*/
class DeltaSetTest : public CxxTest::TestSuite
    {
    public:
        void testCreate()
            {
            HashSet::Handle hHashSet = HashSet::create();
            DeltaSet::Handle hSet = DeltaSet::create(hHashSet);
            TS_ASSERT(hSet != NULL);
            }

        void testAdd()
            {
            HashSet::Handle hHashSet = HashSet::create();
            DeltaSet::Handle hSet   = DeltaSet::create(hHashSet);
            Integer32::View     vKey   = Integer32::create(123);


            TS_ASSERT(hSet->add(vKey) == true);
            TS_ASSERT(hSet->add(vKey) == false);
            }

        void testContains()
            {
            HashSet::Handle hHashSet = HashSet::create();
            DeltaSet::Handle hSet   = DeltaSet::create(hHashSet);
            Integer32::View     vKey   = Integer32::create(123);

            TS_ASSERT(hSet->contains(vKey) == false);
            TS_ASSERT(hSet->add(vKey) == true);
            TS_ASSERT(hSet->contains(vKey) == true);
            }

        void testRetainAll()
            {
            HashSet::Handle hHashSet = HashSet::create();
            DeltaSet::Handle set = DeltaSet::create(hHashSet);
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            set->add(vs);
            set->add(vs2);

            HashSet::Handle hHashSet2 = HashSet::create();
            DeltaSet::Handle set2 = DeltaSet::create(hHashSet2);
            set2->add(vs);
            set2->add(vs3);
            set2->retainAll(set);
            TS_ASSERT(set2->size() == 1);
            TS_ASSERT(set2->contains(vs));
            }
    };
