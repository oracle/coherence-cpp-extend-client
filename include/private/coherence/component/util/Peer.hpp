/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PEER_HPP
#define COH_PEER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/UUID.hpp"

#include "private/coherence/component/util/Service.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/component/net/extend/PofChannel.hpp"
#include "private/coherence/net/URI.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Codec.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionAcceptor.hpp"
#include "private/coherence/net/messaging/ConnectionEvent.hpp"
#include "private/coherence/net/messaging/ConnectionListener.hpp"
#include "private/coherence/net/messaging/ConnectionManager.hpp"
#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::net::extend::PofChannel;
using coherence::component::net::extend::PofConnection;
using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::net::OperationalContext;
using coherence::net::URI;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Codec;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionAcceptor;
using coherence::net::messaging::ConnectionEvent;
using coherence::net::messaging::ConnectionListener;
using coherence::net::messaging::ConnectionManager;
using coherence::net::messaging::Message;
using coherence::net::messaging::Protocol;
using coherence::run::xml::XmlElement;
using coherence::security::auth::Subject;
using coherence::util::HashMap;
using coherence::util::List;
using coherence::util::Map;
using coherence::util::UUID;


/**
* Base definition of a ConnectionManager.
*
* A ConnectionManager has a service thread, an optional execute thread pool,
* and a ConnectionEvent dispatcher thread.
*
* Concrete implementations must implement the abstract send() method using the
* underlying transport. Additionally, the underlying transport must call the
* receive() or post() method when a Message is received over the underlying
* transport.
*
* @author nsa 2008.03.10
*/
class COH_EXPORT Peer
    : public abstract_spec<Peer,
        extends<coherence::component::util::Service>,
        implements<ConnectionManager, Channel::Receiver> >
    {
    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * @internal
        */
        Peer();

    private:
        /**
        * Blocked copy constructor.
        */
        Peer(const Peer&);
		

    // ----- Peer interface -------------------------------------------------

    public:
        /**
        * Accept a new Channel. This method is called by
        * Connection->acceptChannel() and is always run on client threads.
        *
        * @param hConnection  the Connection used to accept channel
        * @param hURI         the URI that identifies the Channel
        * @param hReceiver    the Receiver tha the Channel will register with
        * @param vSubject     the identity under which Messages received by
        *                     the new Channel will be executed.
        *
        * @see coherence::net::messaging::Connection::acceptChannel
        */
        virtual Channel::Handle acceptChannel(PofConnection::Handle hConnection,
                URI::View vUri, Channel::Receiver::Handle hReceiver,
                Subject::View vSubject);

        /**
        * Close the given Channel. This method is called by Channel::close()
        * and is always run on client threads.
        *
        * @param hChannel  the Channel to close
        * @param fNotify   if true, notify the peer that the Channel is being
        *                  closed
        * @param ohe       the optional reason why the Channel is being closed
        * @param fWait     if true, wait for the Channel to close before
        *                  returning
        */
        virtual void closeChannel(PofChannel::Handle hChannel, bool fNotify,
                Exception::Holder ohe, bool fWait = true);

        /**
        * Close the given Connection. This method is called by
        * Connection::close() and is always run on client threads.
        *
        * @param hChannel  the Connection to close
        * @param fNotify   if true, notify the peer that the Connection is
        *                  being closed
        * @param ohe       the optional reason why the Connection is being
        *                  closed
        * @param fWait     if true, wait for the Connection to close before
        *                  returning
        */
        virtual void closeConnection(PofConnection::Handle hConnection,
                bool fNotify, Exception::Holder ohe, bool fWait = true);

        /**
        * Create a new Channel. This method is called by
        * Connection::createChannel() and is always run on client threads.
        *
        * @param hConnection  the connection the new channel should use
        * @param hProtocol    the protocol the new Channel should use
        * @param hReceiver    the receiver the new Channel should use
        *
        * @see coherence::net::messaging::Connection::createChannel
        */
        virtual URI::Handle createChannel(PofConnection::Handle hConnection,
                Protocol::View hProtocol, Channel::Receiver::Handle hReceiver);

        /**
        * Validate a token in order to establish a user's identity.
        *
        * @param vToken  an identity assertion, a statement that asserts an
        *                identity
        *
        * @return a Subject representing the identity
        *
        * @throws SecurityException on assertion error
        */
        virtual Subject::View assertIdentityToken(Object::View vToken);

        /**
        * Transform a Subject to a token that asserts identity.
        *
        * @param vSubject  the Subject to transform
        *
        * @return a token that asserts identity
        *
        * @throws SecurityException on assertion error
        */
        virtual Object::View generateIdentityToken(Subject::View vSubject);

        /**
        * Deserialize the identity token.
        *
        * @param vabToken  the identity assertion token
        *
        * @return the deserialized identity token
        *
        * @throws SecurityException on deserialization error
        */
        virtual Object::View deserializeIdentityToken(Array<octet_t>::View vabToken);

        /**
        * Serialize the identity token.
        *
        * @param vToken  the token to serialize
        *
        * @return a serialized identity token
        *
        * @throws SecurityException on serialization error
        */
        virtual Array<octet_t>::View serializeIdentityToken(Object::View vToken);

        /**
        * Factory method: create a new Connection.
        *
        * Implementations must configure the Connection with a reference to
        * this ConnectionManager (see Connection::setConnectionManager).
        *
        * @return a new Connection object that has yet to be opened
        */
        virtual PofConnection::Handle instantiateConnection() = 0;

        /**
        * Called after a Channel has been opened. This method is called on
        * the service thread.
        *
        * @param hChannel  the Channel that has been opened
        */
        virtual void onChannelOpened(PofChannel::Handle hChannel);

        /**
        * Called after a Connection has closed. This method is called on the
        * service thread.
        *
        * @param hConnection  the Connection that was closed
        */
        virtual void onConnectionClosed(PofConnection::Handle hConnection);

        /**
        * Called after a Connection is closed due to an error or exception.
        * This method is called on the service thread.
        *
        * @param hConnection  the Connection that was closed
        * @param ohe          the reason the Connection was closed
        */
        virtual void onConnectionError(PofConnection::Handle hConnection,
                Exception::Holder ohe);

        /**
        * Called after a Connection has been successfully established. This
        * method is called on the service thread.
        *
        * @param hConnection  the Connection that was opened
        */
        virtual void onConnectionOpened(PofConnection::Handle hConnection);

        /**
        * Open a new Channel. This method is called by
        * Connection::openChannel() and is always run on client threads.
        *
        * @param hConnection  the connection to be used by the opened channel
        * @param hProtocol    the protocol to be used by the Channel
        * @param vsName       the name of the Channel
        * @param hReceiver    the Receiver used by the Channel
        * @param vSubject     the identity under which Messages for the
        *                     channel should be executed
        *
        * @see coherence::net::messaging::Connection::openChannel
        */
        virtual PofChannel::Handle openChannel(PofConnection::Handle hConnection,
                Protocol::View hProtocol, String::View vsName,
                Channel::Receiver::Handle hReceiver, Subject::View vSubject);

        /**
        * Open the given Connection. This method is called by
        * Connection::open() and is always run on client threads.
        *
        * @param hConnection  the Connection to open
        */
        virtual void openConnection(PofConnection::Handle hConnection);

        /**
        * Handle the given Message by either adding it to the service thread
        * queue (internal Messages) or sending asynchronously (external
        * Messages). This method is called on both client and service
        * threads.
        *
        * @param hMessage  the Message to post
        */
        virtual void post(Message::Handle hMessage);

        /**
        * Called by the underlying transport when an encoded Message is
        * received. Called on client threads.
        *
        * @param hrb          the ReadBuffer that contains the encoded Message
        * @param hConnection  the Connection that received the encoded Message
        */
        virtual void receive(ReadBuffer::View vrb, PofConnection::Handle hConnection);

        /**
        * Register a Protocol with this Service
        *
        * @param hProtocol  the Protocol to register
        */
        virtual void registerProtocol(Protocol::View hProtocol);

        /**
        * Register a Receiver with this Service
        *
        * @param hReceiver  the Receiver
        */
        virtual void registerReceiver(Channel::Receiver::Handle hReceiver);

        /**
        * Remove a ConnectionListener from this Service.
        *
        * @param hListener  the ConnectionListener to remove
        */
        virtual void removeConnectionListener(ConnectionListener::Handle hListener);

        /**
        * Stop the Service. The default implemenation of this method simply
        * calls stop().
        */
        virtual void shutdown();


    protected:
        /**
        * Allocate a WriteBuffer for encoding a Message. After the Message
        * has been encoded and the resulting ReadBuffer sent via the
        * Connection, the WriteBuffer is released via the
        * releaseWriteBuffer() method.
        *
        * This method is called on both client and service threads.
        *
        * @return a WriteBuffer that can be used to encode a Message
        */
        virtual WriteBuffer::Handle allocateWriteBuffer() const;

        /**
        * Check the given Connection for a ping timeout. A Connection that
        * has not received a PingResponse for an oustanding PingRequest
        * within the configured PingTimeout will be closed.
        *
        * @param hConnection  the Connection to check
        */
        virtual void checkPingTimeout(PofConnection::Handle hConnection) const;

        /**
        * Check the Connection(s) managed by this ConnectionManager for a
        * ping timeout. A Connection that has not received a PingResponse for
        * an oustanding PingRequest within the configured PingTimeout will be
        * closed.
        */
        virtual void checkPingTimeouts() = 0;

        /**
        * Decode the given BufferInput with the configured Codec and return a
        * new decoded Message. This method is called on either the service
        * thread (see EncodedMessage) or a client (I/O) thread.
        *
        * @param hIn         the BufferInput that contains an encoded Message
        * @param hConnection  the Connection that received the Message
        * @param fFilter     if true, the BufferInput will be filtered using
        *                    the list of configured WrapperStreamFactory
        *                    objects
        *
        * @return the decoded Message or null if the Message was sent via an
        *         unknown Channel
        *
        * @throws IOException on I/O error decoding the Message
        */
        virtual Message::Handle decodeMessage(ReadBuffer::BufferInput::Handle hIn,
                PofConnection::Handle hConnection, bool fFilter);

        /**
        * Dispatch a ConnectionEvent to the EventDispatcher.
        *
        * @param hConnection  the Connection associated with the
        *                     ConnectionEvent
        * @param nEvent       the ID of the ConnectionEvent
        * @param ohe          the optional Throwable associated with the
        *                     ConnectionEvent
        */
        virtual void dispatchConnectionEvent(Connection::Handle hConnection,
                ConnectionEvent::Id nEvent, Exception::Holder ohe);

        /**
        * Encode the given Message with the configured Codec into the specified BufferOutput.
        *
        * @param hMessage  the Message to encode
        * @param hbo       the BufferOutput that will be used to write out the
        *                  encoded Message
        * @param fFilter  if true, the BufferOutput will be filtered using
        *                 the list of configured WrapperStreamFactory objects
        *
        * @throws IOException on encoding error
        */
        virtual void encodeMessage(Message::Handle hMessage,
                WriteBuffer::BufferOutput::Handle hbo, bool fFilter);

        /**
        * Enforce the message size limit of an incoming message.
        *
        * @param cbSize  the Message size
        *
        * @throws IOException if the message size exceeds the limit
        *
        * @since Coherence 12.1.3
        */
        virtual void enforceMaxIncomingMessageSize(int64_t cbSize);

        /**
        * Enforce the message size limit of an outgoing message.
        *
        * @param cbSize  the Message size
        *
        * @throws IOException if the message size exceeds the limit
        *
        * @since Coherence 12.1.3
        */
        virtual void enforceMaxOutgoingMessageSize(int64_t cbSize);

        /**
        * Filter the given BufferInput using the configured List of
        * WrapperStreamFactory objects. If the List of WrapperStreamFactory
        * objects is null or empty, the given BufferInput is returned.
        *
        * @param hIn  the BufferInput to filter
        *
        * @return a filtered BufferInput
        */
        virtual ReadBuffer::BufferInput::Handle filterBufferInput(
                ReadBuffer::BufferInput::Handle hIn) const;

        /**
        * Filter the given BufferOutput using the configured List of
        * WrapperStreamFactory objects. If the List of WrapperStreamFactory
        * objects is null or empty, the given BufferOutput is returned.
        *
        * @param hOut  the BufferOutput to filter
        *
        * @return a filtered BufferOutput
        */
        virtual WriteBuffer::BufferOutput::Handle filterBufferOutput(
                WriteBuffer::BufferOutput::Handle hOut) const;

        /**
        * Called when an exception occurs during Message decoding. This
        * method is called on the service thread.
        *
        * @param ohe         the Throwable thrown during decoding
        * @param in          the BufferInput that contains the encoded
        *                    Message
        * @param connection  the Connection that received the encoded Message
        * @param fFilter     true iff the BufferInput was filtered using the
        *                    list of configured WrapperStreamFactory objects
        *
        * @see onNotify
        */
        virtual void onMessageDecodeException(Exception::Holder ohe,
                ReadBuffer::BufferInput::Handle hIn, PofConnection::Handle hConnection,
                bool fFilter);

        /**
        * Called when an exception occurs during Message encoding. This
        * method is called on both client and service threads.
        *
        * @param ohe       the Throwable thrown during encoding
        * @param hMessage  the Message being encoded\
        *
        * @see send
        */
        virtual void onMessageEncodeException(Exception::Holder ohe,
                Message::Handle hMessage);

        /**
        * {@inheritDoc}
        */
        virtual void onNotify();

        /**
        * Ping the Connection(s) managed by this ConnectionManager.
        */
        virtual void ping() = 0;

        /**
        * Release a ReadBuffer that contains an encode a Message. This method
        * is called on the service thread.
        *
        * @param hrb  the ReadBuffer to release
        */
        virtual void releaseReadBuffer(ReadBuffer::View vrb);

        /**
        * Release a WriteBuffer that was used to encode a Message. This
        * method is called on both client and service threads.
        *
        * @param hwb  the WriteBuffer to release
        * @param ohe  if not null, the WriteBuffer is being released due to a decoding or send error
        *
        * @see #allocateWriteBufer
        */
        virtual void releaseWriteBuffer(WriteBuffer::Handle hwb, Exception::Holder ohe);

        /**
        * Parse the String value of the child XmlElement with the given name
        * as a memory size in bytes. If the specified child XmlElement does
        * not exist or is empty, the specified default value is returned.
        *
        * @param vXml  the parent XmlElement
        * @param vsName  the name of the child XmlElement
        * @param cbDefault  the default value
        *
        * @return the memory size (in bytes) represented by the specified
        *         child XmlElement
        */
        static int64_t parseMemorySize(XmlElement::View vXml,
                String::View vsName, int64_t cbDefault);

        /**
        * Perform an asynchronous send of the given Message using the
        * underlying transport. This method is called on both client and
        * service threads.
        *
        * @param hMessage the Message to send
        *
        * @see Channel::post and Channel::receive
        */
        virtual void send(Message::Handle hMessage);


    // ----- Channel::Receiver interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getName() const;

        /**
        * {@inheritDoc}
        */
        virtual Protocol::View getProtocol() const;

        /**
        * {@inheritDoc}
        */
        virtual void registerChannel(Channel::Handle hChannel);

        /**
        * Called on the service thread.
        */
        virtual void onMessage(Message::Handle hMessage);

        /**
        * {@inheritDoc}
        */
        virtual void unregisterChannel(Channel::Handle hChannel);

        /**
        * {@inheritDoc}
        *
        *  When called as the ConnectionManager for the Channel, this method will
        *  be called on the service thread.
        */
        virtual void onChannelClosed(Channel::Handle hChannel);

    // ----- ConnectionManager interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void addConnectionListener(ConnectionListener::Handle hListeners);

        /**
        * {@inheritDoc}
        */
        virtual Channel::Receiver::View getReceiver(String::View vsReceiverName) const;

        /**
        * {@inheritDoc}
        */
        virtual Channel::Receiver::Handle getReceiver(String::View vsReceiverName);

        /**
        * {@inheritDoc}
        */
        virtual Map::View getReceivers() const;

        /**
        * {@inheritDoc}
        */
        virtual Protocol::View getProtocol(String::View vsName) const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View getProtocols() const;


    // ----- Service interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View formatStats() const;

        /**
        * {@inheritDoc}
        */
        virtual void onServiceStarted();

        /**
        * {@inheritDoc}
        */
        virtual void resetStats();

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onEnter();

        /**
        * {@inheritDoc}
        */
        virtual void onExit();

        /**
        * {@inheritDoc}
        */
        virtual void onServiceStarting();

        /**
        * {@inheritDoc}
        */
        virtual void onServiceStopped();


    // ----- XmlConfigurable interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void configure(XmlElement::View vXml);


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Return the OperationalContext for this Service.
        *
        * @return the OperationalContext for this Service
        *
        * @since Coherence 3.7
        */
        virtual OperationalContext::View getOperationalContext() const;

        /**
        * Set the OperationalContext for this Service.
        *
        * @param vContext  the OperationalContext for this Service
        *
        * @throws IllegalStateException if an OperationalContext was already
        *         set
        *
        * @since Coherence 3.7
        */
        virtual void setOperationalContext(OperationalContext::View vContext);

        /**
        * Return the parent service.
        *
        * @return the parent service for this Peer
        *
        * @since Coherence 3.7
        */
        virtual coherence::net::Service::View getParentService() const;

        /**
        * Set the parent service.
        *
        * @param vService  the parent service for this Peer
        *
        * @since Coherence 3.7
        */
        virtual void setParentService(coherence::net::Service::View vService);

        /**
        * Return the channel used for internal communication.
        *
        * @return the channel used for internal communication
        */
        virtual PofChannel::Handle getChannel();

        /**
        * Return the channel used for internal communication.
        *
        * @return the channel used for internal communication
        */
        virtual PofChannel::View getChannel() const;

        /**
        * Return the connection used for internal communication.
        *
        * @return the connection used for internal communication
        */
        virtual PofConnection::Handle get_Connection();

        /**
        * Return the connection used for internal communication.
        *
        * @return the connection used for internal communication
        */
        virtual PofConnection::View get_Connection() const;

        /**
        * Return the codec used for encoding and decoding messages.
        *
        * @return the codec used for encoding and decoding messages
        */
        virtual Codec::Handle getCodec();

        /**
        * Return the codec used for encoding and decoding messages.
        *
        * @return the codec used for encoding and decoding messages
        */
        virtual Codec::View getCodec() const;

        /**
        * Set the codec used for encoding and decoding messages.
        *
        * @param hCodec  the codec used for encoding and decoding messages
        */
        virtual void setCodec(Codec::Handle hCodec);

        /**
        * Return the connection listeners for the service.
        *
        * @return the connection listeners for the service
        */
        virtual Listeners::Handle getConnectionListeners();

        /**
        * Return the number of milliseconds between successive Connection
        * "pings".
        *
        * @return the number of milliseconds between successive Connection
        *         "pings" or 0 if heartbeats are disabled.
        */
        virtual int64_t getPingInterval() const;

        /**
        * Return the last time the Connection(s) managed by this
        * ConnectionManager were checked for a "ping" timeout.
        *
        * @return the last time the Connection(s) managed by this
        *         ConnectionManager were "pinged".
        */
        virtual int64_t getPingLastCheckMillis() const;

        /**
        * Return the last time the Connection(s) managed by this
        * ConnectionManager were "pinged".
        *
        * @return the last time the Connection(s) managed by this
        *         ConnectionManager were "pinged"
        */
        virtual int64_t getPingLastMillis() const;

        /**
        * Return the next time the Connection(s) managed by this
        * ConnectionManager should be checked for a "ping" timeout.
        *
        * @return the next time the Connection(s) managed by this
        *         ConnectionManager should be checked for a "ping" timeout.
        */
        virtual int64_t getPingNextCheckMillis() const;

        /**
        * Return the next time the Connection(s) managed by this
        * ConnectionManager should be "pinged".
        *
        * @return the next time the Connection(s) managed by this
        *         ConnectionManager should be "pinged".
        */
        virtual int64_t getPingNextMillis() const;

        /**
        * Return the default request timeout for a PingRequest. A timeout of
        * 0 is interpreted as an infinite timeout. This property defaults to
        * the value of the RequestTimeout property.
        *
        * @return the default request timeout for a PingRequest
        */
        virtual int64_t getPingTimeout() const;

        /**
        * Return the unique identifier (UUID) of the process using this
        * ConnectionManager.
        *
        * @return The unique identifier (UUID) of the process using this
        *         ConnectionManager
        */
        static UUID::View getProcessId();

        /**
        * Return the Map of registered Protocol objects.
        *
        * @return the Map of registered Protocol objects
        */
        virtual Map::View getProtocolMap() const;

        /**
        * Return the Map of registered Protocol objects.
        *
        * @return the Map of registered Protocol objects
        */
        virtual Map::Handle getProtocolMap();

        /**
        * Return the Map of Protocol version ranges that can be used to
        * create a Map of MessageFactory objects that may be used by the
        * Connection object created by this ConnectionManager.
        */
        virtual Map::View getProtocolVersionMap() const;

        /**
        * Return the Map of registered Receiver objects.
        *
        * @return the Map of registered Receiver objects
        */
        virtual Map::View getReceiverMap() const;

        /**
        * Return the Map of registered Receiver objects.
        *
        * @return the Map of registered Receiver objects
        */
        virtual Map::Handle getReceiverMap();

        /**
        * Return the default request timeout for all Channel objects created
        * by Connection objects managed by this ConnectionManager. A timeout
        * of 0 is interpreted as an infinite timeout.
        *
        * @return the default request timeout for all Channel objects created
        *         by Connection objects managed by this ConnectionManager
        */
        virtual int64_t getRequestTimeout() const;

        /**
        * Return the maximum size allowed for an incoming message. A value of 0
        * is interpreted as unlimited size.
        *
        * @return the maximum size of a message
        *
        * @since Coherence 12.1.3
        */
        virtual int64_t getMaxIncomingMessageSize() const;

        /**
        * Return the maximum size allowed for an outgoing message. A value of 0
        * is interpreted as unlimited size.
        *
        * @return the maximum size of a message
        *
        * @since Coherence 12.1.3
        */
        virtual int64_t getMaxOutgoingMessageSize() const;

        /**
        * Return the total number of bytes received.
        */
        virtual int64_t getStatsBytesReceived() const;

        /**
        * Return the total number of bytes sent.
        *
        * @return the total number of bytes sent
        */
        virtual int64_t getStatsBytesSent() const;

        /**
        * Return the total number of messages sent.
        *
        * @return the total number of messages sent
        */
        virtual int64_t getStatsSent() const;

        /**
        * Return the total number of timed-out requests since the last time
        * the statistics were reset.
        *
        * @return the total number of timed-out requests since the last time
        *         the statistics were reset
        */
        virtual int64_t getStatsTimeoutCount() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getWaitMillis() const;

        /**
        * Return a List of WrapperStreamFactory objects that affect how
        * Messages are written and read.
        *
        * @return a List of WrapperStreamFactory objects that affect how
        * Messages are written and read
        */
        virtual List::View getWrapperStreamFactoryList() const;


    protected:
        /**
        * Return The MessageFactory used to create Messages processed by
        * this Service.
        *
        * @return the MessageFactory used to create Messages processed by
        * this Service.
        */
        virtual Protocol::MessageFactory::View getMessageFactory();

        /**
        * Set the connection used for internal communication.
        *
        * @param hConnection  the connection used for internal communication
        */
        virtual void set_Connection(PofConnection::Handle hConnection);

        /**
        * Set the channel used for internal communication.
        *
        * @param hChannel  the channel to use for internal communication
        */
        virtual void setChannel(PofChannel::Handle hChannel);

        /**
        * Set the number of milliseconds between successive Connection
        * "pings".
        *
        * @param cMillis  the number of milliseconds between successive
        *                 Connection "pings" or 0 if heartbeats are disabled.
        */
        virtual void setPingInterval(int64_t cMillis);

        /**
        * Set the last time the Connection(s) managed by this
        * ConnectionManager were checked for a "ping" timeout.
        *
        * @param ldt  the last time the Connection(s) managed by this
        *             ConnectionManager were "pinged".
        */
        virtual void setPingLastCheckMillis(int64_t ldt);

        /**
        * Set the last time the Connection(s) managed by this
        * ConnectionManager were "pinged".
        *
        * @param ldt  the last time the Connection(s) managed by this
        *             ConnectionManager were "pinged"
        */
        virtual void setPingLastMillis(int64_t ldt);

        /**
        * Set the default request timeout for a PingRequest. A timeout of 0
        * is interpreted as an infinite timeout. This property defaults to
        * the value of the RequestTimeout property.
        *
        * @param cMillis  the default request timeout for a PingRequest
        */
        virtual void setPingTimeout(int64_t cMillis);

        /**
        * Set the protocol used by this connection.
        *
        * @param hProtocol  the protocol used by this connection
        */
        virtual void setProtocol(Protocol::View hProtocol);

        /**
        * Set the default request timeout for all Channel objects created by
        * Connection objects managed by this ConnectionManager. A timeout of
        * 0 is interpreted as an infinite timeout.
        *
        * @param cMillis  the default request timeout for all Channel objects
        *                 created by Connection objects managed by this
        *                 ConnectionManager
        */
        virtual void setRequestTimeout(int64_t cMillis);

        /**
        * Set the maximum incoming message size.
        *
        * @param cbMax  the maximum size
        *
        * @since Coherence 12.1.3
        */
        virtual void setMaxIncomingMessageSize(int64_t cbMax);

        /**
        * Set the maximum outgoing message size.
        *
        * @param cbMax  the maximum size
        *
        * @since Coherence 12.1.3
        */
        virtual void setMaxOutgoingMessageSize(int64_t cbMax);

        /**
        * Return the total number of bytes received.
        */
        virtual void setStatsBytesReceived(int64_t cb);

        /**
        * Return the total number of bytes sent.
        *
        * @return the total number of bytes sent
        */
        virtual void setStatsBytesSent(int64_t cb);

        /**
        * Set the total number of messages sent.
        *
        * @param pStatsSent  the total number of messages sent
        */
        virtual void setStatsSent(int64_t pStatsSent);

        /**
        * Set the total number of timed-out requests since the last time the
        * statistics were reset.
        *
        * @param cRequests  the total number of timed-out requests since the
        *                   last time the statistics were rest
        */
        virtual void setStatsTimeoutCount(int64_t cRequests);

        /**
        * Set a List of WrapperStreamFactory objects that affect how
        * Messages are written and read.
        *
        * @param hList  a List of WrapperStreamFactory objects that affect how
        * Messages are written and read
        */
        virtual void setWrapperStreamFactoryList(List::Handle hList);


    private:
        /**
        * Set the Map of Protocol version ranges that can be used to
        * create a Map of MessageFactory objects that may be used by the
        * Connection object created by this ConnectionManager.
        *
        * @param vMap  The map of Protocol version objects.
        */
        virtual void setProtocolVersionMap(Map::View vMap);


    // ----- nested class: DispatchEvent --------------------------------

    public:
        /**
        * Runnable event.
        */
        class COH_EXPORT DispatchEvent
            : public class_spec<DispatchEvent,
                extends<coherence::component::util::Service::DispatchEvent> >
            {
            friend class factory<DispatchEvent>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new DispatchEvent instance.
                */
                DispatchEvent();

            private:
                /**
                * Blocked copy constructor.
                */
                DispatchEvent(const DispatchEvent&);


            // ----- DispatchEvent interface ----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void run();
            };


    // ----- Peer child component factory -----------------------------------

    protected:

        /**
        * Factory pattern: create a DispatchEvent instance.
        *
        * @return the new DispatchEvent
        */
        virtual coherence::component::util::Service::DispatchEvent::Handle
                instantiateDispatchEvent() const;

        /**
        * Factory pattern: create a Protocol instance.
        *
        * @return the new Protocol
        */
        virtual Protocol::View instantiateProtocol() const;

        /**
        * Ensure that a Protocol object has been initialized for this
        * Component.
        */
        virtual Protocol::View ensureInitializedProtocol();


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The Channel used for all internal communication.
        */
        FinalHandle<PofChannel> f_hChannel;

        /**
        * The Connection used for all internal communication.
        */
        FinalHandle<PofConnection> f_hConnection;

        /**
        * The Codec used to encode and decode all Messages sent by this
        * ConnectionManager.
        */
        FinalHandle<Codec> f_hCodec;

        /**
        * The list of registered ConnectionListeners.
        */
        FinalHandle<Listeners> f_hConnectionListeners;

        /**
        * The number of milliseconds between successive Connection "pings" or 0
        * if heartbeats are disabled.
        */
        int64_t m_cPingInterval;

        /**
        * The last time the Connection(s) managed by this ConnectionManager were
        * checked for a "ping" timeout.
        */
        int64_t m_ldtPingLastCheckMillis;

        /**
        * The last time the Connection(s) managed by this ConnectionManager were
        * "pinged".
        */
        int64_t m_ldtPingLastMillis;

        /**
        * The default request timeout for a PingRequest. A timeout of 0 is
        * interpreted as an infinite timeout. This property defaults to the
        * value of the RequestTimeout property.
        */
        int64_t m_cPingTimeout;

        /**
        * The protocol used for this connection.
        */
        FinalView<Protocol> f_vProtocol;

        /**
        * The Map of registered Protocol objects.
        * @see ConnectionManager::getProtocol
        */
        FinalHandle<Map> f_hProtocolMap;

        /**
        * A Map of version ranges for registered Protocols. The keys are the
        * names of the Protocols and the values are two element Integer32
        * arrays, the first element being the current version and the second
        * being the supported version of the corresponding Protocol.
        */
        FinalView<Map> f_vProtocolVersionMap;

        /**
        * The Map of registered Receiver objects.
        *
        * @see ConnectionManager::getRecievers
        */
        FinalHandle<Map> f_hReceiverMap;

        /**
        * The default request timeout for all Channel objects created by
        * Connection objects managed by this ConnectionManager. A timeout of
        * 0 is interpreted as an infinite timeout.
        *
        * @see Connection::getRequestTimeout
        */
        int64_t m_cRequestTimeout;

        /**
        * The maximum incoming message size.
        *
        * @see Connection::getMaxIncomingMessageSize
        */
        int64_t m_cbMaxIncomingMessageSize;

        /**
        * The maximum outgoing message size.
        *
        * @see Connection::getMaxOutgoingMessageSize
        */
        int64_t m_cbMaxOutgoingMessageSize;

        /**
        * Statistics: total number of bytes received.
        */
        int64_t m_cStatsBytesReceived;

        /**
        * Statistics: total number of bytes sent.
        */
        int64_t m_cStatsBytesSent;

        /**
        * Statistics: total number of messages sent.
        */
        int64_t m_cStatsSent;

        /**
        * The total number of timed-out requests since the last time the
        * statistics were reset.
        */
        Volatile<int64_t> m_cStatsTimeoutCount;

        /**
        * A List of WrapperStreamFactory objects that affect how Messages are
        * written and read.
        */
        FinalHandle<List> f_hWrapperStreamFactoryList;

        /**
        * The parent Service that instantiated this Peer.
        *
        * @see #assertIdentityToken
        * @see #generateIdentityToken
        */
        WeakView<coherence::net::Service> m_wvParentService;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Debug flag. The value of this flag is set using the following
        * system property: tangosol.coherence.messaging.debug
        */
        static const bool debug;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PEER_HPP
