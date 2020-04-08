/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMMONMOCKS_HPP
#define COH_COMMONMOCKS_HPP

#include "coherence/lang.ns"

#include "mock/BaseMock.hpp"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/net/Invocable.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/MemberEvent.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/net/ViewBuilder.hpp"
#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/ConcurrentMap.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapListenerSupport.hpp"
#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/MapTriggerListener.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/ServiceEvent.hpp"
#include "coherence/util/ServiceListener.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "coherence/util/Versionable.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"
#include "coherence/util/processor/PropertyManipulator.hpp"

#include "private/coherence/component/net/extend/RemoteInvocationService.hpp"
#include "private/coherence/component/net/extend/RemoteService.hpp"
#include "private/coherence/component/util/SafeCacheService.hpp"
#include "private/coherence/component/util/SafeInvocationService.hpp"
#include "private/coherence/component/util/SafeNamedCache.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/ConnectionEvent.hpp"
#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Request.hpp"

#include <ostream>

COH_OPEN_NAMESPACE(mock)

using namespace std;
using mock::BaseMock;
using mock::Expectation;
using coherence::component::net::extend::RemoteInvocationService;
using coherence::component::net::extend::RemoteService;
using coherence::component::util::SafeCacheService;
using coherence::component::util::SafeInvocationService;
using coherence::component::util::SafeNamedCache;
using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::CacheService;
using coherence::net::Invocable;
using coherence::net::InvocationService;
using coherence::net::MemberEvent;
using coherence::net::MemberListener;
using coherence::net::NamedCache;
using coherence::net::ServiceInfo;
using coherence::net::ViewBuilder;
using coherence::net::cache::CacheMap;
using coherence::net::messaging::Message;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Request;
using coherence::net::messaging::ConnectionEvent;
using coherence::net::messaging::Channel;
using coherence::run::xml::XmlElement;
using coherence::security::auth::Subject;
using coherence::util::Collection;
using coherence::util::Comparator;
using coherence::util::ConcurrentMap;
using coherence::util::Converter;
using coherence::util::Filter;
using coherence::util::InvocableMap;
using coherence::util::Iterator;
using coherence::util::Listeners;
using coherence::util::Map;
using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::MapListenerSupport;
using coherence::util::MapTrigger;
using coherence::util::MapTriggerListener;
using coherence::util::Muterator;
using coherence::util::ObservableMap;
using coherence::util::SafeHashMap;
using coherence::util::ServiceEvent;
using coherence::util::ServiceListener;
using coherence::util::Set;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using coherence::util::Versionable;
using coherence::util::processor::AbstractProcessor;
using coherence::util::processor::PropertyManipulator;

// TODO: look into writing scripts to auto generate the mock...

// Dummy test implementation of ObservableMap
class TestObservableMap
    : public class_spec<TestObservableMap,
        extends<SafeHashMap>,
        implements<ObservableMap> >
    {
    friend class factory<TestObservableMap>;

    // ----- constructors -----------------------------------------------

    protected:
        TestObservableMap()
            : super(17, 1.0F, 3.0F)
            {
            }

   // ----- ObservableMap interface -------------------------------------

    public:


        virtual void removeKeyListener(MapListener::Handle /*hListener*/,
            Object::View /*vKey*/)
            {
            }

        virtual void addKeyListener(MapListener::Handle /*hListener*/,
            Object::View /*vKey*/, bool /*fLite*/)
            {
            }

        virtual void addMapListener(MapListener::Handle /*hListener*/)
            {
            }

        virtual void removeMapListener(MapListener::Handle /*hListener*/)
            {
            }

        virtual void addFilterListener(MapListener::Handle /*hListener*/,
            Filter::View /*vFilter*/, bool /*fLite*/)
            {
            }

        virtual void removeFilterListener(MapListener::Handle /*hListener*/,
            Filter::View /*vFilter*/)
            {
            }
    };



/**
 * Dummy Set implementation used for parameter and return values.
 */
class DummySet
    : public class_spec<DummySet,
        extends<Object>,
        implements<Set> >
    {
    friend class factory<DummySet>;

    public:
        size32_t size() const
            {
            return 0;
            }

        bool isEmpty() const
            {
            return false;
            }

        bool contains(Object::View /*v*/) const
            {
            return false;
            }

        bool containsAll(Collection::View /*vCol*/) const
            {
            return false;
            }

        Iterator::Handle iterator() const
            {
            return NULL;
            }

        Muterator::Handle iterator()
            {
            return NULL;
            }

        ObjectArray::Handle toArray(ObjectArray::Handle /* hao */) const
            {
            return NULL;
            }

        bool add(Object::Holder /*oh*/)
            {
            return false;
            }

        bool addAll(Collection::View /*vCol*/)
            {
            return false;
            }

        bool remove(Object::View /*v*/)
            {
            return false;
            }

        bool removeAll(Collection::View /*vCol*/)
            {
            return false;
            }

        bool retainAll(Collection::View /*vCol*/)
            {
            return false;
            }

        void clear()
            {
            }

        bool equals(Object::View vThat) const
            {
            if (NULL == vThat)
                {
                return false;
                }
            else if (this == vThat)
                {
                return true;
                }
            return false;
            }

        size32_t hashCode() const
            {
            return 0;
            }
    };


class DummyEntryProcessor
    : public class_spec<DummyEntryProcessor,
        extends<Object>,
        implements<InvocableMap::EntryProcessor> >
    {
    friend class factory<DummyEntryProcessor>;

    public:
        Object::Holder process(InvocableMap::Entry::Handle /*hEntry*/) const
            {
                return NULL;
            }

        Map::View processAll(Set::View /*vSetEntries*/) const
            {
                return NULL;
            }
    };

class DummyEntryAggregator
    : public class_spec<DummyEntryAggregator,
        extends<Object>,
        implements<InvocableMap::EntryAggregator> >
    {
    friend class factory<DummyEntryAggregator>;

    public:
        Object::Holder aggregate(Set::View /*vSetEntries*/)
            {
                return NULL;
            }
    };

class DummyFilter
    : public class_spec<DummyFilter,
        extends<Object>,
        implements<Filter> >
    {
    friend class factory<DummyFilter>;

    public:
        bool evaluate(Object::View /*v*/) const
            {
            return false;
            }
    };

class DummyComparator
    : public class_spec<DummyComparator,
        extends<Object>,
        implements<Comparator> >
    {
    friend class factory<DummyComparator>;

    public:
        int32_t compare(Object::View /*v1*/, Object::View /*v2*/) const
        {
        return 0;
        }
    };

class DummyValueExtractor
    : public class_spec<DummyValueExtractor,
        extends<Object>,
        implements<ValueExtractor> >
    {
    friend class factory<DummyValueExtractor>;

    public:
        Object::Holder extract(Object::Holder /*vTarget*/) const
            {
            return NULL;
            }

        bool equals(Object::View vThat) const
            {
            if (NULL == vThat)
                {
                return false;
                }
            else if (this == vThat)
                {
                return true;
                }
            return false;
            }

        size32_t hashCode() const
            {
            return 0;
            }
    };

class DummyInvocable
    : public class_spec<DummyInvocable,
        extends<Object>,
        implements<Invocable> >
    {
    friend class factory<DummyInvocable>;

    public:
        void init(InvocationServiceHandle /*hService*/)
            {
            }

        Object::Holder getResult() const
            {
            return NULL;
            }

        void run()
            {
            }
    };

class DummyServiceInfo
    : public class_spec<DummyServiceInfo,
        extends<Object>,
        implements<ServiceInfo> >
    {
    friend class factory<DummyServiceInfo>;

    public:
        String::View getServiceName() const
            {
            return String::create("");
            }

        ServiceType getServiceType() const
            {
            return ServiceInfo::local_cache;
            }
    };

class DummyMemberListener
    : public class_spec<DummyMemberListener,
        extends<Object>,
        implements<MemberListener> >
    {
    friend class factory<DummyMemberListener>;

    public:
        void memberJoined(MemberEvent::View /*vEvt*/)
            {
            }

        void memberLeaving(MemberEvent::View /*vEvt*/)
            {
            }

        void memberLeft(MemberEvent::View /*vEvt*/)
            {
            }
    };

