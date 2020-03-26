/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OCTET_ARRAY_WRITE_BUFFER_HPP
#define COH_OCTET_ARRAY_WRITE_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/AbstractWriteBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"

#include <limits>

COH_OPEN_NAMESPACE2(coherence,io)


/**
* OctetArrayWriteBuffer is an implementation of WriteBuffer on an octet
* array. It is designed to support both fixed length buffers and resizable
* buffers.
*
* This implementation is explicitly not thread-safe.
*
* @author jh  2007.01.11
*/
class COH_EXPORT OctetArrayWriteBuffer
    : public cloneable_spec<OctetArrayWriteBuffer,
        extends<AbstractWriteBuffer> >
    {
    friend class factory<OctetArrayWriteBuffer>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an OctetArrayWriteBuffer on an octet array.
        *
        * @param hab  an octet array
        *
        * @throws NullPointerException if <tt>hab</tt> is NULL
        */
        OctetArrayWriteBuffer(Array<octet_t>::Handle hab);

        /**
        * Construct an OctetArrayWriteBuffer with a certain initial capacity
        * and a certain maximum capacity.
        *
        * @param cbCap  initial capacity
        * @param cbMax  maximum capacity
        *
        * @throws IllegalArgumentException if <tt>cbCap</tt> is greater than
        *         <tt>cbMax</tt>
        */
        OctetArrayWriteBuffer(size32_t cbCap,
                size32_t cbMax = (std::numeric_limits<size32_t>::max)());

        /**
        * Copy constructor.
        */
        OctetArrayWriteBuffer(const OctetArrayWriteBuffer& that);


    // ----- WriteBuffer interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t length() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getCapacity() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getMaximumCapacity() const;

        /**
        * {@inheritDoc}
        */
        virtual void write(size32_t ofDest, octet_t b);

        /**
        * {@inheritDoc}
        */
        virtual void write(size32_t ofDest, Array<octet_t>::View vabSrc,
                size32_t ofSrc, size32_t cbSrc);

        /**
        * {@inheritDoc}
        */
        virtual void write(size32_t ofDest, ReadBuffer::View vBufSrc,
                size32_t ofSrc, size32_t cbSrc);

        /**
        * {@inheritDoc}
        */
        using AbstractWriteBuffer::write;

        /**
        * {@inheritDoc}
        */
        virtual void retain(size32_t of, size32_t cb);

        /**
        * {@inheritDoc}
        */
        using AbstractWriteBuffer::retain;

        /**
        * {@inheritDoc}
        */
        virtual ReadBuffer::View getUnsafeReadBuffer() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual BufferOutput::Handle instantiateBufferOutput();

        /**
        * Validate the ranges for the passed bounds and make sure that the
        * underlying array is big enough to handle them.
        *
        * @param of  the offset that data is about to be written to
        * @param cb  the length of the data that is about to be written
        */
        virtual void checkBounds(size32_t of, size32_t cb);

        /**
        * Grow the underlying octet array to at least the specified size.
        *
        * @param cbCap  the required or requested capacity
        */
        virtual void grow(size32_t cbCap);

        /**
        * Update the length if the passed length is greater than the current
        * buffer length.
        *
        * @param cb  the count of the last octet written (or the index of
        *            the next octet to write)
        */
        virtual void updateLength(size32_t cb);


    // ----- OctetArrayBufferOutput inner class -----------------------------

    public:
        /**
        * OctetArrayBufferOutput is an implementation of BufferOutput
        * optimized for writing to the buffer's underlying octet array. A
        * BufferOutput implementation that delegates to a BufferOutput
        * implementation, except that its offset range is shifted and
        * limited.
        *
        * @author jh  2008.01.11
        */
        class COH_EXPORT OctetArrayBufferOutput
            : public class_spec<OctetArrayBufferOutput,
                extends<AbstractBufferOutput> >
            {
            friend class factory<OctetArrayBufferOutput>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new OctetArrayBufferOutput instance that delegates
                * to the given OctetArrayWriteBuffer.
                *
                * @param hBuf  the delegate WriteBuffer
                */
                OctetArrayBufferOutput(OctetArrayWriteBuffer::Handle hBuf);

            // ----- BufferOutput interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void writeChar16(wchar16_t ch);

                /**
                * {@inheritDoc}
                */
                virtual void writeString(String::View vs);

                /**
                * {@inheritDoc}
                */
                virtual void writeInt16(int16_t n);

                /**
                * {@inheritDoc}
                */
                virtual void writeInt32(int32_t n);

                /**
                * {@inheritDoc}
                */
                virtual void writeInt64(int64_t n);

                /**
                * {@inheritDoc}
                */
                virtual void writeFloat32(float32_t fl);

                /**
                * {@inheritDoc}
                */
                virtual void writeFloat64(float64_t dfl);

            // ----- internal methods -----------------------------------

            protected:
                /**
                * Move the offset within the stream forward.
                *
                * @param cb  the number of octets to advance the offset
                */
                virtual void moveOffset(size32_t cb);

            // ----- data members ---------------------------------------

            protected:
                /**
                 * The delegate write buffer.
                 */
                FinalHandle<OctetArrayWriteBuffer> f_hBuf;
            };


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onEscape(bool fEscaped) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The octet array that holds the binary data.
        */
        MemberHandle<Array<octet_t> > m_hab;

        /**
         * Raw pointer to byte[] within m_hab, or NULL if this object is
         * escaped which would make its use impossible to prove as safe.
         */
        mutable octet_t* m_pab;

        /**
         * The cached size of m_hab.
         */
        size32_t m_cbab;

        /**
        * Number of octets in the octet array that have been written by this
        * WriteBuffer. This is the length.
        */
        size32_t m_cb;

        /**
        * Number of octets that the octet array can be grown to. This is the
        * maximum capacity.
        */
        size32_t m_cbMax;

        /**
        * Cached ReadBuffer to quickly provide an answer to
        * {@link #getUnsafeReadBuffer()}.
        */
        mutable MemberHandle<ReadBuffer> m_hBufUnsafe;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OCTET_ARRAY_WRITE_BUFFER_HPP
