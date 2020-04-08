/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/OpenConnectionRequest.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "private/coherence/component/net/extend/protocol/OpenConnectionResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;
using coherence::util::HashMap;
using coherence::util::Iterator;


// ----- constructors -------------------------------------------------------

OpenConnectionRequest::OpenConnectionRequest()
    :  f_vClientId(self()), m_nEdition(0), f_vIdentityToken(self()),
       f_vMember(self()), f_vProtocolVersionMap(self()), f_vSubject(self()),
       f_vsClusterName(self()), f_vsServiceName(self())
    {
    }

// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle OpenConnectionRequest::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPofResponse::Handle>(
            vFactory->createMessage(OpenConnectionResponse::type_id));
    }


// ----- Message interface --------------------------------------------------

int32_t OpenConnectionRequest::getTypeId() const
    {
    return type_id;
    }


// ---- PortableObject interface --------------------------------------------

void OpenConnectionRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofRequest::readExternal(hIn);

    setClientId(cast<UUID::View>(hIn->readObject(1)));
    setEdition(hIn->readInt32(2));
    setProtocolVersionMap(hIn->readMap(3, HashMap::create()));
    setIdentityToken(hIn->readOctetArray(4));
    setMember(cast<Member::View>(hIn->readObject(5)));
    setClusterName(hIn->readString(6));
    setServiceName(hIn->readString(7));
    }

void OpenConnectionRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofRequest::writeExternal(hOut);

    ClassLoader::View vLoader = SystemClassLoader::getInstance();
    hOut->writeObject(1, getClientId());
    hOut->writeInt32(2, getEdition());
    hOut->writeMap(3, getProtocolVersionMap(),
            vLoader->loadByType(typeid(String)),
            vLoader->loadByType(typeid(ObjectArray)));
    hOut->writeOctetArray(4, getIdentityToken());
    hOut->writeObject(5, getMember());
    hOut->writeString(6, getClusterName());
    hOut->writeString(7, getServiceName());
    }


// ----- Describable interface ----------------------------------------------

String::View OpenConnectionRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", ClientId="            << getClientId()
        << ", Edition="             << getEdition()
        << ", ProtocolVersionMap="  << getVersionMapDescription(getProtocolVersionMap())
        << ", Member="              << getMember()
        << ", Cluster="             << getClusterName()
        << ", Service="             << getServiceName());
    }


// ----- Helper methods -----------------------------------------------------

String::View OpenConnectionRequest::getVersionMapDescription(Map::View vMap) const
    {
    String::View vs = "{";
    for (Iterator::Handle hIter = vMap->entrySet()->iterator(); hIter->hasNext(); )
        {
        Map::Entry::View vEntry =
            cast<Map::Entry::View>(hIter->next());
        String::View     vsName = cast<String::View>(vEntry->getKey());
        Object::View     vVersion = vEntry->getValue();

        vs = COH_TO_STRING(vs << vsName << '=');
        if (instanceof<Integer32::View>(vVersion))
            {
            // strict version requirement
            vs = COH_TO_STRING(vs << vVersion);
            }
        else
            {
            // version range supported
            ObjectArray::View vaVersions = cast<ObjectArray::View>(vVersion);
            vs = COH_TO_STRING(vs << '[' << vaVersions[1] << ", " << vaVersions[0] << ']');
            }
        if (hIter->hasNext())
            {
            vs = COH_TO_STRING(vs << ", ");
            }
        }
    return COH_TO_STRING(vs << '}');
    }


// ----- accessor methods ---------------------------------------------------

UUID::View OpenConnectionRequest::getClientId() const
    {
    return f_vClientId;
    }

int32_t OpenConnectionRequest::getEdition() const
    {
    return m_nEdition;
    }

Array<octet_t>::View OpenConnectionRequest::getIdentityToken() const
    {
    return f_vIdentityToken;
    }

Member::View OpenConnectionRequest::getMember() const
    {
    return f_vMember;
    }

Map::View OpenConnectionRequest::getProtocolVersionMap() const
    {
    return f_vProtocolVersionMap;
    }

Subject::View OpenConnectionRequest::getSubject() const
    {
    return f_vSubject;
    }

String::View OpenConnectionRequest::getClusterName() const
    {
    return f_vsClusterName;
    }

String::View OpenConnectionRequest::getServiceName() const
    {
    return f_vsServiceName;
    }

void OpenConnectionRequest::setClientId(UUID::View vClientId)
    {
    initialize(f_vClientId, vClientId);
    }

void OpenConnectionRequest::setEdition(int32_t nEdition)
    {
    m_nEdition = nEdition;
    }

void OpenConnectionRequest::setIdentityToken(Array<octet_t>::View vToken)
    {
    initialize(f_vIdentityToken, vToken);
    }

void OpenConnectionRequest::setMember(Member::View vMember)
    {
    initialize(f_vMember, vMember);
    }

void OpenConnectionRequest::setProtocolVersionMap(Map::View vMap)
    {
    initialize(f_vProtocolVersionMap, vMap);
    }

void OpenConnectionRequest::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }

void OpenConnectionRequest::setClusterName(String::View vsName)
    {
    initialize(f_vsClusterName, vsName);
    }

void OpenConnectionRequest::setServiceName(String::View vsName)
    {
    initialize(f_vsServiceName, vsName);
    }

COH_CLOSE_NAMESPACE5