class DummyServiceListener
    : public class_spec<DummyServiceListener,
        extends<Object>,
        implements<ServiceListener> >
    {
    friend class factory<DummyServiceListener>;

    public:
        void serviceStarting(ServiceEvent::View /*vEvt*/)
            {
            }
        void serviceStarted(ServiceEvent::View /*vEvt*/)
            {
            }
        void serviceStopping(ServiceEvent::View /*vEvt*/)
            {
            }
        void serviceStopped(ServiceEvent::View /*vEvt*/)
            {
            }
    };

class TestStringSerializer
    : public class_spec<TestStringSerializer,
        extends<Object>,
        implements<Serializer> >
    {
    friend class factory<TestStringSerializer>;

    protected:
        TestStringSerializer()
            {
            }

    public:
        virtual void serialize(WriteBuffer::BufferOutput::Handle hOut,
                Object::View v) const
            {
            hOut->writeString(cast<String::View>(v));
            }

        virtual Object::Holder deserialize(
                ReadBuffer::BufferInput::Handle hIn) const
            {
            return hIn->readString();
            }
    };

class MockProtocol
    : public class_spec<MockProtocol,
        extends<Object>,
        implements<Protocol> >, public BaseMock
    {
    friend class factory<MockProtocol>;

    protected:
        MockProtocol()
            {
            }

    public:
        virtual String::View getName() const
            {
            return cast<String::View>(setExpectation(invocation(
                    "String::View getName() const"))->getObjectReturn());
            }

        virtual int32_t getCurrentVersion() const
            {
            return setExpectation(invocation(
                    "int32_t getCurrentVersion() const"))->getInt32Return();
            }

        virtual int32_t getSupportedVersion() const
            {
            return setExpectation(invocation(
                    "int32_t getSupportedVersion() const"))->getInt32Return();
            }

        virtual MessageFactory::View getMessageFactory(int32_t nVersion) const
            {
            return cast<MessageFactory::View>(setExpectation(invocation(
                    "MessageFactory::View getMessageFactory(int32_t nVersion) const")->
                    withInt32Argument(nVersion))->getObjectReturn());
            }
    };

class MockProtocolMessageFactory
    : public class_spec<MockProtocolMessageFactory,
        extends<Object>,
        implements<Protocol::MessageFactory> >, public BaseMock
    {
    friend class factory<MockProtocolMessageFactory>;

    protected:
        MockProtocolMessageFactory()
            {
            }

    public:
        virtual Protocol::View getProtocol() const
            {
            return cast<Protocol::View>(setExpectation(invocation(
                    "Protocol::View getProtocol() const"))->getObjectReturn());
            }

        virtual int32_t getVersion() const
            {
            return setExpectation(invocation(
                    "int32_t getVersion() const"))->getInt32Return();
            }

        virtual Message::Handle createMessage(int32_t nType) const
            {
            return cast<Message::Handle>(setExpectation(invocation(
                    "Message::Handle createMessage(int32_t nType) const")->
                    withInt32Argument(nType))->getObjectReturn());
            }
    };


class MockChannel
    : public class_spec<MockChannel,
        extends<Object>,
        implements<Channel> >, public BaseMock
    {
    friend class factory<MockChannel>;

    protected:
        MockChannel() : BaseMock()
            {
            }

    public:
        // Channel interface

        virtual void close()
            {
            setExpectation(invocation("void close()"));
            }

        virtual Object::Holder getAttribute(String::View vsName) const
            {
            return setExpectation(invocation(
                    "Object::Holder getAttribute(String::View vsName) const")->
                    withObjectArgument(vsName))->getObjectReturn();
            }

        virtual Map::View getAttributes() const
            {
            return cast<Map::View>(setExpectation(invocation(
                    "Map::View getAttributes() const"))->getObjectReturn());
            }

        virtual ConnectionHandle getConnection()
            {
            return cast<ConnectionHandle>(setExpectation(invocation(
                    "ConnectionHandle getConnection()"))->getObjectReturn());
            }

        virtual ConnectionView getConnection() const
            {
            return cast<ConnectionView>(setExpectation(invocation(
                    "ConnectionView getConnection() const"))->getObjectReturn());
            }

        virtual int32_t getId() const
            {
            return setExpectation(invocation(
                    "int32_t getId() const"))->getInt32Return();
            }

        virtual Protocol::MessageFactory::View getMessageFactory() const
            {
            return cast<Protocol::MessageFactory::View>(setExpectation(invocation(
                    "Protocol::MessageFactory::View getMessageFactory() const"))->
                    getObjectReturn());
            }

        virtual Receiver::Handle getReceiver()
            {
            return cast<Receiver::Handle>(setExpectation(invocation(
                    "Receiver::Handle getReceiver()"))->getObjectReturn());
            }

        virtual Receiver::View getReceiver() const
            {
            return cast<Receiver::View>(setExpectation(invocation(
                    "Receiver::Handle getReceiver() const"))->getObjectReturn());
            }

        virtual Request::Handle getRequest(int64_t lId)
            {
            return cast<Request::Handle>(setExpectation(invocation(
                    "Request::Handle getRequest(int64_t lId)")->
                    withInt64Argument(lId))->getObjectReturn());
            }

        virtual Serializer::View getSerializer() const
            {
            return cast<Serializer::View>(setExpectation(invocation(
                    "Serializer::View getSerializer() const"))->getObjectReturn());
            }

        virtual Subject::View getSubject() const
            {
            return cast<Subject::View>(setExpectation(invocation(
                    "Subject::View getSubject() const"))->getObjectReturn());
            }

        virtual bool isOpen() const
            {
            return setExpectation(invocation(
                    "bool isOpen() const"))->getBoolReturn();
            }

        virtual Object::Holder removeAttribute(String::View vsName)
            {
            return setExpectation(invocation(
                    "Object::Holder removeAttribute(String::View vsName)")->
                    withObjectArgument(vsName))->getObjectReturn();
            }

        virtual Object::Holder request(Request::Handle hRequest)
            {
            return setExpectation(invocation(
                    "Object::Holder request(Request::Handle hRequest)")->
                    withObjectArgument(hRequest))->getObjectReturn();
            }

        virtual Object::Holder request(Request::Handle hRequest, int64_t cMillis)
            {
            return setExpectation(invocation(
                    "Object::Holder request(Request::Handle hRequest, int64_t cMillis)")->
                    withObjectArgument(hRequest)->withInt64Argument(cMillis))->getObjectReturn();
            }

        virtual void send(Message::Handle hMessage)
            {
            setExpectation(invocation(
                    "void send(Message::Handle hMessage)")->withObjectArgument(hMessage));
            }

        virtual Request::Status::Handle send(Request::Handle hRequest)
            {
            return cast<Request::Status::Handle>(setExpectation(invocation(
                    "Request::Status::Handle send(Request::Handle hRequest)")->withObjectArgument(hRequest))->
                    getObjectReturn());
            }

        virtual Object::Holder setAttribute(String::View vsName, Object::Holder ohValue)
            {
            return setExpectation(invocation(
                    "Object::Holder setAttribute(String::View vsName, Object::Holder ohValue)")->
                    withObjectArgument(vsName)->withObjectArgument(ohValue))->getObjectReturn();
            }
    };

class MockSerializer
    : public class_spec<MockSerializer,
        extends<Object>,
        implements<Serializer> >, public BaseMock
    {
    friend class factory<MockSerializer>;

    protected:
        MockSerializer() : BaseMock()
            {
            }

    public:
        virtual void serialize(WriteBuffer::BufferOutput::Handle hOut, Object::View v) const
            {
            setExpectation(invocation(
                    "void serialize(WriteBuffer::BufferOutput::Handle hOut, Object::View v) const")->
                    withObjectArgument(hOut)->withObjectArgument(v));
            }

        virtual Object::Holder deserialize(ReadBuffer::BufferInput::Handle hIn) const
            {
            return setExpectation(invocation(
                    "Object::Holder deserialize(ReadBuffer::BufferInput::Handle hIn) const")->
                    withObjectArgument(hIn))->getObjectReturn();
            }
    };

