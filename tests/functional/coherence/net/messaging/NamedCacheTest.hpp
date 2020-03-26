/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/DefaultOperationalContext.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/ArrayList.hpp"

#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ClearRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsKeyRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsValueRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/GetRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/GetAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/RemoveRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/RemoveAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/SizeRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/DestroyCacheRequest.hpp"
#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/component/util/TcpInitiator.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"
#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/util/logging/Logger.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::net::DefaultOperationalContext;
using coherence::net::OperationalContext;
using coherence::util::ArrayList;
using coherence::component::net::extend::RemoteNamedCache;
using coherence::component::net::extend::protocol::cache::ClearRequest;
using coherence::component::net::extend::protocol::cache::ContainsAllRequest;
using coherence::component::net::extend::protocol::cache::ContainsKeyRequest;
using coherence::component::net::extend::protocol::cache::ContainsValueRequest;
using coherence::component::net::extend::protocol::cache::GetRequest;
using coherence::component::net::extend::protocol::cache::GetAllRequest;
using coherence::component::net::extend::protocol::cache::NamedCacheResponse;
using coherence::component::net::extend::protocol::cache::PutRequest;
using coherence::component::net::extend::protocol::cache::PutAllRequest;
using coherence::component::net::extend::protocol::cache::RemoveRequest;
using coherence::component::net::extend::protocol::cache::RemoveAllRequest;
using coherence::component::net::extend::protocol::cache::SizeRequest;
using coherence::component::net::extend::protocol::cache::service::DestroyCacheRequest;
using coherence::component::util::Peer;
using coherence::component::util::TcpInitiator;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionInitiator;
using coherence::net::messaging::Message;
using coherence::run::xml::XmlElement;
using coherence::util::logging::Logger;


