/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TCP_INITIATOR_OPEN_CONNECTION_RESPONSE_HPP
#define COH_TCP_INITIATOR_OPEN_CONNECTION_RESPONSE_HPP

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/List.hpp"

#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::List;


/**
* Response to a TcpInitiatorOpenConnectionRequest.
*
* @authore lsho 2010.12.30
*/
class COH_EXPORT TcpInitiatorOpenConnectionResponse
    : public class_spec<TcpInitiatorOpenConnectionResponse,
        extends<InitiatorOpenConnectionResponse> >
    {
    friend class factory<TcpInitiatorOpenConnectionResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TcpInitiatorOpenConnectionResponse instance.
        */
        TcpInitiatorOpenConnectionResponse();


    // -------------- Runnable interface  -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- PortableObject implementation ----------------------------------

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
        * Return whether or not this TcpConnection should be redirected.
        *
        * @return true if the TcpConnection should be redirected
        */
        virtual bool isRedirect() const;

        /**
        * Set the redirect flag.
        *
        * @param fRedirect  true if the TcpConnection should be redirected
        */
        virtual void setRedirect(bool fRedirect);

        /**
        * Return a list of TCP/IP addresses that the TcpConnection should be
        * redirected to.
        *
        * @return a list of TCP/IP addresses that the TcpConnection should be
        *         redirected to
        */
        virtual List::View getRedirectList() const;

        /**
        * Set the list of TCP/IP addresses that the TcpConnection should be
        * redirected to. Each element of the list is a two element array,
        * with the first element being the IP address in string format and
        * the second being the port number. 
        *
        * @param vList  a list of TCP/IP addresses that the TcpConnection 
        *               should be redirected to
        */
        virtual void setRedirectList(List::View vList);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Flag that indicates redirect; true if the TcpConnection should be
        * redirected.
        */
        bool m_fRedirect;

        /**
        * A list of TCP/IP addresses that the TcpConnection should be 
        * redirected to. Each element of the list is a two element array, 
        * with the first element being the IP address in string format and
        * the second being the port number.
        */
        FinalView<List> f_vListRedirect;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_TCP_INITIATOR_OPEN_CONNECTION_RESPONSE_HPP
