/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Converter.hpp"


#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ListenerFilterRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ListenerKeyRequest.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace std;
using namespace mock;
using coherence::util::ArrayList;
using coherence::util::Converter;
using coherence::component::net::extend::protocol::cache::ListenerFilterRequest;
using coherence::component::net::extend::protocol::cache::ListenerKeyRequest;

using namespace coherence::component::net::extend;

COH_OPEN_NAMESPACE_ANON(RemoteNamedCacheTest)

COH_PRAGMA_PUSH  // eliminate warnings due to inheritence via dominance

// extends RemoteNamedCache::BinaryCache to facilitate easier unit testing
class TestRemoteNamedBinaryCache
    : public class_spec<TestRemoteNamedBinaryCache,
        extends<RemoteNamedCache::BinaryCache> >
    {
    friend class factory<TestRemoteNamedBinaryCache>;

    protected:
        TestRemoteNamedBinaryCache(RemoteNamedCache::Handle hCache, MapListenerSupport::Handle hSupport)
            : super(hCache, hSupport)
            {
            }
    };


class TestRemoteNamedCache
    : public class_spec<TestRemoteNamedCache,
          extends<RemoteNamedCache> >
    {
    friend class factory<TestRemoteNamedCache>;

    protected:
        virtual BinaryCache::Handle instantiateBinaryCache()
            {
            return TestRemoteNamedBinaryCache::create(this, MockMapListenerSupport::create());
            }
    };

bool matchListenerArgs(ArrayList::View vExpected, ArrayList::View vActual)
    {
    // first arg is converter listener
    // second arg is key as Binary or Filter
    // third arg is isLite flag(will be NULL for remove tests)

    RemoteNamedCache::ConverterListener::View vConvListener =
            cast<RemoteNamedCache::ConverterListener::View>(vActual->get(0));

    return vConvListener->getListener() == vExpected->get(0) &&
            vExpected->get(1)->equals(vActual->get(1)) &&
            vExpected->size() == 3 ? vExpected->get(2)->equals(vActual->get(2)) : true;
    }

// call various functions on the arg that is passed into the mock and
// ensure that that the return vals are correct.  The return vals are
// compared with values provided in the expected list.
bool matchCallbackArg(ArrayList::View vExpected, ArrayList::View vActual)
    {
    MapEvent::View vEvent = cast<MapEvent::View>(vActual->get(0));
    MapEvent::View vExpectedVals = cast<MapEvent::View>(vExpected->get(0));

    // get the key, oldValue and newValue from the event
    if (!vEvent->getKey()->equals(vExpectedVals->getKey()))
        {
        return false;
        }

    if (!vEvent->getOldValue()->equals(vExpectedVals->getOldValue()))
        {
        return false;
        }

    if (!vEvent->getNewValue()->equals(vExpectedVals->getNewValue()))
        {
        return false;
        }

    return true;
    }

COH_CLOSE_NAMESPACE_ANON

