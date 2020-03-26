/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

using namespace coherence::lang;
using namespace coherence::util;
using namespace coherence::util::filter;

/**
* Test Suite for InvocableMapHelper.
*/
class InvocableMapHelperTest : public CxxTest::TestSuite
    {
    public:
        void testQuery()
            {
            Map::Handle map = HashMap::create();
            map->put(String::create("one"),   Integer32::create(1));
            map->put(String::create("two"),   Integer32::create(2));
            map->put(String::create("three"), Integer32::create(3));
            map->put(String::create("four"),  Integer32::create(4));
            map->put(String::create("five"),  Integer32::create(5));

            // simple query for the key set with an always filter
            Set::View set = InvocableMapHelper::query(map, NULL, AlwaysFilter::getInstance(),
                    false, false, NULL);

            TS_ASSERT(map->keySet()->equals(set));

            // simple query for the key set with an greater filter
            set = InvocableMapHelper::query(map, NULL,
                    GreaterFilter::create(IdentityExtractor::create(), Integer32::create(3)),
                    false, false, NULL);

            TS_ASSERT(set->size() == 2);
            TS_ASSERT(set->contains(String::create("four")));
            TS_ASSERT(set->contains(String::create("five")));

            // simple query for the entry set with an always filter
            set = InvocableMapHelper::query(map, NULL, AlwaysFilter::getInstance(),
                    true, false, NULL);

            TS_ASSERT(map->entrySet()->equals(set));

            // simple query for the entry set with an always filter and sort
            set = InvocableMapHelper::query(map, NULL, AlwaysFilter::getInstance(),
                    true, true, NULL);

            TS_ASSERT(set->size() == 5);
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(1)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(2)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(3)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(4)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(5)));

            // simple query for the entry set with an greater filter
            set = InvocableMapHelper::query(map, NULL,
                    GreaterFilter::create(IdentityExtractor::create(), Integer32::create(3)),
                    true, false, NULL);

            TS_ASSERT(set->size() == 2);
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(4)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(5)));

            // simple query for the entry set with a limit filter and sort
            LimitFilter::Handle hFilter = LimitFilter::create(AlwaysFilter::getInstance(), 3);
            set = InvocableMapHelper::query(map, NULL, hFilter,
                    true, true, NULL);

            TS_ASSERT(set->size() == 3);
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(1)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(2)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(3)));

            hFilter->nextPage();
            set = InvocableMapHelper::query(map, NULL, hFilter,
                    true, true, NULL);

            TS_ASSERT(set->size() == 2);
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(4)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(5)));
            }

        void testQueryWithIndex()
            {
            Map::Handle map = HashMap::create();
            map->put(String::create("one"),   Integer32::create(1));
            map->put(String::create("two"),   Integer32::create(2));
            map->put(String::create("three"), Integer32::create(3));
            map->put(String::create("four"),  Integer32::create(4));
            map->put(String::create("five"),  Integer32::create(5));

            IdentityExtractor::Handle extractor  = IdentityExtractor::create();

            Map::Handle            mapIndex = HashMap::create();
            SimpleMapIndex::Handle index    = SimpleMapIndex::create(extractor, true, (Comparator::View)NULL);

            for (Iterator::Handle iter = map->entrySet()->iterator(); iter->hasNext();)
                {
                Map::Entry::View entry = cast<Map::Entry::View>(iter->next());
                index->insert(entry);
                }

            mapIndex->put(extractor, index);

            // simple query for the key set with an greater filter
            Set::View set = InvocableMapHelper::query(map, mapIndex,
                    GreaterFilter::create(extractor, Integer32::create(3)),
                    false, false, NULL);

            TS_ASSERT(set->size() == 2);
            TS_ASSERT(set->contains(String::create("four")));
            TS_ASSERT(set->contains(String::create("five")));

            // simple query for the key set with an greater filter
            set = InvocableMapHelper::query(map, mapIndex,
                    GreaterFilter::create(extractor, Integer32::create(3)),
                    true, false, NULL);

            TS_ASSERT(set->size() == 2);
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(4)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(5)));

            // simple query for the entry set with a limit filter and sort
            LimitFilter::Handle hFilter = LimitFilter::create(AlwaysFilter::getInstance(), 3);
            set = InvocableMapHelper::query(map, NULL, hFilter,
                    true, true, NULL);

            TS_ASSERT(set->size() == 3);
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(1)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(2)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(3)));

            hFilter->nextPage();
            set = InvocableMapHelper::query(map, NULL, hFilter,
                    true, true, NULL);

            TS_ASSERT(set->size() == 2);
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(4)));
            TS_ASSERT(checkEntrySetValue(set, Integer32::create(5)));
            }

    private:
        static bool checkEntrySetValue(Set::View entrySet, Object::View value)
            {
            for (Iterator::Handle iterator = entrySet->iterator(); iterator->hasNext();)
                {
                Map::Entry::View entry = cast<Map::Entry::View>(iterator->next());
                if (entry->getValue()->equals(value))
                    {
                    return true;
                    }
                }
            return false;
            }
    };