class MockMapListenerSupport
    : public class_spec<MockMapListenerSupport,
        extends<MapListenerSupport> >, public BaseMock
    {
    friend class factory<MockMapListenerSupport>;

    protected:
        MockMapListenerSupport() : BaseMock()
            {
            }

    public:
        // ----- MapListenerSupport interface -------------------------------


        virtual void addListener(MapListener::Handle hListener, Filter::View vFilter, bool fLite)
            {
            setExpectation(invocation(
                    "void addListener(MapListener::Handle hListener, Filter::View vFilter, bool fLite)")->
                    withObjectArgument(hListener)->withObjectArgument(vFilter)->withBoolArgument(fLite));

            }

        virtual void addListener(MapListener::Handle hListener, Object::View vKey, bool fLite)
            {
            setExpectation(invocation(
                    "void addListener(MapListener::Handle hListener, Object::View vKey, bool fLite)")->
                    withObjectArgument(hListener)->withObjectArgument(vKey)->withBoolArgument(fLite));
            }

        virtual void removeListener(MapListener::View vListener, Filter::View vFilter)
            {
            setExpectation(invocation(
                    "void removeListener(MapListener::View vListener, Filter::View vFilter)")->
                    withObjectArgument(vListener)->withObjectArgument(vFilter));
            }

        virtual void removeListener(MapListener::View vListener, Object::View vKey)
            {
            setExpectation(invocation(
                    "void removeListener(MapListener::View vListener, Object::View vKey)")->
                    withObjectArgument(vListener)->withObjectArgument(vKey));
            }

        virtual void clear()
            {
            setExpectation(invocation("void clear()"));
            }

        virtual bool isEmpty() const
            {
            return setExpectation(invocation("bool isEmpty() const"))->getBoolReturn();
            }

        virtual bool isEmpty(Filter::View vFilter) const
            {
            return setExpectation(invocation(
                "bool isEmpty(Filter::View vFilter) const")->withObjectArgument(vFilter))->getBoolReturn();
            }

        virtual bool isEmpty(Object::View vKey) const
            {
            return setExpectation(invocation("bool isEmpty(Object::View vKey) const")->
                    withObjectArgument(vKey))->getBoolReturn();
            }

        virtual bool containsStandardListeners(Filter::View vFilter) const
            {
            return setExpectation(invocation(
                    "bool containsStandardListeners(Filter::View vFilter) const")->
                    withObjectArgument(vFilter))->getBoolReturn();
            }

        virtual bool containsStandardListeners(Object::View vKey) const
            {
            return setExpectation(invocation(
                    "bool containsStandardListeners(Object::View vKey) const")->
                    withObjectArgument(vKey))->getBoolReturn();
            }

        virtual Set::View getFilterSet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View getFilterSet() const"))->getObjectReturn());
            }

        virtual Set::View getKeySet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View getKeySet() const"))->getObjectReturn());
            }

        virtual Listeners::View getListeners(Filter::View vFilter) const
            {
            return cast<Listeners::View>(setExpectation(invocation(
                    "Listeners::View getListeners(Filter::View vFilter) const")->withObjectArgument(vFilter))->
                    getObjectReturn());
            }

        virtual Listeners::View getListeners(Object::View vKey) const
            {
            return cast<Listeners::View>(setExpectation(invocation(
                    "Listeners::View getListeners(Object::View vKey) const")->withObjectArgument(vKey))->
                    getObjectReturn());
            }

        virtual Listeners::View collectListeners(MapEvent::View vEvent)
            {
            return cast<Listeners::View>(setExpectation(invocation(
                    "Listeners::View collectListeners(MapEvent::View vEvent)")->withObjectArgument(vEvent))->
                    getObjectReturn());
            }

        virtual MapEvent::Handle enrichEvent(MapEvent::Handle vEvent, Listeners::View vListeners)
            {
            return cast<MapEvent::Handle>(setExpectation(invocation("MapEvent::Handle enrichEvent(MapEvent::Handle vEvent, Listeners::View vListeners)")->
                    withObjectArgument(vEvent)->withObjectArgument(vListeners))->
                    getObjectReturn());

            }
        virtual void fireEvent(MapEvent::Handle hEvent, bool fStrict)
            {
            setExpectation(invocation(
                    "void fireEvent(MapEvent::Handle hEvent, bool fStrict)")->withObjectArgument(hEvent)->withBoolArgument(fStrict));
            }
    };

class MockConcurrentMap
    : public class_spec<MockConcurrentMap,
        extends<Object>,
        implements<ConcurrentMap> >, public BaseMock
    {
    friend class factory<MockConcurrentMap>;

    // ----- constructors -------------------------------------------

    protected:
        MockConcurrentMap() : BaseMock()
            {
            }

    // ----- Converter interface ------------------------------------

    public:
        virtual bool lock(Object::View vKey, int64_t cWait) const
            {
            return setExpectation(invocation(
                "bool lock(Object::View vKey, int64_t cWait)")->withObjectArgument(vKey)->
                withLongIntegerArgument(cWait))->getBoolReturn();
            }

        virtual bool lock(Object::View vKey) const
            {
            return setExpectation(invocation(
                "bool lock(Object::View vKey)")->withObjectArgument(vKey))
                ->getBoolReturn();
            }

        virtual bool unlock(Object::View vKey) const
            {
            return setExpectation(invocation(
                "bool unlock(Object::View vKey)")->withObjectArgument(vKey))
                ->getBoolReturn();
            }

        virtual size32_t size() const
            {
            return setExpectation(invocation("size32_t size() const"))->getIntegerReturn();
            }

        virtual bool isEmpty() const
            {
            return setExpectation(invocation("bool isEmpty() const"))->getBoolReturn();
            }

        virtual bool containsKey(Object::View vKey) const
            {
            return setExpectation(invocation("bool containsKey(Object::View vKey) const")->withObjectArgument(vKey))->getBoolReturn();
            }

        virtual bool containsValue(Object::View vValue) const
            {
            return setExpectation(invocation("bool containsValue(Object::View vValue) const")->withObjectArgument(vValue))->getBoolReturn();
            }

        virtual Object::Holder get(Object::View vKey) const
            {
            return setExpectation(invocation("Object::Holder get(Object::View vKey) const")->withObjectArgument(vKey))->getObjectReturn();
            }

        virtual Object::Holder get(Object::View vKey)
            {
            return setExpectation(invocation("Object::Holder get(Object::View vKey)")->withObjectArgument(vKey))->getObjectReturn();
            }

        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue)
            {
            return setExpectation(invocation("Object::Holder put(Object::View vKey, Object::Holder ohValue)")->withObjectArgument(vKey)->withObjectArgument(ohValue))->getObjectReturn();
            }

        virtual Object::Holder remove(Object::View vKey)
            {
            return setExpectation(invocation("Object::Holder remove(Object::View vKey)")->withObjectArgument(vKey))->getObjectReturn();
            }
        using Map::remove;

        virtual void putAll(Map::View vMap)
            {
            setExpectation(invocation("void putAll(Map::View vMap)")->withObjectArgument(vMap));
            }

        virtual void clear()
            {
            setExpectation(invocation("void clear()"));
            }

        virtual Set::View keySet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View keySet() const"))->getObjectReturn());
            }

        virtual Set::Handle keySet()
            {

            return cast<Set::Handle>(setExpectation(invocation("Set::Handle keySet()"))->getObjectReturn());
            }

        virtual Collection::View values() const
            {
            return cast<Collection::View>(setExpectation(invocation("Collection::View values() const"))->getObjectReturn());
            }

        virtual Collection::Handle values()
            {
            return cast<Collection::Handle>(setExpectation(invocation("Collection::Handle values()"))->getObjectReturn());
            }

        virtual Set::View entrySet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View entrySet() const"))->getObjectReturn());
            }

        virtual Set::Handle entrySet()
            {
            return cast<Set::Handle>(setExpectation(invocation("Set::Handle entrySet()"))->getObjectReturn());
            }
    };


class MockFilter
    : public class_spec<MockFilter,
        extends<Object>,
        implements<Filter> >, public BaseMock
    {
    friend class factory<MockFilter>;

    // ----- constructors -----------------------------------------------

    protected:
        MockFilter()
            {
            }

    public:
        virtual bool evaluate(Object::View v) const
            {
            return setExpectation(invocation(
                "bool evaluate(Object::View v) const")->withObjectArgument(v))
                ->getBoolReturn();
            }

        virtual bool equals(Object::View v) const
            {
            return setExpectation(invocation("bool equals(Object::View v) const")->
                    withObjectArgument(v))->getBoolReturn();
            }

    };