/**
* NamedCache Test Suite.
*/
class NamedCacheTest : public CxxTest::TestSuite
    {
    MemberHandle<Connection> m_hConnection;
    MemberHandle<ConnectionInitiator> m_hInitiator;
    MemberHandle<Channel> hNamedCacheChannel;
    MemberView<Protocol::MessageFactory> vNamedCacheFactory;
    MemberHandle<RemoteNamedCache::ConverterFromBinary> convFromBinary;
    MemberHandle<RemoteNamedCache::ConverterValueToBinary> convToBinary;
    MemberHandle<Channel> hCacheServiceChannel;

    public:
        NamedCacheTest()
            : m_hConnection(System::common()),
            m_hInitiator(System::common()),
            hNamedCacheChannel(System::common()),
            vNamedCacheFactory(System::common()),
            convFromBinary(System::common()),
            convToBinary(System::common()),
            hCacheServiceChannel(System::common())
            {
            }

    // ----- test fixtures --------------------------------------------------

    public:
        /**
         * Runs before each test
         */
        void setUp()
            {
            OperationalContext::View vContext = DefaultOperationalContext::create();

            // Need to manage logger manually as we are not using CacheFactory
            Logger::getLogger()->shutdown();
            Logger::Handle hLogger = Logger::getLogger();
            hLogger->configure(vContext);
            hLogger->start();

            stringstream ss;
                ss << "         <initiator-config>"
                   << "            <tcp-initiator>"
                   << "               <remote-addresses>"
                   << "                  <socket-address>"
                   << "                     <address system-property=\"tangosol.coherence.proxy.address\">127.0.0.1</address>"
                   << "                     <port system-property=\"tangosol.coherence.proxy.port\">32000</port>"
                   << "                  </socket-address>"
                   << "               </remote-addresses>"
                   << "            </tcp-initiator>"
                   << "         </initiator-config>";

            m_hInitiator = TcpInitiator::create();
            m_hInitiator->setOperationalContext(vContext);
            XmlElement::Handle hXml = SimpleParser::create()->parseXml(ss);
            XmlHelper::replaceSystemProperties(hXml, "system-property");
            m_hInitiator->configure(hXml);
            m_hInitiator->registerProtocol(CacheServiceProtocol::getInstance());
            m_hInitiator->registerProtocol(NamedCacheProtocol::getInstance());
            m_hInitiator->start();
            m_hConnection = m_hInitiator->ensureConnection();

            convFromBinary = RemoteNamedCache::ConverterFromBinary::create();
            convToBinary = RemoteNamedCache::ConverterValueToBinary::create();

            hCacheServiceChannel = m_hConnection->openChannel(CacheServiceProtocol::getInstance(), "CacheServiceProxy", NULL, NULL);
            vNamedCacheFactory = hCacheServiceChannel->getMessageFactory();
            EnsureCacheRequest::Handle hEnsureRequest = cast<EnsureCacheRequest::Handle>(vNamedCacheFactory->createMessage(EnsureCacheRequest::type_id));
            hEnsureRequest->setCacheName("dist-extend-direct");
            URI::View vUri = URI::create(cast<String::View>(hCacheServiceChannel->request(cast<Request::Handle>(hEnsureRequest))));
            hNamedCacheChannel = m_hConnection->acceptChannel(vUri, NULL, NULL);
            vNamedCacheFactory = hNamedCacheChannel->getMessageFactory();

            convToBinary->setSerializer(hNamedCacheChannel->getSerializer());
            convFromBinary->setSerializer(hNamedCacheChannel->getSerializer());
            }

        /**
         * Runs after each test
         */
        void tearDown()
            {
            ConnectionInitiator::Handle hInitiator = m_hInitiator;
            if (hInitiator != NULL)
                {
                hInitiator->stop();
                m_hInitiator = NULL;
                }
            Logger::getLogger()->shutdown();
            }


    public:
        /**
        * Test MessageFactory.
        */
        void testMessageFactory()
            {
            Message::Handle hGetRequest = vNamedCacheFactory->createMessage(GetRequest::type_id);
            TS_ASSERT(instanceof<GetRequest::View>(hGetRequest));
            TS_ASSERT(GetRequest::type_id == hGetRequest->getTypeId());

            Message::Handle hPutRequest = vNamedCacheFactory->createMessage(PutRequest::type_id);
            TS_ASSERT(instanceof<PutRequest::View>(hPutRequest));
            TS_ASSERT(PutRequest::type_id == hPutRequest->getTypeId());

            Message::Handle hSizeRequest = vNamedCacheFactory->createMessage(SizeRequest::type_id);
            TS_ASSERT(instanceof<SizeRequest::View>(hSizeRequest));
            TS_ASSERT(SizeRequest::type_id == hSizeRequest->getTypeId());

            Message::Handle hGetAllRequest = vNamedCacheFactory->createMessage(GetAllRequest::type_id);
            TS_ASSERT(instanceof<GetAllRequest::View>(hGetAllRequest));
            TS_ASSERT(GetAllRequest::type_id == hGetAllRequest->getTypeId());

            Message::Handle hPutAllRequest = vNamedCacheFactory->createMessage(PutAllRequest::type_id);
            TS_ASSERT(instanceof<PutAllRequest::View>(hPutAllRequest));
            TS_ASSERT(PutAllRequest::type_id == hPutAllRequest->getTypeId());

            Message::Handle hClearRequest = vNamedCacheFactory->createMessage(ClearRequest::type_id);
            TS_ASSERT(instanceof<ClearRequest::View>(hClearRequest));
            TS_ASSERT(ClearRequest::type_id == hClearRequest->getTypeId());

            Message::Handle hContainsKeyRequest = vNamedCacheFactory->createMessage(ContainsKeyRequest::type_id);
            TS_ASSERT(instanceof<ContainsKeyRequest::View>(hContainsKeyRequest));
            TS_ASSERT(ContainsKeyRequest::type_id == hContainsKeyRequest->getTypeId());

            Message::Handle hContainsValueRequest = vNamedCacheFactory->createMessage(ContainsValueRequest::type_id);
            TS_ASSERT(instanceof<ContainsValueRequest::View>(hContainsValueRequest));
            TS_ASSERT(ContainsValueRequest::type_id == hContainsValueRequest->getTypeId());

            Message::Handle hContainsAllRequest = vNamedCacheFactory->createMessage(ContainsAllRequest::type_id);
            TS_ASSERT(instanceof<ContainsAllRequest::View>(hContainsAllRequest));
            TS_ASSERT(ContainsAllRequest::type_id == hContainsAllRequest->getTypeId());

            Message::Handle hRemoveRequest = vNamedCacheFactory->createMessage(RemoveRequest::type_id);
            TS_ASSERT(instanceof<RemoveRequest::View>(hRemoveRequest));
            TS_ASSERT(RemoveRequest::type_id == hRemoveRequest->getTypeId());

            Message::Handle hRemoveAllRequest = vNamedCacheFactory->createMessage(RemoveAllRequest::type_id);
            TS_ASSERT(instanceof<RemoveAllRequest::View>(hRemoveAllRequest));
            TS_ASSERT(RemoveAllRequest::type_id == hRemoveAllRequest->getTypeId());
            }

        /**
        * Test PutRequest.
        */
        void testPutRequest()
            {
            String::Handle key = "testPutKey";
            String::Handle value1 = "testPutValue1";
            String::Handle value2 = "testPutValue2";
            Object::Holder previousValue;

            GetRequest::Handle hGetRequest = cast<GetRequest::Handle>(vNamedCacheFactory->createMessage(GetRequest::type_id));
            hGetRequest->setKey(convToBinary->convert(key));

            Response::Handle hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hGetRequest))->waitForResponse(-1);
            if (hResponse->isFailure() == false)
                {
                previousValue = convFromBinary->convert(hResponse->getResult());
                }

            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(vNamedCacheFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(convToBinary->convert(key));
            hPutRequest->setValue(convToBinary->convert(value1));
            hPutRequest->setReturnRequired(true);

            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            Codec::Handle hCodec = PofCodec::create();
            hCodec->encode(hNamedCacheChannel, hPutRequest, hbout);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            PutRequest::Handle hPutRequestResult = cast<PutRequest::Handle>(hCodec->decode(hNamedCacheChannel, hbin));
            TS_ASSERT(hPutRequestResult->getExpiryDelay() == hPutRequest->getExpiryDelay());
            TS_ASSERT(hPutRequestResult->isReturnRequired() == hPutRequest->isReturnRequired());
            TS_ASSERT(hPutRequestResult->getValue()->equals(convToBinary->convert(value1)));
            hPutRequest = hPutRequestResult;

            Response::Handle hPutResponse = hNamedCacheChannel->send(cast<Request::Handle>(hPutRequest))->waitForResponse(-1);
            TS_ASSERT(hPutRequest->getTypeId() == PutRequest::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hPutResponse));
            TS_ASSERT_EQUALS(7, hPutRequest->getImplVersion());
            TS_ASSERT(hPutResponse->isFailure() == false);
            TS_ASSERT(hPutRequest->getId() == hPutResponse->getRequestId());
            TS_ASSERT(hPutResponse->getTypeId() == 0);

            if (hPutRequest->isReturnRequired() == true)
                {
                if (previousValue == NULL)
                    {
                    TS_ASSERT(hPutResponse->getResult() == NULL);
                    }
                else
                    {
                    TS_ASSERT(hPutResponse->getResult()->equals(convToBinary->convert(previousValue)));
                    }
                }

            hPutRequest = cast<PutRequest::Handle>(vNamedCacheFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(convToBinary->convert(key));
            hPutRequest->setValue(convToBinary->convert(value2));
            hPutRequest->setReturnRequired(true);

            hPutResponse = hNamedCacheChannel->send(cast<Request::Handle>(hPutRequest))->waitForResponse(-1);
            TS_ASSERT(hPutResponse->isFailure() == false);
            TS_ASSERT(hPutRequest->getId() == hPutResponse->getRequestId());
            TS_ASSERT(hPutResponse->getTypeId() == 0);
            if (hPutRequest->isReturnRequired() == true)
                {
                TS_ASSERT(value1->equals(convFromBinary->convert(hPutResponse->getResult())));
                }
            }

        /**
        * Test GetRequest.
        */
        void testGetRequest()
            {
            String::Handle key = "testGetKey";
            String::Handle value = "testGetValue";

            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(vNamedCacheFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(convToBinary->convert(key));
            hPutRequest->setValue(convToBinary->convert(value));
            hNamedCacheChannel->request(cast<Request::Handle>(hPutRequest));

            GetRequest::Handle hGetRequest = cast<GetRequest::Handle>(vNamedCacheFactory->createMessage(GetRequest::type_id));
            hGetRequest->setKey(convToBinary->convert(key));

            Response::Handle hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hGetRequest))->waitForResponse(-1);
            TS_ASSERT(hPutRequest->getTypeId() == PutRequest::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hResponse));
            TS_ASSERT_EQUALS(7, hGetRequest->getImplVersion());
            TS_ASSERT(hResponse->isFailure() == false);
            TS_ASSERT(hGetRequest->getId() == hResponse->getRequestId());
            TS_ASSERT(hResponse->getTypeId() == 0);
            TS_ASSERT(value->equals(convFromBinary->convert(hResponse->getResult())));
            }

        /**
        * Test GetRequest and PutAllRequest.
        */
        void testGetAndPutAllRequest()
            {
            ObjectArray::Handle keys = ObjectArray::create(3);
            keys[0] = String::create("Goran Milosavljevic");
            keys[1] = String::create("Ana Cikic");
            keys[2] = String::create("Ivan Cikic");

            ObjectArray::Handle values = ObjectArray::create(3);
            values[0] = String::create("10.0.0.180");
            values[1] = String::create("10.0.0.181");
            values[2] = String::create("10.0.0.182");

            HashMap::Handle addresses = HashMap::create();
            addresses->put(convToBinary->convert(keys[0]), convToBinary->convert(values[0]));
            addresses->put(convToBinary->convert(keys[1]), convToBinary->convert(values[1]));
            addresses->put(convToBinary->convert(keys[2]), convToBinary->convert(values[2]));

            PutAllRequest::Handle hPutAllRequest = cast<PutAllRequest::Handle>(vNamedCacheFactory->createMessage(PutAllRequest::type_id));
            hPutAllRequest->setMap(addresses);

            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            Codec::Handle hCodec = PofCodec::create();
            hCodec->encode(hNamedCacheChannel, hPutAllRequest, hbout);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            PutAllRequest::Handle hResult = cast<PutAllRequest::Handle>(hCodec->decode(hNamedCacheChannel, hbin));

            TS_ASSERT(hResult->getMap()->size() == 3);
            TS_ASSERT(addresses->get(convToBinary->convert(keys[0]))->equals(hResult->getMap()->get(convToBinary->convert(keys[0]))));
            TS_ASSERT(addresses->get(convToBinary->convert(keys[1]))->equals(hResult->getMap()->get(convToBinary->convert(keys[1]))));
            TS_ASSERT(addresses->get(convToBinary->convert(keys[2]))->equals(hResult->getMap()->get(convToBinary->convert(keys[2]))));

            hPutAllRequest = cast<PutAllRequest::Handle>(vNamedCacheFactory->createMessage(PutAllRequest::type_id));
            hPutAllRequest->setMap(addresses);
            Response::Handle hPutAllResponse = hNamedCacheChannel->send(cast<Request::Handle>(hPutAllRequest))->waitForResponse(-1);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hPutAllResponse));
            TS_ASSERT(hPutAllResponse->isFailure() == false);
            TS_ASSERT(hPutAllRequest->getId() == hPutAllResponse->getRequestId());
            TS_ASSERT(hPutAllResponse->getResult() == NULL);

            GetAllRequest::Handle hGetAllRequest = cast<GetAllRequest::Handle>(vNamedCacheFactory->createMessage(GetAllRequest::type_id));
            ArrayList::Handle hNames = ArrayList::create();
            hNames->add(convToBinary->convert(keys[1]));
            hNames->add(convToBinary->convert(keys[2]));
            hGetAllRequest->setKeySet(hNames);

            Response::Handle hGetAllResponse = hNamedCacheChannel->send(cast<Request::Handle>(hGetAllRequest))->waitForResponse(-1);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hGetAllResponse));
            TS_ASSERT(hGetAllResponse->isFailure() == false);
            TS_ASSERT(hGetAllRequest->getId() == hGetAllResponse->getRequestId());
            TS_ASSERT(hGetAllResponse->getTypeId() == 0);
            TS_ASSERT(instanceof<Map::View>(hGetAllResponse->getResult()));
            Map::View vResult = cast<Map::View>(hGetAllResponse->getResult());
            TS_ASSERT(addresses->get(convToBinary->convert(keys[1]))->equals(vResult->get(convToBinary->convert(keys[1]))));
            TS_ASSERT(addresses->get(convToBinary->convert(keys[2]))->equals(vResult->get(convToBinary->convert(keys[2]))));
            }

        /**
        * Test SizeRequest.
        */
        void testSizeRequest()
            {
            ClearRequest::Handle hClearRequest = cast<ClearRequest::Handle>(vNamedCacheFactory->createMessage(ClearRequest::type_id));

            Response::Handle hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hClearRequest))->waitForResponse(-1);
            TS_ASSERT(hClearRequest->getTypeId() == ClearRequest::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hResponse));
            TS_ASSERT(hClearRequest->getId() == hResponse->getRequestId());
            TS_ASSERT(hResponse->isFailure() == false);

            SizeRequest::Handle hSizeRequest = cast<SizeRequest::Handle>(vNamedCacheFactory->createMessage(SizeRequest::type_id));

            Response::Handle hSizeResponseBefore = hNamedCacheChannel->send(cast<Request::Handle>(hSizeRequest))->waitForResponse(-1);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hSizeResponseBefore));
            TS_ASSERT(hSizeResponseBefore->isFailure() == false);
            TS_ASSERT(hSizeRequest->getId() == hSizeResponseBefore->getRequestId());
            TS_ASSERT(hSizeResponseBefore->getTypeId() == 0);
            TS_ASSERT(instanceof<Integer32::View>(hSizeResponseBefore->getResult()));
            Integer32::View sizeBefore = cast<Integer32::View>(hSizeResponseBefore->getResult());
            TS_ASSERT(sizeBefore->getInt32Value() == 0);

            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(vNamedCacheFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(convToBinary->convert(String::create("newKey")));
            hPutRequest->setValue(convToBinary->convert(String::create("newValue")));
            hNamedCacheChannel->request(cast<Request::Handle>(hPutRequest));

            hSizeRequest = cast<SizeRequest::Handle>(vNamedCacheFactory->createMessage(SizeRequest::type_id));
            Response::Handle hSizeResponseAfter = hNamedCacheChannel->send(cast<Request::Handle>(hSizeRequest))->waitForResponse(-1);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hSizeResponseAfter));
            TS_ASSERT(hSizeResponseAfter->isFailure() == false);
            TS_ASSERT(hSizeRequest->getId() == hSizeResponseAfter->getRequestId());
            TS_ASSERT(hSizeResponseAfter->getTypeId() == 0);
            TS_ASSERT(instanceof<Integer32::View>(hSizeResponseAfter->getResult()));
            Integer32::View sizeAfter = cast<Integer32::View>(hSizeResponseAfter->getResult());
            TS_ASSERT(sizeAfter->getInt32Value() == 1);
            }

        /**
        * Test NamedCache Exception.
        */
        void testNamedCacheException()
            {
            ObjectArray::Handle keys = ObjectArray::create(3);
            keys[0] = String::create("Goran Milosavljevic");
            keys[1] = String::create("Ana Cikic");
            keys[2] = String::create("Ivan Cikic");

            ObjectArray::Handle values = ObjectArray::create(3);
            values[0] = String::create("10.0.0.180");
            values[1] = String::create("10.0.0.181");
            values[2] = String::create("10.0.0.182");

            HashMap::Handle addresses = HashMap::create();
            addresses->put(convToBinary->convert(keys[0]), convToBinary->convert(values[0]));
            addresses->put(convToBinary->convert(keys[1]), convToBinary->convert(values[1]));
            addresses->put(convToBinary->convert(keys[2]), convToBinary->convert(values[2]));

            PutAllRequest::Handle hPutAllRequest = cast<PutAllRequest::Handle>(vNamedCacheFactory->createMessage(PutAllRequest::type_id));
            hPutAllRequest->setMap(addresses);
            hNamedCacheChannel->request(cast<Request::Handle>(hPutAllRequest));

            DestroyCacheRequest::Handle hDestroyRequest = cast<DestroyCacheRequest::Handle>(hCacheServiceChannel->getMessageFactory()->createMessage(DestroyCacheRequest::type_id));
            hDestroyRequest->setCacheName("dist-extend-direct");
            Response::Handle hResponse = hCacheServiceChannel->send(cast<Request::Handle>(hDestroyRequest))->waitForResponse(-1);
            // the sleep was added to work around COH-4544
            Thread::sleep(5000);

            GetAllRequest::Handle hGetAllRequest = cast<GetAllRequest::Handle>(vNamedCacheFactory->createMessage(GetAllRequest::type_id));
            ArrayList::Handle hNames = ArrayList::create();
            hNames->add(convToBinary->convert(keys[1]));
            hNames->add(convToBinary->convert(keys[2]));
            hGetAllRequest->setKeySet(hNames);

             try
                {
                hNamedCacheChannel->send(cast<Request::Handle>(hGetAllRequest))->waitForResponse(-1);
                // should never reach here - no longer true, due to COH-8696
                //TS_ASSERT(1 == 0);
                }
            catch(Exception::View ve)
                {
                TS_ASSERT(ve != NULL);
                TS_ASSERT(instanceof<PortableException::View>(ve));
                }
            }

        /**
        * Test ContainsKeyRequest.
        */
        void testContainsKeyRequest()
            {
            String::Handle key = "testContainsKeyKey";
            String::Handle value = "testContainsKeyValue";

            ClearRequest::Handle hClearRequest = cast<ClearRequest::Handle>(vNamedCacheFactory->createMessage(ClearRequest::type_id));
            Response::Handle hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hClearRequest))->waitForResponse(-1);
            TS_ASSERT(hResponse->isFailure() == false);

            ContainsKeyRequest::Handle hContainsKeyRequest = cast<ContainsKeyRequest::Handle>(vNamedCacheFactory->createMessage(ContainsKeyRequest::type_id));
            hContainsKeyRequest->setKey(convToBinary->convert(key));

            Response::Handle hContainsKeyResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsKeyRequest))->waitForResponse(-1);
            TS_ASSERT(hContainsKeyRequest->getTypeId() == ContainsKeyRequest::type_id);
            TS_ASSERT(hContainsKeyResponse->getTypeId() == NamedCacheResponse::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hContainsKeyResponse));
            TS_ASSERT(hContainsKeyResponse->isFailure() == false);
            TS_ASSERT(hContainsKeyRequest->getId() == hContainsKeyResponse->getRequestId());
            TS_ASSERT(instanceof<Boolean::View>(hContainsKeyResponse->getResult()));
            Boolean::View vBoolResult = cast<Boolean::View>(hContainsKeyResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(false)));

            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(vNamedCacheFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(convToBinary->convert(key));
            hPutRequest->setValue(convToBinary->convert(value));
            hNamedCacheChannel->request(cast<Request::Handle>(hPutRequest));

            hContainsKeyRequest = cast<ContainsKeyRequest::Handle>(vNamedCacheFactory->createMessage(ContainsKeyRequest::type_id));
            hContainsKeyRequest->setKey(convToBinary->convert(key));
            hContainsKeyResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsKeyRequest))->waitForResponse(-1);
            TS_ASSERT(hContainsKeyRequest->getTypeId() == ContainsKeyRequest::type_id);
            TS_ASSERT(hContainsKeyResponse->getTypeId() == NamedCacheResponse::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hContainsKeyResponse));
            TS_ASSERT(hContainsKeyResponse->isFailure() == false);
            TS_ASSERT(hContainsKeyRequest->getId() == hContainsKeyResponse->getRequestId());
            TS_ASSERT(instanceof<Boolean::View>(hContainsKeyResponse->getResult()));
            vBoolResult = cast<Boolean::View>(hContainsKeyResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(true)));
            }

        /**
        * Test ContainsValueRequest.
        */
        void testContainsValueRequest()
            {
            String::Handle key = "testContainsValueKey";
            String::Handle value = "testContainsValueValue";

            ClearRequest::Handle hClearRequest = cast<ClearRequest::Handle>(vNamedCacheFactory->createMessage(ClearRequest::type_id));
            Response::Handle hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hClearRequest))->waitForResponse(-1);
            TS_ASSERT(hResponse->isFailure() == false);

            ContainsValueRequest::Handle hContainsValueRequest = cast<ContainsValueRequest::Handle>(vNamedCacheFactory->createMessage(ContainsValueRequest::type_id));
            hContainsValueRequest->setValue(convToBinary->convert(value));

            Response::Handle hContainsValueResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsValueRequest))->waitForResponse(-1);
            TS_ASSERT(hContainsValueRequest->getTypeId() == ContainsValueRequest::type_id);
            TS_ASSERT(hContainsValueResponse->getTypeId() == NamedCacheResponse::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hContainsValueResponse));
            TS_ASSERT(hContainsValueResponse->isFailure() == false);
            TS_ASSERT(hContainsValueRequest->getId() == hContainsValueResponse->getRequestId());
            TS_ASSERT(instanceof<Boolean::View>(hContainsValueResponse->getResult()));
            Boolean::View vBoolResult = cast<Boolean::View>(hContainsValueResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(false)));

            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(vNamedCacheFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(convToBinary->convert(key));
            hPutRequest->setValue(convToBinary->convert(value));
            hNamedCacheChannel->request(cast<Request::Handle>(hPutRequest));

            hContainsValueRequest = cast<ContainsValueRequest::Handle>(vNamedCacheFactory->createMessage(ContainsValueRequest::type_id));
            hContainsValueRequest->setValue(convToBinary->convert(value));
            hContainsValueResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsValueRequest))->waitForResponse(-1);
            TS_ASSERT(hContainsValueRequest->getTypeId() == ContainsValueRequest::type_id);
            TS_ASSERT(hContainsValueResponse->getTypeId() == NamedCacheResponse::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hContainsValueResponse));
            TS_ASSERT(hContainsValueResponse->isFailure() == false);
            TS_ASSERT(hContainsValueRequest->getId() == hContainsValueResponse->getRequestId());
            TS_ASSERT(instanceof<Boolean::View>(hContainsValueResponse->getResult()));
            vBoolResult = cast<Boolean::View>(hContainsValueResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(true)));
            }

        /**
        * Test RemoveRequest.
        */
        void testRemoveRequest()
            {
            String::Handle key = "testRemoveKey";
            String::Handle value = "testRemoveValue";

            ClearRequest::Handle hClearRequest = cast<ClearRequest::Handle>(vNamedCacheFactory->createMessage(ClearRequest::type_id));
            Response::Handle hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hClearRequest))->waitForResponse(-1);
            TS_ASSERT(hResponse->isFailure() == false);

            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(vNamedCacheFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(convToBinary->convert(key));
            hPutRequest->setValue(convToBinary->convert(value));
            hPutRequest->setReturnRequired(true);
            Response::Handle hPutResponse = hNamedCacheChannel->send(cast<Request::Handle>(hPutRequest))->waitForResponse(-1);
            TS_ASSERT(hPutResponse->isFailure() == false);
            TS_ASSERT(convFromBinary->convert(hPutResponse->getResult()) == NULL);

            ContainsKeyRequest::Handle hContainsKeyRequest = cast<ContainsKeyRequest::Handle>(vNamedCacheFactory->createMessage(ContainsKeyRequest::type_id));
            hContainsKeyRequest->setKey(convToBinary->convert(key));
            Response::Handle hContainsKeyResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsKeyRequest))->waitForResponse(-1);
            TS_ASSERT(hContainsKeyResponse->isFailure() == false);
            Boolean::View vBoolResult = cast<Boolean::View>(hContainsKeyResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(true)));

            RemoveRequest::Handle hRemoveRequest = cast<RemoveRequest::Handle>(vNamedCacheFactory->createMessage(RemoveRequest::type_id));
            hRemoveRequest->setKey(convToBinary->convert(key));
            hRemoveRequest->setReturnRequired(true);
            Response::Handle hRemoveResponse = hNamedCacheChannel->send(cast<Request::Handle>(hRemoveRequest))->waitForResponse(-1);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hRemoveResponse));
            TS_ASSERT(hRemoveResponse->getTypeId() == NamedCacheResponse::type_id);
            TS_ASSERT(hRemoveRequest->getId() == hRemoveResponse->getRequestId());
            TS_ASSERT(hRemoveResponse->isFailure() == false);
            if (hRemoveRequest->isReturnRequired() == true)
                {
                TS_ASSERT(hRemoveResponse->getResult()->equals(convToBinary->convert(value)));
                }

            hRemoveRequest = cast<RemoveRequest::Handle>(vNamedCacheFactory->createMessage(RemoveRequest::type_id));
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            Codec::Handle hCodec = PofCodec::create();
            hRemoveRequest->setKey(convToBinary->convert(key));
            hRemoveRequest->setReturnRequired(true);
            hCodec->encode(hNamedCacheChannel, hRemoveRequest, hbout);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            RemoveRequest::Handle hRemoveResult = cast<RemoveRequest::Handle>(hCodec->decode(hNamedCacheChannel, hbin));

            hRemoveRequest = cast<RemoveRequest::Handle>(vNamedCacheFactory->createMessage(RemoveRequest::type_id));
            hRemoveRequest->setKey(convToBinary->convert(key));
            hRemoveRequest->setReturnRequired(true);
            TS_ASSERT(hRemoveRequest->isReturnRequired() == hRemoveResult->isReturnRequired());
            TS_ASSERT(hRemoveRequest->getKey()->equals(hRemoveResult->getKey()));

            TS_ASSERT(hRemoveRequest->getTypeId() == RemoveRequest::type_id);

            hContainsKeyRequest = cast<ContainsKeyRequest::Handle>(vNamedCacheFactory->createMessage(ContainsKeyRequest::type_id));
            hContainsKeyRequest->setKey(convToBinary->convert(key));
            hContainsKeyResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsKeyRequest))->waitForResponse(-1);
            TS_ASSERT(hContainsKeyResponse->isFailure() == false);
            TS_ASSERT(instanceof<Boolean::View>(hContainsKeyResponse->getResult()));
            vBoolResult = cast<Boolean::View>(hContainsKeyResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(false)));
            }

        /**
        * Test ContainsAllRequest and RemoveAllRequest.
        */
        void testContainsAllAndRemoveAllRequest()
            {
            ObjectArray::Handle keys = ObjectArray::create(3);
            keys[0] = String::create("Goran Milosavljevic");
            keys[1] = String::create("Ana Cikic");
            keys[2] = String::create("Ivan Cikic");

            ObjectArray::Handle values = ObjectArray::create(3);
            values[0] = String::create("10.0.0.180");
            values[1] = String::create("10.0.0.181");
            values[2] = String::create("10.0.0.182");

            HashMap::Handle addresses = HashMap::create();
            addresses->put(convToBinary->convert(keys[0]), convToBinary->convert(values[0]));
            addresses->put(convToBinary->convert(keys[1]), convToBinary->convert(values[1]));
            addresses->put(convToBinary->convert(keys[2]), convToBinary->convert(values[2]));

            ClearRequest::Handle hClearRequest = cast<ClearRequest::Handle>(vNamedCacheFactory->createMessage(ClearRequest::type_id));
            Response::Handle hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hClearRequest))->waitForResponse(-1);
            TS_ASSERT(hResponse->isFailure() == false);

            PutAllRequest::Handle hPutAllRequest = cast<PutAllRequest::Handle>(vNamedCacheFactory->createMessage(PutAllRequest::type_id));
            hPutAllRequest->setMap(addresses);
            hResponse = hNamedCacheChannel->send(cast<Request::Handle>(hPutAllRequest))->waitForResponse(-1);
            TS_ASSERT(hResponse->isFailure() == false);

            ContainsAllRequest::Handle hContainsAllRequest = cast<ContainsAllRequest::Handle>(vNamedCacheFactory->createMessage(ContainsAllRequest::type_id));
            ArrayList::Handle hList = ArrayList::create();
            hList->add(convToBinary->convert(keys[0]));
            hList->add(convToBinary->convert(keys[2]));
            hList->add(convToBinary->convert(String::create("dummy")));
            hContainsAllRequest->setKeySet(hList);
            Response::Handle hContainsAllResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsAllRequest))->waitForResponse(-1);

            TS_ASSERT(hContainsAllRequest->getTypeId() == ContainsAllRequest::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hContainsAllResponse));
            TS_ASSERT(hContainsAllResponse->getTypeId() == NamedCacheResponse::type_id);
            TS_ASSERT(hContainsAllRequest->getId() == hContainsAllResponse->getRequestId());
            TS_ASSERT(hContainsAllResponse->isFailure() == false);
            TS_ASSERT(instanceof<Boolean::View>(hContainsAllResponse->getResult()));
            Boolean::View vBoolResult = cast<Boolean::View>(hContainsAllResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(false)));

            hContainsAllRequest = cast<ContainsAllRequest::Handle>(vNamedCacheFactory->createMessage(ContainsAllRequest::type_id));
            hList->remove(convToBinary->convert(String::create("dummy")));
            hContainsAllRequest->setKeySet(hList);
            hContainsAllResponse = hNamedCacheChannel->send(cast<Request::Handle>(hContainsAllRequest))->waitForResponse(-1);
            TS_ASSERT(hContainsAllResponse->isFailure() == false);
            TS_ASSERT(instanceof<Boolean::View>(hContainsAllResponse->getResult()));
            vBoolResult = cast<Boolean::View>(hContainsAllResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(true)));

            RemoveAllRequest::Handle hRemoveAllRequest = cast<RemoveAllRequest::Handle>(vNamedCacheFactory->createMessage(RemoveAllRequest::type_id));
            hRemoveAllRequest->setKeySet(hList);
            Response::Handle hRemoveAllResponse = hNamedCacheChannel->send(cast<Request::Handle>(hRemoveAllRequest))->waitForResponse(-1);
            TS_ASSERT(hRemoveAllRequest->getTypeId() == RemoveAllRequest::type_id);
            TS_ASSERT(instanceof<NamedCacheResponse::View>(hRemoveAllResponse));
            TS_ASSERT(hRemoveAllResponse->getTypeId() == NamedCacheResponse::type_id);
            TS_ASSERT(hRemoveAllRequest->getId() == hRemoveAllResponse->getRequestId());
            TS_ASSERT(hRemoveAllResponse->isFailure() == false);
            TS_ASSERT(instanceof<Boolean::View>(hRemoveAllResponse->getResult()));
            vBoolResult = cast<Boolean::View>(hRemoveAllResponse->getResult());
            TS_ASSERT(vBoolResult->equals(Boolean::create(true)));
            }
    };
