/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/util/Iterator.hpp"
#include "coherence/util/TreeSet.hpp"

#include <iostream>

using coherence::util::Iterator;
using coherence::util::SortedSet;
using coherence::util::TreeSet;

/**
* Test Suite for TreeSet
*/
class TreeSetTest
    : public CxxTest::TestSuite,
      public class_spec<TreeSetTest>
    {
    public:
        void testOrderedAdd()
            {
            SortedSet::Handle hTreeSet = ensureSet();

            Iterator::Handle hIter = hTreeSet->iterator();
            for (int32_t i = 0; hIter->hasNext(); ++i)
                {
                Integer32::View hnValue = cast<Integer32::View>(hIter->next());
                TS_ASSERT(hnValue->getInt32Value() == i);
                }
            }

        void testSubSet()
            {
            SortedSet::Handle hTreeSet = ensureSet();

            // x >= 1 && x < 4
            SortedSet::View vSubSet = hTreeSet->subSet(Integer32::create(1), Integer32::create(4));

            TS_ASSERT(vSubSet->size() == 3);
            TS_ASSERT(vSubSet->contains(Integer32::create(1)));
            TS_ASSERT(vSubSet->contains(Integer32::create(2)));
            TS_ASSERT(vSubSet->contains(Integer32::create(3)));
            }

        void testHeadSet()
            {
            SortedSet::Handle hTreeSet = ensureSet();

            // x < 3
            SortedSet::View vSubSet = hTreeSet->headSet(Integer32::create(3));

            TS_ASSERT(vSubSet->size() == 3);
            TS_ASSERT(vSubSet->contains(Integer32::create(0)));
            TS_ASSERT(vSubSet->contains(Integer32::create(1)));
            TS_ASSERT(vSubSet->contains(Integer32::create(2)));
            }

        void testTailSet()
            {
            SortedSet::Handle hTreeSet = ensureSet();

            // x > 1
            SortedSet::View vSubSet = hTreeSet->tailSet(Integer32::create(1));

            TS_ASSERT(vSubSet->size() == 3);
            TS_ASSERT(vSubSet->contains(Integer32::create(0)));
            TS_ASSERT(vSubSet->contains(Integer32::create(1)));
            TS_ASSERT(vSubSet->contains(Integer32::create(2)));
            }

        void testFirstAndLast()
            {
            SortedSet::Handle hTreeSet = ensureSet();

            // x > 1
            Integer32::View vFirst = cast<Integer32::View>(hTreeSet->first());
            Integer32::View vLast = cast<Integer32::View>(hTreeSet->last());

            TS_ASSERT(Integer32::create(0)->equals(vFirst));
            TS_ASSERT(Integer32::create(4)->equals(vFirst));
            }

    private:
        SortedSet::Handle ensureSet()
            {
            TreeSet::Handle hTreeSet = TreeSet::create();

            hTreeSet->add(Integer32::create(2));
            hTreeSet->add(Integer32::create(0));
            hTreeSet->add(Integer32::create(3));
            hTreeSet->add(Integer32::create(4));
            hTreeSet->add(Integer32::create(1));

            return hTreeSet;
            }
    };
