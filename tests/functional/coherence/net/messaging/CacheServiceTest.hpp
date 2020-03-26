/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/pof/PofBufferReader.hpp"
#include "coherence/io/pof/PofBufferWriter.hpp"
#include "coherence/io/pof/PortableException.hpp"
#include "coherence/net/DefaultOperationalContext.hpp"
#include "coherence/net/OperationalContext.hpp"

#include "coherence/run/xml/XmlElement.hpp"

#include "private/coherence/component/net/extend/PofCodec.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceFactory.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/EnsureCacheRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/DestroyCacheRequest.hpp"
#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/component/util/TcpInitiator.hpp"
#include "private/coherence/net/URI.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Codec.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Response.hpp"
#include "private/coherence/net/messaging/Request.hpp"
#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/run/xml/SimpleParser.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"
#include "private/coherence/util/StringHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::PortableException;
using coherence::component::net::extend::PofCodec;
using coherence::component::net::extend::protocol::cache::NamedCacheProtocol;
using coherence::component::net::extend::protocol::cache::PutRequest;
using coherence::component::net::extend::protocol::cache::service::CacheServiceFactory;
using coherence::component::net::extend::protocol::cache::service::CacheServiceProtocol;
using coherence::component::net::extend::protocol::cache::service::CacheServiceResponse;
using coherence::component::net::extend::protocol::cache::service::EnsureCacheRequest;
using coherence::component::net::extend::protocol::cache::service::DestroyCacheRequest;
using coherence::component::util::Peer;
using coherence::component::util::TcpInitiator;
using coherence::net::DefaultOperationalContext;
using coherence::net::OperationalContext;
using coherence::net::URI;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Codec;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionInitiator;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Response;
using coherence::net::messaging::Request;
using coherence::run::xml::SimpleElement;
using coherence::run::xml::SimpleParser;
using coherence::run::xml::XmlElement;
using coherence::run::xml::XmlHelper;
using coherence::util::StringHelper;
using coherence::util::logging::Logger;

