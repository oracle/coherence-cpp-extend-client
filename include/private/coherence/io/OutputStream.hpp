/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OUTPUT_STREAM_HPP
#define COH_OUTPUT_STREAM_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,io)

/**
* OutputStream represents an abstract class to output a stream of octets.
*
* @author ch  2009.09.22
*/
class COH_EXPORT OutputStream
    : public abstract_spec<OutputStream>
    {
    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);


    // ----- OutputStream definition  ---------------------------------------

    public:
        /**
        * Closes this output stream and releases any system resources associated
        * with this stream. The general contract of close is that it closes the
        * output stream. A closed stream cannot perform output operations and
        * cannot be reopened.
        * <p/>
        * The close method of OutputStream does nothing.
        */
        virtual void close();

        /**
        * Flushes this output stream and forces any buffered output octets to
        * be written out.
        * <p/>
        * The flush method of OutputStream does nothing.
        */
        virtual void flush();

        /**
        * Writes cb octets from the specified octet array starting at i
        * to this output stream. The general contract for write(vab, i, cb)
        * is that some of the octets in the array vab are written to the output
        * stream in order; element vab[i] is the first octet written and
        * vab[i + cb - 1] is the last octet written by this operation.
        * <p/>
        *  @param vab  the octet array to write
        *  @param i    the start offset in the data
        *  @param cb   the number of octets to write
        *
        * @throws IOException if an I/O error occurs
        */
        virtual void write(Array<octet_t>::View vab, size32_t i = 0,
                           size32_t cb = npos);

        /**
        * Writes the specified octet to this output stream.
        * <p/>
        * Subclasses of OutputStream must provide an implementation for this
        * method.
        *
        * @param b  the octet to write
        *
        * @throws IOException if an I/O error occurs
        */
        virtual void write(octet_t b) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OUTPUT_STREAM_HPP