class MockObservableMap
    : public class_spec<MockObservableMap,
        extends<Object>,
        implements<ObservableMap, CacheMap> >, public BaseMock
    {
    friend class factory<MockObservableMap>;

    // ----- constructors -------------------------------------------

    protected:
        MockObservableMap()
            {
            }

    public:
        virtual void removeKeyListener(MapListener::Handle hListener,
            Object::View voKey)
            {
            setExpectation(invocation("void removeKeyListener(MapListener::Handle hListener, Object::View voKey)")->
                    withObjectArgument(hListener)->withObjectArgument(voKey));
            }

        virtual void addKeyListener(MapListener::Handle hListener,
            Object::View voKey, bool fLite)
            {
            setExpectation(invocation(
                    "addKeyListener(MapListener::Handle hListener, Object::View voKey, bool fLite)")->
                    withObjectArgument(hListener)->withObjectArgument(voKey)->withBoolArgument(fLite));
            }

        virtual void addMapListener(MapListener::Handle hListener)
            {
            setExpectation(invocation(
                    "void addMapListener(MapListener::Handle hListener)")->
                    withObjectArgument(hListener));
            }

        virtual void removeMapListener(MapListener::Handle hListener)
            {
            setExpectation(invocation(
                    "void removeMapListener(MapListener::Handle hListener)")->
                    withObjectArgument(hListener));
            }

        virtual void addFilterListener(MapListener::Handle hListener,
            Filter::View vFilter, bool fLite)
            {
            setExpectation(invocation(
                    "void addFilterListener(MapListener::Handle hListener, Filter::View vFilter, bool fLite)")->
                    withObjectArgument(hListener)->withObjectArgument(vFilter)->withBoolArgument(fLite));
            }

        virtual void removeFilterListener(MapListener::Handle hListener,
            Filter::View vFilter)
            {
            setExpectation(invocation("void removeFilterListener(MapListener::Handle hListener, Filter::View vFilter)")->withObjectArgument(hListener)->withObjectArgument(vFilter));
            }

        virtual size32_t size() const
            {
            return setExpectation(invocation("size32_t size() const"))->getIntegerReturn();
            }

        virtual bool isEmpty() const
            {
            return setExpectation(invocation("bool isEmpty() const"))->getBoolReturn();
            }

        virtual bool containsKey(Object::View vKey) const
            {
            return setExpectation(invocation("bool containsKey(Object::View vKey) const")->
                    withObjectArgument(vKey))->getBoolReturn();
            }

        virtual bool containsValue(Object::View vValue) const
            {
            return setExpectation(invocation("bool containsValue(Object::View vValue) const")->
                    withObjectArgument(vValue))->getBoolReturn();
            }

        virtual Object::Holder get(Object::View vKey) const
            {
            return setExpectation(invocation("Object::Holder get(Object::View vKey) const")->
                    withObjectArgument(vKey))->getObjectReturn();
            }

        virtual Object::Holder get(Object::View vKey)
            {
            return setExpectation(invocation("Object::Holder get(Object::View vKey)")->
                    withObjectArgument(vKey))->getObjectReturn();
            }

        virtual Map::View getAll(Collection::View vCollKeys) const
            {
            return cast<Map::View>(setExpectation(invocation("Map::View getAll(Collection::View vCollKeys)")->
                    withObjectArgument(vCollKeys))->getObjectReturn());
            }

        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue)
            {
            return setExpectation(invocation("Object::Holder put(Object::View vKey, Object::Holder ohValue)")->
                    withObjectArgument(vKey)->withObjectArgument(ohValue))->getObjectReturn();
            }

        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue, int64_t cMillis)
            {
            return setExpectation(invocation("Object::Holder put(Object::View vKey, Object::Holder ohValue, int64_t cMillis)")->
                    withObjectArgument(vKey)->withObjectArgument(ohValue)->withInt64Argument(cMillis))->getObjectReturn();
            }

        virtual Object::Holder remove(Object::View vKey)
            {
            return setExpectation(invocation("Object::Holder remove(Object::View vKey)")->
                    withObjectArgument(vKey))->getObjectReturn();
            }
        using Map::remove;

        virtual void putAll(Map::View vMap)
            {
            setExpectation(invocation("void putAll(Map::View vMap)")->withObjectArgument(vMap));
            }

        virtual void clear()
            {
            setExpectation(invocation("void clear()"));
            }

        virtual Set::View keySet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View keySet() const"))->getObjectReturn());
            }

        virtual Set::Handle keySet()
            {

            return cast<Set::Handle>(setExpectation(invocation("Set::Handle keySet()"))->getObjectReturn());
            }

        virtual Collection::View values() const
            {
            return cast<Collection::View>(setExpectation(invocation("Collection::View values() const"))->
                    getObjectReturn());
            }

        virtual Collection::Handle values()
            {
            return cast<Collection::Handle>(setExpectation(invocation("Collection::Handle values()"))->
                    getObjectReturn());
            }

        virtual Set::View entrySet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View entrySet() const"))->getObjectReturn());
            }

        virtual Set::Handle entrySet()
            {
            return cast<Set::Handle>(setExpectation(invocation("Set::Handle entrySet()"))->getObjectReturn());
            }
    };

class MockConverter
    : public class_spec<MockConverter,
        extends<Object>,
        implements<Converter> >, public BaseMock
    {
    friend class factory<MockConverter>;

    // ----- constructors -----------------------------------------------

    protected:
        MockConverter()
            {
            }

    // ----- Converter interface ----------------------------------------

    public:
        virtual Object::Holder convert(Object::Holder oh) const
            {
            return setExpectation(invocation(
                "Object::Holder convert(Object::Holder oh) const")->withObjectArgument(oh))
                ->getObjectReturn();
            }
    };

class MockMapEvent
    : public class_spec<MockMapEvent,
        extends<MapEvent> >, public BaseMock
    {
    friend class factory<MockMapEvent>;

    // ----- constructors -----------------------------------------------

    protected:
        MockMapEvent()
            : super((ObservableMap::Handle) NULL, 1, (Object::View) NULL,
                    (Object::View) NULL, (Object::View) NULL)
            {
            }

    // ----- MapEvent interface ----------------------------------------

    public:
        virtual void dispatch(Listeners::View vListeners) const
            {
            setExpectation(invocation("void dispatch(Listeners::View vListeners) const")->withObjectArgument(vListeners));
            }

        virtual void dispatch(Listeners::View vListeners,
                bool fStrict) const
            {
            setExpectation(invocation("void dispatch(Listeners::View vListeners, bool fStrict) const")->
                    withObjectArgument(vListeners)->withBoolArgument(fStrict));
            }

        virtual void dispatch(MapListener::Handle hListener) const
            {
            setExpectation(invocation("void dispatch(MapListener::Handle hListener)")->withObjectArgument(hListener));
            }

        virtual void outputDescription(std::ostream& out) const
            {
            out << setExpectation(invocation(
                    "String::View getDescription() const"))->getObjectReturn();
            }

        virtual ObservableMap::Handle getMap() const
            {
            return cast<ObservableMap::Handle>(setExpectation(invocation(
                    "ObservableMap::Handle getMap() const"))->getObjectReturn());
            }

        virtual int32_t getId() const
            {
            return setExpectation(invocation("int32_t getId() const"))->getIntegerReturn();
            }

        virtual Object::View getKey() const
            {
            return setExpectation(invocation("Object::View getKey() const"))->getObjectReturn();
            }

        virtual Object::View getOldValue() const
            {
            return setExpectation(invocation("Object::View getOldValue() const"))->getObjectReturn();
            }

        virtual Object::View getNewValue() const
            {
            return setExpectation(invocation("Object::View getNewValue() const"))->getObjectReturn();
            }

        virtual Object::Handle getSource() const
            {
            return cast<Object::Handle>(setExpectation(invocation(
                    "Object::Handle getSource() const"))->getObjectReturn());
            }
    };


class MockMapListener
    : public class_spec<MockMapListener,
        extends<Object>,
        implements<MapListener> >, public BaseMock
    {
    friend class factory<MockMapListener>;

    // ----- TestMapListener interface --------------------------------------

    public:

        virtual void entryInserted(MapEvent::View vEvent)
            {
            setExpectation(invocation("void entryInserted(MapEvent::View vEvent)")->withObjectArgument(vEvent));
            }

        virtual void entryUpdated(MapEvent::View vEvent)
            {
            setExpectation(invocation("void entryUpdated(MapEvent::View vEvent)")->withObjectArgument(vEvent));
            }

        virtual void entryDeleted(MapEvent::View vEvent)
            {
            setExpectation(invocation("void entryDeleted(MapEvent::View vEvent)")->withObjectArgument(vEvent));
            }
    };

