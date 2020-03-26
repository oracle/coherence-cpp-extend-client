/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_READ_BUFFER_HPP
#define COH_ABSTRACT_READ_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"

// ----- forward declarations -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,util)

class Binary;

COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,io)


/**
* Abstract base implementation of the ReadBuffer interface.
*
* @author jh  2007.12.31
*/
class COH_EXPORT AbstractReadBuffer
    : public abstract_spec<AbstractReadBuffer,
        extends<Object>,
        implements<ReadBuffer> >
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

        /**
        * Binary Handle definition.
        */
        typedef TypedHandle<coherence::util::Binary> BinaryHandle;


    // ----- ReadBuffer interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual BufferInput::Handle getBufferInput() const;

        /**
        * {@inheritDoc}
        */
        virtual ReadBuffer::View getReadBuffer(size32_t of, size32_t cb) const;

        /**
        * {@inheritDoc}
        */
        virtual BinaryView toBinary() const;

        /**
        * {@inheritDoc}
        */
        virtual BinaryView toBinary(size32_t of, size32_t cb) const;

        /**
        * {@inheritDoc}
        */
        virtual Array<octet_t>::View toOctetArray() const;

        /**
        * {@inheritDoc}
        */
        virtual Array<octet_t>::View toOctetArray(size32_t of, size32_t cb) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Factory method: Instantiate a ReadBuffer for a portion of this
        * ReadBuffer.
        *
        * @param of  the beginning index, inclusive
        * @param cb  the number of bytes to include in the resulting
        *            ReadBuffer
        *
        * @return a ReadBuffer that represents a portion of this ReadBuffer
        */
        virtual ReadBuffer::View instantiateReadBuffer(size32_t of,
                size32_t cb) const = 0;

        /**
        * Factory method: Instantiate a BufferInput object to read data from
        * the ReadBuffer.
        *
        * @return a new BufferInput reading from this ReadBuffer
        */
        virtual BufferInput::Handle instantiateBufferInput() const;

        /**
        * Check if the specified read is within bounds.
        *
        * @param of  the absolute offset of the read operation
        * @param cb  the length in bytes of the read operation
        *
        * @throws IndexOutOfBoundsException if the specified read is not
        *         within bounds
        */
        virtual void checkBounds(size32_t of, size32_t cb) const;


    // ----- AbstractBufferInput inner class --------------------------------

    public:
        /**
        * This is an implementation of the BufferInput interface that
        * delegates its work back to its ReadBuffer.
        *
        * @author jh  2007.12.31
        */
        class COH_EXPORT AbstractBufferInput
            : public class_spec<AbstractBufferInput,
                extends<Object>,
                implements<BufferInput> >
            {
            friend class factory<AbstractBufferInput>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new AbstractBufferInput instance that delegates
                * to the given AbstractReadBuffer.
                *
                * @param vBuf  the delegate AbstractReadBuffer
                */
                AbstractBufferInput(AbstractReadBuffer::View vBuf);


            // ----- BufferInput interface ------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual ReadBuffer::View getBuffer() const;

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
                virtual size32_t available() const;

                /**
                * {@inheritDoc}
                */
                virtual void skip(size32_t cb);

                /**
                * {@inheritDoc}
                */
                virtual octet_t read();

                /**
                * {@inheritDoc}
                */
                virtual void read(Array<octet_t>::Handle hab);

                /**
                * {@inheritDoc}
                */
                virtual void read(Array<octet_t>::Handle hab, size32_t of,
                        size32_t cb);

                /**
                * {@inheritDoc}
                */
                virtual ReadBuffer::View readBuffer(size32_t cb);

                /**
                * {@inheritDoc}
                */
                virtual bool readBoolean();

                /**
                * {@inheritDoc}
                */
                virtual wchar16_t readChar16();

                /**
                * {@inheritDoc}
                */
                virtual String::View readString();

                /**
                * {@inheritDoc}
                */
                virtual int16_t readInt16();

                /**
                * {@inheritDoc}
                */
                virtual int32_t readInt32();

                /**
                * {@inheritDoc}
                */
                virtual int64_t readInt64();

                /**
                * {@inheritDoc}
                */
                virtual float32_t readFloat32();

                /**
                * {@inheritDoc}
                */
                virtual float64_t readFloat64();

            // ----- internal methods -----------------------------------

            protected:
                /**
                * Update the internal stream offset.
                *
                * @param of  the new offset
                */
                virtual void setOffsetInternal(size32_t of);

                /**
                * Reads the specified-length UTF-8 data and converts it to a
                * String value.
                *
                * @param cb  the number of bytes that the UTF data takes up
                *            in the stream
                *
                * @return a String value
                *
                * @throws UTFDataFormatException if the bytes that were
                *         read were not a valid UTF-8 encoded string
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual String::View readUTF(size32_t cb);

                /**
                * Convert a UTF-8 encoded section of the binary stream into a
                * String.
                *
                * @param of  the offset within the stream
                * @param cb  the length in bytes within the stream
                *
                * @return the String value
                *
                * @throws IOException on convertion error
                */
                virtual String::View convertUTF(size32_t of, size32_t cb) const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The delegate AbstractReadBuffer.
                */
                FinalView<AbstractReadBuffer> f_vBuf;

                /**
                * Current stream offset into the delegate AbstractReadBuffer.
                */
                size32_t m_of;
            };


    // ----- constants ------------------------------------------------------

    public:
        /**
        * An empty octet array (by definition immutable).
        */
        static Array<octet_t>::Handle getEmptyOctetArray();

        /**
        * An empty Binary object.
        *
        * @since Coherence 3.7.1
        */
        static BinaryHandle getEmptyBinary();

        /**
        * A zero-length ReadBuffer (by definition immutable).
        */
        static ReadBuffer::Handle getEmptyReadBuffer();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_READ_BUFFER_HPP
