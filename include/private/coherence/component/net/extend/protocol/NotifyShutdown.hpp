/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NOTIFY_SHUTDOWN_HPP
#define COH_NOTIFY_SHUTDOWN_HPP

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofMessage;


/**
* This internal Message is sent to a ConnectionManager it is supposed to shut
* down. The ConnectionManager must clean up and unregister itself. Note that
* the only task of the shut-down is to begin the process of shutting down the
* service; technically the ConnectionManager does not have to be stopped by
* the time the shutdown Message completes its processing, although the default
* implementation does stop it immediately.
*
* @author nsa 2008.03.19
*/
class COH_EXPORT NotifyShutdown
    : public class_spec<NotifyShutdown,
        extends<AbstractPofMessage> >
    {
    friend class factory<NotifyShutdown>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        NotifyShutdown();

    private:
        /**
        * Blocked copy constructor.
        */
        NotifyShutdown(const NotifyShutdown&);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;

        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = -5;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_NOTIFY_SHUTDOWN_HPP
