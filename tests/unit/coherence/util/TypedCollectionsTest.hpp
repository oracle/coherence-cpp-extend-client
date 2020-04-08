/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/net/cache/LocalCache.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/TypedCollections.hpp"

using namespace coherence::lang;

using coherence::net::cache::LocalCache;
using coherence::util::ArrayList;

/**
* Test Suite for the TypedCollections.
*/
class TypedCollectionsTest : public CxxTest::TestSuite
    {
    public:
        typedef TypedCollections::TypedMap<String::View, BoxHandle<const Integer32> > SIMap;
        typedef TypedCollections::TypedList<String::View> StringList;
        typedef TypedCollections::TypedSet<String::View> StringSet;

        void testMapCreate()
            {
            SIMap::Handle hsiMap = SIMap::create(LocalCache::create());
            }

        void testMapPut()
            {
            SIMap::Handle hsiMap = SIMap::create(LocalCache::create());
            TS_ASSERT(hsiMap->put("hello", 123) == Integer32::View(NULL));
            }

        void testMapGet()
            {
            SIMap::Handle hsiMap = SIMap::create(LocalCache::create());

            hsiMap->put("hello", 123);
            TS_ASSERT(hsiMap->get("hello") == 123);
            }

        void testMapEntrySet()
            {
            SIMap::Handle hsiMap = SIMap::create(LocalCache::create());
            hsiMap->put("hello", 123);
            hsiMap->put("world", 456);

            TS_ASSERT(hsiMap->entrySetTyped()->size() == 2);
            int32_t cIters = 0;
            for (SIMap::EntrySet::Iterator::Handle iter =
                    hsiMap->entrySetTyped()->iteratorTyped(); iter->hasNext(); )
                {
                ++cIters;
                SIMap::Entry::View vEntry = iter->nextTyped();
                TS_ASSERT(hsiMap->entrySetTyped()->contains(vEntry));
                if (vEntry->getKeyTyped()->equals("hello"))
                    {
                    TS_ASSERT(vEntry->getValueTyped() == 123);
                    }
                else if (vEntry->getKeyTyped()->equals("world"))
                    {
                    TS_ASSERT(vEntry->getValueTyped() == 456);
                    }
                else
                    {
                    TS_ASSERT(false);
                    }
                }
            TS_ASSERT(cIters == 2);
            }

        // ---- TypedList tests ---------------------------------------------

        void testListAddGetTyped()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            list->add(vs);
            String::View vVal = list->getTyped(0);
            TS_ASSERT(vVal== vs);

            list->add(1, vs2);
            vVal = list->getTyped(1);
            TS_ASSERT(vVal == vs2);
            }

        void testListIteratorTyped()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            String::View vs4 = String::create("test4");
            list->add(vs);
            list->add(vs2);
            list->add(vs3);
            list->add(vs4);

            StringList::Iterator::Handle hIter = list->iteratorTyped();
            String::View result = hIter->nextTyped();
            TS_ASSERT(result == vs);
            result = hIter->nextTyped();
            TS_ASSERT(result == vs2);
            result = hIter->nextTyped();
            TS_ASSERT(result == vs3);
            result = hIter->nextTyped();
            TS_ASSERT(result == vs4);
            }

        void testListListIteratorTyped()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            String::View vs4 = String::create("test4");
            list->add(vs);
            list->add(vs2);
            list->add(vs3);
            list->add(vs4);

            StringList::ListIterator::Handle hIter = list->listIteratorTyped();
            String::View result = hIter->nextTyped();
            TS_ASSERT(result == vs);
            result = hIter->nextTyped();
            TS_ASSERT(result == vs2);
            result = hIter->nextTyped();
            TS_ASSERT(result == vs3);
            result = hIter->nextTyped();
            TS_ASSERT(result == vs4);

            result = hIter->previousTyped();
            TS_ASSERT(result == vs4);

            hIter = list->listIteratorTyped(2);
            result = hIter->nextTyped();
            TS_ASSERT(result == vs3);


            StringList::ListIterator::Handle hMuter = list->listIteratorTyped();
            TS_ASSERT(vs == hMuter->next());
            hMuter->remove();
            TS_ASSERT(! hMuter->hasPrevious());

            hMuter->add(vs4);
            TS_ASSERT(hMuter->previous() == vs4);

            hMuter->set(vs);
            TS_ASSERT(hMuter->next() == vs);
            }

        void testListListIterator()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            String::View vs4 = String::create("test4");
            list->add(vs);
            list->add(vs2);
            list->add(vs3);
            list->add(vs4);

            ListMuterator::Handle hIter = list->listIterator();
            TS_ASSERT(hIter->next() == vs);
            TS_ASSERT(hIter->next() == vs2);
            TS_ASSERT(hIter->next() == vs3);
            TS_ASSERT(hIter->next() == vs4);

            TS_ASSERT(hIter->previous() == vs4);

            hIter = list->listIterator(2);
            TS_ASSERT(hIter->next() == vs3);


            ListMuterator::Handle hMuter = list->listIterator();
            TS_ASSERT(vs == hMuter->next());
            hMuter->remove();
            TS_ASSERT(! hMuter->hasPrevious());

            hMuter->add(vs4);
            TS_ASSERT(hMuter->previous() == vs4);

            hMuter->set(vs);
            TS_ASSERT(hMuter->next() == vs);
            }

        void testListIterator()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            String::View vs4 = String::create("test4");
            list->add(vs);
            list->add(vs2);
            list->add(vs3);
            list->add(vs4);

            Iterator::Handle hIter = list->iterator();
            TS_ASSERT(hIter->next() == vs);
            TS_ASSERT(hIter->next() == vs2);
            TS_ASSERT(hIter->next() == vs3);
            TS_ASSERT(hIter->next() == vs4);
            TS_ASSERT(!hIter->hasNext());
            }



        void testListRemoveTyped()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            list->add(vs);

            String::View result = list->removeTyped(0);
            TS_ASSERT(result == vs);
            }

        void testListRemove()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            list->add(vs);

            // TS_ASSERT(list->remove(0) == vs); // crashes SunPro compiler
            TS_ASSERT(((List::Handle) list)->remove(0) == vs);
            }

        void testListSet()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            list->add(vs);

            String::View vs2 = String::create("test2");
            String::View result = list->set(0, vs2);
            TS_ASSERT(result == vs);
            TS_ASSERT(vs2 == list->get(0));
            }

        void testListSubList()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            String::View vs4 = String::create("test4");
            list->add(vs);
            list->add(vs2);
            list->add(vs3);
            list->add(vs4);

            StringList::Handle subList = list->subListTyped(2, 3);
            String::View result = subList->getTyped(0);
            TS_ASSERT(result == vs3);
            }

        void testListAddGet()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            list->add(vs);
            TS_ASSERT(list->get(0)== vs);
            list->add(1, vs2);
            TS_ASSERT(vs2 == list->getTyped(1));
            }

        void testListIndexOf()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            list->add(vs);
            list->add(vs2);
            list->add(vs);
            list->add(vs2);

            TS_ASSERT(list->indexOf(vs2) == 1);
            }

        void testListLastIndexOf()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            list->add(vs);
            list->add(vs2);
            list->add(vs);
            list->add(vs2);

            TS_ASSERT(list->lastIndexOf(vs) == 2);
            }

        void testListContains()
        {
        StringList::Handle list = StringList::create(ArrayList::create());
        String::View vs = String::create("test");
        list->add(vs);
        TS_ASSERT(list->contains(vs));
        }

        void testListAsTypedCollection()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            list->add(vs);

            TypedCollections::TypedCollection<String::View>::Handle coll =
                    list->asTypedCollection();
            TS_ASSERT(coll->size() == 1);
            }

        void testListAddAll()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            list->add(vs);
            list->add(vs2);

            StringList::Handle list2 = StringList::create(ArrayList::create());
            TS_ASSERT(list2->addAll(list));
            TS_ASSERT(list2->size() == 2);
            }

        void testListRetainAll()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            list->add(vs);
            list->add(vs2);

            StringList::Handle list2 = StringList::create(ArrayList::create());
            TS_ASSERT(list2->add(vs));
            TS_ASSERT(list2->add(vs3));
            list2->retainAll(list);
            TS_ASSERT(list2->size() == 1);
            TS_ASSERT(vs == list2->get(0));
            }

        void testListRemoveAll()
            {
            StringList::Handle list = StringList::create(ArrayList::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            list->add(vs);
            list->add(vs2);

            StringList::Handle list2 = StringList::create(ArrayList::create());
            list2->add(vs);
            list2->add(vs2);
            list2->add(vs3);
            list2->removeAll(list);
            TS_ASSERT(list2->size() == 1);
            TS_ASSERT(vs3 == list2->get(0));
            }

        // ----- TypedSet tests ---------------------------------------------

        void testSetAddAll()
            {
            StringSet::Handle set = StringSet::create((Set::Handle)HashSet::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            set->add(vs);
            set->add(vs2);

            StringSet::Handle set2 = StringSet::create((Set::Handle)HashSet::create());
            TS_ASSERT(set2->addAll(set));
            TS_ASSERT(set2->size() == 2);
            }

        void testSetRetainAll()
            {
            StringSet::Handle set = StringSet::create((Set::Handle)HashSet::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            set->add(vs);
            set->add(vs2);

            StringSet::Handle set2 = StringSet::create((Set::Handle)HashSet::create());
            set2->add(vs);
            set2->add(vs3);
            set2->retainAll(set);
            TS_ASSERT(set2->size() == 1);
            TS_ASSERT(set2->contains(vs));
            }

        void testSetRemoveAll()
            {
            StringSet::Handle set = StringSet::create((Set::Handle)HashSet::create());
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            set->add(vs);
            set->add(vs2);

            StringSet::Handle set2 = StringSet::create((Set::Handle)HashSet::create());
            set2->add(vs);
            set2->add(vs2);
            set2->add(vs3);
            set2->removeAll(set);
            TS_ASSERT(set2->size() == 1);
            TS_ASSERT(set2->contains(vs3));
            }

        void testHandleCollection()
            {
            typedef TypedCollections::TypedMap<String::View, Integer32::Handle> StringIntMap;
            StringIntMap::Handle hMap = StringIntMap::create(HashMap::create());
            hMap->put("abc", Integer32::create(123));
            Integer32::Handle hInt = hMap->get("abc");
            TS_ASSERT(hInt->getValue() == 123);
            }
    };

