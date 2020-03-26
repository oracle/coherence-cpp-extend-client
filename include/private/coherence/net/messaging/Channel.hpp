/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CHANNEL_HPP
#define COH_CHANNEL_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Request.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::io::Serializer;
using coherence::security::auth::Subject;
using coherence::util::Map;

class Connection;


/**
* A Channel is a communication construct that allows one or more threads to
* send and receive {@link Message} objects via a {@link Connection}.
* <p/>
* Channel objects are created from a Connection. Once created, a Channel can
* be used to:
* <ul>
*   <li>asynchronously {@link #send send} a Message</li>
*   <li>asynchronously {@link #send(Request) send} a Request</li>
*   <li>synchronously {@link #request(Request) send} a Request</li>
*   <li>asynchronously {@link #getReceiver receive} a Message</li>
* </ul>
* Once a Channel has been {@link #close closed}, any attempt to send a
* Message using the Channel may result in an exception.
* <p/>
* All Channel implementations must be fully thread-safe.
*
* @author jh  2007.12.19
*
* @see Connection
* @see Message
* @see Request
* @see Response
*/
class COH_EXPORT Channel
    : public interface_spec<Channel>
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

        /**
        * Connection Handle definition.
        */
        typedef TypedHandle<Connection> ConnectionHandle;

        /**
        * Connection View definition.
        */
        typedef TypedHandle<const Connection> ConnectionView;


    // ----- Channel interface ----------------------------------------------

    public:
        /**
        * Return the Connection that created this Channel.
        *
        * @return the Connection that created this Channel
        */
        virtual ConnectionHandle getConnection() = 0;

        /**
        * Return the Connection that created this Channel.
        *
        * @return the Connection that created this Channel
        */
        virtual ConnectionView getConnection() const = 0;

        /**
        * Return the unique identifier for this Channel.
        * <p/>
        * The returned identifier is only unique among Channel objects
        * created from the same underlying Connection. In other words,
        * Channel objects created by different Connection objects may have
        * the same unique identifier, but Channel objects created by the same
        * Connection cannot.
        *
        * @return a unique integer identifier for this Channel
        */
        virtual int32_t getId() const = 0;

        /**
        * Return true if this Channel is open.
        *
        * @return true if this Channel is open
        */
        virtual bool isOpen() const = 0;

        /**
        * Close the Channel and reclaim all resources held by the Channel.
        * <p/>
        * When this method is invoked, it will not return until Message
        * processing has been shut down in an orderly fashion. This means
        * that the {@link Receiver} object associated with this Channel (if
        * any) have finished processing and that all pending requests are
        * completed or canceled. If the Receiver is processing a Message at
        * the time when close is invoked, all the facilities of the Channel
        * must remain available until it finishes.
        * <p/>
        * If the Channel is not open, calling this method has no effect.
        */
        virtual void close() = 0;

        /**
        * Return the MessageFactory used create Message objects that may be
        * sent through this Channel over the underlying Connection.
        *
        * @return the MessageFactory for this Channel
        */
        virtual Protocol::MessageFactory::View getMessageFactory() const = 0;

        /**
        * Return the {@link Serializer} used to serialize and deserialize
        * payload objects carried by Message objects sent through this
        * Channel.
        *
        * @return the Serializer for this Channel
        */
        virtual Serializer::View getSerializer() const = 0;

        /**
        * Return the optional Subject associated with this Channel.
        * <p/>
        * If a Subject is associated with this Channel, any operation
        * performed upon receipt of a Message sent through this Channel will
        * be done on behalf of the Subject.
        *
        * @return the Subject associated with this Channel
        */
        virtual Subject::View getSubject() const = 0;

        /**
        * Return the object bound with the specified name to this Channel, or
        * null if no object is bound with that name.
        *
        * @param vsName  the name with which the object was bound
        *
        * @return the object bound with the given name or null if no such
        *         binding exists
        */
        virtual Object::Holder getAttribute(String::View vsName) const = 0;

        /**
        * Return the map of Channel attributes.
        * <p/>
        * The keys of the map are the names with which the corresponding
        * values have been bound to the Channel.
        * <p/>
        * The client should assume that the returned map is an immutable
        * snapshot of the actual map of attribute objects maintained by this
        * Connection.
        *
        * @return a map of attributes bound to this Channel
        */
        virtual Map::View getAttributes() const = 0;

        /**
        * Bind an object to the specified name in this Channel.
        * <p/>
        * If an object is already bound to the specified name, it is replaced
        * with the given object.
        * <p/>
        * Channel attributes are local to the binding peer's Channel. In
        * other words, attributes bound to this Channel object will not be
        * bound to the peer's Channel object.
        *
        * @param vsName   the name with which to bind the object
        * @param ohValue  the object to bind
        *
        * @return the object that the newly bound object replaced (if any)
        */
        virtual Object::Holder setAttribute(String::View vsName,
                Object::Holder ohValue) = 0;

        /**
        * Unbind the object that was bound with the specified name to this
        * Channel.
        *
        * @param vsName  the name with which the object was bound
        *
        * @return the object that was unbound
        */
        virtual Object::Holder removeAttribute(String::View vsName) = 0;

        /**
        * Asynchronously send a Message to the peer endpoint through this
        * Channel over the underlying Connection.
        *
        * @param hMessage  the Message to send
        */
        virtual void send(Message::Handle hMessage) = 0;

        /**
        * Asynchronously send a Request to the peer endpoint through this
        * Channel over the underlying Connection.
        *
        * @param hRequest  the Request to send
        *
        * @return a Status object representing the asynchronous Request
        */
        virtual Request::Status::Handle send(Request::Handle hRequest) = 0;

        /**
        * Synchronously send a Request to the peer endpoint through this
        * Channel over the underlying Connection and return the result of
        * processing the Request.
        *
        * @param hRequest  the Request to send
        *
        * @return the result sent by the peer
        *
        * @throws Exception if an error or exception occurs while processing
        *                   the Request.
        * @throws Exception if the Request is cancelled, a timeout occurs, or
        *                   the waiting thread is interrupted
        */
        virtual Object::Holder request(Request::Handle hRequest) = 0;

        /**
        * Synchronously send a Request to the peer endpoint through this
        * Channel over the underlying Connection and return the result of
        * processing the Request.
        *
        * @param hRequest  the Request to send
        * @param cMillis   the number of milliseconds to wait for the result;
        *                  pass zero to block the calling thread indefinitely
        *
        * @return the result sent by the peer
        *
        * @throws Exception if an error or exception occurs while processing
        *                   the Request.
        * @throws Exception if the Request is cancelled, a timeout occurs, or
        *                   the waiting thread is interrupted
        */
        virtual Object::Holder request(Request::Handle hRequest, int64_t cMillis) = 0;

        /**
        * Return the outstanding Request with the given identifier or null if
        * no such Request exists.
        * <p/>
        * This method can be used during Response execution to correlate the
        * Response with the Request for which the Response was sent.
        *
        * @param lId  the unique identifer of the outstanding Request
        *
        * @return the outstanding Request with the given identifer or null if
        *         no such Request exists
        */
        virtual Request::Handle getRequest(int64_t lId) = 0;


    // ----- Receiver inner interface ---------------------------------------

    public:
        /**
        * A Receiver processes unsolicited Message objects sent via any
        * number of Channel objects.
        * <p/>
        * A Receiver acts as a server-side proxy, in that it can be
        * registered with a ConnectionAcceptor, it can be looked up, and
        * Channels from multiple clients can be established to it. In this
        * sense, the Receiver represents server-side state shared across any
        * number of client Channels, and thus provides an efficient mechanism
        * for demultiplexing multi-client communication into a shared service
        * proxy, and locating state that is shared across all of those client
        * Channels. Conversely, the Channel object represents client-specific
        * state, allowing per- client information to be efficiently managed
        * on the server side.
        * <p/>
        * While the Receiver is particularly useful as a server-side proxy,
        * it is also useful on the client, allowing a client to publish named
        * services to a server, and in the case of both named services and
        * any other Channels created by a client, it allows a client to
        * efficiently manage stateful communication and process unsolicited
        * Message objects.
        *
        * @author jh 2007.12.19
        */
        class COH_EXPORT Receiver
            : public interface_spec<Receiver>
            {
            // ----- Receiver interface ----------------------------------

            public:
                /**
                * Return the name of this Receiver.
                *
                * If the Receiver is registered with a ConnectionManager, the
                * registration and any subsequent accesses are by the
                * Receiver's name, meaning that the name must be unique
                * within the domain of the ConnectionManager.
                *
                * @return the Receiver name
                */
                virtual String::View getName() const = 0;

                /**
                * The Protocol understood by the Receiver.
                *
                * Only Channel objects with the specified Protocol can be
                * registered with this Receiver.
                *
                * @return the Protocol used by this Receiver
                */
                virtual Protocol::View getProtocol() const = 0;

                /**
                * Notify this Receiver that it has been associated with a
                * Channel. This method is invoked by the Channel when a
                * Receiver is associated with the Channel.
                *
                * Once registered, the Receiver will receive all unsolicited
                * Message objects sent through the Channel until the Channel
                * is unregistered or closed. Without a Receiver, the
                * unsolicited Message objects are executed with only a
                * Channel as context; with a Receiver, the Receiver is given
                * the Message to process, and may execute the Message in turn.
                *
                * @param hChannel  a Channel that has been associated with
                *                 this Receiver
                */
                virtual void registerChannel(Channel::Handle hChannel) = 0;

                /**
                * Called when an unsolicited (non-Response) Message is
                * received by a Channel that had been previously registered
                * with this Receiver.
                *
                * @param message  an unsolicited Message received by a
                *                 registered Channel
                */
                virtual void onMessage(Message::Handle hMessage) = 0;

                /**
                * Unregister the given Channel with this Receiver. This
                * method is invoked by the Channel when a Receiver is
                * disassociated with the Channel.
                *
                * Once unregistered, the Receiver will no longer receive
                * unsolicited Message objects sent through the Channel.
                *
                * @param hChannel  a Channel that was disassociated with this
                *                  Receiver
                */
                virtual void unregisterChannel(Channel::Handle hChannel) = 0;

                /**
                * Notify this Receiver that the Channel it was associated with has
                * been closed.
                *
                * This method may be invoked after the Receiver has been unregistered.
                *
                * The default implementation, provided for backwards compatibility,
                * is a no-op.
                *
                * @param channel  a Channel that has been associated with this
                *                 Receiver
                *
                *  @since 12.2.1.2.0
                */
                virtual void onChannelClosed(Channel::Handle /* hChannel */)
                    {
                    // no-op
                    }
            };

        /**
        * Return the optional Receiver that processes unsolicited Message
        * objects sent through this Channel over the underlying Connection.
        *
        * @return the Receiver for this Channel or null if a Receiver has not
        *         been associated with this Channel
        */
        virtual Receiver::Handle getReceiver() = 0;

        /**
        * Return the optional Receiver that processes unsolicited Message
        * objects sent through this Channel over the underlying Connection.
        *
        * @return the Receiver for this Channel or null if a Receiver has not
        *         been associated with this Channel
        */
        virtual Receiver::View getReceiver() const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CHANNEL_HPP
