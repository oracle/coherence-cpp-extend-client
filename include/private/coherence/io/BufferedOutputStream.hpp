/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BUFFERED_OUTPUTSTREAM_HPP
#define COH_BUFFERED_OUTPUTSTREAM_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/OutputStream.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

/**
* BufferedOutputStream provides a buffered variant of an OutputStream
*
* @author ch  2009.09.22
*/
class COH_EXPORT BufferedOutputStream
    : public class_spec<BufferedOutputStream,
        extends<OutputStream> >
    {
    friend class factory<BufferedOutputStream>;

    // ----- constructors  --------------------------------------------------

    protected:
        /**
        * Creates a new buffered output stream to write data to the specified
        * underlying output stream with the specified buffer size.
        *
        * @param hOut  the underlying output stream
        * @param cb    the size of the buffer
        *
        * @exception IllegalArgumentException if size &lt;= 0.
        */
        BufferedOutputStream(OutputStream::Handle hOut, size32_t cb = 8192);


    // ----- OutputStream definition  ---------------------------------------

     public:
        /**
        * {@inheritDoc}
        */
        virtual void close();

        /**
        * {@inheritDoc}
        */
        virtual void flush();

        /**
        * {@inheritDoc}
        */
        virtual void write(Array<octet_t>::View vab, size32_t i = 0,
                size32_t cb = npos);

        /**
        * {@inheritDoc}
        */
        virtual void write(octet_t b);


    // ----- helpers --------------------------------------------------------

     private:
        /**
        * Flush the internal buffer
        */
        void flushBuffer();


    // ----- data members  --------------------------------------------------

    protected:
        /**
        * The output stream.
        */
        FinalHandle<OutputStream> f_hOut;

        /**
        * The internal m_habBufferfer where data is stored.
        */
        FinalHandle<Array<octet_t> > f_habBuffer;

        /**
        * The number of valid octets in the buffer. This value is always in the
        * range 0 through m_habBuffer->length; elements m_habBuffer[0] through
        * m_habBuffer[m_cb-1] contain valid octet data.
        */
        size32_t m_cb;
    };

COH_CLOSE_NAMESPACE2

#endif //COH_BUFFERED_OUTPUTSTREAM_HPP