/**
* Mock NamedCache implementation.
*/
class MockNamedCache
    : public class_spec<MockNamedCache,
        extends<Object>,
        implements<NamedCache> >, public BaseMock
    {
    friend class factory<MockNamedCache>;

    // ----- NamedCache interface -------------------------------------------
    public:
        virtual size32_t size() const
            {
            return setExpectation(invocation("size32_t size() const"))->getIntegerReturn();
            }

        virtual bool isEmpty() const
            {
            return setExpectation(invocation("bool isEmpty() const"))->getBoolReturn();
            }

        virtual bool containsKey(Object::View /*vKey*/) const
            {
            return setExpectation(invocation("bool containsKey(Object::View vKey) const"))->getBoolReturn();
            }

        virtual bool containsValue(Object::View /*vValue*/) const
            {
            return setExpectation(invocation("bool containsValue(Object::View vValue) const"))->getBoolReturn();
            }

        virtual Object::Holder get(Object::View vKey) const
            {
            Expectation::Handle ex = setExpectation(invocation("Object::Holder get(Object::View vKey) const")->
                    withObjectArgument(vKey));
            return ex->getObjectReturn();
            }

        virtual Object::Holder get(Object::View vKey)
            {
            Expectation::Handle ex = setExpectation(invocation("Object::Holder get(Object::View vKey)")->
                    withObjectArgument(vKey));
            return ex->getObjectReturn();
            }

        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue)
            {
            Expectation::Handle ex = setExpectation(invocation("Object::Holder put(Object::View vKey, Object::Holder ohValue)")->
                    withObjectArgument(vKey)->withObjectArgument(ohValue));
            return ex->getObjectReturn();
            }

        virtual Object::Holder remove(Object::View vKey)
            {
            Expectation::Handle ex = setExpectation(invocation("Object::Holder remove(Object::View vKey)")->
                    withObjectArgument(vKey));
            return ex->getObjectReturn();
            }
        using Map::remove;

        virtual void putAll(Map::View /*vMap*/)
            {
            setExpectation(invocation("void putAll(Map::View vMap)"));
            }

        virtual void clear()
            {
            setExpectation(invocation("void clear()"));
            }

        virtual Set::View keySet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View keySet() const"))->
                    getObjectReturn());
            }

        virtual Set::Handle keySet()
            {
            return cast<Set::Handle>(setExpectation(invocation("Set::Handle keySet()"))->
                    getObjectReturn());
            }

        virtual Collection::View values() const
            {
            return cast<Collection::View>(setExpectation(invocation("Collection::View values() const"))->
                    getObjectReturn());
            }

        virtual Collection::Handle values()
            {
            return cast<Collection::Handle>(setExpectation(invocation("Collection::Handle values()"))->
                    getObjectReturn());
            }

        virtual Set::View entrySet() const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View entrySet() const"))->
                    getObjectReturn());
            }

        virtual Set::Handle entrySet()
            {
            return cast<Set::Handle>(setExpectation(invocation("Set::Handle entrySet()"))->
                    getObjectReturn());
            }

        virtual Map::View getAll(Collection::View vKeys) const
            {
            Expectation::Handle ex = setExpectation(invocation("Map::Handle getAll(Collection::View vKeys) const")->
                    withObjectArgument(vKeys));
            return cast<Map::Handle>(ex->getObjectReturn());
            }

        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue,
                int64_t cMillis)
            {
            Expectation::Handle ex = setExpectation(invocation("Object::Holder put(Object::View vKey, Object::Holder ohValue, int64_t cMillis)")->
                    withObjectArgument(vKey)->
                    withObjectArgument(ohValue)->
                    withLongIntegerArgument(cMillis));
            return ex->getObjectReturn();
            }

        virtual void release()
            {
            setExpectation(invocation("void release()"));
            }

        virtual void destroy()
            {
            setExpectation(invocation("void destroy()"));
            }

        virtual String::View getCacheName() const
            {
            return cast<String::View>(setExpectation(invocation("String::View getCacheName() const"))->
                    getObjectReturn());
            }

        virtual bool isActive() const
            {
            return setExpectation(invocation("bool isActive() const"))->getBoolReturn();
            }

        virtual NamedCache::CacheServiceHandle getCacheService()
            {
            return cast<CacheService::Handle>(setExpectation(invocation("NamedCache::CacheServiceHandle getCacheService()"))->
                    getObjectReturn());
            }

        virtual NamedCache::CacheServiceView getCacheService() const
            {
            return cast<CacheService::View>(setExpectation(invocation("NamedCache::CacheServiceView getCacheService() const"))->
                    getObjectReturn());
            }

        virtual bool lock(Object::View vKey, int64_t cWait) const
            {
            return setExpectation(invocation("void setUserContext(Object::Holder ohCtx)")->withObjectArgument(vKey)->withLongIntegerArgument(cWait))->getBoolReturn();
            }

        virtual bool lock(Object::View vKey) const
            {
            return setExpectation(invocation("bool lock(Object::View vKey)")->withObjectArgument(vKey))->getBoolReturn();
            }

        virtual bool unlock(Object::View vKey) const
            {
            return setExpectation(invocation("bool unlock(Object::View vKey)")->withObjectArgument(vKey))->getBoolReturn();
            }

        virtual Object::Holder invoke(Object::View vKey, EntryProcessor::Handle hAgent)
            {
            return setExpectation(invocation("Object::View invoke(Object::View vKey, EntryProcessor::Handle hAgent)")->withObjectArgument(vKey)->withObjectArgument(hAgent))->getObjectReturn();
            }

        virtual Map::View invokeAll(Collection::View vCollKeys, EntryProcessor::Handle hAgent)
            {
            return cast<Map::View>(setExpectation(invocation("Map::View invokeAll(Collection::View vCollKeys, EntryProcessor::Handle hAgent)")->withObjectArgument(vCollKeys)->withObjectArgument(hAgent))->getObjectReturn());
            }

        virtual Map::View invokeAll(Filter::View vFilter, EntryProcessor::Handle hAgent)
            {
            return cast<Map::View>(setExpectation(invocation("Map::View invokeAll(Filter::View vFilter, EntryProcessor::Handle hAgent)")->withObjectArgument(vFilter)->withObjectArgument(hAgent))->getObjectReturn());
            }

        virtual Object::Holder aggregate(Collection::View vCollKeys, EntryAggregator::Handle hAgent) const
            {
            return setExpectation(invocation("Object::View aggregate(Collection::View vCollKeys, EntryAggregator::Handle hAgent)")->withObjectArgument(vCollKeys)->withObjectArgument(hAgent))->getObjectReturn();
            }

        virtual Object::Holder aggregate(Filter::View vFilter, EntryAggregator::Handle hAgent) const
            {
            return setExpectation(invocation("Object::View aggregate(Filter::View vFilter, EntryAggregator::Handle hAgent)")->withObjectArgument(vFilter)->withObjectArgument(hAgent))->getObjectReturn();
            }

        virtual Set::View keySet(Filter::View vFilter) const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View keySet(Filter::View vFilter) const")->withObjectArgument(vFilter))->getObjectReturn());
            }

        virtual Set::View entrySet(Filter::View vFilter) const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View entrySet(Filter::View vFilter) const")->withObjectArgument(vFilter))->getObjectReturn());
            }

        virtual Set::View entrySet(Filter::View vFilter, Comparator::View vComparator) const
            {
            return cast<Set::View>(setExpectation(invocation("Set::View entrySet(Filter::View vFilter, Comparator::View vComparator) const")->withObjectArgument(vFilter)->withObjectArgument(vComparator))->getObjectReturn());
            }

        virtual void addIndex(ValueExtractor::View vExtractor, bool fOrdered, Comparator::View vComparator)
            {
            setExpectation(invocation("void addIndex(ValueExtractor::View vExtractor, bool fOrdered, Comparator::View vComparator)")->withObjectArgument(vExtractor)->withBoolArgument(fOrdered)->withObjectArgument(vComparator));
            }

        virtual void removeIndex(ValueExtractor::View vExtractor)
            {
            setExpectation(invocation("void removeIndex(ValueExtractor::View vExtractor)")->withObjectArgument(vExtractor));
            }

        virtual ViewBuilder::Handle view()
            {
            return cast<ViewBuilder::Handle>(setExpectation(invocation("ViewBuilder::Handle view()"))->getObjectReturn());
            }

    // ----- ObservableMap interface ----------------------------------------

    public:
        virtual void removeKeyListener(MapListener::Handle hListener,
            Object::View voKey)
            {
            setExpectation(invocation("void removeKeyListener(MapListener::Handle hListener, Object::View voKey)")->
                    withObjectArgument(hListener)->withObjectArgument(voKey));
            }

        virtual void addKeyListener(MapListener::Handle hListener,
            Object::View voKey, bool fLite)
            {
            setExpectation(invocation(
                    "addKeyListener(MapListener::Handle hListener, Object::View voKey, bool fLite)")->
                    withObjectArgument(hListener)->withObjectArgument(voKey)->withBoolArgument(fLite));
            }

        virtual void addMapListener(MapListener::Handle hListener)
            {
            setExpectation(invocation(
                    "void addMapListener(MapListener::Handle hListener)")->
                    withObjectArgument(hListener));
            }

        virtual void removeMapListener(MapListener::Handle hListener)
            {
            setExpectation(invocation(
                    "void removeMapListener(MapListener::Handle hListener)")->
                    withObjectArgument(hListener));
            }

        virtual void addFilterListener(MapListener::Handle hListener,
            Filter::View vFilter = NULL, bool fLite = false)
            {
            setExpectation(invocation(
                    "void addFilterListener(MapListener::Handle hListener, Filter::View vFilter, bool fLite)")->
                    withObjectArgument(hListener)->withObjectArgument(vFilter)->withBoolArgument(fLite));
            }

        virtual void removeFilterListener(MapListener::Handle hListener,
            Filter::View vFilter = NULL)
            {
            setExpectation(invocation(
                    "void removeFilterListener(MapListener::Handle hListener, Filter::View vFilter)")->
                    withObjectArgument(hListener)->withObjectArgument(vFilter));
            }
    };

