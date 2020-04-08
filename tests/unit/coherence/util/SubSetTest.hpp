/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/SubSet.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for SubSet.
*/
class SubSetTest : public CxxTest::TestSuite
    {
    public:
        void testCreate()
            {
            HashSet::Handle hHashSet = HashSet::create();
            SubSet::Handle hSet = SubSet::create(hHashSet);
            TS_ASSERT(hSet != NULL);
            }

        void testContains()
            {
            HashSet::Handle hHashSet = HashSet::create();
            Integer32::View     vKey   = Integer32::create(123);
            TS_ASSERT(hHashSet->contains(vKey) == false);
            TS_ASSERT(hHashSet->add(vKey) == true);

            SubSet::Handle hSet   = SubSet::create(hHashSet);

            TS_ASSERT(hSet->contains(vKey) == true);
            }

        void testRetainAll()
            {
            HashSet::Handle hSetOrig = HashSet::create();
            String::View vs = String::create("i");
            String::View vs2 = String::create("ii");
            String::View vs3 = String::create("iii");
            String::View vs4 = String::create("iv");
            String::View vs5 = String::create("v");
            hSetOrig->add(vs);
            hSetOrig->add(vs2);
            hSetOrig->add(vs3);
            hSetOrig->add(vs4);
            hSetOrig->add(vs5);

            // test retaining 3 elements
            Set::Handle hSet = SubSet::create(hSetOrig);
            Collection::Handle hRetainCollA = HashSet::create();
            hRetainCollA->add(vs2);
            hRetainCollA->add(vs3);
            hRetainCollA->add(vs4);
            TS_ASSERT(hSet->retainAll(hRetainCollA));
            TS_ASSERT(hSet->size() == 3);
            TS_ASSERT(hSet->contains(vs2) && hSet->contains(vs3) && hSet->contains(vs4));

            // test subsequent retainAll from 3 elements to 2
            HashSet::Handle hRetainCollB = HashSet::create();
            hRetainCollB->add(vs2);
            hRetainCollB->add(vs4);
            TS_ASSERT(hSet->retainAll(hRetainCollB));
            TS_ASSERT(hSet->size() == 2);
            TS_ASSERT(hSet->contains(vs2) && hSet->contains(vs4));

            // test where passed set is significantly smaller than retained set
            hSet = SubSet::create(hSetOrig);
            hRetainCollA = HashSet::create();
            hRetainCollA->add(vs);
            hRetainCollA->add(vs2);
            hRetainCollA->add(vs3);
            hRetainCollA->add(vs4);
            TS_ASSERT(hSet->retainAll(hRetainCollA));
            TS_ASSERT(hSet->size() == 4);

            hRetainCollB = HashSet::create();
            hRetainCollB->add(vs2);
            TS_ASSERT(hSet->retainAll(hRetainCollB));
            TS_ASSERT(hSet->size() == 1);
            TS_ASSERT(hSet->contains(vs2));

            // test when no action is taken
            hSet = SubSet::create(hSetOrig);
            TS_ASSERT(!hSet->retainAll(hSetOrig));

            // test when items have also been removed
            hSet = SubSet::create(hSetOrig);
            TS_ASSERT(hSet->remove(vs2));
            TS_ASSERT(hSet->remove(vs4));

            hRetainCollA = HashSet::create();
            hRetainCollA->add(vs);
            hRetainCollA->add(vs3);
            hRetainCollA->add(vs5);
            TS_ASSERT(!hSet->retainAll(hRetainCollA));
            TS_ASSERT(hSet->size() == 3);
            TS_ASSERT(hSet->contains(vs) && hSet->contains(vs3) && hSet->contains(vs5));

            hRetainCollB = HashSet::create();
            hRetainCollB->add(vs);
            hRetainCollB->add(vs5);
            TS_ASSERT(hSet->retainAll(hRetainCollB));
            TS_ASSERT(hSet->size() == 2);
            TS_ASSERT(hSet->contains(vs) && hSet->contains(vs5));
            }
    };
