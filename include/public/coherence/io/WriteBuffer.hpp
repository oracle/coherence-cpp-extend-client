/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WRITE_BUFFER_HPP
#define COH_WRITE_BUFFER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"

// ----- forward declarations -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,util)

class Binary;

COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,io)


/**
* The WriteBuffer interface represents an in-memory block of binary data
* that is being accumulated (written to).
*
* @author jh  2007.12.20
*/
class COH_EXPORT WriteBuffer
    : public interface_spec<WriteBuffer>
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


    // ----- WriteBuffer interface ------------------------------------------

    public:
        /**
        * Determine the length of the data that is in the buffer. This is the
        * actual number of octets of data that have been written to the
        * buffer, not the capacity of the buffer.
        *
        * @return the number of octets of data represented by this
        *         WriteBuffer
        */
        virtual size32_t length() const = 0;

        /**
        * Determine the number of octets that the buffer can hold without
        * resizing itself. In other words, a WriteBuffer has <tt>
        * getCapacity() - #length()</tt> octets that can be written
        * to it without overflowing the current underlying buffer allocation.
        * Since the buffer is an abstract concept, the actual mechanism for
        * the underlying buffer is not known.
        *
        * Note that if the maximum size returned by
        * #getMaximumCapacity() is greater than the current size
        * returned by this method, then the WriteBuffer will automatically
        * resize itself to allocate more space when the amount of data
        * written to it passes the current size.
        *
        * @return the number of octets of data that this WriteBuffer can hold
        *         without resizing its underlying buffer
        */
        virtual size32_t getCapacity() const = 0;

        /**
        * Determine the maximum number of octets that the buffer can hold. If
        * the maximum size is greater than the current size, then the buffer
        * is expected to resize itself as necessary up to the maximum size in
        * order to contain the data given to it.
        *
        * @return the maximum number of octets of data that the WriteBuffer
        *         can hold
        */
        virtual size32_t getMaximumCapacity() const = 0;

        /**
        * Store the specified octet at the specified offset within the
        * buffer.
        *
        * For purposes of side-effects and potential exceptions, this method
        * is functionally equivalent to the following code:
        * <pre><code>
        * Array<octet_t>::Handle habSrc = Array<octet_t>::create(1);
        * habSrc[0] = b;
        * write(ofDest, habSrc, 0, 1);
        * </code></pre>
        *
        * @param ofDest  the offset within this buffer to store the passed
        *                data
        * @param b       the octet to store in this buffer
        */
        virtual void write(size32_t ofDest, octet_t b) = 0;

        /**
        * Store the specified octets at the specified offset within the
        * buffer.
        *
        * For purposes of side-effects and potential exceptions, this method
        * is functionally equivalent to the following code:
        * <pre><code>
        * write(ofDest, vabSrc, 0, vabSrc.length);
        * </code></pre>
        *
        * @param ofDest  the offset within this buffer to store the passed
        *                data
        * @param vabSrc  the array of octets to store in this buffer
        */
        virtual void write(size32_t ofDest, Array<octet_t>::View vabSrc) = 0;

        /**
        * Store the specified number of octets from the specified location
        * within the passed octet array at the specified offset within this
        * buffer.
        *
        * As a result of this method, the buffer length as reported by the
        * <tt>#length()</tt> method will become
        * <tt>max(#length(), ofDest + cbSrc)</tt>.
        *
        * As a result of this method, the buffer capacity as reported by the
        * <tt>#getCapacity()</tt> method will not change if the new
        * value returned by <tt>#length()</tt> would not exceed the
        * old value returned by <tt>#getCapacity()</tt>; otherwise,
        * the capacity will be increased such that <tt>{#getCapacity()
        * >= #length()</tt>. Regardless, it is always true that
        * <tt>#getCapacity() >= #length()</tt> and <tt>
        * #getMaximumCapacity() >= #getCapacity()</tt>. If the buffer
        * capacity cannot be increased due to resource constraints, an
        * undesignated Exception will be thrown.
        *
        * @param ofDest  the offset within this buffer to store the passed
        *                data
        * @param vabSrc  the array containing the octets to store in this
        *                buffer
        * @param ofSrc   the offset within the passed octet array to copy
        *                from
        * @param cbSrc   the number of octets to copy from the passed octet
        *                array
        *
        * @throws NullPointerException if <code>vabSrc</code> is NULL
        * @throws IndexOutOfBoundsException if <tt>ofSrc + cbSrc</tt> is
        *         greater than <tt>vabSrc->length</tt>, or if
        *         <tt>ofDest + cbSrc</tt> is greater than
        *         <tt>#getMaximumCapacity()</tt>
        */
        virtual void write(size32_t ofDest, Array<octet_t>::View vabSrc,
                size32_t ofSrc, size32_t cbSrc) = 0;

        /**
        * Store the contents of the specified ReadBuffer at the specified
        * offset within this buffer.
        *
        * For purposes of side-effects and potential exceptions, this method
        * is functionally equivalent to the following code:
        * <pre><code>
        * Array<octet_t>::View vabSrc = vBufSrc->toOctetArray();
        * write(ofDest, vabSrc, 0, vabSrc->length);
        * </code></pre>
        *
        * @param ofDest   the offset within this buffer to store the passed
        *                 data
        * @param vBufSrc  the array of octets to store in this buffer
        */
        virtual void write(size32_t ofDest, ReadBuffer::View vBufSrc) = 0;

        /**
        * Store the specified portion of the contents of the specified
        * ReadBuffer at the specified offset within this buffer.
        *
        * For purposes of side-effects and potential exceptions, this method
        * is functionally equivalent to the following code:
        * <pre><code>
        * Array<octet_t>::View vabSrc = vbufSrc.toOctetArray(ofSrc, cbSrc);
        * write(ofDest, vabSrc, 0, vabSrc->length);
        * </code></pre>
        *
        * @param ofDest   the offset within this buffer to store the passed
        *                 data
        * @param vBufSrc  the source ReadBuffer
        * @param ofSrc    the offset within the passed ReadBuffer to copy
        *                 from
        * @param cbSrc    the number of octets to copy from the passed
        *                 ReadBuffer
        */
        virtual void write(size32_t ofDest, ReadBuffer::View vBufSrc,
                size32_t ofSrc, size32_t cbSrc) = 0;

        /**
        * Starting with the octet at offset <tt>of</tt>, retain the remainder
        * of this WriteBuffer, such that the octet at offset <tt>of</tt> is
        * shifted to offset 0, the octet at offset <tt>of + 1</tt> is shifted
        * to offset 1, and so on up to the octet at offset
        * <tt>#length() - 1</tt>, which is shifted to offset
        * <tt>#length() - of - 1</tt>. After this method, the length
        * of of the buffer as indicated by the #length() method will
        * be equal to <tt>#length() - of</tt>.
        *
        * This method is functionally equivalent to the following code:
        * <pre><code>
        * retain(of, length() - of);
        * </code></pre>
        *
        * @param of  the offset of the first octet within the WriteBuffer
        *            that will be retained
        *
        * @throws IndexOutOfBoundsException if <tt>of</tt> is greater than
        *         <tt>#length()</tt>
        */
        virtual void retain(size32_t of) = 0;

        /**
        * Starting with the octet at offset <tt>of</tt>, retain <tt>cb</tt>
        * octets in this WriteBuffer, such that the octet at offset
        * <tt>of</tt> is shifted to offset 0, the octet at offset <tt>of +
        * 1</tt> is shifted to offset 1, and so on up to the octet at offset
        * <tt>of + cb - 1</tt>, which is shifted to offset <tt>cb - 1</tt>.
        * After this method, the length of the buffer as indicated by the
        * #length() method will be equal to <tt>cb</tt>.
        *
        * Legal values for the offset of the first octet to retain
        * <tt>of</tt> are <tt>(of >= 0 && of <= #length())</tt>.
        * Legal values for the number of octets to retain <tt>cb</tt> are
        * <tt>(cb >= 0 && cb <= #length())</tt>, such that
        * <tt>(of + cb <= #length())</tt>.
        *
        * If <tt>cb</tt> is zero, then this method will have the same effect
        * as clear. If <tt>of</tt> is zero, then this method will have the
        * effect of truncating the data in the buffer, but no octets will be
        * shifted within the buffer.
        *
        * The effect on the capacity of the buffer is implementation-
        * specific; some implementations are expected to retain the same
        * capacity while others are expected to shrink accordingly.
        *
        * @param of  the offset of the first octet within the WriteBuffer
        *            that will be retained
        * @param cb  the number of octets to retain
        *
        * @throws IndexOutOfBoundsException if <tt>of + cb</tt> is greater
        *         than <tt>#length()</tt>
        */
        virtual void retain(size32_t of, size32_t cb) = 0;

        /**
        * Set the length of the buffer as indicated by the #length()
        * method to zero.
        *
        * The effect on the capacity of the buffer is implementation-specific;
        * some implementations are expected to retain the same capacity while
        * others are expected to shrink accordingly.
        */
        virtual void clear() = 0;

        /**
        * Obtain a WriteBuffer starting at a particular offset within this
        * WriteBuffer.
        *
        * This is functionally equivalent to:
        * <code><pre>
        * return getWriteBuffer(of, getMaximumCapacity() - of);
        * </pre></code>
        *
        * @param of  the beginning index, inclusive
        *
        * @return a WriteBuffer that represents a portion of this WriteBuffer
        *
        * @throws IndexOutOfBoundsException if <tt>of</tt> is larger than the
        *         <tt>#getMaximumCapacity()</tt> of this WriteBuffer
        */
        virtual WriteBuffer::Handle getWriteBuffer(size32_t of) = 0;

        /**
        * Obtain a WriteBuffer for a portion of this WriteBuffer.
        *
        * Use of the resulting buffer will correspond to using this buffer
        * directly but with the offset being passed to the buffer methods
        * automatically having <tt>of</tt> added. As a result, the length of
        * this buffer can be modified by writing to the new buffer; however,
        * changes made directly to this buffer will not affect the length of
        * the new buffer.
        *
        * Note that the resulting WriteBuffer is limited in the number of
        * octets that can be written to it; in other words, its
        * <tt>#getMaximumCapacity()</tt> must return the same value
        * as was passed in <tt>cb</tt>.
        *
        * @param of  the offset of the first octet within this WriteBuffer to
        *            map to offset 0 of the new WriteBuffer
        * @param cb  the number of octets to cover in the resulting
        *            WriteBuffer
        *
        * @return a WriteBuffer that represents a portion of this WriteBuffer
        *
        * @throws IndexOutOfBoundsException  if <tt>of + cb</tt> is larger
        *         than the <tt>#getMaximumCapacity()</tt> of this
        *         WriteBuffer
        */
        virtual WriteBuffer::Handle getWriteBuffer(size32_t of, size32_t cb) = 0;

        /**
        * Get a ReadBuffer object that is a snapshot of this WriteBuffer's
        * data.
        *
        * This method is functionally equivalent to the following code:
        * <pre><code>
        * Array<octet_t>::View vab = toOctetArray();
        * return OctetArrayReadBuffer::create(vab, 0, vab->length(), true);
        * </code></pre>
        *
        * @return a ReadBuffer that reflects the point-in-time contents of
        *         this WriteBuffer; the returned ReadBuffer is inherently
        *         immutable
        */
        virtual ReadBuffer::View getReadBuffer() const = 0;

        /**
        * Get a ReadBuffer object to read data from this buffer. This method
        * is not guaranteed to return a snapshot of this buffer's data, nor
        * is it guaranteed to return a live view of this buffer, which means
        * that subsequent changes to this WriteBuffer may or may not affect
        * the contents and / or the length of the returned ReadBuffer.
        *
        * To get a snapshot, use the #getReadBuffer() method.
        *
        * @return a ReadBuffer that reflects the contents of this WriteBuffer
        *         but whose behavior is undefined if the WriteBuffer is
        *         modified
        */
        virtual ReadBuffer::View getUnsafeReadBuffer() const = 0;

        /**
        * Returns an octet array that holds the complete contents of this
        * WriteBuffer.
        *
        * This method is functionally equivalent to the following code:
        * <pre><code>
        * return getUnsafeReadBuffer()->toOctetArray();
        * </code></pre>
        *
        * @return the contents of this WriteBuffer as an octet array
        */
        virtual Array<octet_t>::View toOctetArray() const = 0;

        /**
        * Returns a new Binary object that holds the complete contents of
        * this WriteBuffer.
        * <p>
        * This method is functionally equivalent to the following code:
        * <pre><code>
        * return getUnsafeReadBuffer()->toBinary();
        * </code></pre>
        *
        * @return the contents of this WriteBuffer as a Binary object
        *
        * @since Coherence 3.7.1
        */
        virtual BinaryView toBinary() const = 0;


    // ----- BufferOutput inner interface -----------------------------------

    public:
        /**
        * The BufferOutput interface represents a data output stream on top
        * of a WriteBuffer.
        *
        * @author jh  2007.12.20
        */
        class COH_EXPORT BufferOutput
            : public interface_spec<BufferOutput>
            {
            // ----- BufferOutput interface -----------------------------

            public:
                /**
                * Get the WriteBuffer object that this BufferOutput is
                * writing to.
                *
                * @return the underlying WriteBuffer object
                */
                virtual WriteBuffer::Handle getBuffer() = 0;

                /**
                * Get the WriteBuffer object that this BufferOutput is
                * writing to.
                *
                * @return the underlying WriteBuffer object
                */
                virtual WriteBuffer::View getBuffer() const = 0;

                /**
                * Determine the current offset of this BufferOutput within
                * the underlying WriteBuffer.
                *
                * @return the offset of the next octet to write to the
                *         WriteBuffer
                */
                virtual size32_t getOffset() const = 0;

                /**
                * Specify the offset of the next octet to write to the
                * underlying WriteBuffer.
                *
                * @param of  the offset of the next octet to write to the
                *            WriteBuffer
                *
                * @throws IndexOutOfBoundsException if
                *         <code>of > getBuffer()->getMaximumCapacity()</code>
                */
                virtual void setOffset(size32_t of) = 0;

                /**
                * Write the given octet.
                *
                * @param b  the octet to write
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void write(octet_t b) = 0;

                /**
                * Writes all the octets in the array <tt>vab</tt>.
                *
                * @param vab the octet array to write
                *
                * @throws IOException if an I/O error occurs
                * @throws NullPointerException if <tt>vab</tt> is NULL
                */
                virtual void write(Array<octet_t>::View vab) = 0;

                /**
                * Writes <code>cb</code> octets starting at offset
                * <code>of</code> from the array <code>vab</code>.
                *
                * @param vab  the octet array to write from
                * @param of   the offset into <code>vab</code> to start
                *             writing from
                * @param cb   the number of octets from <code>vab</code> to
                *             write
                *
                * @throws IOException if an I/O error occurs
                * @throws NullPointerException if <code>vab</code> is NULL
                * @throws IndexOutOfBoundsException if <code>of+cb</code> is
                *         greater than <code>vab->length</code>
                */
                virtual void write(Array<octet_t>::View vab, size32_t of,
                        size32_t cb) = 0;

                /**
                * Write all the octets from the passed ReadBuffer object.
                *
                * This is functionally equivalent to the following code:
                * <code><pre>
                * getBuffer()->write(getOffset(), vBuf);
                * </pre></code>
                *
                * @param vBuf  a ReadBuffer object
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeBuffer(ReadBuffer::View vBuf) = 0;

                /**
                * Write <code>cb</code> octets from the passed ReadBuffer
                * object starting at offset <code>of</code> within the passed
                * ReadBuffer.
                *
                * This is functionally equivalent to the following code:
                * <code><pre>
                * getBuffer()->write(getOffset(), vbuf, of, cb);
                * </pre></code>
                *
                * @param vBuf  a ReadBuffer object
                * @param of    the offset within the ReadBuffer of the first
                *              octet to write to this BufferOutput
                * @param cb    the number of octets to write
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeBuffer(ReadBuffer::View vBuf, size32_t of,
                        size32_t cb) = 0;

                /**
                * Write a boolean value.
                *
                * @param f  the <tt>bool</tt> to be written
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeBoolean(bool f) = 0;

                /**
                * Write a 16-bit Unicode character value.
                *
                * @param ch  the Unicode character as a <code>wchar16_t</code>
                *            value
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeChar16(wchar16_t ch) = 0;

                /**
                * Write a sequence of UTF-8 encoded 16-bit Unicode
                * characters.
                *
                * @param vs  a String value to write; may be NULL
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeString(String::View vs) = 0;

                /**
                * Write a 16-bit integer value.
                *
                * @param n  the <code>int16_t</code> value to be written
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeInt16(int16_t n) = 0;

                /**
                * Write a 32-bit integer value.
                *
                * @param n  the <code>int32_t</code> value to be written
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeInt32(int32_t n) = 0;

                /**
                * Write a 64-bit integer value.
                *
                * @param n  the <code>int64_t</code> value to be written
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeInt64(int64_t n) = 0;

                /**
                * Write a 32-bit floating-point value.
                *
                * @param fl  the <code>float32_t</code> value to be written
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeFloat32(float32_t fl) = 0;

                /**
                * Write a 64-bit floating-point value.
                *
                * @param dfl  the <code>float64_t</code> value to be written
                *
                * @throws IOException if an I/O error occurs
                */
                virtual void writeFloat64(float64_t dfl) = 0;
            };

        /**
        * Get a BufferOutput object to write data to this buffer starting at
        * a particular offset.
        *
        * Note that each call to this method will return a new BufferOutput
        * object, with the possible exception being that a zero-length
        * non-resizing WriteBuffer could always return the same instance
        * (since it is not writable).
        *
        * @param of  the offset of the first octet of this buffer that the
        *            BufferOutput will write to
        *
        * @return a BufferOutput that will write to this buffer
        */
        virtual BufferOutput::Handle getBufferOutput(size32_t of = 0) = 0;

        /**
        * Get a BufferOutput object to write data to this buffer. The
        * BufferOutput object returned by this method is set to append to the
        * WriteBuffer, meaning that its offset is pre-set to the
        * #length() of this buffer.
        *
        * This is functionally equivalent to:
        * <pre><code>
        * return getBufferOutput(length());
        * </code></pre>
        *
        * @return a BufferOutput configured to append to this buffer
        */
        virtual BufferOutput::Handle getAppendingBufferOutput() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_WRITE_BUFFER_HPP
