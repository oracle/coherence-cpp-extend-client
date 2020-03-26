/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CODEC_HPP
#define COH_CODEC_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"

#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Message.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;


/**
* A Codec converts a {@link Message} object to and from a binary
* representation.
*
* @author jh 2007.12.19
*/
class COH_EXPORT Codec
    : public interface_spec<Codec>
    {
    // ----- Codec interface ------------------------------------------------

    public:
        /**
        * Encode and write a binary representation of the given Message to
        * the given stream.
        * <p/>
        * Using the passed Channel, the Codec has access to both the
        * MessageFactory for the Channel and the underlying Connection.
        *
        * @param vChannel  the Channel object through which the binary-
        *                  encoded Message will be passed
        * @param vMessage  the Message to encode
        * @param hOut      the BufferOutput to write the binary
        *                  representation of the Message to
        *
        * @throws IOException if an error occurs encoding or writing the
        *         Message
        */
        virtual void encode(Channel::View vChannel, Message::View vMessage,
                WriteBuffer::BufferOutput::Handle hOut) = 0;

        /**
        * Reads a binary-encoded Message from the passed BufferInput object.
        * <p/>
        * Using the passed Channel, the Codec has access to both the
        * MessageFactory for the Channel and the underlying Connection.
        *
        * @param vChannel  the Channel object through which the binary-
        *                  encoded Message was passed
        * @param hIn       the BufferInput containing the binary-encoded
        *                  Message
        *
        * @return the Message object encoded in the given BufferInput
        *
        * @throws IOException if an error occurs reading or decoding the
        *         Message
        */
        virtual Message::Handle decode(Channel::View vChannel,
                ReadBuffer::BufferInput::Handle hIn) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CODEC_HPP