// MockNamedCache that is synchronous

class MockNamedCacheSynchronous
    : public class_spec<MockNamedCacheSynchronous,
        extends<MockNamedCache>,
        implements<MapListenerSupport::SynchronousListener> >
    {
    friend class factory<MockNamedCacheSynchronous>;
     // ----- TestMapListener interface --------------------------------------

     public:

         virtual void entryInserted(MapEvent::View vEvent)
             {
             setExpectation(invocation("void entryInserted(MapEvent::View vEvent)")->withObjectArgument(vEvent));
             }

         virtual void entryUpdated(MapEvent::View vEvent)
             {
             setExpectation(invocation("void entryUpdated(MapEvent::View vEvent)")->withObjectArgument(vEvent));
             }

         virtual void entryDeleted(MapEvent::View vEvent)
             {
             setExpectation(invocation("void entryDeleted(MapEvent::View vEvent)")->withObjectArgument(vEvent));
             }
    };


class MockMapTrigger
    : public class_spec<MockMapTrigger,
        extends<Object>,
        implements<MapTrigger> >
    {
    friend class factory<MockMapTrigger>;

    // ----- MapTrigger interface ---------------------------------------

    public:
        virtual void process(MapTrigger::Entry::Handle /*vEntry*/)
             {
             }
    };

// mock MapTriggerListener

class MockMapTriggerListener
    : public class_spec<MockMapTriggerListener,
        extends<MapTriggerListener> >, public BaseMock
    {
    friend class factory<MockMapTriggerListener>;

    protected:
         MockMapTriggerListener()
            : super(MockMapTrigger::create())
             {
             }

    public:
         virtual void onMapEvent(MapEvent::View vEvent)
             {
             setExpectation(invocation("void onMapEvent(MapEvent::View vEvent)")->withObjectArgument(vEvent));
             }

         virtual MapTrigger::View getTrigger() const
             {
             return cast<MapTrigger::View>(setExpectation(invocation(
                    "MapTrigger::View getTrigger() const"))->getObjectReturn());
             }

         virtual MapTrigger::Handle getTrigger()
            {
            return cast<MapTrigger::Handle>(setExpectation(invocation(
                    "MapTrigger::Handle getTrigger()"))->getObjectReturn());
            }

         virtual void entryInserted(MapEvent::View vEvent)
            {
            setExpectation(invocation("void entryInserted(MapEvent::View vEvent)")->withObjectArgument(vEvent));
            }

        virtual void entryUpdated(MapEvent::View vEvent)
            {
            setExpectation(invocation("void entryUpdated(MapEvent::View vEvent)")->withObjectArgument(vEvent));
            }

        virtual void entryDeleted(MapEvent::View vEvent)
            {
            setExpectation(invocation("void entryDeleted(MapEvent::View vEvent)")->withObjectArgument(vEvent));
            }
    };


class MockValueExtractor
    : public class_spec<MockValueExtractor,
        extends<Object>,
        implements<ValueExtractor> >, public BaseMock
    {
    friend class factory<MockValueExtractor>;

    public:
    virtual Object::Holder extract(Object::Holder ohTarget) const
        {
        return setExpectation(invocation("Object::Holder extract(Object::Holder ohTarget) const")->withObjectArgument(ohTarget))->getObjectReturn();
        }
    virtual bool equals(Object::View v) const
        {
        return setExpectation(invocation("bool equals(Object::View v) const")->withObjectArgument(v))->getBoolReturn();
        }

    virtual size32_t hashCode() const
        {
        return setExpectation(invocation("size32_t hashCode() const"))->getIntegerReturn();
        }
    };


class MockValueUpdater
    : public class_spec<MockValueUpdater,
        extends<Object>,
        implements<ValueUpdater> >, public BaseMock
    {
    friend class factory<MockValueUpdater>;

    public:
    virtual void update(Object::Handle hTarget,
            Object::Holder ohValue) const
        {
        setExpectation(invocation("void update(Object::Handle hTarget, Object::Holder ohValue) const")->withObjectArgument(hTarget)->withObjectArgument(ohValue));
        }
};

class MockPropertyManipulator
    : public class_spec<MockPropertyManipulator,
        extends<PropertyManipulator> >, public BaseMock
    {
    friend class factory<MockPropertyManipulator>;

    public:
    virtual ValueExtractor::View getExtractor() const
        {
        return cast<ValueExtractor::View>(setExpectation(invocation("ValueExtractor::View getExtractor() const"))->getObjectReturn());
        }

    virtual ValueUpdater::View getUpdater() const
        {
        return cast<ValueUpdater::View>(setExpectation(invocation("ValueUpdater::View getUpdater() const"))->getObjectReturn());
        }

    };

