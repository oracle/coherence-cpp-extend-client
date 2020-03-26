/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"
#include "coherence/util/MapListenerSupport.hpp"
#include "coherence/util/ConverterCollections.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/Set.hpp"

using namespace std;
using namespace coherence::util;
using namespace mock;

COH_OPEN_NAMESPACE_ANON(MapListenerSupportTest)

class TestFilter
    : public class_spec<TestFilter,
        extends<Object>,
        implements<Filter> >
    {
    friend class factory<TestFilter>;

    protected:
        TestFilter()
            {
            }

    public: 
        virtual bool evaluate(Object::View /*v*/) const
            {
            return true;
            }
    };

COH_CLOSE_NAMESPACE_ANON

class MapListenerSupportTest : public CxxTest::TestSuite
    {
    public:

        void testAddListener_Filter()
            {
            // add first listener(non-lite)
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapListener::Handle hListener = MockMapListener::create();
            Filter::View vFilter = DummyFilter::create();

            hListenerSupport->addListener(hListener, vFilter, false);
            Listeners::View vListeners = hListenerSupport->getListeners(vFilter);
            TS_ASSERT(1 == vListeners->listeners()->length);
            TS_ASSERT(hListener == vListeners->listeners()[0]);
            TS_ASSERT(hListenerSupport->containsStandardListeners(vFilter));

            // add same listener but this time as lite
            hListenerSupport->addListener(hListener, vFilter, true);
            vListeners = hListenerSupport->getListeners(vFilter);
            TS_ASSERT(1 == vListeners->listeners()->length);
            TS_ASSERT(hListener == vListeners->listeners()[0]);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter));

            // add second listener(lite)
            hListener = MockMapListener::create();
            hListenerSupport->addListener(hListener, vFilter, true);
            vListeners = hListenerSupport->getListeners(vFilter);
            TS_ASSERT(2 == vListeners->listeners()->length);
            TS_ASSERT(hListener == vListeners->listeners()[1]);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter));
        }


        void testAddListener_Key()
            {
            // add first listener(non-lite)
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapListener::Handle hListener = MockMapListener::create();
            Object::View key = String::create("key");

            hListenerSupport->addListener(hListener, key, false);
            Listeners::View vListeners = hListenerSupport->getListeners(key);
            TS_ASSERT(1 == vListeners->listeners()->length);
            TS_ASSERT(hListener == vListeners->listeners()[0]);
            TS_ASSERT(hListenerSupport->containsStandardListeners(key));

            // add same listener but this time as lite
            hListenerSupport->addListener(hListener, key, true);
            vListeners = hListenerSupport->getListeners(key);
            TS_ASSERT(1 == vListeners->listeners()->length);
            TS_ASSERT(hListener == vListeners->listeners()[0]);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(key));

            // add second listener(lite)
            hListener = MockMapListener::create();
            hListenerSupport->addListener(hListener, key, true);
            vListeners = hListenerSupport->getListeners(key);
            TS_ASSERT(2 == vListeners->listeners()->length);
            TS_ASSERT(hListener == vListeners->listeners()[1]);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(key));

            //TODO: How to test optimizationPlan and listenersCached?
        }

        void testAddListener_Filter_NullListener()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            Filter::View vFilter = DummyFilter::create();

            hListenerSupport->addListener((MapListener::Handle) NULL, vFilter, false);
            TS_ASSERT(NULL == hListenerSupport->getListeners(vFilter));
            }

        void testAddListener_Key_NullListener()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            Object::View key = String::create("test");

            hListenerSupport->addListener((MapListener::Handle) NULL, key, false);
            TS_ASSERT(NULL == hListenerSupport->getListeners(key));
            }

        void testRemoveListener_Filter()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            Filter::View vFilter = DummyFilter::create();
            Filter::View vFilter2 = DummyFilter::create();
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();
            MapListener::Handle hListener3 = MockMapListener::create();

            hListenerSupport->addListener(hListener, vFilter, false);
            hListenerSupport->addListener(hListener2, vFilter2, true);
            hListenerSupport->addListener(hListener3, vFilter2, false);

            hListenerSupport->removeListener(hListener, vFilter);

            TS_ASSERT(NULL == hListenerSupport->getListeners(vFilter));

            TS_ASSERT(2 == hListenerSupport->getListeners(vFilter2)->listeners()->length);
            TS_ASSERT(hListenerSupport->containsStandardListeners(vFilter2));

            hListenerSupport->removeListener(hListener3, vFilter2);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter2));

            TS_ASSERT(1 == hListenerSupport->getListeners(vFilter2)->listeners()->length);

            hListenerSupport->removeListener(hListener2, vFilter2);
            TS_ASSERT(NULL == hListenerSupport->getListeners(vFilter2));
            }

        void testRemoveListener_Key()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            Object::View vKey = String::create("key");
            Object::View vKey2 = String::create("key2");
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();
            MapListener::Handle hListener3 = MockMapListener::create();

            hListenerSupport->addListener(hListener, vKey, false);
            hListenerSupport->addListener(hListener2, vKey2, true);
            hListenerSupport->addListener(hListener3, vKey2, false);

            hListenerSupport->removeListener(hListener, vKey);

            TS_ASSERT(NULL == hListenerSupport->getListeners(vKey));

            TS_ASSERT(2 == hListenerSupport->getListeners(vKey2)->listeners()->length);
            TS_ASSERT(hListenerSupport->containsStandardListeners(vKey2));

            hListenerSupport->removeListener(hListener3, vKey2);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vKey2));

            TS_ASSERT(1 == hListenerSupport->getListeners(vKey2)->listeners()->length);

            hListenerSupport->removeListener(hListener2, vKey2);
            TS_ASSERT(NULL == hListenerSupport->getListeners(vKey2));
            }

        void testClear()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            Object::View vKey = String::create("key");
            Filter::View vFilter = DummyFilter::create();
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();

            hListenerSupport->addListener(hListener, vKey, false);
            hListenerSupport->addListener(hListener2, vFilter, false);

            hListenerSupport->clear();

            TS_ASSERT(hListenerSupport->isEmpty());
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vKey));
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter));
            }

        void testIsEmpty()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();

            TS_ASSERT(hListenerSupport->isEmpty());

            Filter::View vFilter = DummyFilter::create();
            Object::View vKey = String::create("key");
            Filter::View vDummyKey = DummyFilter::create();
            MapListener::Handle hListener = MockMapListener::create();
            hListenerSupport->addListener(hListener, vFilter, false);

            TS_ASSERT(!hListenerSupport->isEmpty());
            TS_ASSERT(!hListenerSupport->isEmpty(vFilter));
            TS_ASSERT(hListenerSupport->isEmpty(vKey));
            TS_ASSERT(hListenerSupport->isEmpty(vDummyKey));

            hListenerSupport->addListener(hListener, vKey, true);

            TS_ASSERT(!hListenerSupport->isEmpty(vKey));

            hListenerSupport->removeListener(hListener, vFilter);
            TS_ASSERT(hListenerSupport->isEmpty(vFilter));
            TS_ASSERT(!hListenerSupport->isEmpty(vKey));
            TS_ASSERT(!hListenerSupport->isEmpty());

            hListenerSupport->removeListener(hListener, vKey);
            TS_ASSERT(hListenerSupport->isEmpty(vFilter));
            TS_ASSERT(hListenerSupport->isEmpty(vKey));
            TS_ASSERT(hListenerSupport->isEmpty());
            }

        void testContainsStandardlisteners()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();

            Filter::View vFilter = DummyFilter::create();
            Object::View vKey = String::create("key");
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();

            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter));
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vKey));

            hListenerSupport->addListener(hListener, vFilter, true);
            hListenerSupport->addListener(hListener, vKey, true);

            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter));
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vKey));

            hListenerSupport->addListener(hListener2, vFilter, false);
            TS_ASSERT(hListenerSupport->containsStandardListeners(vFilter));
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vKey));

            hListenerSupport->addListener(hListener2, vKey, false);
            TS_ASSERT(hListenerSupport->containsStandardListeners(vFilter));
            TS_ASSERT(hListenerSupport->containsStandardListeners(vKey));

            hListenerSupport->addListener(hListener2, vFilter, true);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter));
            TS_ASSERT(hListenerSupport->containsStandardListeners(vKey));

            hListenerSupport->addListener(hListener2, vKey, true);
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vFilter));
            TS_ASSERT(!hListenerSupport->containsStandardListeners(vKey));
            }

        void testGetFilterSet()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            TS_ASSERT(hListenerSupport->getFilterSet()->size() == 0);

            Filter::View vFilter = DummyFilter::create();
            Filter::View vFilter2 = DummyFilter::create();
            Filter::View vFilter3 = DummyFilter::create();
            MapListener::Handle hListener = MockMapListener::create();

            hListenerSupport->addListener(hListener, vFilter, true);
            hListenerSupport->addListener(hListener, vFilter2, false);
            hListenerSupport->addListener(hListener, vFilter3, false);

            Set::View vsetFilters = hListenerSupport->getFilterSet();
            TS_ASSERT(vsetFilters->size() == 3);
            TS_ASSERT(vsetFilters->contains(vFilter));
            TS_ASSERT(vsetFilters->contains(vFilter2));
            TS_ASSERT(vsetFilters->contains(vFilter3));
            }

        void testGetKeySet()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            TS_ASSERT(hListenerSupport->getKeySet()->size() == 0);

            Object::View vKey = String::create("key");
            Object::View vKey2 = String::create("key2");
            Object::View vKey3 = String::create("key3");
            MapListener::Handle hListener = MockMapListener::create();

            hListenerSupport->addListener(hListener, vKey, true);
            hListenerSupport->addListener(hListener, vKey2, false);
            hListenerSupport->addListener(hListener, vKey3, false);

            Set::View vsetKeys = hListenerSupport->getKeySet();
            TS_ASSERT(vsetKeys->size() == 3);
            TS_ASSERT(vsetKeys->contains(vKey));
            TS_ASSERT(vsetKeys->contains(vKey2));
            TS_ASSERT(vsetKeys->contains(vKey3));
            }

        void testGetListeners_Filter()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            Filter::View vFilter = DummyFilter::create();
            Filter::View vFilter2 = DummyFilter::create();
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();
            MapListener::Handle hListener3 = MockMapListener::create();

            TS_ASSERT(NULL == hListenerSupport->getListeners(vFilter));

            hListenerSupport->addListener(hListener, vFilter2, false);
            TS_ASSERT(NULL == hListenerSupport->getListeners(vFilter));
            TS_ASSERT(hListenerSupport->getListeners(vFilter2)->listeners()->length == 1  &&
                    hListenerSupport->getListeners(vFilter2)->contains(hListener));

            hListenerSupport->addListener(hListener2, vFilter, false);
            hListenerSupport->addListener(hListener3, vFilter, true);
            TS_ASSERT(hListenerSupport->getListeners(vFilter)->listeners()->length == 2);
            TS_ASSERT(hListenerSupport->getListeners(vFilter)->contains(hListener2));
            TS_ASSERT(hListenerSupport->getListeners(vFilter)->contains(hListener3));
            }

        void testGetListeners_Key()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            Object::View vKey = String::create("key");
            Object::View vKey2 = String::create("key2");
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();
            MapListener::Handle hListener3 = MockMapListener::create();

            TS_ASSERT(NULL == hListenerSupport->getListeners(vKey));

            hListenerSupport->addListener(hListener, vKey2, false);
            TS_ASSERT(NULL == hListenerSupport->getListeners(vKey));
            TS_ASSERT(hListenerSupport->getListeners(vKey2)->listeners()->length == 1  &&
                    hListenerSupport->getListeners(vKey2)->contains(hListener));

            hListenerSupport->addListener(hListener2, vKey, false);
            hListenerSupport->addListener(hListener3, vKey, true);
            TS_ASSERT(hListenerSupport->getListeners(vKey)->listeners()->length == 2);
            TS_ASSERT(hListenerSupport->getListeners(vKey)->contains(hListener2));
            TS_ASSERT(hListenerSupport->getListeners(vKey)->contains(hListener3));
            }

        void testCollectListeners_AllKey()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapEvent::View vEvent = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), String::create("old"), String::create("new"));
            TS_ASSERT(hListenerSupport->collectListeners(vEvent)->listeners()->length == 0);

            // add key listener with different key than event
            Object::View vKey = String::create("bogus-key");
            MapListener::Handle hListener = MockMapListener::create();
            hListenerSupport->addListener(hListener, vKey, false);
            TS_ASSERT(hListenerSupport->collectListeners(vEvent)->listeners()->length == 0);

            // add key listener with key matching event
            Object::View vKey2 = String::create("key");
            hListenerSupport->addListener(hListener, vKey2, false);
            Listeners::View vListeners = hListenerSupport->collectListeners(vEvent);
            TS_ASSERT(vListeners->listeners()->length == 1 && vListeners->contains(hListener));

            // add another listener with key matching event
            MapListener::Handle hListener2 = MockMapListener::create();
            hListenerSupport->addListener(hListener2, vKey2, true);
            vListeners = hListenerSupport->collectListeners(vEvent);
            TS_ASSERT(vListeners->listeners()->length == 2);
            TS_ASSERT(vListeners->contains(hListener));
            TS_ASSERT(vListeners->contains(hListener2));
            }

        void testCollectListeners_NullFilter()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapEvent::View vEvent = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), String::create("old"), String::create("new"));
            Filter::View vFilter = DummyFilter::create();
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();

            // add listener with NULL filter
            hListenerSupport->addListener(hListener, (Filter::View) NULL, false);
            Listeners::View vListeners = hListenerSupport->collectListeners(vEvent);
            TS_ASSERT(vListeners->listeners()->length == 1);
            TS_ASSERT(vListeners->contains(hListener));

            // added second listgener with NULL filter
            hListenerSupport->addListener(hListener2, (Filter::View) NULL, true);
            vListeners = hListenerSupport->collectListeners(vEvent);
            TS_ASSERT(vListeners->listeners()->length == 2);
            TS_ASSERT(vListeners->contains(hListener));
            TS_ASSERT(vListeners->contains(hListener2));
            }

        void testCollectListeners_FilterEvaluateFalse()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapEvent::View vEvent = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), String::create("old"), String::create("new"));
            MapListener::Handle hListener = MockMapListener::create();

            // add listener with filter that returns false from evaluate
            MockFilter::Handle hMockFilter = MockFilter::create();
            hMockFilter->setStrict(true);
            // set mock expectations;
            hMockFilter->evaluate(vEvent);
            hMockFilter->lastExpectation()->setBoolReturn(false);
            hMockFilter->replay();

            hListenerSupport->addListener(hListener, (Filter::View)hMockFilter, false);

            TS_ASSERT(hListenerSupport->collectListeners(vEvent)->listeners()->length == 0);
            hMockFilter->verify();
            }

        void testCollectListeners_FilterEvaluateTrue()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapEvent::View vEvent = MapEvent::create(TestObservableMap::create(),
                    1, String::create("key"), String::create("old"), String::create("new"));
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();

            // add listener with filter that returns true from evaluate
            MockFilter::Handle hMockFilter = MockFilter::create();
            hMockFilter->setStrict(true);
            // set mock expectations;
            hMockFilter->evaluate(vEvent);
            hMockFilter->lastExpectation()->setBoolReturn(true);
            hMockFilter->replay();

            hListenerSupport->addListener(hListener, (Filter::View)hMockFilter, false);
            hListenerSupport->addListener(hListener2, (Filter::View)hMockFilter, true);

            Listeners::View vListeners = hListenerSupport->collectListeners(vEvent);
            TS_ASSERT(vListeners->listeners()->length == 2);
            TS_ASSERT(vListeners->contains(hListener));
            TS_ASSERT(vListeners->contains(hListener2));
            hMockFilter->verify();
            }

        void testCollectListeners_FilterEvent()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();
            MapListener::Handle hListener3 = MockMapListener::create();
            TestFilter::Handle hTestFilter = TestFilter::create();
            TestFilter::Handle hTestFilter2 = TestFilter::create();

            // hTestFilter3 is equal to hTestFilter
            TestFilter::Handle hTestFilter3 = hTestFilter;

            // create filter array for event
            ObjectArray::Handle haFilters = ObjectArray::create(2);
            haFilters[0] = NULL;
            haFilters[1] = hTestFilter;

            // Create FilterEvent
            MapListenerSupport::FilterEvent::View vEvent = MapListenerSupport::FilterEvent::create(
                    TestObservableMap::create(), 1, String::create("key"), String::create("old"),
                    String::create("new"), false, haFilters);

            // Add with NULL filter.  Should be returned from collectListeners.
            hListenerSupport->addListener(hListener, (Filter::View)NULL, false);
            TS_ASSERT(hListenerSupport->collectListeners(vEvent)->listeners()->length == 1);

            // add with a non-NULL non-matching filter; shouldn't be returned
            hListenerSupport->addListener(hListener2, (Filter::View) hTestFilter2, false);
            TS_ASSERT(hListenerSupport->collectListeners(vEvent)->listeners()->length == 1);

            // add with a non-NULL matching filter; should be returned
            hListenerSupport->addListener(hListener3, (Filter::View) hTestFilter3, false);
            Listeners::View vListeners = hListenerSupport->collectListeners(vEvent);
            TS_ASSERT(vListeners->listeners()->length == 2);
            TS_ASSERT(vListeners->contains(hListener) && vListeners->contains(hListener3));
            }

        void testCollectListeners_ConverterCollection()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapListener::Handle hListener = MockMapListener::create();
            MapListener::Handle hListener2 = MockMapListener::create();
            TestFilter::Handle hTestFilter = TestFilter::create();

            // hTestFilter2 is equal to hTestFilter
            TestFilter::Handle hTestFilter2 = hTestFilter;

            // create filter array for event
            ObjectArray::Handle haFilters = ObjectArray::create(1);
            haFilters[0] = hTestFilter;

            // Create FilterEvent
            MapListenerSupport::FilterEvent::View vFilterEvent = MapListenerSupport::FilterEvent::create(
                    TestObservableMap::create(), 1, String::create("key"), String::create("old"),
                    String::create("new"), false, haFilters);

            // wrap filter event with a ConverterCacheEvent
            ConverterCollections::ConverterCacheEvent::View vEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(), vFilterEvent,
                            MockConverter::create(), MockConverter::create());

            // add with a non-NULL non-matching filter; shouldn't be returned
            hListenerSupport->addListener(hListener, (Filter::View) TestFilter::create(), false);
            TS_ASSERT(hListenerSupport->collectListeners(vEvent)->listeners()->length == 0);

            // add with a non-NULL matching filter; should be returned
            hListenerSupport->addListener(hListener2, (Filter::View) hTestFilter2, false);
            Listeners::View vListeners = hListenerSupport->collectListeners(vEvent);
            TS_ASSERT(vListeners->listeners()->length == 1);
            TS_ASSERT(vListeners->listeners()[0] == hListener2);
            }

        void testFireEvent()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();
            MapListener::Handle hListener = MockMapListener::create();
            MockMapEvent::Handle hEvent = MockMapEvent::create();

            // add listener with NULL filter
            hListenerSupport->addListener(hListener, (Filter::View) NULL, false);

            // set expectations of mock
            hEvent->setStrict(true);
            Listeners::View vListeners = hListenerSupport->collectListeners(hEvent);
            MapListenerSupport::enrichEvent(hEvent, vListeners)->dispatch(vListeners, false);
            hEvent->replay();

            hListenerSupport->fireEvent(hEvent, false);
            hEvent->verify();
            }

        void testConvertEvent_MapEvent()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();

            TestObservableMap::Handle hmap = TestObservableMap::create();
            MapEvent::View vMapEvent = MapEvent::create(TestObservableMap::create(), 1,
                    String::create("key"), String::create("old"), String::create("new"));
            MockConverter::View vKeyConverter = MockConverter::create();
            MockConverter::View vValConverter = MockConverter::create();

            MapEvent::Handle hConvEvent = hListenerSupport->convertEvent(
                    vMapEvent, hmap, vKeyConverter, vValConverter);

            TS_ASSERT(instanceof<ConverterCollections::ConverterMapEvent::Handle>(hConvEvent));

            ConverterCollections::ConverterMapEvent::Handle hMapConvEvent =
                    cast<ConverterCollections::ConverterMapEvent::Handle>(hConvEvent);

            TS_ASSERT(vMapEvent == hMapConvEvent->getMapEvent());
            TS_ASSERT(hmap == hMapConvEvent->getMap());
            TS_ASSERT(vKeyConverter == hMapConvEvent->getConverterKeyUp());
            TS_ASSERT(vValConverter == hMapConvEvent->getConverterValueUp());

            // pass in NULL converters
            hConvEvent = hListenerSupport->convertEvent(
                    vMapEvent, hmap, NULL, NULL);

            hMapConvEvent = cast<ConverterCollections::ConverterMapEvent::Handle>(hConvEvent);

            // ensure that NULL is converted to converter that simply returns
            // passed in value
            String::View vs = String::create("test");
            TS_ASSERT(Converter::convert(hMapConvEvent->getConverterKeyUp(), vs) == vs);
            TS_ASSERT(Converter::convert(hMapConvEvent->getConverterValueUp(), vs) == vs);
            }

        void testConvertEvent_CacheEvent()
            {
            MapListenerSupport::Handle hListenerSupport = MapListenerSupport::create();

            TestObservableMap::Handle hmap = TestObservableMap::create();
            MapEvent::View vMapEvent = CacheEvent::create(TestObservableMap::create(), 1,
                    String::create("key"), String::create("old"), String::create("new"), false);
            MockConverter::View vKeyConverter = MockConverter::create();
            MockConverter::View vValConverter = MockConverter::create();

            MapEvent::Handle hConvEvent = hListenerSupport->convertEvent(
                    vMapEvent, hmap, vKeyConverter, vValConverter);

            TS_ASSERT(instanceof<ConverterCollections::ConverterCacheEvent::Handle>(hConvEvent));

            ConverterCollections::ConverterCacheEvent::Handle hCacheConvEvent =
                    cast<ConverterCollections::ConverterCacheEvent::Handle>(hConvEvent);

            TS_ASSERT(vMapEvent == hCacheConvEvent->getCacheEvent());
            TS_ASSERT(hmap == hCacheConvEvent->getMap());
            TS_ASSERT(vKeyConverter == hCacheConvEvent->getConverterKeyUp());
            TS_ASSERT(vValConverter == hCacheConvEvent->getConverterValueUp());

            // pass in NULL converters
            hConvEvent = hListenerSupport->convertEvent(
                    vMapEvent, hmap, NULL, NULL);

            hCacheConvEvent = cast<ConverterCollections::ConverterCacheEvent::Handle>(hConvEvent);

            // ensure that NULL is converted to converter that simply returns
            // passed in value
            String::View vs = String::create("test");
            TS_ASSERT(Converter::convert(hCacheConvEvent->getConverterKeyUp(), vs) == vs);
            TS_ASSERT(Converter::convert(hCacheConvEvent->getConverterValueUp(), vs) == vs);
            }

        void testToStream()
            {
            // TODO
            }
    };
