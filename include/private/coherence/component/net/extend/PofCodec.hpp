/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_CODEC_HPP
#define COH_POF_CODEC_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/util/Describable.hpp"

#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Codec.hpp"
#include "private/coherence/net/messaging/Message.hpp"



COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Codec;
using coherence::net::messaging::Message;
using coherence::util::Describable;


/**
* Implementation of the Codec interface that encodes/decodes Messages using
* POF.
*
* @author jh  2008.02.12
*/
class COH_EXPORT PofCodec
    : public class_spec<PofCodec,
        extends<Describable>,
        implements<Codec> >
    {
    friend class factory<PofCodec>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PofCodec.
        */
        PofCodec();


    // ----- Codec interface ------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void encode(Channel::View vChannel, Message::View vMessage,
                WriteBuffer::BufferOutput::Handle hOut);

        /**
        * {@inheritDoc}
        */
        virtual Message::Handle decode(Channel::View vChannel,
                ReadBuffer::BufferInput::Handle hIn);


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;
};

COH_CLOSE_NAMESPACE4

#endif // COH_POF_CODEC_HPP
