/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_CHANNEL_HPP
#define COH_POF_CHANNEL_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/ThreadGate.hpp"

#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Request.hpp"



COH_OPEN_NAMESPACE3(coherence,component,util)
class Peer;
COH_CLOSE_NAMESPACE3

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::util::Peer;
using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;
using coherence::io::Serializer;
using coherence::io::pof::PofContext;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofSerializer;
using coherence::io::pof::PofWriter;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Connection;
using coherence::net::messaging::Message;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Request;
using coherence::security::auth::Subject;
using coherence::util::Describable;
using coherence::util::Map;
using coherence::util::ThreadGate;


/**
* Implementation of the Channel interface that is also a PofContext.
*
* @author jh  2008.02.06
*/
class COH_EXPORT PofChannel
    : public class_spec<PofChannel,
        extends<Describable>,
        implements<Channel, PofContext, PofSerializer> >
    {
    friend class factory<PofChannel>;

    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Peer Handle definition
        */
        typedef TypedHandle<Peer> PeerHandle;

        /**
        * Peer Handle definition
        */
        typedef TypedHandle<const Peer> PeerView;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PofChannel.
        */
        PofChannel();


    // ----- PofChannel interface -------------------------------------------

    public:
        /**
        * Close the Channel due to an exception or error.
        *
        * @param fNotify  if true, notify the peer that the Channel is being
        *                 closed
        * @param ohe      the optional reason why the Channel is being closed
        */
        virtual void close(bool fNotify, Exception::Holder ohe);

        /**
        * The close() implementation method. This method is called on the
        * service thread.
        *
        * @param fNotify  if true, notify the peer that the Channel is being
        *                 closed
        * @param ohe      the optional reason why the Channel is being closed
        * @param cMillis  the number of milliseconds to wait for the Channel
        *                 to close; pass 0 to perform a non-blocking close
        *                 or -1 to wait forever
        */
        virtual bool closeInternal(bool fNotify,
                Exception::Holder ohe, int64_t cMillis);

        /**
        * Create a new Message of the specified type using this Channel's
        * MessageFactory.
        *
        * @param nType  the type identifier of the Message to create
        *
        * @return a new Message of the specified type
        */
        virtual Message::Handle createMessage(int32_t nType) const;

        /**
        * Deserialize the next object in a POF buffer using this Channel as
        * as a PofContext.
        *
        * @param hIn  the BufferInput used to read from the POF buffer
        *
        * @return a newly deserialized object
        *
        * @throws IOException on deserialization error
        */
        virtual Object::Holder deserialize(ReadBuffer::BufferInput::Handle hIn) const;

        /**
        * Attempt to close the Channel ThreadGate.
        *
        * @param cMillis  the number of milliseconds to wait for the
        *                 ThreadGate to close; pass 0 to perform a
        *                 non-blocking close or -1 to wait forever
        *
        * @return true if the Channel ThreadGate was closed; false otherwise
        */
        virtual bool gateClose(int64_t cMillis);

        /**
        * Open the Channel ThreadGate.
        */
        virtual void gateOpen();

        /**
        * Generate and return a new unique Request identifier.
        *
        * @return the new unique Request identifier
        */
        virtual int64_t generateRequestId();

        /**
        * Enter the Connection and Channel ThreadGate (in that order).
        *
        * @throws ConnectionException if the Connection or Channel is closing
        *         or closed
        */
        virtual void gateEnter();

        /**
        * Exit the Channel and Connection ThreadGate (in that order).
        */
        virtual void gateExit();


        /**
        * Return the ConnectionManager that created this Channel or null if
        * the Channel has been closed.
        *
        * @return the ConnectionManager
        */
        virtual PeerView getConnectionManager() const;

        /**
        * Return the ConnectionManager that created this Channel or null if
        * the Channel has been closed.
        *
        * @return the ConnectionManager
        */
        virtual PeerHandle getConnectionManager();

        /**
        * Called after a Request has completed either successfully or
        * unsuccessfully.
        *
        * @param vStatus  the Status representing the asynchronous Request
        */
        virtual void onRequestCompleted(Request::Status::View vStatus);

        /**
        * Open the Channel.
        */
        virtual void open();

        /**
        * The open() implementation method. This method is called on the
        * service thread.
        */
        virtual void openInternal();

        /**
        * Called when a Message is received via this Channel. This method is
        * called on the service thread ("Channel0" Messages) or on a daemon
        * thread.
        *
        * @param hMessage
        *
        * @see Peer#send
        */
        virtual void receive(Message::Handle hMessage);

        /**
        * Serialize the next object in a POF buffer using this Channel as
        * as a PofContext.
        *
        * @param hIn  the BufferOutput used to write to the POF buffer
        *
        * @throws IOException on serialization error
        */
        virtual void serialize(WriteBuffer::BufferOutput::Handle hOut,
                Object::View v) const;


    // ----- Channel interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Connection::Handle getConnection();

        /**
        * {@inheritDoc}
        */
        virtual Connection::View getConnection() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getId() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isOpen() const;

        /**
        * {@inheritDoc}
        */
        virtual void close();

        /**
        * {@inheritDoc}
        */
        virtual Protocol::MessageFactory::View getMessageFactory() const;

        /**
        * {@inheritDoc}
        */
        virtual Serializer::View getSerializer() const;

        /**
        * {@inheritDoc}
        */
        virtual Subject::View getSubject() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getAttribute(String::View vsName) const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View getAttributes() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder setAttribute(String::View vsName,
                Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder removeAttribute(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual void send(Message::Handle hMessage);

        /**
        * {@inheritDoc}
        */
        virtual Request::Status::Handle send(Request::Handle hRequest);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder request(Request::Handle hRequest);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder request(Request::Handle hRequest, int64_t cMillis);

        /**
        * {@inheritDoc}
        */
        virtual Request::Handle getRequest(int64_t lId);

        /**
        * {@inheritDoc}
        */
        virtual Receiver::Handle getReceiver();

        /**
        * {@inheritDoc}
        */
        virtual Receiver::View getReceiver() const;


    // ----- PofContext interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual PofSerializer::View getPofSerializer(int32_t nTypeId) const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getUserTypeIdentifier(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getUserTypeIdentifier(Class::View vClass) const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getUserTypeIdentifier(String::View vsClass) const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getClassName(int32_t nTypeId) const;

        /**
        * {@inheritDoc}
        */
        virtual Class::View getClass(int32_t nTypeId) const;

        /**
        * {@inheritDoc}
        */
        virtual bool isUserType(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual bool isUserType(Class::View vClass) const;

        /**
        * {@inheritDoc}
        */
        virtual bool isUserType(String::View vsClass) const;


    // ----- PofSerializer interface ----------------------------------------

    public:
        /**
        * Deserialize the next object in a POF stream using this Channel's
        * Serializer.
        *
        * @param hIn  the PofReader used to read from the POF stream
        *
        * @return a newly deserialized object
        *
        * @throws IOException on deserialization error
        */
        virtual Object::Holder deserialize(PofReader::Handle hIn) const;

        /**
        * Serialize the next object in a POF stream using this Channel's
        * Serializer.
        *
        * @param hIn  the PofWriter used to write to the POF stream
        *
        * @throws IOException on serialization error
        */
        virtual void serialize(PofWriter::Handle hOut, Object::View vo) const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * @throws ConnectionException if the Channel is closed or closing
        */
        virtual void assertOpen() const;

        /**
        * Calculate the default timeout in milliseconds for the given Request.
        *
        * @param hRequest  the Request
        *
        * @return the default timeout for the given Request in milliseconds
        */
        virtual int64_t calculateRequestTimeout(Request::Handle hRequest); //TODO const?

        /**
        * Execute the given Message.
        *
        * @param message  the Message to execute
        */
        virtual void execute(Message::Handle hMessage);


        /**
        * @return  a new unique Request identifier for a Request sent through
        *          this Channel
        */
        virtual int64_t getRequestId();

        /**
        * Asynchronous Message send implementation.
        *
        * @param hMessage  the Message to send asynchronously
        */
        virtual void post(Message::Handle hMessage);

        /**
        * Create a Status for the given Request and register the Status in
        * the Request Map.
        *
        * @param hRequest  the Request to register; must not be NULL
        *
        * @return the new Status that represents the asynchronous Request
        */
        virtual Request::Status::Handle registerRequest(Request::Handle hRequest);

        /**
        * Unregister the given Status from the Request Map.
        *
        * @param vStatus  the Status to unregister; must not be NULL
        */
        virtual void unregisterRequest(Request::Status::View vStatus);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the Connection that created this Channel.
        *
        * @return the Connection that created this Channel
        *
        * @throws ConnectionException if the Channel has been closed
        */
        virtual PofConnection::Handle ensureConnection();

        /**
        * Return the Connection that created this Channel.
        *
        * @return the Connection that created this Channel
        *
        * @throws ConnectionException if the Channel has been closed
        */
        virtual PofConnection::View ensureConnection() const;

        /**
        * Return the ConnectionManager that created this Channel's Connection.
        *
        * @return the ConnectionManager that created this Channel's
        *         Connection
        *
        * @throws ConnectionException if the Channel has been closed
        */
        virtual PeerHandle ensureConnectionManager();

        /**
        * Return the ConnectionManager that created this Channel's Connection.
        *
        * @return the ConnectionManager that created this Channel's
        *         Connection
        *
        * @throws ConnectionException if the Channel has been closed
        */
        virtual PeerView ensureConnectionManager() const;

        /**
        * Determine if the calling thread is currently executing within
        * the Channel's ThreadGate.
        *
        * @return true if the calling thread is currently executing within
        *         the Channel's ThreadGate
        */
        virtual bool isActiveThread() const;

        /**
        * Determine if the peer should be notified when closing the Channel
        * during ThreadGate exit.
        *
        * @return true iff the peer should be notified
        *
        * @see #isCloseOnExit
        */
        virtual bool isCloseNotify() const;

        /**
        * Configure whether or not the peer should be notified when closing
        * the Channel during ThreadGate exit.
        *
        * @param f  true iff the peer should be notified
        *
        * @see #isCloseOnExit
        */
        virtual void setCloseNotify(bool f);

        /**
        * Determine if the Channel should be closed upon ThreadGate exit.
        *
        * @return true iff the Channel should be closed
        */
        virtual bool isCloseOnExit() const;

        /**
        * Configure whether or not the Channel should be closed upon
        * ThreadGate exit.
        *
        * @param f  true iff the Channel should be closed
        */
        virtual void setCloseOnExit(bool f);

        /**
        * Return the optional Exception that will be pased to the close()
        * method when the Connection is closed upon ThreadGate exit.
        *
        * @return the optional Exception
        *
        * @see #isCloseOnExit
        */
        virtual Exception::Holder getCloseException() const;

        /**
        * Configure the optional Exception that will be pased to the close()
        * method when the Connection is closed upon ThreadGate exit.
        *
        * @param ohe  the optional Exception
        *
        * @see #isCloseOnExit
        */
        virtual void setCloseException(Exception::Holder ohe);

        /**
        * Configure the Connection that created this Channel.
        *
        * @param hConnection  the Connection that created this Channel
        */
        virtual void setConnection(Connection::Handle hConnection);

        /**
        * Configure the Channel's identifier.
        *
        * @param nId  the identifier
        */
        virtual void setId(int32_t nId);

        /**
        * Configure the Channel's MessageFactory.
        *
        * @param vFactory  the MessageFactory
        */
        virtual void setMessageFactory(Protocol::MessageFactory::View vFactory);

        /**
        * Set whether the Channel is open or not.
        *
        * @param fOpen  whether the Channel is open or not
        */
        virtual void setOpen(bool fOpen);

        /**
        * Configure the Channel's Receiver.
        *
        * @param hReceiver  the Receiver
        */
        virtual void setReceiver(Receiver::Handle hReceiver);

        /**
        * Setter for property RequestId.<p>
        * A counter used to generate unique identifiers for Requests sent
        * through this Channel.
        * 
        * @param nId  the request id
        */
        virtual void setRequestId(int64_t nId);

        /**
        * Configure the Channel's Serializer.
        *
        * @param vSerializer  the Serializer
        */
        virtual void setSerializer(Serializer::View vSerializer);

        /**
        * Configure the Channel's Subject.
        *
        * @param vSubject  the Subject
        */
        virtual void setSubject(Subject::View vSubject);

        /**
        * True if either the channel subject or the proxy service subject
        * exist. When both are null, we can optimize out the Subject.doAs() call.
        *
        * @return true iff the channel or proy service Subject is null
        */
        virtual bool isSecureContext() const;

        /**
        * Set whether the channel subject or the proxy service subject
        * exist.
        *
        * @param fThreadSubjectNull  whether the thread Subject is null.
        */
        virtual void setSecureContext(bool fSecureContext);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Map used to store Channel attributes.
        */
        FinalHandle<Map> f_hMapAttributes;

        /**
        * Peer notification flag used when the Channel is closed upon
        * exiting the ThreadGate (see m_fCloseOnExit).
        */
        Volatile<bool> m_fCloseNotify;

        /**
        * If true, the Thread that is currently executing within the Channel
        * should close it immedately upon exiting the Channel's ThreadGate.
        */
        Volatile<bool> m_fCloseOnExit;

        /**
        * The Exception to pass to the close() method when the Channel is
        * closed upon exiting the ThreadGate (see m_fCloseOnExit).
        */
        FinalHolder<Exception> f_oheCloseException;

        /**
        * The Connection that created this Channel.
        */
        WeakHandle<Connection> m_whConnection;

        /**
        * The unique identifier for this Channel.
        */
        int32_t m_nId;

        /**
        * The MessageFactory used create Message objects that may be sent
        * through this Channel.
        */
        FinalView<Protocol::MessageFactory> f_vMessageFactory;

        /**
        * True if the Channel is open; false otherwise.
        */
        Volatile<bool> m_fOpen;

        /**
        * The optional Receiver that processes unsolicited Message objects
        * sent through this Channel.
        */
        MemberHandle<Receiver> m_hReceiver;

        /**
        * The list of open RequestStatus objects, indexed by Request
        * identifier.
        */
        FinalHandle<LongArray> f_hlaRequest;

        /**
        * ThreadGate protecting additions to m_hlaRequest
        */
        FinalHandle<ThreadGate> f_hGateRequest;

        /**
        * A counter used to generate unique identifiers for Requests sent
        * through this Channel.
        */
        int64_t m_nRequestId;

        /**
        * The Serializer used to serialize and deserialize payload objects
        * carried by Messages sent through this Channel.
        */
        FinalView<Serializer> f_vSerializer;

        /**
        * The optional Subject associated with the Channel.
        */
        FinalView<Subject> f_vSubject;

        /**
        * A ThreadGate used to prevent concurrent use of this Channel while
        * it is being closed.
        */
        FinalHandle<ThreadGate> f_hThreadGate;

        /**
        * True if either the channel subject or the proxy service subject
        * exist. When both are null, we can optimize out the Subject.doAs() call.
        */
        Volatile<bool> m_fSecureContext;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_POF_CHANNEL_HPP