/**
* CacheService Test Suite.
*/
class CacheServiceTest : public CxxTest::TestSuite
    {
    MemberHandle<Connection> m_hConnection;
    MemberHandle<ConnectionInitiator> m_hInitiator;

    public:
        CacheServiceTest()
            : m_hConnection(System::common()),
              m_hInitiator(System::common())
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
        * Test CacheService.
        */
        void testCacheService()
            {
            Channel::Handle hChannel = m_hConnection->openChannel(CacheServiceProtocol::getInstance(), "CacheServiceProxy", NULL, NULL);
            Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
            TS_ASSERT(vFactory != NULL);
            TS_ASSERT(instanceof<CacheServiceFactory::View>(vFactory));
            TS_ASSERT(instanceof<CacheServiceProtocol::View>(vFactory->getProtocol()));
            TS_ASSERT(m_hInitiator->getProtocol("CacheServiceProtocol") != NULL);

            EnsureCacheRequest::View vEnsureRequest = cast<EnsureCacheRequest::View>(vFactory->createMessage(EnsureCacheRequest::type_id));
            DestroyCacheRequest::View vDestroyRequest = cast<DestroyCacheRequest::View>(vFactory->createMessage(DestroyCacheRequest::type_id));
            TS_ASSERT(vEnsureRequest != NULL);
            TS_ASSERT(vDestroyRequest != NULL);
            TS_ASSERT(instanceof<EnsureCacheRequest::View>(vEnsureRequest));
            TS_ASSERT(instanceof<DestroyCacheRequest::View>(vDestroyRequest));
            }

        void testEnsureCacheRequest()
            {
            Channel::Handle hChannel = m_hConnection->openChannel(CacheServiceProtocol::getInstance(), "CacheServiceProxy", NULL, NULL);
            Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
            EnsureCacheRequest::Handle hEnsureRequest = cast<EnsureCacheRequest::Handle>(vFactory->createMessage(EnsureCacheRequest::type_id));
            hEnsureRequest->setCacheName("dist-extend-direct");

            Response::Handle hResponse = hChannel->send(cast<Request::Handle>(hEnsureRequest))->waitForResponse(-1);
            TS_ASSERT(instanceof<EnsureCacheRequest::View>(hEnsureRequest));
            TS_ASSERT(EnsureCacheRequest::type_id == hEnsureRequest->getTypeId());
            TS_ASSERT(instanceof<CacheServiceResponse::View>(hResponse));
            CacheServiceResponse::View vCacheResponse = cast<CacheServiceResponse::View>(hResponse);
            TS_ASSERT(vCacheResponse->isFailure() == false);
            TS_ASSERT(vCacheResponse->getResult() != NULL);
            TS_ASSERT(vCacheResponse->getRequestId() == hEnsureRequest->getId());
            TS_ASSERT(vCacheResponse->getTypeId() == 0);

            URI::View vUri = URI::create(cast<String::View>(vCacheResponse->getResult()));
            TS_ASSERT(vUri != NULL);
            Channel::Handle hChannelResult = m_hConnection->acceptChannel(vUri, NULL, NULL);
            TS_ASSERT(hChannelResult != NULL);
            }

        void testEnsureCacheRequestException()
            {
            Channel::Handle hChannel = m_hConnection->openChannel(CacheServiceProtocol::getInstance(), "CacheServiceProxy", NULL, NULL);
            Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
            EnsureCacheRequest::Handle hEnsureRequest = cast<EnsureCacheRequest::Handle>(vFactory->createMessage(EnsureCacheRequest::type_id));
            hEnsureRequest->setCacheName("");

            TS_ASSERT(instanceof<EnsureCacheRequest::View>(hEnsureRequest));
            TS_ASSERT(EnsureCacheRequest::type_id == hEnsureRequest->getTypeId());

            Request::Status::Handle hStatus = hChannel->send(cast<Request::Handle>(hEnsureRequest));

            try
                {
                hStatus->waitForResponse(-1);
                }
            catch(Exception::View ex)
                {
                TS_ASSERT(hStatus != NULL);
                TS_ASSERT(instanceof<PortableException::View>(ex));
                PortableException::View hPortableException = cast<PortableException::View>(ex);
                TS_ASSERT(hPortableException != NULL);
                }
            }

        void testDestroyCacheRequest()
            {
            Channel::Handle hChannel = m_hConnection->openChannel(CacheServiceProtocol::getInstance(), "CacheServiceProxy", NULL, NULL);
            Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
            EnsureCacheRequest::Handle hEnsureRequest = cast<EnsureCacheRequest::Handle>(vFactory->createMessage(EnsureCacheRequest::type_id));
            hEnsureRequest->setCacheName("dist-extend-direct");

            URI::View vUri = URI::create(cast<String::View>(hChannel->request(hEnsureRequest)));
            Channel::Handle hNamedChannel = m_hConnection->acceptChannel(vUri, NULL, NULL);
            TS_ASSERT(hNamedChannel->isOpen() == true);

            DestroyCacheRequest::Handle hDestroyRequest = cast<DestroyCacheRequest::Handle>(vFactory->createMessage(DestroyCacheRequest::type_id));
            hDestroyRequest->setCacheName("dist-extend-direct");
            Response::Handle hResponse = hChannel->send(cast<Request::Handle>(hDestroyRequest))->waitForResponse(-1);
            TS_ASSERT(instanceof<DestroyCacheRequest::View>(hDestroyRequest));
            TS_ASSERT(DestroyCacheRequest::type_id == hDestroyRequest->getTypeId());
            TS_ASSERT(instanceof<CacheServiceResponse::View>(hResponse));
            CacheServiceResponse::View vCacheResponse = cast<CacheServiceResponse::View>(hResponse);
            TS_ASSERT(vCacheResponse->isFailure() == false);
            TS_ASSERT(vCacheResponse->getResult() != NULL);
            TS_ASSERT(vCacheResponse->getRequestId() == hDestroyRequest->getId());
            TS_ASSERT(vCacheResponse->getTypeId() == 0);

            vFactory = hNamedChannel->getMessageFactory();
            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(vFactory->createMessage(PutRequest::type_id));
            hPutRequest->setKey(String::create("goran"));
            hPutRequest->setValue(Integer32::create(3));
            hPutRequest->setReturnRequired(true);

            try
                {
                hNamedChannel->send(cast<Request::Handle>(hPutRequest))->waitForResponse(-1);
                }
            catch(Exception::View ex)
                {
                TS_ASSERT(ex != NULL);
                TS_ASSERT(instanceof<PortableException::View>(ex));
                }
            }

        void testEnsureCacheRequestSerDeser()
            {
            Channel::Handle hChannel = m_hConnection->openChannel(CacheServiceProtocol::getInstance(), "CacheServiceProxy", NULL, NULL);
            Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
            EnsureCacheRequest::Handle hEnsureRequest = cast<EnsureCacheRequest::Handle>(vFactory->createMessage(EnsureCacheRequest::type_id));
            hEnsureRequest->setCacheName("dist-extend-direct");

            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();

            Codec::Handle hCodec = PofCodec::create();
            hCodec->encode(hChannel, hEnsureRequest, hbout);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            EnsureCacheRequest::Handle hEnsureRequestResult = cast<EnsureCacheRequest::Handle>(hCodec->decode(hChannel, hbin));
            TS_ASSERT(hEnsureRequest->getCacheName()->equals(hEnsureRequestResult->getCacheName()));
            }
    };