class RemoteNamedCacheTest : public CxxTest::TestSuite
    {
    public:

    void testAddKeyListener()
        {
        RemoteNamedCache::Handle hCache = TestRemoteNamedCache::create();

        // create mocks and test classes
        TestRemoteNamedBinaryCache::Handle hBinaryCache         = cast<TestRemoteNamedBinaryCache::Handle>(hCache->getBinaryCache());
        MockMapListenerSupport::Handle     hMockListenerSupport = cast<MockMapListenerSupport::Handle>(hBinaryCache->getMapListenerSupport());
        MockMapListener::Handle            hMockListener        = MockMapListener::create();
        MockChannel::Handle                hMockChannel         = MockChannel::create();
        MockProtocolMessageFactory::Handle hMockFactory         = MockProtocolMessageFactory::create();
        ListenerKeyRequest::Handle         hListenerKeyRequest  = ListenerKeyRequest::create();
        TestStringSerializer::Handle       hStringSerializer    = TestStringSerializer::create();

        // need to set the serializer on the cache now so we can create binary key
        // even though when the channel is set, the serializer is set again
        RemoteNamedCache::Handle hCache2 = TestRemoteNamedCache::create();
        hCache2->getConverterKeyToBinary()->setSerializer(hStringSerializer);
        Object::View vKey = String::create("test");
        Object::View vBinaryKey = hCache2->getConverterBinaryToUndecoratedBinary()
                ->convert(hCache2->getConverterKeyToBinary()->convert(vKey));

        // mock listener support expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty(vBinaryKey);
        hMockListenerSupport->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, vBinaryKey, false);
        hMockListenerSupport->setMatcher(&matchListenerArgs);
        hMockListenerSupport->replay();

        // mock channel expectations
        hMockChannel->getSerializer();
        hMockChannel->getSerializer();
        hMockChannel->setObjectReturn(hStringSerializer);
        hMockChannel->isOpen();
        hMockChannel->setBoolReturn(true);
        hMockChannel->getMessageFactory();
        hMockChannel->setObjectReturn(hMockFactory);
        hMockChannel->request(hListenerKeyRequest);
        hMockChannel->replay();

        // mock MockProtocolMessageFactory expectations
        hMockFactory->createMessage(ListenerKeyRequest::type_id);
        hMockFactory->setObjectReturn(hListenerKeyRequest);
        hMockFactory->replay();

        // set state on cache
        hCache->registerChannel(hMockChannel);

        // invoke function being tested
        hCache->addKeyListener(hMockListener, vKey, false);

        // verify mock calls
        hMockListenerSupport->verify();
        hMockChannel->verify();
        hMockFactory->verify();

        // assert state of request
        TS_ASSERT(hListenerKeyRequest->isAdd());
        TS_ASSERT(vBinaryKey->equals(hCache->getConverterBinaryToUndecoratedBinary()
                ->convert(hListenerKeyRequest->getKey())));
        TS_ASSERT(!hListenerKeyRequest->isLite());
        TS_ASSERT(NULL == hListenerKeyRequest->getTrigger());
        }

    void testAddFilterListener()
        {
        RemoteNamedCache::Handle hCache = TestRemoteNamedCache::create();

        // create mocks and test classes
        TestRemoteNamedBinaryCache::Handle hBinaryCache         = cast<TestRemoteNamedBinaryCache::Handle>(hCache->getBinaryCache());
        MockMapListenerSupport::Handle     hMockListenerSupport = cast<MockMapListenerSupport::Handle>(hBinaryCache->getMapListenerSupport());
        MockMapListener::Handle            hMockListener          = MockMapListener::create();
        MockChannel::Handle                hMockChannel           = MockChannel::create();
        MockProtocolMessageFactory::Handle hMockFactory           = MockProtocolMessageFactory::create();
        ListenerFilterRequest::Handle      hListenerFilterRequest = ListenerFilterRequest::create();
        MockFilter::Handle                 hMockFilter            = MockFilter::create();

        // mock listener support expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty((Filter::View) hMockFilter);
        hMockListenerSupport->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, (Filter::View) hMockFilter, false);
        hMockListenerSupport->setMatcher(&matchListenerArgs);
        hMockListenerSupport->replay();

        // mock channel expectations
        hMockChannel->isOpen();
        hMockChannel->setBoolReturn(true);
        hMockChannel->getMessageFactory();
        hMockChannel->setObjectReturn(hMockFactory);
        hMockChannel->request(hListenerFilterRequest);
        hMockChannel->replay();

        // mock MockProtocolMessageFactory expectations
        hMockFactory->createMessage(ListenerFilterRequest::type_id);
        hMockFactory->setObjectReturn(hListenerFilterRequest);
        hMockFactory->replay();

        // ensure that calling equals on itself returns true
        hMockFilter->equals(hMockFilter);
        hMockFilter->setBoolReturn(true);
        hMockFilter->replay();

        // set state on cache
        hCache->registerChannel(hMockChannel);

        // invoke function being tested
        hCache->addFilterListener(hMockListener, hMockFilter, false);

        // verify mock calls
        hMockListenerSupport->verify();
        hMockChannel->verify();
        hMockFactory->verify();

        // assert state of request
        TS_ASSERT(hListenerFilterRequest->isAdd());
        TS_ASSERT(hListenerFilterRequest->getFilter() == hMockFilter);
        // first filter added is at position 1
        TS_ASSERT(hListenerFilterRequest->getFilterId() == 1);
        TS_ASSERT(!hListenerFilterRequest->isLite());
        TS_ASSERT(NULL == hListenerFilterRequest->getTrigger());

        // assert filter was registered
        TS_ASSERT(hBinaryCache->getFilterArray()->get(1) == hMockFilter);
        }

    void testRemoveKeyListener()
        {
        RemoteNamedCache::Handle hCache = TestRemoteNamedCache::create();

        // create mocks and test classes
        TestRemoteNamedBinaryCache::Handle hBinaryCache         = cast<TestRemoteNamedBinaryCache::Handle>(hCache->getBinaryCache());
        MockMapListenerSupport::Handle     hMockListenerSupport = cast<MockMapListenerSupport::Handle>(hBinaryCache->getMapListenerSupport());
        MockMapListener::Handle            hMockListener        = MockMapListener::create();
        MockChannel::Handle                hMockChannel         = MockChannel::create();
        MockProtocolMessageFactory::Handle hMockFactory         = MockProtocolMessageFactory::create();
        ListenerKeyRequest::Handle         hListenerKeyRequest  = ListenerKeyRequest::create();
        TestStringSerializer::Handle       hStringSerializer    = TestStringSerializer::create();

        // need to set the serializer on the cache now so we can create binary key
        // even though when the channel is set, the serializer is set again
        RemoteNamedCache::Handle hCache2 = TestRemoteNamedCache::create();
        hCache2->getConverterKeyToBinary()->setSerializer(hStringSerializer);
        Object::View vKey = String::create("test");
        Object::View vBinaryKey = hCache2->getConverterBinaryToUndecoratedBinary()
                ->convert(hCache2->getConverterKeyToBinary()->convert(vKey));

        // mock listener support expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->removeListener(hMockListener, vBinaryKey);
        hMockListenerSupport->setMatcher(&matchListenerArgs);
        hMockListenerSupport->isEmpty(vBinaryKey);
        hMockListenerSupport->setBoolReturn(true);
        hMockListenerSupport->replay();

        // mock channel expectations
        hMockChannel->getSerializer();
        hMockChannel->getSerializer();
        hMockChannel->setObjectReturn(hStringSerializer);
        hMockChannel->isOpen();
        hMockChannel->setBoolReturn(true);
        hMockChannel->getMessageFactory();
        hMockChannel->setObjectReturn(hMockFactory);
        hMockChannel->request(hListenerKeyRequest);
        hMockChannel->replay();

        // mock MockProtocolMessageFactory expectations
        hMockFactory->createMessage(ListenerKeyRequest::type_id);
        hMockFactory->setObjectReturn(hListenerKeyRequest);
        hMockFactory->replay();

        // set state on cache
        hCache->registerChannel(hMockChannel);

        // invoke function being tested
        hCache->removeKeyListener(hMockListener, vKey);

        // verify mock calls
        hMockListenerSupport->verify();
        hMockChannel->verify();
        hMockFactory->verify();

        // assert state of request
        TS_ASSERT(!hListenerKeyRequest->isAdd());
        TS_ASSERT(vBinaryKey->equals(hCache->getConverterBinaryToUndecoratedBinary()
                ->convert(hListenerKeyRequest->getKey())));
        TS_ASSERT(!hListenerKeyRequest->isLite());
        TS_ASSERT(NULL == hListenerKeyRequest->getTrigger());
        }

    void testRemoveFilterListener()
        {
        RemoteNamedCache::Handle hCache = TestRemoteNamedCache::create();

        // create mocks and test classes
        TestRemoteNamedBinaryCache::Handle hBinaryCache         = cast<TestRemoteNamedBinaryCache::Handle>(hCache->getBinaryCache());
        MockMapListenerSupport::Handle     hMockListenerSupport = cast<MockMapListenerSupport::Handle>(hBinaryCache->getMapListenerSupport());
        MockMapListener::Handle            hMockListener          = MockMapListener::create();
        MockChannel::Handle                hMockChannel           = MockChannel::create();
        MockProtocolMessageFactory::Handle hMockFactory           = MockProtocolMessageFactory::create();
        ListenerFilterRequest::Handle      hListenerFilterRequest = ListenerFilterRequest::create();
        MockFilter::Handle                 hMockFilter            = MockFilter::create();

        // mock listener support expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->removeListener(hMockListener, (Filter::View) hMockFilter);
        hMockListenerSupport->setMatcher(&matchListenerArgs);
        hMockListenerSupport->isEmpty((Filter::View) hMockFilter);
        hMockListenerSupport->setBoolReturn(true);
        hMockListenerSupport->replay();

        // mock channel expectations
        hMockChannel->isOpen();
        hMockChannel->setBoolReturn(true);
        hMockChannel->getMessageFactory();
        hMockChannel->setObjectReturn(hMockFactory);
        hMockChannel->request(hListenerFilterRequest);
        hMockChannel->replay();

        // mock MockProtocolMessageFactory expectations
        hMockFactory->createMessage(ListenerFilterRequest::type_id);
        hMockFactory->setObjectReturn(hListenerFilterRequest);
        hMockFactory->replay();

        // ensure that calling equals on itself returns true
        hMockFilter->equals(hMockFilter);
        hMockFilter->setBoolReturn(true);
        hMockFilter->replay();

        // set state on cache
        hCache->registerChannel(hMockChannel);
        // add the filter to simulate an earlier addFilterRequest
        hBinaryCache->getFilterArray()->set(1, hMockFilter);

        // invoke function being tested
        hCache->removeFilterListener(hMockListener, hMockFilter);

        // verify mock calls
        hMockListenerSupport->verify();
        hMockChannel->verify();
        hMockFactory->verify();

        // assert state of request
        TS_ASSERT(!hListenerFilterRequest->isAdd());
        TS_ASSERT(hListenerFilterRequest->getFilter() == hMockFilter);
        // filter was added at position 1
        TS_ASSERT(hListenerFilterRequest->getFilterId() == 1);
        TS_ASSERT(!hListenerFilterRequest->isLite());
        TS_ASSERT(NULL == hListenerFilterRequest->getTrigger());

        // assert filter was removed
        TS_ASSERT(hBinaryCache->getFilterArray()->isEmpty());
        }

    // ConverterListener tests

    void testConverterListener_Accessors()
        {
        RemoteNamedCache::ConverterListener::Handle hListener = RemoteNamedCache::ConverterListener::create();

        Converter::View vConverter = MockConverter::create();;
        hListener->setConverter(vConverter);
        TS_ASSERT(hListener->getConverter() == vConverter);

        MapListener::Handle hMapListener = MockMapListener::create();
        hListener->setListener(hMapListener);
        TS_ASSERT(hListener->getListener() == hMapListener);

        NamedCache::Handle hNamedCache = MockNamedCache::create();
        hListener->setNamedCache(hNamedCache);
        TS_ASSERT(hListener->getNamedCache() == hNamedCache);
        }


    void testConverterListener_entryInserted()
        {
        RemoteNamedCache::ConverterListener::Handle hListener = RemoteNamedCache::ConverterListener::create();
        MockConverter::Handle hMockConverter   = MockConverter::create();
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        MockMapListener::Handle hMockListener  = MockMapListener::create();
        MockMapEvent::Handle hMockEvent        = MockMapEvent::create();
        MockMapEvent::Handle hMockEventArg     = MockMapEvent::create();

        Object::View vKey         = String::create("key");
        Object::View vConvKey     = String::create("converted-key");
        Object::View vOldVal      = String::create("old");
        Object::View vConvOldVal  = String::create("converted-old");
        Object::View vNewVal      = String::create("new");
        Object::View vConvNewVal  = String::create("converted-new");
        hListener->setConverter(hMockConverter);
        hListener->setNamedCache(hMockNamedCache);
        hListener->setListener(hMockListener);

        // mock expectations for listener
        hMockListener->setStrict(true);
        hMockListener->entryInserted(hMockEventArg);
        hMockListener->setMatcher(&matchCallbackArg);
        hMockListener->replay();

        // mock expectations on Converter.  These are a result of calling
        // getKey(), getOldValue() and getNewValue() in arg matcher used on
        // hMockListener->entryInserted
        hMockConverter->setStrict(true);
        hMockConverter->convert(vKey);
        hMockConverter->setObjectReturn(vConvKey);
        hMockConverter->convert(vOldVal);
        hMockConverter->setObjectReturn(vConvOldVal);
        hMockConverter->convert(vNewVal);
        hMockConverter->setObjectReturn(vConvNewVal);
        hMockConverter->replay();

        // mock expectations on underlying event
        hMockEvent->getId();
        hMockEvent->setInt32Return(MapEvent::entry_inserted);
        hMockEvent->getKey();
        hMockEvent->setObjectReturn(vKey);
        hMockEvent->getOldValue();
        hMockEvent->setObjectReturn(vOldVal);
        hMockEvent->getNewValue();
        hMockEvent->setObjectReturn(vNewVal);
        hMockEvent->replay();

        hMockEventArg->getKey();
        hMockEventArg->setObjectReturn(vConvKey);
        hMockEventArg->getOldValue();
        hMockEventArg->setObjectReturn(vConvOldVal);
        hMockEventArg->getNewValue();
        hMockEventArg->setObjectReturn(vConvNewVal);
        hMockEventArg->replay();

        // invoke function being tested
        hListener->entryInserted(hMockEvent);

        hMockListener->verify();
        hMockConverter->verify();
        hMockEvent->verify();
        }

    void testConverterListener_entryUpdated()
        {
        RemoteNamedCache::ConverterListener::Handle hListener = RemoteNamedCache::ConverterListener::create();
        MockConverter::Handle hMockConverter   = MockConverter::create();
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        MockMapListener::Handle hMockListener  = MockMapListener::create();
        MockMapEvent::Handle hMockEvent        = MockMapEvent::create();
        MockMapEvent::Handle hMockEventArg     = MockMapEvent::create();

        Object::View vKey         = String::create("key");
        Object::View vConvKey     = String::create("converted-key");
        Object::View vOldVal      = String::create("old");
        Object::View vConvOldVal  = String::create("converted-old");
        Object::View vNewVal      = String::create("new");
        Object::View vConvNewVal  = String::create("converted-new");
        hListener->setConverter(hMockConverter);
        hListener->setNamedCache(hMockNamedCache);
        hListener->setListener(hMockListener);

        // mock expectations for listener
        hMockListener->setStrict(true);
        hMockListener->entryUpdated(hMockEventArg);
        hMockListener->setMatcher(&matchCallbackArg);
        hMockListener->replay();

        // mock expectations on Converter.  These are a result of calling
        // getKey(), getOldValue() and getNewValue() in arg matcher used on
        // hMockListener->entryInserted
        hMockConverter->setStrict(true);
        hMockConverter->convert(vKey);
        hMockConverter->setObjectReturn(vConvKey);
        hMockConverter->convert(vOldVal);
        hMockConverter->setObjectReturn(vConvOldVal);
        hMockConverter->convert(vNewVal);
        hMockConverter->setObjectReturn(vConvNewVal);
        hMockConverter->replay();

        // mock expectations on underlying event
        hMockEvent->getId();
        hMockEvent->setInt32Return(MapEvent::entry_updated);
        hMockEvent->getKey();
        hMockEvent->setObjectReturn(vKey);
        hMockEvent->getOldValue();
        hMockEvent->setObjectReturn(vOldVal);
        hMockEvent->getNewValue();
        hMockEvent->setObjectReturn(vNewVal);
        hMockEvent->replay();

        hMockEventArg->getKey();
        hMockEventArg->setObjectReturn(vConvKey);
        hMockEventArg->getOldValue();
        hMockEventArg->setObjectReturn(vConvOldVal);
        hMockEventArg->getNewValue();
        hMockEventArg->setObjectReturn(vConvNewVal);
        hMockEventArg->replay();

        // invoke function being tested
        hListener->entryUpdated(hMockEvent);

        hMockListener->verify();
        hMockConverter->verify();
        hMockEvent->verify();
        }

    void testConverterListener_entryDeleted()
        {
        RemoteNamedCache::ConverterListener::Handle hListener = RemoteNamedCache::ConverterListener::create();
        MockConverter::Handle hMockConverter   = MockConverter::create();
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        MockMapListener::Handle hMockListener  = MockMapListener::create();
        MockMapEvent::Handle hMockEvent        = MockMapEvent::create();
        MockMapEvent::Handle hMockEventArg     = MockMapEvent::create();

        Object::View vKey         = String::create("key");
        Object::View vConvKey     = String::create("converted-key");
        Object::View vOldVal      = String::create("old");
        Object::View vConvOldVal  = String::create("converted-old");
        Object::View vNewVal      = String::create("new");
        Object::View vConvNewVal  = String::create("converted-new");
        hListener->setConverter(hMockConverter);
        hListener->setNamedCache(hMockNamedCache);
        hListener->setListener(hMockListener);

        // mock expectations for listener
        hMockListener->setStrict(true);
        hMockListener->entryDeleted(hMockEventArg);
        hMockListener->setMatcher(&matchCallbackArg);
        hMockListener->replay();

        // mock expectations on Converter.  These are a result of calling
        // getKey(), getOldValue() and getNewValue() in arg matcher used on
        // hMockListener->entryInserted
        hMockConverter->setStrict(true);
        hMockConverter->convert(vKey);
        hMockConverter->setObjectReturn(vConvKey);
        hMockConverter->convert(vOldVal);
        hMockConverter->setObjectReturn(vConvOldVal);
        hMockConverter->convert(vNewVal);
        hMockConverter->setObjectReturn(vConvNewVal);
        hMockConverter->replay();

        // mock expectations on underlying event
        hMockEvent->getId();
        hMockEvent->setInt32Return(MapEvent::entry_deleted);
        hMockEvent->getKey();
        hMockEvent->setObjectReturn(vKey);
        hMockEvent->getOldValue();
        hMockEvent->setObjectReturn(vOldVal);
        hMockEvent->getNewValue();
        hMockEvent->setObjectReturn(vNewVal);
        hMockEvent->replay();

        hMockEventArg->getKey();
        hMockEventArg->setObjectReturn(vConvKey);
        hMockEventArg->getOldValue();
        hMockEventArg->setObjectReturn(vConvOldVal);
        hMockEventArg->getNewValue();
        hMockEventArg->setObjectReturn(vConvNewVal);
        hMockEventArg->replay();

        // invoke function being tested
        hListener->entryDeleted(hMockEvent);

        hMockListener->verify();
        hMockConverter->verify();
        hMockEvent->verify();
        }

    };



