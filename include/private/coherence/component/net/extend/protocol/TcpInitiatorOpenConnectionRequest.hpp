/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TCP_INITIATOR_OPEN_CONNECTION_REQUEST_HPP
#define COH_TCP_INITIATOR_OPEN_CONNECTION_REQUEST_HPP

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionRequest.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* This Request is used to open a new Connection.
*
* @author lsho 2010.12.30
*/
class COH_EXPORT TcpInitiatorOpenConnectionRequest
    : public class_spec<TcpInitiatorOpenConnectionRequest,
        extends<InitiatorOpenConnectionRequest> >
    {
    friend class factory<TcpInitiatorOpenConnectionRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TcpInitiatorOpenConnectionRequest instance.
        */
        TcpInitiatorOpenConnectionRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        TcpInitiatorOpenConnectionRequest(const TcpInitiatorOpenConnectionRequest&);


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


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Set the PofConnection to open.
        *
        * @param hConnection  the PofConnection to be opened
        */
        virtual void setConnectionOpen(PofConnection::Handle hConnection);

        /**
        * Return true if the TcpInitiator supports redirection.
        *
        * @return true to indicate that redirect is supported by the
        *         TcpInitiator
        */
        virtual bool isRedirectSupported() const;

        /**
        * Return whether or not the TcpConnection is being opened in response
        * to a redirection.
        *
        * @return true if the TcpConnection is being opened in response to a
        *         redirection
        */
        virtual bool isRedirect() const;

        /**
        * Set the redirect flag.
        *
        * @param fRedirect  whether or not the TcpConnection is being opened
        *                   in response to a redirection
        */
        virtual void setRedirect(bool fRedirect);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Flag to indicate redirect; true if the TcpConnection is being 
        * opened in response to a redirection.
        */
        bool m_fRedirect;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_TCP_INITIATOR_OPEN_CONNECTION_REQUEST_HPP
