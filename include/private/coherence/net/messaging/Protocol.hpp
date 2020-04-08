/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PROTOCOL_HPP
#define COH_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/net/messaging/Message.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)


/**
* A Protocol is a binding between a unique name, version information, and a
* set of {@link Message} types. It is used to describe the types of
* {@link Message} objects (the "dialect", so to speak) that can be exchanged
* between two endpoints through a {@link Channel} via a {@link Connection}.
* <p/>
* Before a Connection can be created or accepted, one or more Protocol
* instances must be registered with the client and server-side
* {@link ConnectionManager}. During Connection establishment, the client's
* {@link ConnectionInitiator} sends information about each registered
* Protocol. A compatable set of Protocol objects (or superset) must be
* registered with server's ConnectionManager in order for the the Connection
* to be accepted.
* <p/>
* All Channel implementations must be fully thread-safe.
*
* @author jh  2007.12.20
*
* @see Channel
* @see Connection
* @see ConnectionAcceptor
* @see ConnectionInitiator
* @see ConnectionManager
* @see Protocol
*/
class COH_EXPORT Protocol
    : public interface_spec<Protocol>
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- Protocol interface ---------------------------------------------

    public:
        /**
        * Return the unique name of this Protocol.
        * <p/>
        * This name serves as a unique identifier for the Protocol;
        * therefore, only a single instance of a Protocol with a given name
        * may be registered with a ConnectionManager.
        *
        * @return the Protocol name
        */
        virtual String::View getName() const = 0;

        /**
        * Determine the newest protocol version supported by this Protocol.
        *
        * @return the version number of this Protocol
        */
        virtual int32_t getCurrentVersion() const = 0;

        /**
        * Determine the oldest protocol version supported by this Protocol.
        *
        * @return the oldest protocol version that this Protocol object
        *         supports
        */
        virtual int32_t getSupportedVersion() const = 0;


    // ----- MessageFactory inner interface ---------------------------------

    public:
        /**
        * A MessageFactory is a factory for {@link Message} objects.
        *
        * @author jh  2007.12.20
        *
        * @see Message
        */
        class COH_EXPORT MessageFactory
            : public interface_spec<MessageFactory>
            {
            // ----- MessageFactory interface ---------------------------

            public:
                /**
                * Return the Protocol for which this MessageFactory creates
                * Message objects.
                *
                * @return the Protocol associated with this MessageFactory
                */
                virtual Protocol::View getProtocol() const = 0;

                /**
                * Return the Protocol version supported by this
                * MessageFactory.
                *
                * @return the Protocol version associated with this
                *         MessageFactory
                */
                virtual int32_t getVersion() const = 0;

                /**
                * Create a new Message object of the specified type.
                *
                * @param nType  the type identifier of the Message class to
                *               instantiate
                *
                * @return the new Message object
                *
                * @throws IllegalArgumentException if the specified type is
                *         unknown to this MessageFactory
                */
                virtual Message::Handle createMessage(int32_t nType) const = 0;
            };

        /**
        * Return a MessageFactory that can be used to create Message objects
        * for the specified version of this Protocol.
        *
        * @param nVersion  the desired Protocol version
        *
        * @return a MessageFactory that can create Message objects for the
        *         specified version of this Protocol
        *
        * @throws IllegalArgumentException if the specified protocol version
        *         is not supported by this Protocol
        */
        virtual MessageFactory::View getMessageFactory(int32_t nVersion) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PROTOCOL_HPP
