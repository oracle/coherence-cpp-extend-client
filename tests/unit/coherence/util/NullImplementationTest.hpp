/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/NullImplementation.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::util::ArrayList;
using coherence::util::NullImplementation;


/**
* Test suite for the NullImplementation class.
*/
class NullImplementationTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test NullCollection creation.
        */
        void testNullCollection()
            {
            Collection::Handle hCol = NullImplementation::getCollection();
            TS_ASSERT(hCol != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullCollection::Handle>(hCol));
            TS_ASSERT(hCol->size() == 0);
            Collection::Handle hCol2 = ArrayList::create();
            TS_ASSERT(hCol->equals(hCol2));
            hCol2->add(String::create("test"));
            TS_ASSERT(hCol->equals(hCol2) == false);
            TS_ASSERT(hCol->toArray()->length == 0);

            Iterator::Handle hIter = hCol->iterator();
            TS_ASSERT(hIter != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullMuterator::Handle>(hIter));
            TS_ASSERT(hIter->hasNext() == false);
            }

        /**
        * Test NullCollection iterator current.
        */
        void testNullCollectionIteratorNext()
            {
            Iterator::Handle hIter = NullImplementation::NullCollection::getInstance()->iterator();
            TS_ASSERT_THROWS_ANYTHING(hIter->next());
            }

        /**
        * Test NullMap.
        */
        void testNullMap()
            {
            Map::Handle hNullMap = NullImplementation::getMap();
            TS_ASSERT(hNullMap != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullMap::View>(hNullMap));
            TS_ASSERT(hNullMap->size() == 0);
            hNullMap->put(String::create("key"), String::create("value"));
            TS_ASSERT(hNullMap->size() == 0);
            hNullMap->clear();
            TS_ASSERT(hNullMap->size() == 0);
            Map::Handle hMap = HashMap::create();
            TS_ASSERT(hNullMap->equals(hMap));
            hMap->put(String::create("key"), String::create("value"));
            TS_ASSERT(hNullMap->equals(hMap) == false);
            TS_ASSERT(hNullMap->containsKey(String::create("key")) == false);
            hNullMap->remove(String::create("key"));
            TS_ASSERT(hNullMap->size() == 0);

            Collection::Handle hKeys = cast<Collection::Handle>(hNullMap->keySet());
            TS_ASSERT(hKeys != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullSet::View>(hKeys));
            TS_ASSERT(hKeys->size() == 0);

            Collection::Handle hValues = cast<Collection::Handle>(hNullMap->values());
            TS_ASSERT(hValues != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullCollection::View>(hValues));
            TS_ASSERT(hValues->size() == 0);

            Iterator::Handle hIterator = hNullMap->entrySet()->iterator();
            TS_ASSERT(hIterator != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullMuterator::View>(hIterator));
            TS_ASSERT(hIterator->hasNext() == false);
            TS_ASSERT_THROWS_ANYTHING(hIterator->next());
            }

        /**
        * Test NullObservableMap.
        */
        void testNullObservableMap()
            {
            ObservableMap::Handle hObserveMap = NullImplementation::getObservableMap();
            TS_ASSERT(hObserveMap != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullObservableMap::View>(hObserveMap));
            TS_ASSERT(hObserveMap->size() == 0);
            Collection::Handle hList = ArrayList::create();
            hList->add(String::create("test"));
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("key1"), String::create("value1"));
            hMap->put(String::create("key2"), String::create("value2"));
            hObserveMap->putAll(hMap);
            TS_ASSERT(hObserveMap->size() == 0);
            Object::Holder hObj = hObserveMap->put(String::create("key1"), String::create("value1"));
            TS_ASSERT(hObserveMap->containsKey(String::create("key1")) == false);
            TS_ASSERT(hObserveMap->containsValue(String::create("value1")) == false);
            TS_ASSERT(hObj == NULL);

            Set::View vEntries = hObserveMap->entrySet();
            TS_ASSERT(vEntries != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullSet::View>(vEntries));
            TS_ASSERT(vEntries->size() == 0);
            Iterator::Handle hIterator = vEntries->iterator();
            TS_ASSERT(hIterator != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullMuterator::View>(hIterator));
            TS_ASSERT(hIterator->hasNext() == false);
            TS_ASSERT_THROWS_ANYTHING(hIterator->next());
            }

        /**
        * Test NullValueExtractor.
        */
        void testNullValueExtractor()
            {
            ValueExtractor::Handle hValueExtractor = NullImplementation::getValueExtractor();
            TS_ASSERT(hValueExtractor != NULL);
            TS_ASSERT(instanceof<NullImplementation::NullValueExtractor::View>(hValueExtractor));
            Object::Handle o = Object::create();
            Object::Holder o1 = hValueExtractor->extract(o);
            TS_ASSERT(o->equals(o1));
            }
    };
