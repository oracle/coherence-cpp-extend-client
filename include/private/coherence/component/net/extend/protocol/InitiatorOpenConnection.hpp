/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INITIATOR_OPEN_CONNECTION_HPP
#define COH_INITIATOR_OPEN_CONNECTION_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/protocol/OpenConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


/**
* Internal Request used to create a Channel.
*
* @authore nsa 2008.03.19
*/
class COH_EXPORT InitiatorOpenConnection
    : public class_spec<InitiatorOpenConnection,
        extends<OpenConnection> >
    {
    friend class factory<InitiatorOpenConnection>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InitiatorOpenConnection instance.
        */
        InitiatorOpenConnection();

    private:
        /**
        * Blocked copy constructor.
        */
        InitiatorOpenConnection(const InitiatorOpenConnection&);


    // ----- AbstractPofRequest interface -----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = OpenConnection::type_id;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CONNECTION_HPP