/*
class MockPofReader : public virtual PofReader, public BaseMock
    {
    // ----- handle definitions ---------------------------------------------
    public:
    typedef TypedHandle<MockPofReader> Handle;

    // ----- constructors ---------------------------------------------------
    public:
    MockPofReader()
            : BaseMock()
        {
        }

    virtual ~MockPofReader()
        {
        }

    //TODO : Finish the PofReader Mock - need to add new type support to BaseMock

    virtual bool readBoolean(int32_t iProp)
        {
        return setExpectation(invocation("bool readBoolean(int32_t iProp)")->withIntegerArgument(iProp))->getBoolReturn();
        }

    virtual octet_t readOctet(int32_t iProp)
        {
        return setExpectation(invocation("octet_t readOctet(int32_t iProp)")->withIntegerArgument(iProp))->getIntegerReturn();
        }

    virtual wchar16_t readChar16(int32_t iProp)
        {
        return setExpectation(invocation("wchar16_t readChar16(int32_t iProp)")->withIntegerArgument(iProp))->getIntegerReturn();
        }

    virtual int16_t readInt16(int32_t iProp)
        {
        return setExpectation(invocation("int16_t readInt16(int32_t iProp)")->withIntegerArgument(iProp))->getIntegerReturn();
        }

    virtual int32_t readInt32(int32_t iProp)
        {
        return setExpectation(invocation("int16_t readInt16(int32_t iProp)")->withIntegerArgument(iProp))->getIntegerReturn();
        }

    virtual int64_t readInt64(int32_t iProp)
        {
        return setExpectation(invocation("int16_t readInt16(int32_t iProp)")->withIntegerArgument(iProp))->getIntegerReturn();
        }

    virtual float32_t readFloat32(int32_t iProp)
        {
        }

    virtual float64_t readFloat64(int32_t iProp)
        {
        }


    virtual Array<bool>::Handle readBooleanArray(int32_t iProp)
        {
        }

    virtual Array<octet_t>::Handle readOctetArray(int32_t iProp)
        {
        }

    virtual Array<wchar16_t>::Handle readChar16Array(int32_t iProp)
        {
        }

    virtual Array<int16_t>::Handle readInt16Array(int32_t iProp)
        {
        }

    virtual Array<int32_t>::Handle readInt32Array(int32_t iProp)
        {
        }

    virtual Array<int64_t>::Handle readInt64Array(int32_t iProp)
        {
        }

    virtual Array<float32_t>::Handle readFloat32Array(int32_t iProp)
        {
        }

    virtual Array<float64_t>::Handle readFloat64Array(int32_t iProp)
        {
        }


    virtual String::Handle readString(int32_t iProp)
        {
        }

    virtual RawDate::Handle readRawDate(int32_t iProp)
        {
        }

    virtual RawTime::Handle readRawTime(int32_t iProp)
        {
        }

    virtual RawDateTime::Handle readRawDateTime(int32_t iProp)
        {
        }

    virtual RawYearMonthInterval::Handle readRawYearMonthInterval(
        int32_t iProp)
        {
        }

    virtual RawTimeInterval::Handle readRawTimeInterval(int32_t iProp)
        {
        }

    virtual RawDayTimeInterval::Handle readRawDayTimeInterval(int32_t iProp)
        {
        }

    virtual Object::Handle readObject(int32_t iProp)
        {
        }

    virtual ObjectArray::Handle readObjectArray(int32_t iProp,
        ObjectArray::Handle hao = NULL)
        {
        }

    virtual LongArray::Handle readLongArray(int32_t iProp,
        LongArray::Handle hla = NULL)
        {
        }

    virtual Collection::Handle readCollection(int32_t iProp,
        Collection::Handle hCol = NULL)
        {
        }

    virtual Map::Handle readMap(int32_t iProp, Map::Handle hMap = NULL)
        {
        }

    virtual PofContext::View getPofContext() const
        {
        }

    virtual void setPofContext(PofContext::View vCtx)
        {
        }

    virtual int32_t getUserTypeId() const
        {
        }

    virtual int32_t getVersionId() const
        {
        }

    virtual ReadBuffer::View readRemainder()
        {
        }

    };
*/

class MockVersionable
    : public class_spec<MockVersionable,
        extends<Object>,
        implements<Versionable> >, public BaseMock
{
friend class factory<MockVersionable>;

public:
virtual Comparable::View getVersionIndicator() const
    {
    return cast<Comparable::View>(setExpectation(invocation("Comparable::View getVersionIndicator() const"))->getObjectReturn());
    }

virtual void incrementVersion()
    {
    setExpectation(invocation("void incrementVersion()"));
    }
};


class MockComparable
    : public class_spec<MockComparable,
        extends<Object>,
        implements<Comparable> >, public BaseMock
{
friend class factory<MockComparable>;

public:

    virtual int32_t compareTo(Object::View v) const
    {
    return setExpectation(invocation("int32_t compareTo(Object::View v) const")->withObjectArgument(v))->getIntegerReturn();
    }

};


class MockProcessor
    : public class_spec<MockProcessor,
        extends<AbstractProcessor> >, public BaseMock
    {
    friend class factory<MockProcessor>;

public:
    virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const
        {
        return setExpectation(invocation("Object::View process(InvocableMap::Entry::Handle hEntry) const")->withObjectArgument(hEntry))
            ->getObjectReturn();
        }
};


class MockInvocableMapEntry
    : public class_spec<MockInvocableMapEntry,
        extends<Object>,
        implements<InvocableMap::Entry> >, public BaseMock
    {
    friend class factory<MockInvocableMapEntry>;

public:
    virtual Object::Holder setValue(Object::Holder ohValue, bool fSynthetic)
        {
        return setExpectation(invocation("Object::Holder setValue(Object::Holder ohValue, bool fSynthetic)")->
                withObjectArgument(ohValue)->withBoolArgument(fSynthetic))->getObjectReturn();
        }

    virtual void update(ValueUpdater::View vUpdater,
            Object::Holder ohValue)
        {
        setExpectation(invocation("void update(ValueUpdater::View vUpdater, Object::Holder ohValue)")->
                withObjectArgument(vUpdater)->withObjectArgument(ohValue));
        }

    virtual bool isPresent() const
        {
        return setExpectation(invocation("bool isPresent() const"))->
                getBoolReturn();
        }

    virtual void remove(bool fSynthetic)
        {
        setExpectation(invocation("void remove(bool fSynthetic)")->
                withBoolArgument(fSynthetic));
        }

    virtual Object::View getKey() const
        {
        return setExpectation(invocation("Object::View getKey() const"))->
                getObjectReturn();
        }

    virtual Object::Holder getValue() const
        {
        return setExpectation(invocation("Object::Holder getValue() const"))->
                getObjectReturn();
        }

    virtual Object::Holder getValue()
        {
        return setExpectation(invocation("Object::Holder getValue()"))->
                getObjectReturn();
        }

    virtual Object::Holder setValue(Object::Holder ohValue)
        {
        return setExpectation(invocation("Object::Holder setValue(Object::Holder ohValue)")->
                withObjectArgument(ohValue))->getObjectReturn();
        }

    virtual Object::Holder extract(
            ValueExtractor::View vExtractor) const
        {
        return setExpectation(invocation("Object::Holder extract(ValueExtractor::View vExtractor) const")->
                withObjectArgument(vExtractor))->getObjectReturn();
        }
    };

/**
* Mock CacheService implementation.
*/
class MockCacheService
    : public class_spec<MockCacheService,
        extends<Object>,
        implements<CacheService> >, public BaseMock
    {
    friend class factory<MockCacheService>;

    // ----- constructors ---------------------------------------------------
    protected:
        MockCacheService()
            : m_mockNamedCache(self(), MockNamedCache::create(), /*fMutable*/ true)
            {
            }

    // ----- CacheService interface -----------------------------------------
    public:
        /**
        * Return the MockNamedCache associated with this Mock
        */
        virtual NamedCache::Handle ensureCache(String::View /*vsName*/)
            {
            return getMockNamedCache();
            }

        virtual Collection::View getCacheNames() const
            {
            return cast<Collection::View>(setExpectation(invocation("Collection::View getCacheNames() const"))->
                    getObjectReturn());
            }

        virtual void releaseCache(NamedCache::Handle hMap)
            {
            setExpectation(invocation("void releaseCache(NamedCache::Handle hMap)")->withObjectArgument(hMap));
            }

        virtual void destroyCache(NamedCache::Handle hMap)
            {
            setExpectation(invocation("void destroyCache(NamedCache::Handle hMap)")->withObjectArgument(hMap));
            }

        virtual void configure(XmlElement::View vXml)
            {
            setExpectation(invocation("void configure(XmlElement::View vXml)")->withObjectArgument(vXml));
            }

        virtual bool isRunning() const
            {
            return setExpectation(invocation("bool isRunning() const"))->getBoolReturn();
            }

        virtual void start()
            {
            setExpectation(invocation("void start()"));
            }

        virtual void shutdown()
            {
            setExpectation(invocation("void shutdown()"));
            }

        virtual void stop()
            {
            setExpectation(invocation("void stop()"));
            }

        virtual void addServiceListener(ServiceListener::Handle hListener)
            {
            setExpectation(invocation("void addServiceListener(ServiceListener::Handle hListener)")->withObjectArgument(hListener));
            }

        virtual void removeServiceListener(ServiceListener::Handle hListener)
            {
            setExpectation(invocation("void removeServiceListener(ServiceListener::Handle hListener)")->withObjectArgument(hListener));
            }

        virtual ServiceInfo::View getInfo() const
            {
            return cast<ServiceInfo::View>(setExpectation(invocation("ServiceInfo::View getInfo() const"))->
                    getObjectReturn());
            }

        virtual void addMemberListener(MemberListener::Handle hListener)
            {
            setExpectation(invocation("void addMemberListener(MemberListener::Handle hListener)")->withObjectArgument(hListener));
            }

        virtual void removeMemberListener(MemberListener::Handle hListener)
            {
            setExpectation(invocation("void removeMemberListener(MemberListener::Handle hListener)")->withObjectArgument(hListener));
            }

        virtual Object::Holder getUserContext() const
            {
            return setExpectation(invocation("Object::Holder getUserContext() const"))->getObjectReturn();
            }

        virtual void setUserContext(Object::Holder ohCtx)
            {
            setExpectation(invocation("void setUserContext(Object::Holder ohCtx)")->withObjectArgument(ohCtx));
            }

        virtual Serializer::View getSerializer() const
            {
            return cast<Serializer::View>(setExpectation(invocation("void getSerializer()"))->getObjectReturn());
            }

        virtual void setThreadGroup(ThreadGroup::Handle hGroup)
            {
            setExpectation(invocation("void setThreadGroup(ThreadGroup::Handle hGroup)")->withObjectArgument(hGroup));
            }

        virtual ThreadGroup::Handle getThreadGroup()
            {
            return cast<ThreadGroup::Handle>(setExpectation(invocation("ThreadGroup::Handle getThreadGroup()"))->getObjectReturn());
            }

        virtual ThreadGroup::View getThreadGroup() const
            {
            return cast<ThreadGroup::View>(setExpectation(invocation("ThreadGroup::View getThreadGroup() const"))->getObjectReturn());
            }


    // ----- accessor -------------------------------------------------------
    public:
        MockNamedCache::Handle getMockNamedCache()
            {
            return m_mockNamedCache;
            }

    // ----- data members ---------------------------------------------------
    protected:

        /**
        * Keep a MockNamedCache instance and always return it from
        * ensureCache.
        */
        mutable MemberHandle<MockNamedCache> m_mockNamedCache;
    };

