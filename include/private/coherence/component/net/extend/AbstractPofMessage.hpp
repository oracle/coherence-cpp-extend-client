/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_POF_MESSAGE_HPP
#define COH_ABSTRACT_POF_MESSAGE_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Describable.hpp"

#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Message.hpp"



COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::io::Evolvable;
using coherence::io::ReadBuffer;
using coherence::io::pof::PortableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Message;
using coherence::util::Binary;
using coherence::util::Describable;


/**
* Abstract implementation of the Message interface that supports POF
* serialization.
*
* @author jh  2008.02.12
*/
class COH_EXPORT AbstractPofMessage
    : public abstract_spec<AbstractPofMessage,
        extends<Describable>,
        implements<Message, Evolvable, PortableObject> >
    // Evolvable, Portable used over EvolvablePortable as it is more efficient
    // to do "flat" instance of checks, and this one is done on every message
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractPofMessage();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractPofMessage(const AbstractPofMessage&);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Channel::Handle getChannel();

        /**
        * {@inheritDoc}
        */
        virtual Channel::View getChannel() const;

        /**
        * {@inheritDoc}
        */
        virtual void setChannel(Channel::Handle hChannel);

        /**
        * {@inheritDoc}
        */
        virtual bool isExecuteInOrder() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isSolicited() const;


    // ----- EvolvablePortableObject interface ------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getImplVersion() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getDataVersion() const;

        /**
        * {@inheritDoc}
        */
        virtual void setDataVersion(int32_t nVersion);

        /**
        * {@inheritDoc}
        */
        virtual Binary::View getFutureData() const;

        /**
        * {@inheritDoc}
        */
        virtual void setFutureData(Binary::View vBinFuture);

        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Configure the serialization version supported by this Message.
        *
        * @param nVersion  the serialization version
        */
        virtual void setImplVersion(int32_t nVersion);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Channel through which the Message will be sent, was sent, or
        * was received.
        */
        FinalHandle<Channel> f_hChannel;

        /**
        * The version associated with the data stream from which this Message
        * was deserialized.
        */
        int32_t m_nDataVersion;

        /**
        * The serialization version supported by the implementing class.
        */
        int32_t m_nImplVersion;

        /**
        * The unknown remainder of the data stream from which this Message is
        * being deserialized.
        */
        FinalView<Binary> f_vBinFuture;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_POF_MESSAGE_HPP
