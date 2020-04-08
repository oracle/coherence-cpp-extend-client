/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NOTIFY_STARTUP_HPP
#define COH_NOTIFY_STARTUP_HPP

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofMessage;


/**
* This internal Message is sent to a ConnectionManager when it first has been
* started.
*
* @author nsa 2008.03.19
*/
class COH_EXPORT NotifyStartup
    : public class_spec<NotifyStartup,
        extends<AbstractPofMessage> >
    {
    friend class factory<NotifyStartup>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        NotifyStartup();

    private:
        /**
        * Blocked copy constructor.
        */
        NotifyStartup(const NotifyStartup&);


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
        static const int32_t type_id = -6;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_NOTIFY_STARTUP_HPP
