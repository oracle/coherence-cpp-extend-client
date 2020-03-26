/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DELEGATING_WRITE_BUFFER_HPP
#define COH_DELEGATING_WRITE_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/AbstractWriteBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


/**
* A DelegatingWriteBuffer is a WriteBuffer that writes through to an
* underlying (or "containing") WriteBuffer. Basically, it allows a process
* that is writing to a WriteBuffer to ask for a "protected" sub-portion of
* that WriteBuffer to hand to a second process, such that the second process
* can not affect (or even read from) the WriteBuffer outside of the portion
* that the first process explicitly designated as viewable and modifiable.
* <p>
* This implementation is explicitly not thread-safe.
*
* @author jh  2007.01.10
*/
class COH_EXPORT DelegatingWriteBuffer
    : public cloneable_spec<DelegatingWriteBuffer,
        extends<AbstractWriteBuffer> >
    {
    friend class factory<DelegatingWriteBuffer>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a DelegatingWriteBuffer that will delegate to the
        * containing WriteBuffer.
        *
        * @param hBuf  the containing WriteBuffer
        * @param of    the offset within the containing WriteBuffer that this
        *              WriteBuffer is starting at
        * @param cb    the maximum capacity for this WriteBuffer
        */
        DelegatingWriteBuffer(WriteBuffer::Handle hBuf, size32_t of,
                size32_t cb);

        /**
        * Copy constructor.
        */
        DelegatingWriteBuffer(const DelegatingWriteBuffer& that);


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
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual ReadBuffer::View getUnsafeReadBuffer() const;

        /**
        * {@inheritDoc}
        */
        virtual Array<octet_t>::View toOctetArray() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual BufferOutput::Handle instantiateBufferOutput();

        /**
        * Test an offset and length of data to write to see if it can be
        * written to this buffer.
        *
        * @param of  offset to write data at
        * @param cb  length in octets of data
        */
        virtual void checkBounds(size32_t of, size32_t cb) const;

        /**
        * Update the length if the passed length is greater than the current
        * buffer length.
        *
        * @param cb  the possible new length
        */
        virtual void updateLength(size32_t cb);


    // ----- DelegatingBufferOutput inner class -----------------------------

    public:
        /**
        * A BufferOutput implementation that delegates to a BufferOutput
        * implementation, except that its offset range is shifted and
        * limited.
        *
        * @author jh  2008.01.10
        */
        class COH_EXPORT DelegatingBufferOutput
            : public class_spec<DelegatingBufferOutput,
                extends<AbstractBufferOutput> >
            {
            friend class factory<DelegatingBufferOutput>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new DelegatingBufferOutput instance that delegates
                * to the BufferOutput of the underlying WriteBuffer of the
                * given DelegatingWriteBuffer.
                *
                * @param hBuf  the delegate WriteBuffer
                */
                DelegatingBufferOutput(DelegatingWriteBuffer::Handle hBuf);

            // ----- BufferOutput interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                using AbstractBufferOutput::write;

                /**
                * {@inheritDoc}
                */
                virtual void write(octet_t b);

                /**
                * {@inheritDoc}
                */
                virtual void write(Array<octet_t>::View vab, size32_t of,
                    size32_t cb);

                /**
                * {@inheritDoc}
                */
                virtual void writeBuffer(ReadBuffer::View vBuf);

                /**
                * {@inheritDoc}
                */
                virtual void writeBuffer(ReadBuffer::View vBuf, size32_t of,
                        size32_t cb);

                /**
                * {@inheritDoc}
                */
                virtual void writeBoolean(bool f);

                /**
                * {@inheritDoc}
                */
                virtual void writeChar16(wchar16_t ch);

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
                * The BufferOutput to delegate to.
                */
                FinalHandle<BufferOutput> f_hOut;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The WriteBuffer to delegate to; the "containing" WriteBuffer.
        */
        FinalHandle<WriteBuffer> f_hBuf;

        /**
        * Offset into the containing WriteBuffer where this WriteBuffer
        * starts.
        */
        size32_t m_ofStart;

        /**
        * Length in octets of this WriteBuffer.
        */
        size32_t m_cb;

        /**
        * Maximum number of octets in this WriteBuffer.
        */
        size32_t m_cbMax;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DELEGATING_WRITE_BUFFER_HPP
