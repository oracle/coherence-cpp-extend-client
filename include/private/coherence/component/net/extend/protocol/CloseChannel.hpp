/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLOSE_CHANNEL_HPP
#define COH_CLOSE_CHANNEL_HPP

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/component/net/extend/PofChannel.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::component::net::extend::PofChannel;


/**
* Internal Request used to close a Channel.
*
* @authore nsa 2008.03.18
*/
class COH_EXPORT CloseChannel
    : public class_spec<CloseChannel,
        extends<AbstractPofRequest> >
    {
    friend class factory<CloseChannel>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        CloseChannel();


    // ----- AbstractPofRequest interface -----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofResponse::Handle instantiateResponse(
                        Protocol::MessageFactory::View vFactory) const;

        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the optional reason why the Channel is being closed.
        *
        * @return the optional reason why the Channel is being closed
        */
        virtual Exception::Holder getCause() const;

        /**
        * Return the Channel to close.
        *
        * @return the Channel to close
        */
        virtual PofChannel::View getChannelClose() const;

        /**
        * Return the Channel to close.
        *
        * @return the Channel to close
        */
        virtual PofChannel::Handle getChannelClose();

        /**
        * Return whether to notify the peer that the Channel is being closed.
        *
        * @return true if the Peer should be notified that the Channel is
        *         being closed
        */
        virtual bool isNotify() const;

        /**
        * Set the optional reason why the Channel is being closed.
        *
        * @param oheCause  the optional reason why the Channel is being closed
        */
        virtual void setCause(Exception::Holder oheCause);

        /**
        * Set the Channel to close.
        *
        * @param hChannel  the Channel to close
        */
        virtual void setChannelClose(PofChannel::Handle hChannel);

        /**
        * Set whether to notify the peer that the Channel is being closed.
        *
        * @param fNotify  true if the Peer should be notified that the
        *                 Channel is being closed
        */
        virtual void setNotify(bool fNotify);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = -2;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The optional reason why the Channel is being closed.
        */
        FinalHolder<Exception> f_oheCause;

        /**
        * The Channel to close.
        */
        FinalHandle<PofChannel> f_hChannelClose;

        /**
        * If true, notify the peer that the Channel is being closed.
        */
        bool m_fNotify;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_CLOSE_CHANNEL_HPP
