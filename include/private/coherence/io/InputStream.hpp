/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INPUT_STREAM_HPP
#define COH_INPUT_STREAM_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,io)

/*
* InputStream represents an abstract class to read a stream of octets.
*
* @author ch  2009.09.22
*/
class COH_EXPORT InputStream
    : public abstract_spec<InputStream>
    {
    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);


    // ----- InputStream interface ------------------------------------------

    public:
         /**
         * Read an octet value.
         *
         * @return an <code>octet_t</code> value
         *
         * @throws EOFException if the value could not be read
         *         because no more data remains to be read
         * @throws IOException if an I/O error occurs
         */
         virtual octet_t read() = 0;

         /**
         * Reads up to cb octets of data from the input stream into an array
         * of octets. An attempt is made to read as many as cb octets, but a
         * smaller number may be read.
         * <p/>
         * The read(vab, i, cb) method for class InputStream simply calls
         * the method read() repeatedly. If the first such call results in an
         * IOException, that exception is returned from the call to the
         * read(vab, i, cb) method. If any subsequent call to
         * read() results in a IOException, the exception is caught and
         * treated as if it were end of file; the octets read up to that point
         * are stored into hab and the number of octets read before the
         * exception occurred is returned. The default implementation of this
         * method blocks until the requested amount of input data cb has been
         * read, end of file is detected, or an exception is thrown.
         * Subclasses are encouraged to provide a more efficient implementation
         * of this method.
         *
         * @param hab  the array to store the octets which are read from the
         *             stream
         * @param i    the start offset in array hab at which the data is
         *             written
         * @param cb   the maximum number of octets to read, or npos to read
         *             hab->length - i
         *
         * @throws EOFException  if the stream is exhausted before the number
         *                       of octets indicated by the array length could
         *                       be read
         * @throws IOException   if an I/O error occurs
         */
         virtual size32_t read(Array<octet_t>::Handle hab, size32_t i = 0,
                 size32_t cb = npos);

         /**
         * Reads cb octets from an input stream. This method blocks until one
         * of the following conditions occurs:
         *
         * <ul>cb octets of input data are available, in which case a normal
         * return is made.</ul>
         * <ul>End of file is detected, in which case an EOFException is
         * thrown.</ul>
         * <ul>An I/O error occurs, in which case an IOException other than
         * EOFException is thrown.</ul>
         *
         * @param hab  the array to store the octets which are read from the
         *             stream
         * @param i    the start offset in array hab at which the data is
         *             written
         * @param cb   the maximum number of octets to read, or npos to read
         *             hab->length - i
         *
         * @throws EOFException  if the stream is exhausted before the number
         *                       of octets indicated by the array length could
         *                       be read
         * @throws IOException   if an I/O error occurs
         */
         virtual void readFully(Array<octet_t>::Handle hab, size32_t i = 0,
                 size32_t cb = npos);

         /**
         * Closes this input stream and releases any system resources
         * associated with the stream.
         */
         virtual void close();

         /**
         * Returns an estimate of the number of octets that can be read (or
         * skipped over) from this input stream without blocking by the next
         * invocation of a method for this input stream. The next invocation
         * might be the same thread or another thread. A single read or skip
         * of this many octets will not block, but may read or skip fewer bytes.
         * <p/>
         * Note that while some implementations of InputStream will return the
         * total number of octets in the stream, many will not. It is never
         * correct to use the return value of this method to allocate a buffer
         * intended to hold all data in this stream.
         * <p/>
         * A subclass' implementation of this method may choose to throw an
         * IOException if this input stream has been closed by invoking the
         * close() method.
         * <p/>
         * The available method for class InputStream always returns 0.
         * <p/>
         * This method should be overridden by subclasses.
         *
         * @returns an estimate of the number of bytes.
         */
         virtual size32_t available();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INPUT_STREAM_HPP
