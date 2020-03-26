/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_READ_BUFFER_HPP
#define COH_READ_BUFFER_HPP

#include "coherence/lang.ns"

// ----- forward declarations -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,util)

class Binary;

COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,io)


/**
* The ReadBuffer interface represents an in-memory block of binary data.
*
* @author jh  2007.12.20
*/
class COH_EXPORT ReadBuffer
    : public interface_spec<ReadBuffer>
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


    // ----- ReadBuffer interface -------------------------------------------

    public:
        /**
        * Determine the length of the buffer.
        *
        * @return the number of octets of data represented by this ReadBuffer
        */
        virtual size32_t length() const = 0;

        /**
        * Returns the octet at the specified offset. An offset ranges
        * from <code>0</code> to <code>length() - 1</code>. The first octet
        * of the sequence is at offset <code>0</code>, the next at offset
        * <code>1</code>, and so on, as for array indexing.
        *
        * @param of  the offset (index) of the octet
        *
        * @return the octet at the specified offset in this ReadBuffer
        *
        * @throws  IndexOutOfBoundsException if the <code>of</code> argument
        *          is not less than the length of this ReadBuffer
        */
        virtual octet_t read(size32_t of) const = 0;

        /**
        * Copies octets from this ReadBuffer into the destination octet
        * array.
        * <p>
        * The first octet to be copied is at offset <code>ofBegin</code>;
        * the last octet to be copied is at offset <code>ofEnd-1</code>
        * (thus the total number of octets to be copied is <code>ofEnd -
        * ofBegin</code>). The octets are copied into the subarray of
        * <code>habDest</code> starting at offset <code>ofDest</code> and
        * ending at index:
        * <pre>
        *     ofDest + (ofEnd - ofBegin) - 1
        * </pre>
        * This method allows the caller to extract a chunk of octets into the
        * caller's own array.
        *
        * @param ofBegin  offset of the first octet in the ReadBuffer to copy
        * @param ofEnd    offset after the last octet in the ReadBuffer to
        *                 copy
        * @param habDest  the destination octet array
        * @param ofDest   the offset in the destination octet array to copy
        *                 the first octet to
        *
        * @throws IndexOutOfBoundsException if any of the following is true:
        *   <ul>
        *   <li><code>ofBegin</code> is greater than <code>ofEnd</code>
        *   <li><code>ofEnd</code> is greater than the length of this
        *       ReadBuffer;
        *   <li><code>ofDest + (ofEnd - ofBegin)</code> is larger than
        *       <code>habDest->length</code>
        *   </ul>
        * @throws NullPointerException if <code>habDest</code> is NULL
        */
        virtual void read(size32_t ofBegin, size32_t ofEnd,
                Array<octet_t>::Handle habDest, size32_t ofDest) const = 0;

        /**
        * Obtain a ReadBuffer for a portion of this ReadBuffer.
        *
        * @param of  the beginning index, inclusive
        * @param cb  the number of octets to include in the resulting
        *            ReadBuffer
        *
        * @return a ReadBuffer that represents a portion of this ReadBuffer
        *
        * @throws  IndexOutOfBoundsException if <code>of + cb</code> is
        *          larger than the length of this ReadBuffer
        */
        virtual ReadBuffer::View getReadBuffer(size32_t of, size32_t cb) const = 0;

        /**
        * Returns a new Binary object that holds the complete contents of
        * this ReadBuffer.
        * <p>
        * This is the equivalent of <code>toBinary(0, length())</code>.
        *
        * @return  the contents of this ReadBuffer as a Binary object
        *
        * @since Coherence 3.7.1
        */
        virtual BinaryView toBinary() const = 0;

        /**
        * Returns a Binary object that holds the specified portion of this
        * ReadBuffer.
        * <p>
        * This is the equivalent of
        * <code>getReadBuffer(of, cb)->toBinary()</code>.
        *
        * @param of  the beginning index, inclusive
        * @param cb  the number of octets to include in the Binary object
        *
        * @return a Binary object containing the specified portion of this
        *         ReadBuffer
        *
        * @throws IndexOutOfBoundsException if <code>of + cb</code> is
        *         larger than the length of this <code>ReadBuffer</code>
        *         object
        *
        * @since Coherence 3.7.1
        */
        virtual BinaryView toBinary(size32_t of, size32_t cb) const = 0;

        /**
        * Get the contents of the ReadBuffer as an octet array.
        * <p>
        * This is the equivalent of <code>toOctetArray(0, length())</code>.
        *
        * @return an octet array with the contents of this ReadBuffer object
        */
        virtual Array<octet_t>::View toOctetArray() const = 0;

        /**
        * Get a portion of the contents of the ReadBuffer as an octet array.
        *
        * @param of  the beginning index, inclusive
        * @param cb  the number of octets to include in the resulting array
        *
        * @return an octet array containing the specified portion of this
        *         ReadBuffer
        *
        * @throws IndexOutOfBoundsException if <code>of + cb</code> is
        *         larger than the length of this ReadBuffer
        */
        virtual Array<octet_t>::View toOctetArray(size32_t of, size32_t cb) const = 0;


    // ----- BufferInput inner interface ------------------------------------

    public:
        /**
        * The BufferInput interface represents a data input stream on top of
        * a ReadBuffer.
        *
        * @author jh  2007.12.20
        */
        class COH_EXPORT BufferInput
            : public interface_spec<BufferInput>
            {
            // ----- BufferInput interface ------------------------------

            public:
                /**
                * Get the ReadBuffer object that this BufferInput is reading
                * from.
                *
                * @return the underlying ReadBuffer object
                */
                virtual ReadBuffer::View getBuffer() const = 0;

                /**
                * Determine the current offset of this BufferInput within the
                * underlying ReadBuffer.
                *
                * @return the offset of the next octet to read from the
                *         ReadBuffer
                */
                virtual size32_t getOffset() const = 0;

                /**
                * Specify the offset of the next octet to read from the
                * underlying ReadBuffer.
                *
                * @param of  the offset of the next octet to read from the
                *            ReadBuffer
                *
                * @throws IndexOutOfBoundsException if
                *         <code>of > getBuffer()->length()</code>
                */
                virtual void setOffset(size32_t of) = 0;

                /**
                * Returns the number of octets that can be read (or skipped
                * over) from this input stream before the end of the stream
                * is reached.
                *
                * @return the number of octets that can be read (or skipped
                *         over) from this BufferInput
                */
                virtual size32_t available() const = 0;

                /**
                * Skips over the specified number of octets of data.
                *
                * @param cb  the number of octets to skip over
                *
                * @throws EOFException if the stream is exhausted before the
                *         number of octets indicated could be skipped
                * @throws IOException if an I/O error occurs
                */
                virtual void skip(size32_t cb) = 0;

                /**
                * Read an octet value.
                *
                * @return an <code>octet_t</code> value
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual octet_t read() = 0;

                /**
                * Read <code>hab->length</code> octets and store them in
                * <code>hab</code>.
                *
                * @param hab  the array to store the octets which are read
                *             from the stream
                *
                * @throws NullPointerException if the passed array is NULL
                * @throws EOFException if the stream is exhausted before the
                *         number of octets indicated by the array length could
                *         be read
                * @throws IOException if an I/O error occurs
                */
                virtual void read(Array<octet_t>::Handle hab) = 0;

                /**
                * Read <code>cb</code> octets from the input stream and store
                * them into the passed array <code>hab</code> starting at
                * offset <code>of</code>.
                *
                * @param hab  the array to store the octets which are read
                *             from the stream
                * @param of   the offset into the array that the read octets
                *             will be stored
                * @param cb   the number of octets to read
                *
                * @throws NullPointerException if the passed array is NULL
                * @throws IndexOutOfBoundsException if <code>of+cb</code>
                *         is greater than the length of the <code>hab</code>
                * @throws EOFException if the stream is exhausted before the
                *         number of octets indicated could be read
                * @throws IOException if an I/O error occurs
                */
                virtual void read(Array<octet_t>::Handle hab, size32_t of,
                        size32_t cb) = 0;

                /**
                * Read <code>cb</code> octets and return them as a ReadBuffer
                * object.
                *
                * @param cb  the number of octets to read
                *
                * @return a ReadBuffer object composed of <code>cb</code>
                *         octets read from the BufferInput
                *
                * @throws EOFException if the stream is exhausted before the
                *         number of octets indicated could be read
                * @throws IOException if an I/O error occurs
                */
                virtual ReadBuffer::View readBuffer(size32_t cb) = 0;

                /**
                * Read a boolean value.
                *
                * @return either <code>true</code> or <code>false</code>
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual bool readBoolean() = 0;

                /**
                * Read a 16-bit Unicode character value.
                *
                * @return a 16-bit Unicode character as a
                *         <code>wchar16_t</code> value
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual wchar16_t readChar16() = 0;

                /**
                * Read a sequence of UTF-8 encoded 16-bit Unicode characters.
                *
                * @return a String value; may be NULL
                *
                * @throws IOException if an I/O error occurs
                */
                virtual String::View readString() = 0;

                /**
                * Read a 16-bit integer value.
                *
                * @return an <code>int16_t</code> value
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual int16_t readInt16() = 0;

                /**
                * Read a 32-bit integer value.
                *
                * @return an <code>int32_t</code> value
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual int32_t readInt32() = 0;

                /**
                * Read a 64-bit integer value.
                *
                * @return an <code>int64_t</code> value
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual int64_t readInt64() = 0;

                /**
                * Read a 32-bit floating-point value.
                *
                * @return a <code>float32_t</code> value
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual float32_t readFloat32() = 0;

                /**
                * Read a 64-bit floating-point value.
                *
                * @return a <code>float64_t</code> value
                *
                * @throws EOFException if the value could not be read because
                *         no more data remains to be read
                * @throws IOException if an I/O error occurs
                */
                virtual float64_t readFloat64() = 0;
            };

        /**
        * Get a BufferInput object to read data from this buffer. Note that
        * each call to this method will return a new BufferInput object, with
        * the possible exception being that a zero-length ReadBuffer could
        * always return the same instance (since there is nothing to read).
        *
        * @return a BufferInput that is reading from this buffer starting at
        *         offset zero
        */
        virtual BufferInput::Handle getBufferInput() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_READ_BUFFER_HPP
