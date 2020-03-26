/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_CONNECTION_HPP
#define COH_ABSTRACT_CONNECTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/net/Member.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/ThreadGate.hpp"
#include "coherence/util/UUID.hpp"

#include "private/coherence/net/URI.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionManager.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Request.hpp"



COH_OPEN_NAMESPACE3(coherence,component,util)
class Peer;
COH_CLOSE_NAMESPACE3

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::util::Peer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::net::Member;
using coherence::net::URI;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionManager;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Request;
using coherence::security::auth::Subject;
using coherence::util::Collection;
using coherence::util::Describable;
using coherence::util::LongArray;
using coherence::util::Map;
using coherence::util::ThreadGate;
using coherence::util::UUID;


/**
* Abstract implementation of the Connection interface.
*
* @author jh  2008.02.07
*/
class COH_EXPORT PofConnection
    : public class_spec<PofConnection,
        extends<Describable>,
        implements<Connection> >
    {
    friend class factory<PofConnection>;

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
        * Create a new PofConnection.
        */
        PofConnection();


    // ----- PofConnection interface -----------------------------------

    public:
        /**
        * The acceptChannel() implementation method. This method is called
        * on the service thread.
        *
        * @param vUri         the URI
        * @param vSerializer  the Serializer
        * @param hReceiver    the Receiver
        * @param vSubject     the Subject
        * @param vab          the identity token
        *
        * @return the accepted Channel
        */
        virtual Request::Status::Handle acceptChannelInternal(URI::View vUri,
                Serializer::View vSerializer,
                Channel::Receiver::Handle hReceiver,
                Subject::View vSubject,
                Array<octet_t>::View vab);

        /**
        * The acceptChannel() recipient implementation method. This method
        * is called on the service thread in response to a
        * ChannelAcceptRequest.
        *
        * @param nId       the Channel identifier
        * @param vSubject  the Subject
        */
        virtual void acceptChannelRequest(int32_t nId, Subject::View vSubject);

        /**
        * The acceptChannel() initiator implementation method. This method is
        * called on the service thread in response to a
        * ChannelAcceptResponse.
        *
        * @param nId          the Channel identifier
        * @param vFactory     the MessageFactory
        * @param vSerializer  the Serializer
        * @param hReceiver    the Receiver
        * @param vSubject     the Subject
        *
        * @return the accepted Channel
        */
        virtual Channel::Handle acceptChannelResponse(int32_t nId,
                Protocol::MessageFactory::View vFactory,
                Serializer::View vSerializer,
                Channel::Receiver::Handle hReceiver,
                Subject::View vSubject);

        /**
        * Close the Connection.
        *
        * @param fNotify  if true, notify the peer that the Connection is
        *                 being closed
        * @param ohe      the optional reason why the Connection is being
        *                 closed
        * @param fWait    if true wait for the ConnectionManager to close the
        *                 Connection when called on a client thread. This
        *                 method will always wait for the ConnectionManager
        *                 to close the Connection if called on the service
        *                 thread.
        */
        virtual void close(bool fNotify, Exception::Holder ohe,
                bool fWait = true);

        /**
        * The close() implementation method. This method is called on the
        * service thread.
        *
        * @param fNotify  if true, notify the peer that the Connection is
        *                 being closed
        * @param ohe      the optional reason why the Connection is being
        *                 closed
        * @param cMillis  the number of milliseconds to wait for the
        *                 Connection to close; pass 0L to perform a
        *                 non-blocking close or -1L to wait forever
        *
        * @return true iff the invocation of this method closed the
        *         Connection
        */
        virtual bool closeInternal(bool fNotify,
                Exception::Holder ohe, int64_t cMillis);

        /**
        * The createChannel() implementation method. This method is called on
        * the service thread.
        *
        * @param vProtocol    the Protocol
        * @param vSerializer  the Serializer
        * @param hReceiver    the Receiver
        *
        * @return the URI that identifies the new Channel
        */
        virtual URI::View createChannelInternal(Protocol::View vProtocol,
                Serializer::View vSerializer,
                Channel::Receiver::Handle hReceiver);

        /**
        * Attempt to close the Connection ThreadGate.
        *
        * @param cMillis  the number of milliseconds to wait for the
        *                 ThreadGate to close; pass 0 to perform a
        *                 non-blocking close or -1 to wait forever
        *
        * @return true if the Connection ThreadGate was closed; false
        *         otherwise
        */
        virtual bool gateClose(int64_t cMillis);

        /**
        * Open the Connection ThreadGate.
        */
        virtual void gateOpen();

        /**
        * Enter the Connection ThreadGate.
        *
        * @throws ConnectionException if the Connection is closing or closed
        */
        virtual void gateEnter();

        /**
        * Exit the Connection ThreadGate.
        */
        virtual void gateExit();

        /**
        * Open the Connection.
        */
        virtual void open();

        /**
        * The openChannel() implementation method. This method is called on
        * the service thread.
        *
        * @param vProtocol    the Protocol
        * @param vsName       the Receiver name
        * @param vSerializer  the Serializer
        * @param hReceiver    the Receiver
        * @param vSubject     the Subject
        * @oaram vabToken     the Subject's identity token
        *
        * @return the Request to open a new Channel
        */
        virtual Request::Status::Handle openChannelInternal(
                Protocol::View vProtocol,
                String::View vsName,
                Serializer::View vSerializer,
                Channel::Receiver::Handle hReceiver,
                Subject::View vSubject,
                Array<octet_t>::View vabToken);

        /**
        * The openChannel() recipient implementation method. This method is
        * called on the service thread in response to a ChannelOpenRequest.
        *
        * @param vsProtocol   the Protocol name
        * @param vSerializer  the Serializer
        * @param hReceiver    the Receiver
        * @param vSubject     the Subject
        *
        * @return the identifier of the newly opened Channel
        */
        virtual int32_t openChannelRequest(String::View vsProtocol,
                Serializer::View vSerializer,
                Channel::Receiver::Handle hReceiver,
                Subject::View vSubject);

        /**
        * The openChannel() initiator implementation method. This method is
        * called on the service thread in response to a ChannelOpenResponse.
        *
        * @param nId          the Channel identifier
        * @param vFactory     the MessageFactory
        * @param vSerializer  the Serializer
        * @param hReceiver    the Receiver
        * @param vSubject     the Subject
        *
        * @return the newly opened Channel
        */
        virtual Channel::Handle openChannelResponse(int32_t nId,
                Protocol::MessageFactory::View vFactory,
                Serializer::View vSerializer,
                Channel::Receiver::Handle hReceiver,
                Subject::View vSubject);

        /**
        * The open() implementation method. This method is called on the
        * service thread.
        */
        virtual void openInternal();

        /**
        * Send a PingRequest via "Channel0" and update the PingLastMillis
        * property. This method will only send a PingRequest if one is not
        * already outstanding.
        *
        * This method is only called on the service thread.
        *
        * @return true if a PingRequest was sent
        */
        virtual bool ping();

        /**
        * Reset the Connection statistics.
        */
        virtual void resetStats();

        /**
        * Send the given WriteBuffer over this Connection.
        *
        * @param vwb  the WriteBuffer to send
        *
        * @throws ConnectionException on fatal Connection error
        */
        virtual void send(WriteBuffer::View vwb);

        /*
        * Unregister the given Channel from the ChannelArray.
        *
        * @param vChannel  the Channel to unregister; must not be NULL
        */
        virtual void unregisterChannel(Channel::View vChannel);


    // ----- Connection interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ConnectionManager::Handle getConnectionManager();

        /**
        * {@inheritDoc}
        */
        virtual ConnectionManager::View getConnectionManager() const;

        /**
        * {@inheritDoc}
        */
        virtual UUID::View getId() const;

        /**
        * {@inheritDoc}
        */
        virtual UUID::View getPeerId() const;

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
        virtual Channel::Handle openChannel(Protocol::View vProtocol,
                String::View vsName,
                Channel::Receiver::Handle hReceiver = NULL,
                Subject::View vSubject = NULL);

        /**
        * {@inheritDoc}
        */
        virtual URI::View createChannel(Protocol::View vProtocol,
                Channel::Receiver::Handle hReceiver = NULL);

        /**
        * {@inheritDoc}
        */
        virtual Channel::Handle acceptChannel(URI::View vUri,
                Channel::Receiver::Handle hReceiver = NULL,
                Subject::View vSubject = NULL);

        /**
        * {@inheritDoc}
        */
        virtual Channel::Handle getChannel(int32_t nId);

        /**
        * {@inheritDoc}
        */
        virtual Channel::View getChannel(int32_t nId) const;

        /**
        * {@inheritDoc}
        */
        virtual Collection::View getChannels() const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * @throws ConnectionException if the Connection is closed or closing
        */
        virtual void assertOpen() const;

        /**
        * Generate a new unique Channel identifier.
        *
        * If the ConnectionManager that created this Channel is a
        * ConnectionAcceptor, the returned value will be in the range:
        *
        * [-Integer.max_value, 0)
        *
        * If the ConnectionManager that created this Channel is a
        * ConnectionInitiator, the returned value will be in the range:
        *
        * (0, Integer.max_value)
        *
        * The space of identifiers must be partitioned in order to prevent
        * collisions.
        *
        * @return a new unique Channel identifier
        */
        virtual int32_t generateChannelId() const;

        /**
        * Register the given Channel.
        *
        * @param hChannel  the Channel to register; must not be NULL
        */
        virtual void registerChannel(Channel::Handle hChannel);


    // ---- accessors -------------------------------------------------------

    public:
        /**
        * Return the ConnectionManager that created this Connection.
        *
        * @return the ConnectionManager that created this Connection
        *
        * @throws ConnectionException if the Connection has been closed
        */
        virtual PeerHandle ensureConnectionManager();

        /**
        * Return the ConnectionManager that created this Connection.
        *
        * @return the ConnectionManager that created this Connection
        *
        * @throws ConnectionException if the Connection has been closed
        */
        virtual PeerView ensureConnectionManager() const;

        /**
        * Determine if the calling thread is currently executing within
        * the Connection's ThreadGate.
        *
        * @return true if the calling thread is currently executing within
        *         the Connection's ThreadGate
        */
        virtual bool isActiveThread() const;

        /**
        * Determine if the peer should be notified when closing the
        * Connection during ThreadGate exit.
        *
        * @return true iff the peer should be notified
        *
        * @see #isCloseOnExit
        */
        virtual bool isCloseNotify() const;

        /**
        * Configure whether or not the peer should be notified when closing
        * the Connection during ThreadGate exit.
        *
        * @param f  true iff the peer should be notified
        *
        * @see #isCloseOnExit
        */
        virtual void setCloseNotify(bool f);

        /**
        * Determine if the Connection should be closed upon ThreadGate exit.
        *
        * @return true iff the Connection should be closed
        */
        virtual bool isCloseOnExit() const;

        /**
        * Configure whether or not the Connection should be closed upon
        * ThreadGate exit.
        *
        * @param f  true iff the Connection should be closed
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
        * Configure the ConnectionManager that created this Connection.
        *
        * @param hManager  the ConnectionManager that created this Connection
        */
        virtual void setConnectionManager(ConnectionManager::Handle hManager);

        /**
        * Configure the Connection's unique identifier.
        *
        * @param vUUID  the unique identifier
        */
        virtual void setId(UUID::View vUUID);

        /**
        * Return the Member object for this Connection.
        *
        * @return the Member object for this Connection
        *
        * @since Coherence 3.7
        */
        virtual Member::View getMember() const;

        /**
        * Set the Member object for this Connection.
        *
        * @param vMember  the Member object for this Connection
        *
        * @since Coherence 3.7
        */
        virtual void setMember(Member::View vMember);

        /**
        * Return the Map of MessageFactory objects that may be used by
        * Channel objects created by this Connection, keyed by Protocol
        * name.
        *
        * @return the MessageFactory Map
        */
        virtual Map::View getMessageFactoryMap() const;

        /**
        * Configure the Map of MessageFactory objects that may be used by
        * Channel objects created by this Connection, keyed by Protocol
        * name.
        *
        * @param vMap the MessageFactory Map
        */
        virtual void setMessageFactoryMap(Map::View vMap);

        /**
        * Return the product edition used by the peer.
        *
        * @return the product edition used by the peer
        */
        virtual int32_t getPeerEdition() const;

        /**
        * Configure the product edition used by the peer.
        *
        * @param nEdition  the product edition used by the peer
        */
        virtual void setPeerEdition(int32_t nEdition);

        /**
        * Configure the unique identity of the peer
        *
        * @param vUUID  the peer's UUID
        */
        virtual void setPeerId(UUID::View vUUID);

        /**
        * Return send time of the last outstanding PingRequest or 0 if a
        * PingRequest is not outstanding.
        *
        * @return the send time (in milliseconds)
        */
        virtual int64_t getPingLastMillis() const;

        /**
        * Set the send time of the last outstanding PingRequest or 0 if a
        * PingRequest is not outstanding.
        *
        * @param ldt  the send time (in milliseconds)
        */
        virtual void setPingLastMillis(int64_t ldt);

        /**
        * Return the number of bytes received by this Connection since the
        * last time statistics were reset.
        *
        * @return the number of bytes received
        */
        virtual int64_t getStatsBytesReceived() const;

        /**
        * Set the number of bytes received by this Connection since the last
        * time statistics were reset.
        *
        * @param cb  the number of bytes received
        */
        virtual void setStatsBytesReceived(int64_t cb);

        /**
        * Return the number of bytes sent through this Connection since the
        * last time statistics were reset.
        *
        * @return the number of bytes sent
        */
        virtual int64_t getStatsBytesSent() const;

        /**
        * Set the number of bytes sent through this Connection since the last
        * time statistics were reset.
        *
        * @param cb  the number of bytes sent
        */
        virtual void setStatsBytesSent(int64_t cb);

        /**
        * Return the number of messages received by this Connection since the
        * last time statistics were reset.
        *
        * @return the number of messages received
        */
        virtual int64_t getStatsReceived() const;

        /**
        * Set the number of messages received by this Connection since the
        * last time statistics were reset.
        *
        * @param cMessage  the number of messages received
        */
        virtual void setStatsReceived(int64_t cMessage);

        /**
        * Set the date/time value that the stats have been reset
        *
        * @param ldtReset  the date/time value the stats have been reset
        */
        virtual void setStatsReset(int64_t ldtReset);

        /**
        * Determine the last time (in milliseconds) that the statistics for
        * this Connection were reset.
        *
        * @return the last time (in milliseconds) that statistics were reset
        *
        * @see #resetStats
        */
        virtual int64_t getStatsReset() const;

        /**
        * Return the number of messages sent through this Connection since
        * the last time statistics were reset.
        *
        * @return the number of messages sent
        */
        virtual int64_t getStatsSent() const;

        /**
        * Set the number of messages sent through this Connection since the
        * last time statistics were reset.
        *
        * @param cMessage  the number of messages sent
        */
        virtual void setStatsSent(int64_t cMessage);


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The maximum number of pending new Channel objects. If the limit is
        * reached, a pending Channel will be discarded.
        */
        static const size32_t max_pending = 1000;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A LongArray of open Channel objects created by this Connection,
        * indexed by Channel identifier.
        */
        mutable FinalHandle<LongArray> f_hlaChannel;

        /**
        * A LongArray of newly created Channel objects that have not yet
        * accepted by the peer, indexed by Channel identifier.
        */
        mutable FinalHandle<LongArray> f_hlaChannelPending;

        /**
        * Peer notification flag used when the Connection is closed upon
        * exiting the ThreadGate (see m_fCloseOnExit).
        */
        Volatile<bool> m_fCloseNotify;

        /**
        * If true, the Thread that is currently executing within the
        * Connection should close the Connection immedately upon exiting the
        * Connection's ThreadGate.
        */
        Volatile<bool> m_fCloseOnExit;

        /**
        * The Exception to pass to the close() method when the Connection is
        * closed upon exiting the ThreadGate (see m_fCloseOnExit).
        */
        MemberHolder<Exception> m_oheCloseException;

        /**
        * The ConnectionManager that created or accepted this Connection.
        */
        WeakHandle<ConnectionManager> m_whManager;

        /**
        * The unique identifier of this Connection object.
        */
        FinalView<UUID> f_vUUID;

        /**
        * A Map of MessageFactory objects that may be used by Channel
        * objects created by this Connection, keyed by Protocol name.
        */
        FinalView<Map> f_vMapMessageFactory;

        /**
        * True if the Connection is open; false otherwise.
        */
        Volatile<bool> m_fOpen;

        /**
        * The Member object for this Connection.
        */
        FinalView<Member> f_vMember;

        /**
        * The product edition used by the peer.
        */
        int32_t m_nPeerEdition;

        /**
        * The unique identifier of the peer process to which this Connection
        * object is connected.
        */
        MemberView<UUID> m_vUUIDPeer;

        /**
        * The send time of the last outstanding PingRequest or 0 if a
        * PingRequest is not outstanding.
        */
        int64_t m_ldtPingLast;

        /**
        * Statistics: total number of bytes received over this Connection.
        */
        int64_t m_cStatsBytesReceived;

        /**
        * Statistics: total number of bytes sent over this Connection.
        */
        int64_t m_cStatsBytesSent;

        /**
        * Statistics: total number of Messages received over this Connection.
        */
        int64_t m_cStatsReceived;

        /**
        * Statistics: date/time value that the stats have been reset.
        */
        int64_t m_ldtStatsReset;

        /**
        * Statistics: total number of messages sent over this Connection.
        */
        int64_t m_cStatsSent;

        /**
        * A ThreadGate used to prevent concurrent use of this Connection
        * while it is being closed.
        */
        FinalHandle<ThreadGate> f_hThreadGate;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_CONNECTION_HPP
