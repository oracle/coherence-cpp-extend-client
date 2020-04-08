/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashSet.hpp"
#include "coherence/util/Hashtable.hpp"
#include "coherence/util/SafeHashSet.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for HashSet.
*/
class HashSetTest : public CxxTest::TestSuite
    {

    // ----- inner classes -----------------------------------------

    /**
    * Test MapListener used to verify that the toArray() constructs
    * an ObjectArray that has listeners that can be cast to
    * MapListener::Handle.
    */
    class TestMapListener
        : public class_spec<TestMapListener,
            extends<Object>,
            implements<MapListener> >
        {
        friend class factory<TestMapListener>;

        protected:
            TestMapListener()
                {
                }

        public:
            virtual void entryInserted(MapEvent::View /* vEvent */)
                {}

            virtual void entryUpdated(MapEvent::View /* vEvent */)
                {}

            virtual void entryDeleted(MapEvent::View /* vEvent */)
                {}

        };

    public:
        void testCreate()
            {
            HashSet::Handle hSet = HashSet::create();
            TS_ASSERT(hSet != NULL);

            Map::Handle hMap = Hashtable::create();
            hMap->put(Integer32::create(123), Integer32::create(456));
            hMap->put(Integer32::create(234), Integer32::create(567));
            hMap->put(Integer32::create(345), Integer32::create(678));

            Collection::View vCol = hMap->values();
            hSet = HashSet::create(vCol);
            TS_ASSERT(vCol->equals((Collection::View) hSet));
            }

        void testAdd()
            {
            HashSet::Handle hSet   = HashSet::create();
            Integer32::View     vKey   = Integer32::create(123);


            TS_ASSERT(hSet->add(vKey) == true);
            TS_ASSERT(hSet->add(vKey) == false);
            }

        void testAddAll()
            {
            Map::Handle hMap = Hashtable::create();
            hMap->put(Integer32::create(123), String::create("ABC"));
            hMap->put(Integer32::create(234), String::create("BCD"));
            hMap->put(Integer32::create(345), String::create("CDE"));
            Collection::View vCol  = hMap->values();

            HashSet::Handle hSet = HashSet::create();
            hSet->addAll(vCol);
            TS_ASSERT(vCol->equals((Collection::View) hSet));
            }

        void testAddHandle()
            {
            HashSet::Handle   hSet = HashSet::create();
            Integer32::Handle hKey = Integer32::create(123);

            hSet->add(hKey);
            Iterator::Handle hIter = hSet->iterator();
            Integer32::Handle hGet = cast<Integer32::Handle>(hIter->next());
            TS_ASSERT(hGet == hKey);
            }

        void testContains()
            {
            HashSet::Handle hSet   = HashSet::create();
            Integer32::View     vKey   = Integer32::create(123);

            TS_ASSERT(hSet->contains(vKey) == false);
            TS_ASSERT(hSet->add(vKey) == true);
            TS_ASSERT(hSet->contains(vKey) == true);
            }

        void testRetainAll()
            {
            HashSet::Handle set = HashSet::create();
            String::View vs = String::create("test");
            String::View vs2 = String::create("test2");
            String::View vs3 = String::create("test3");
            set->add(vs);
            set->add(vs2);

            HashSet::Handle set2 = HashSet::create();
            set2->add(vs);
            set2->add(vs3);
            set2->retainAll(set);
            TS_ASSERT(set2->size() == 1);
            TS_ASSERT(set2->contains(vs));
            }

        void testSafeHashSet()
            {
            SafeHashSet::Handle hSet = SafeHashSet::create();
            TS_ASSERT(hSet != NULL);

            Integer32::Handle hKey = Integer32::create(123);
            TS_ASSERT(hSet->contains(hKey) == false);
            hSet->add(hKey);
            TS_ASSERT(hSet->contains(hKey) == true);
            Iterator::Handle hIter = hSet->iterator();
            Integer32::Handle hGet = cast<Integer32::Handle>(hIter->next());
            TS_ASSERT(hGet == hKey);
            }

        void testtoArray()
            {
            HashSet::Handle hSet = HashSet::create();
            TS_ASSERT(hSet != NULL);

            TestMapListener::Handle hKey1 = TestMapListener::create();
            TestMapListener::Handle hKey2 = TestMapListener::create();
            hSet->add(hKey1);
            hSet->add(hKey2);

            ObjectArray::Handle hoa = hSet->toArray();
            for (size32_t i = 0; i < hoa->length; i++)
                {
                MapListener::Handle hMapListener = cast<MapListener::Handle>(hoa[i]);
                }
            }
    };
