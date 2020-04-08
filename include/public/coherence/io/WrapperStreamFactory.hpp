/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WRAPPER_STREAM_FACTORY_HPP
#define COH_WRAPPER_STREAM_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


/**
* Provides the means to wrap an BufferInput and BufferOutput, such that
* functionality such as compression and encryption can be implemented
* in a layered, pluggable fashion.
*
* @author nsa  2008.04.07
*/
class COH_EXPORT WrapperStreamFactory
    : public interface_spec<WrapperStreamFactory>
    {
    // ----- WrapperStreamFactory interface ---------------------------------

    public:
        /**
        * Requests an InputStream that wraps the passed InputStream.
        *
        * @param  hIn  the coherence::io::ReadBuffer::BufferInput to be
        *              wrapped
        *
        * @return a BufferInput that delegates to ("wraps") the passed
        *         BufferInput
        */
        virtual ReadBuffer::BufferInput::Handle getInputStream(
                ReadBuffer::BufferInput::Handle hIn) const = 0;

        /**
        * Requests an OutputStream that wraps the passed OutputStream.
        *
        * @param  hOut  the coherence::io::WriteBuffer::BufferOutput to be
        *               wrapped
        *
        * @return a BufferOutput that delegates to ("wraps") the passed
        *         BufferOutput
        */
        virtual WriteBuffer::BufferOutput::Handle getOutputStream(
                WriteBuffer::BufferOutput::Handle hOut) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_WRAPPER_STREAM_FACTORY_HPP
