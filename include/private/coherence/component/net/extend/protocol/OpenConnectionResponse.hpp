/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OPEN_CONNECTION_RESPONSE_HPP
#define COH_OPEN_CONNECTION_RESPONSE_HPP

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofResponse;


/**
* This Request is used to accept a Channel that was spawned by a peer.
*
* @authore nsa 2008.03.20
*/
class COH_EXPORT OpenConnectionResponse
    : public abstract_spec<OpenConnectionResponse,
        extends<AbstractPofResponse> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        OpenConnectionResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        OpenConnectionResponse(const OpenConnectionResponse&);


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


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the Map of negotiated Protocols for this Connection.
        *
        * @return the Map of negotiated Protocols for this Connection
        *
        * @since Coherence 12.1.3
        */
        virtual Map::View getProtocolVersionMap() const;

        /**
        * Set the Map of negotiated Protocols for this Connection.
        *
        * @param vMap  the Map of negotiated Protocols for this Connection
        *
        * @since Coherence 12.1.3
        */
        virtual void setProtocolVersionMap(Map::View vMap);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 0;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A map of negotiated Protocols. The keys are the names of the required
        * Protocols and the values are the negotiated version numbers of the 
        * corresponding Protocol.
        */
        FinalView<Map> f_vProtocolVersionMap;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CONNECTION_RESPONSE_HPP
