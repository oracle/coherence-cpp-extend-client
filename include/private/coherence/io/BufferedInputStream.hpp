/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BUFFERED_INPUT_STREAM_HPP
#define COH_BUFFERED_INPUT_STREAM_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/InputStream.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

/**
* BufferedInputStream provides a buffered variant of an InputStream
*
* @author ch  2009.09.22
*/
class COH_EXPORT BufferedInputStream
    : public class_spec<BufferedInputStream,
        extends<InputStream> >
    {
    friend class factory<BufferedInputStream>;

    // ----- constructors  --------------------------------------------------

    protected:
        /**
        * Creates a <code>BufferedInputStream</code>
        * with the specified buffer size,
        * and saves its  argument, the input stream
        * <code>in</code>, for later use.  An internal
        * buffer array of length  <code>size</code>
        * is created and stored in <code>buf</code>.
        *
        * @param   hIn  the underlying input stream
        * @param   cb   the buffer size
        */
        BufferedInputStream(InputStream::Handle hIn, size32_t cb = 8192);


    // ----- InputStream definition  --------------------------------------

     public:
        /**
        * {@inheritDoc}
        */
        virtual void close();

        /**
        * {@inheritDoc}
        */
        virtual size32_t read(Array<octet_t>::Handle hab, size32_t i = 0,
                size32_t cb = npos);

        /**
        * {@inheritDoc}
        */
        virtual octet_t read();

        /**
        * {@inheritDoc}
        */
        virtual size32_t available();

    // ----- helpers  -------------------------------------------------------


    private:
        /**
        * Read characters into a portion of an array, reading from the
        * underlying stream at most once if necessary.
        */
        size32_t readInternal(Array<octet_t>::Handle hab, size32_t i,
                size32_t cb);

        /**
        * Fills the buffer with more data, taking into account
        * shuffling and other tricks for dealing with marks.
        * Assumes that it is being called by a synchronized method.
        * This method also assumes that all data has already been read in,
        * hence m_iNext > m_cb.
        *
        * @return the number of additionally buffered octets
        */
        size32_t fillBuffer();


    // ----- data members  --------------------------------------------------

    protected:
        /**
        * The input stream from which to buffer the data.
        */
        FinalHandle<InputStream> f_hIn;

        /**
        * The internal buffer where data is stored.
        */
        FinalHandle<Array<octet_t> > f_habBuffer;

        /**
        * The index one greater than the index of the last valid octet in
        * the buffer.
        */
        size32_t m_cb;

        /**
        * The current position in the buffer. This is the index of the next
        * character to be read from the <code>m_habBuffer</code> array.
        */
        size32_t m_iNext;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_BUFFERED_INPUT_STREAM_H_
