/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OPEN_CONNECTION_REQUEST_HPP
#define COH_OPEN_CONNECTION_REQUEST_HPP

#include "coherence/net/Member.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/UUID.hpp"

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::net::Member;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Protocol;
using coherence::security::auth::Subject;
using coherence::util::UUID;


/**
* This Request is used to accept a Channel that was spawned by a peer.
*
* @author nsa 2008.03.17
*/
class COH_EXPORT OpenConnectionRequest
    : public abstract_spec<OpenConnectionRequest,
        extends<AbstractPofRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        OpenConnectionRequest();


    // ----- AbstractPofRequest interface -----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofResponse::Handle instantiateResponse(
                        Protocol::MessageFactory::View vFactory) const;


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Return a human-readable description of the protocol version map.
        *
        * @return return a String representation of the protocol version map
        */
        virtual String::View getVersionMapDescription(Map::View vMap) const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the unique identifier (UUID) of the client that sent this
        * Request.
        *
        * @return the unique identifier (UUID) of the client that sent this
        *         Request
        */
        virtual UUID::View getClientId() const;

        /**
        * Return the product edition used by the client.
        *
        * @return the product edition used by the client
        */
        virtual int32_t getEdition() const;

        /**
        * Return an optional token representing a Subject to associate with
        * "Channel0". Operations performed on receipt of Messages sent via
        * "Channel0" will be performed on behalf of this Subject.
        *
        * @return an optional token representing a Subject to associate with
        *         "Channel0"
        */
        virtual Array<octet_t>::View getIdentityToken() const;

        /**
        * Return the Member object of the client that sent this Request.
        *
        * @return the Member object of the client that sent this Request
        *
        * @since Coherence 3.7
        */
        virtual Member::View getMember() const;

        /**
        * Return the Map of required Protocols for this Connection.
        *
        * @return the Map of required Protocols for this Connection
        */
        virtual Map::View getProtocolVersionMap() const;

        /**
        * Return the identity under which Messages received by "Channel0" will
        * be executed.
        *
        * @return the identity under which Messages received by "Channel0"
        *         will be executed
        */
        virtual Subject::View getSubject() const;

        /**
         * Return the name of the cluster to connect to.
         *
         * @return the cluster name
         */
        virtual String::View getClusterName() const;

        /**
         * Return the name of the service to connect to.
         *
         * @return the service name
         */
        virtual String::View getServiceName() const;

        /**
        * Set the unique identifier (UUID) of the client that sent this
        * Request.
        *
        * @param vClientId  the unique identifier (UUID) of the client that
        *                   sent this Request
        */
        virtual void setClientId(UUID::View vClietId);

        /**
        * Set the product edition used by the client.
        *
        * @param nEdition  the product edition used by the client
        */
        virtual void setEdition(int32_t nEdition);

        /**
        * Set an optional token representing a Subject to associate with
        * "Channel0". Operations performed on receipt of Messages sent via
        * the "Channel0" will be performed on behalf of this Subject.
        *
        * @param vToken  an optional token representing a Subject to associate
        *                with "Channel0"
        */
        virtual void setIdentityToken(Array<octet_t>::View vToken);

        /**
        * Set the Member object of the client that sent this Request.
        *
        * @param vMember  the Member object of the client that sent this
        *                 Request
        *
        * @since Coherence 3.7
        */
        virtual void setMember(Member::View vMember);

        /**
        * Set the Map of required Protocols for this Connection.
        *
        * @param vMap  the Map of required Protocols for this Connection
        */
        virtual void setProtocolVersionMap(Map::View vMap);

        /**
        * Set the identity under which Messages received by "Channel0" will be
        * executed.
        *
        * @param vSubject  the identity under which Messages received by
        *                  "Channel0" will be executed
        */
        virtual void setSubject(Subject::View vSubject);

        /**
         * Specify the name of the cluster to connect to.
         *
         * @param vsName the cluster name
         */
        virtual void setClusterName(String::View vsName);

        /**
         * Specify the name of the service to connect to.
         *
         * @param vsName the service name
         */
        virtual void setServiceName(String::View vsName);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 1;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The unique identifier (UUID) of the client that sent this Request.
        */
        FinalView<UUID> f_vClientId;

        /**
        * The product edition used by the client.
        */
        int32_t m_nEdition;

        /**
        * An optional token representing a Subject to associate with
        * "Channel0". Operations performed on receipt of Messages sent via
        * "Channel0" will be performed on behalf of this Subject.
        */
        FinalView<Array<octet_t> > f_vIdentityToken;

        /**
        * The Member object of the client that sent this Request.
        */
        FinalView<Member> f_vMember;

        /**
        * A map of required Protocols. The keys are the names of the required
        * Protocols and the values are two element Integer arrays, the first
        * element being the current version and the second being the supported
        * version of the corresponding Protocol.
        */
        FinalView<Map> f_vProtocolVersionMap;

        /**
        * The identity under which Messages received by "Channel0" will be
        * executed.
        */
        FinalView<Subject> f_vSubject;

        /**
        * The name of the cluster the peer wishes to connect to.
        *
        * @since 12.2.1
        */
        FinalView<String> f_vsClusterName;

        /**
        * The name of the service the peer wishes to connect to.
        *
        * @since 12.2.1
        */
        FinalView<String> f_vsServiceName;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CONNECTION_REQUEST_HPP
