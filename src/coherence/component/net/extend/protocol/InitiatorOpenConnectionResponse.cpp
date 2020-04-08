/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionResponse.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/UUID.hpp"

#include "private/coherence/component/net/extend/protocol/OpenConnectionRequest.hpp"
#include "private/coherence/component/util/Initiator.hpp"
#include "private/coherence/component/net/extend/PofChannel.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofChannel;
using coherence::component::net::extend::PofConnection;
using coherence::component::util::Initiator;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::UUID;


// ----- constructors -------------------------------------------------------

InitiatorOpenConnectionResponse::InitiatorOpenConnectionResponse()
    {
    }


// ----- Runnable interface -------------------------------------------------

void InitiatorOpenConnectionResponse::run()
    {
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel());
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    if (isFailure())
        {
        PofConnection::Handle hConnection =
                cast<PofConnection::Handle>(hChannel0->getConnection());

        Object::View    vResult = getResult();
        Exception::View eResult = instanceof<Exception::View>(vResult)
                ? cast<Exception::View>(vResult) : NULL;

        hConnection->closeInternal(false, eResult, -1L);
        return;
        }

    PofConnection::Handle hConnection =
        cast<PofConnection::Handle>(hChannel0->getConnection());

    Initiator::Handle   hModule = cast<Initiator::Handle>(hChannel0->getReceiver());
    ObjectArray::Handle hao     = cast<ObjectArray::Handle>(getResult());

    COH_ENSURE_EQUALITY(hao->length, 2);

    // extract the "Channel0" configuration from the OpenConnectionRequest
    OpenConnectionRequest::Handle hRequest =
            cast<OpenConnectionRequest::Handle>(
                    hChannel0->getRequest(getRequestId()));
    COH_ENSURE(NULL != hRequest);

    hConnection->setId(cast<UUID::Handle>(hao[0]));
    hConnection->setMember(hRequest->getMember());
    hConnection->setPeerId(cast<UUID::Handle>(hao[1]));
    hChannel0->setSubject(hRequest->getSubject());

    // configure the MessageFactory map for the Connection
    Map::View   vMapProtocol = hModule->getProtocolMap();
    Map::Handle hMapFactory  = HashMap::create(vMapProtocol->size());
    Map::View   vMapVersion  = getProtocolVersionMap();
    if (NULL != vMapVersion)
        {
        for (Iterator::Handle hIter = vMapVersion->entrySet()->iterator(); hIter->hasNext(); )
            {
            Map::Entry::View vEntry    = cast<Map::Entry::View>(hIter->next());
            String::View     vsName    = cast<String::View>(vEntry->getKey());
            Integer32::View  vVersion  = cast<Integer32::View>(vEntry->getValue());
            Protocol::View   vProtocol = cast<Protocol::View>(vMapProtocol->get(vsName));
        
            hMapFactory->put(vsName, vProtocol->getMessageFactory(vVersion->getValue()));
            }
        }
    for (Iterator::Handle hIter = vMapProtocol->entrySet()->iterator(); hIter->hasNext(); )
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        String::View     vsName = cast<String::View>(vEntry->getKey());
        
        if (!hMapFactory->containsKey(vsName))
            {
            Protocol::View vProtocol = cast<Protocol::View>(vEntry->getValue());
            hMapFactory->put(vsName, vProtocol->getMessageFactory(vProtocol->getCurrentVersion()));
            }
        }
    hConnection->setMessageFactoryMap(cast<Map::View>(hMapFactory));

    // the Connection is now ready for use
    hModule->onConnectionOpened(hConnection);
    }

COH_CLOSE_NAMESPACE5
