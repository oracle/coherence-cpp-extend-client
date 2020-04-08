/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashSet.hpp"
#include "coherence/util/LiteSet.hpp"
#include "coherence/util/Set.hpp"

using namespace coherence::lang;

using coherence::util::HashSet;
using coherence::util::LiteSet;
using coherence::util::Set;

/**
* Test Suite for LiteSet.
*/
class LiteSetTest : public CxxTest::TestSuite
    {
    public:
        void testCreate()
            {
            Set::Handle hLiteSet = LiteSet::create();
            hLiteSet->add(String::create("test"));
            hLiteSet->add(String::create("test1"));
            TS_ASSERT(hLiteSet->add(String::create("test2")));
            TS_ASSERT(!hLiteSet->add(String::create("test2")));
            TS_ASSERT_EQUALS((size32_t) 3, hLiteSet->size());

            Set::Handle hHashSet = HashSet::create();
            hHashSet->add(String::create("key"));
            hLiteSet = LiteSet::create(hHashSet);
            TS_ASSERT_EQUALS((size32_t) 1, hLiteSet->size());

            hHashSet->add(String::create("key2"));
            hLiteSet = LiteSet::create(hHashSet);
            TS_ASSERT_EQUALS((size32_t) 2, hLiteSet->size());

            hHashSet->add(String::create("key3"));
            hLiteSet = LiteSet::create(hHashSet);
            TS_ASSERT_EQUALS((size32_t) 3, hLiteSet->size());

            Set::View vClone = cast<Set::View>(hLiteSet->clone());
            TS_ASSERT(vClone->contains(String::create("key")));

            // Test going from empty to beyond the optimized size
            hLiteSet = LiteSet::create();
            hHashSet = HashSet::create();
            for (size32_t i = 1; i <= 10; ++i)
                {
                hHashSet->add(Integer32::create(i));
                TS_ASSERT(hLiteSet->add(Integer32::create(i)));
                TS_ASSERT_EQUALS(i, hLiteSet->size());
                }

            // Populate from another set that exceeds the optimized size
            TS_ASSERT_EQUALS(hHashSet->size(), LiteSet::create(hHashSet)->size());
            }

        void testContainsAndRemove()
            {
            String::View vs1 = "test1";
            String::View vs2 = "test2";
            String::View vs3 = "test3";

            Set::Handle hSet = LiteSet::create();
            TS_ASSERT(!hSet->contains(vs1));
            TS_ASSERT(hSet->isEmpty());
            TS_ASSERT_EQUALS((size32_t) 0, hSet->size())

            hSet->add(vs1);
            TS_ASSERT(hSet->contains(vs1));
            TS_ASSERT(!hSet->contains(vs2));
            TS_ASSERT(!hSet->isEmpty());
            TS_ASSERT_EQUALS((size32_t) 1, hSet->size())

            hSet->add(vs2);
            TS_ASSERT(hSet->contains(vs1));
            TS_ASSERT(hSet->contains(vs2));
            TS_ASSERT(!hSet->contains(vs3));
            TS_ASSERT_EQUALS((size32_t) 2, hSet->size())

            hSet->add(vs3);
            TS_ASSERT(hSet->contains(vs1));
            TS_ASSERT(hSet->contains(vs2));
            TS_ASSERT(hSet->contains(vs3));
            TS_ASSERT_EQUALS((size32_t) 3, hSet->size())

            TS_ASSERT(hSet->remove(vs1));
            TS_ASSERT(!hSet->remove(vs1));
            TS_ASSERT(!hSet->contains(vs1));
            TS_ASSERT(hSet->contains(vs2));
            TS_ASSERT(hSet->contains(vs3));
            TS_ASSERT_EQUALS((size32_t) 2, hSet->size())

            TS_ASSERT(hSet->remove(vs2));
            TS_ASSERT(!hSet->remove(vs2));
            TS_ASSERT(!hSet->contains(vs2));
            TS_ASSERT(hSet->contains(vs3));
            TS_ASSERT_EQUALS((size32_t) 1, hSet->size())

            TS_ASSERT(hSet->remove(vs3));
            TS_ASSERT(!hSet->remove(vs3));
            TS_ASSERT(!hSet->contains(vs3));
            TS_ASSERT(hSet->isEmpty());
            TS_ASSERT_EQUALS((size32_t) 0, hSet->size())
            }

        void testIterator()
            {
            Set::Handle hHashSet = HashSet::create();
            Integer32::View     vKey = Integer32::create(123);
            TS_ASSERT(hHashSet->contains(vKey) == false);
            TS_ASSERT(hHashSet->add(vKey) == true);

            Set::Handle hLiteSet = LiteSet::create(hHashSet);

            TS_ASSERT(hLiteSet->contains(vKey) == true);

            for (Iterator::Handle hIter = hLiteSet->iterator(); hIter->hasNext();)
                {
                Object::View vValue = hIter->next();
                TS_ASSERT(vValue->equals(vKey));
                }

            hLiteSet->remove(vKey);
            TS_ASSERT(hLiteSet->contains(vKey) == false);
            }

        void testRetainAll()
            {
            Set::Handle  hHashSet = HashSet::create();
            String::View vs       = String::create("test");
            String::View vs2      = String::create("test2");
            String::View vs3      = String::create("test3");
            hHashSet->add(vs);
            hHashSet->add(vs2);

            Set::Handle hHashSet2 = HashSet::create();
            hHashSet2->add(vs);
            hHashSet2->add(vs3);
            Set::Handle hLiteSet = LiteSet::create(hHashSet2);
            hLiteSet->retainAll(hHashSet);
            TS_ASSERT_EQUALS((size32_t) 1, hLiteSet->size());
            TS_ASSERT(hLiteSet->contains(vs));
            }
    };
