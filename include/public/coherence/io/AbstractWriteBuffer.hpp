/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_WRITE_BUFFER_HPP
#define COH_ABSTRACT_WRITE_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"

// ----- forward declarations -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,util)

class Binary;

COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,io)


/**
* The AbstractWriteBuffer is a partial implementation of the WriteBuffer
* interface intended to be used as a base class for easily creating concrete
* WriteBuffer implementations.
* <p>
* This implementation is explicitly not thread-safe.
*
* @author jh  2008.01.08
*/
class COH_EXPORT AbstractWriteBuffer
    : public abstract_spec<AbstractWriteBuffer,
        extends<Object>,
        implements<WriteBuffer> >
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

        /**
        * Binary View definition.
        */
        typedef TypedHandle<const coherence::util::Binary> BinaryView;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractWriteBuffer();

        /**
        * @internal
        */
        AbstractWriteBuffer(const AbstractWriteBuffer& that);


    // ----- WriteBuffer interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t getMaximumCapacity() const;

        /**
        * {@inheritDoc}
        */
        virtual void write(size32_t ofDest, Array<octet_t>::View vabSrc);

        /**
        * {@inheritDoc}
        */
        virtual void write(size32_t ofDest, ReadBuffer::View vBufSrc);

        /**
        * {@inheritDoc}
        */
        virtual void write(size32_t ofDest, ReadBuffer::View vBufSrc,
                size32_t ofSrc, size32_t cbSrc);

        /**
        * {@inheritDoc}
        */
        using WriteBuffer::write;

        /**
        * {@inheritDoc}
        */
        virtual void retain(size32_t of);

        /**
        * {@inheritDoc}
        */
        using WriteBuffer::retain;

        /**
        * {@inheritDoc}
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual WriteBuffer::Handle getWriteBuffer(size32_t of);

        /**
        * {@inheritDoc}
        */
        virtual WriteBuffer::Handle getWriteBuffer(size32_t of, size32_t cb);

        /**
        * {@inheritDoc}
        */
        virtual ReadBuffer::View getReadBuffer() const;

        /**
        * {@inheritDoc}
        */
        virtual Array<octet_t>::View toOctetArray() const;

        /**
        * {@inheritDoc}
        */
        virtual BinaryView toBinary() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Factory method: Instantiate a BufferOutput object to write data to
        * the WriteBuffer.
        *
        * @return a new BufferOutput writing to this ReadBuffer
        */
        virtual BufferOutput::Handle instantiateBufferOutput();

        /**
        * Get a small buffer for formating data.
        *
        * @return a small octet array
        */
        virtual Array<octet_t>::Handle tmpbuf();

        /**
        * Get a buffer for formating data.
        *
        * @param cb  the minimum size for the buffer
        *
        * @return an octet array that is at least <tt>cb</tt> octets long
        */
        virtual Array<octet_t>::Handle tmpbuf(size32_t cb);


    // ----- AbstractBufferOutput inner class -------------------------------

    public:
        /**
        * AbstractBufferOutput is a concrete implementation of BufferOutput
        * for the non-concrete AbstractWriteBuffer implementation.
        *
        * @author jh  2008.01.08
        */
        class COH_EXPORT AbstractBufferOutput
            : public class_spec<AbstractBufferOutput,
                extends<Object>,
                implements<BufferOutput> >
            {
            friend class factory<AbstractBufferOutput>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new AbstractBufferOutput instance that delegates
                * to the given AbstractWriteBuffer.
                *
                * @param hBuf  the delegate AbstractWriteBuffer
                */
                AbstractBufferOutput(AbstractWriteBuffer::Handle hBuf);

            // ----- BufferOutput interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual WriteBuffer::Handle getBuffer();

                /**
                * {@inheritDoc}
                */
                virtual WriteBuffer::View getBuffer() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t getOffset() const;

                /**
                * {@inheritDoc}
                */
                virtual void setOffset(size32_t of);

                /**
                * {@inheritDoc}
                */
                virtual void write(octet_t b);

                /**
                * {@inheritDoc}
                */
                virtual void write(Array<octet_t>::View vab);

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

            // ----- data members ---------------------------------------

            protected:
                /**
                * The delegate AbstractWriteBuffer.
                */
                FinalHandle<AbstractWriteBuffer> f_hBuf;

                /**
                * Current write offset within the AbstractWriteBuffer.
                */
                size32_t m_ofWrite;
            };

        /**
        * {@inheritDoc}
        */
        virtual BufferOutput::Handle getBufferOutput(size32_t of = 0);

        /**
        * {@inheritDoc}
        */
        virtual BufferOutput::Handle getAppendingBufferOutput();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A temp buffer to use for building the data to write.
        */
        MemberHandle<Array<octet_t> > m_habBuf;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_WRITE_BUFFER_HPP