/**
* Mock SafeCacheService implementation.
*/
class MockSafeCacheService
    : public class_spec<MockSafeCacheService,
        extends<SafeCacheService> >, public BaseMock
    {
    friend class factory<MockSafeCacheService>;

    // ----- constructors ---------------------------------------------------
    protected:
        MockSafeCacheService()
            : BaseMock(),
              m_mockCacheService(self(), MockCacheService::create(), /*fMutable*/ true)
            {
            }

    // ----- SafeCacheService interface -------------------------------------
    public:
        /**
        * Return the MockCacheService associated with this Mock
        */
        virtual CacheService::Handle ensureRunningCacheService(bool /*fDrain*/)
            {
            return m_mockCacheService;
            }

        virtual CacheService::View ensureRunningCacheService(bool /*fDrain*/) const
            {
            return m_mockCacheService;
            }

        virtual CacheService::Handle ensureRunningCacheServiceInternal(bool /*fDrain*/) const
            {
            return m_mockCacheService;
            }

        virtual void releaseCache(NamedCache::Handle hNamedCache)
            {
            setExpectation(invocation("void releaseCache(NamedCache::Handle hNamedCache)")->
                    withObjectArgument(hNamedCache));
            }

        virtual void destroyCache(NamedCache::Handle hNamedCache)
            {
            setExpectation(invocation("void destroyCache(NamedCache::Handle hNamedCache)")->
                    withObjectArgument(hNamedCache));
            }

        virtual void drainEvents() const
            {
            setExpectation(invocation("void drainEvents() const"));
            }


    // ----- accessor -------------------------------------------------------
    public:
        MockCacheService::Handle getMockCacheService()
            {
            return m_mockCacheService;
            }


    // ----- data members ---------------------------------------------------
    protected:
        /**
        * Keep a MockCacheService instance and always return it from
        * ensureRunningCacheService.
        */
        mutable MemberHandle<MockCacheService> m_mockCacheService;
    };

    /**
    * Mock RemoteInvocationService implementation.
    */
    class MockRemoteInvocationService
        : public class_spec<MockRemoteInvocationService,
            extends<RemoteInvocationService> >, public BaseMock
        {
        friend class factory<MockRemoteInvocationService>;

        // ----- constructors ---------------------------------------------------
        protected:
            MockRemoteInvocationService()
                {
                }

        public:
        // ----- Service interface -------------------------------------

            virtual Map::View query(Invocable::Handle hTask, Set::View vSetMembers)
                {
                return cast<Map::View>(setExpectation(invocation("Map::View query(Invocable::Handle hTask, Set::View vSetMembers)")->
                        withObjectArgument(hTask)->
                        withObjectArgument(vSetMembers))->
                        getObjectReturn());
                }

            virtual void configure(XmlElement::View vXml)
                {
                setExpectation(invocation("void configure(XmlElement::View vXml)")->withObjectArgument(vXml));
                }

            virtual bool isRunning() const
                {
                return setExpectation(invocation("bool isRunning() const"))->getBoolReturn();
                }

            virtual void start()
                {
                setExpectation(invocation("void start()"));
                }

            virtual void shutdown()
                {
                setExpectation(invocation("void shutdown()"));
                }

            virtual void stop()
                {
                setExpectation(invocation("void stop()"));
                }

            virtual void addServiceListener(ServiceListener::Handle hListener)
                {
                setExpectation(invocation("void addServiceListener(ServiceListener::Handle hListener)")->withObjectArgument(hListener));
                }

            virtual void removeServiceListener(ServiceListener::Handle hListener)
                {
                setExpectation(invocation("void removeServiceListener(ServiceListener::Handle hListener)")->withObjectArgument(hListener));
                }

            virtual ServiceInfo::View getInfo() const
                {
                return cast<ServiceInfo::View>(setExpectation(invocation("ServiceInfo::View getInfo() const"))->
                        getObjectReturn());
                }

            virtual void addMemberListener(MemberListener::Handle hListener)
                {
                setExpectation(invocation("void addMemberListener(MemberListener::Handle hListener)")->withObjectArgument(hListener));
                }

            virtual void removeMemberListener(MemberListener::Handle hListener)
                {
                setExpectation(invocation("void removeMemberListener(MemberListener::Handle hListener)")->withObjectArgument(hListener));
                }

            virtual Object::Holder getUserContext() const
                {
                return setExpectation(invocation("Object::Holder getUserContext() const"))->getObjectReturn();
                }

            virtual void setUserContext(Object::Holder ohCtx)
                {
                setExpectation(invocation("void setUserContext(Object::Holder ohCtx)")->withObjectArgument(ohCtx));
                }

            virtual String::View getServiceName() const
                {
                return cast<String::View>(setExpectation(invocation("String::View getServiceName() const"))->
                        getObjectReturn());
                }

            virtual ServiceType getServiceType() const
                {
                return (ServiceType)setExpectation(invocation("ServiceType getServiceType() const"))->
                        getIntegerReturn();
                }

            virtual String::View getDescription() const
                {
                return cast<String::View>(setExpectation(invocation("String::View getDescription() const")));
                }

            virtual TypedHandle<const String> toString() const
                {
                return cast<TypedHandle<const String> >(setExpectation(invocation("TypedHandle<const String> toString() const")));
                }

            virtual void serviceStarting(ServiceEvent::View vEvt)
                {
                setExpectation(invocation("void serviceStarting(ServiceEvent::View vEvt)")->withObjectArgument(vEvt));
                }

            virtual void serviceStarted(ServiceEvent::View vEvt)
                {
                setExpectation(invocation("void serviceStarted(ServiceEvent::View vEvt)")->withObjectArgument(vEvt));
                }

            virtual void serviceStopping(ServiceEvent::View vEvt)
                {
                setExpectation(invocation("void serviceStopping(ServiceEvent::View vEvt)")->withObjectArgument(vEvt));
                }

            virtual void serviceStopped(ServiceEvent::View vEvt)
                {
                setExpectation(invocation("void serviceStopped(ServiceEvent::View vEvt)")->withObjectArgument(vEvt));
                }

            virtual void connectionOpened(ConnectionEvent::View vEvt)
                {
                setExpectation(invocation("void connectionOpened(ConnectionEvent::View vEvt)")->withObjectArgument(vEvt));
                }

            virtual void connectionClosed(ConnectionEvent::View vEvt)
                {
                setExpectation(invocation("void connectionClosed(ConnectionEvent::View vEvt)")->withObjectArgument(vEvt));
                }

            virtual void connectionError(ConnectionEvent::View vEvt)
                {
                setExpectation(invocation("void connectionError(ConnectionEvent::View vEvt)")->withObjectArgument(vEvt));
                }

            virtual Channel::Handle openChannel()
                {
                return cast<Channel::Handle>(setExpectation(invocation("Channel::Handle openChannel()"))->
                        getObjectReturn());
                }
        };

COH_CLOSE_NAMESPACE

#endif // COH_COMMONMOCKS_HPP
