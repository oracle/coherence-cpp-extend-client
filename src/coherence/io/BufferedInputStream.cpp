/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/BufferedInputStream.hpp"

#include <iostream>
COH_OPEN_NAMESPACE2(coherence,io)

BufferedInputStream::BufferedInputStream(InputStream::Handle hIn, size32_t cb)
    : f_hIn(self(), hIn), f_habBuffer(self(), Array<octet_t>::create(cb)),
      m_cb(0),  m_iNext(0)
    {
    }

size32_t BufferedInputStream::fillBuffer()
    {
    size32_t cb = m_cb - m_iNext;
    if (cb == 0)
        {
        Array<octet_t>::Handle habBuffer = f_habBuffer;
        m_iNext = 0;
        try
            {
            return m_cb = f_hIn->read(habBuffer, 0, habBuffer->length);
            }
        catch (Exception::View e)
            {
            // We must update m_cb to ensure that it only reflects buffered bytes
            // it is possible that the call to read buffered some unknown number
            // of additional bytes.  These additional bytes can be safely ignored.

            // In the case of an EOFException there would have to have been no
            // additional bytes buffered, per the contract of read()

            // In the case of some other Exception it doesn't really matter how
            // many bytes were buffered, the underlying stream is screwed, and we
            // can only rely on what we'd previously buffered.

            m_cb = 0;
            throw;
            }
        }
    return cb;
    }

octet_t BufferedInputStream::read()
    {
    if (m_iNext == m_cb && fillBuffer() == 0)
        {
        // nothing is available on buffer or stream; could be EOF; could simply
        // be pending (socket); block on the underlying stream
        return f_hIn->read();
        }
    return ((Array<octet_t>::View) f_habBuffer)[m_iNext++];
    }

size32_t BufferedInputStream::read(Array<octet_t>::Handle hab,
        size32_t i, size32_t cb)
    {
    size32_t cbBuffer = hab->length;

    if (cb == npos)
        {
        cb = cbBuffer - i;
        }

    if (i + cb > cbBuffer)
        {
        COH_THROW (IndexOutOfBoundsException::create());
        }

    size32_t cRead      = 0;
    size32_t cReadTotal = 0;

    while ((cRead = readInternal(hab, i + cReadTotal, cb - cReadTotal)) > 0)
        {
        if ((cReadTotal += cRead) == cb)
            {
            break;
            }
        }
    return cReadTotal;
    }

size32_t BufferedInputStream::readInternal(Array<octet_t>::Handle hab,
        size32_t i, size32_t cb)
    {
    Array<octet_t>::View vab         = f_habBuffer;
    size32_t             cbAvailable = m_cb - m_iNext;
    size32_t             cbReadTotal = 0;

    // copy the content over the available octets to the buffer if it would
    // fit into the requested cb octets.
    if (cbAvailable > 0)
        {
        cbReadTotal = cbAvailable < cb ? cbAvailable : cb;

        Array<octet_t>::copy(vab, m_iNext, hab, i, cbReadTotal);
        m_iNext += cbReadTotal;
        if (cbReadTotal == cb)
            {
            return cb;
            }

        cbAvailable -= cbReadTotal;
        }

    // buffer has been emptied; if the request size is greater then buffer
    // size then simply read into hab and avoid the buffer until the final
    // read
    size32_t cbBuffer = vab->length;
    while (cb - cbReadTotal >= cbBuffer)
        {
        size32_t cbRead = f_hIn->read(hab, i + cbReadTotal,
                cb - cbReadTotal);
        cbReadTotal += cbRead;
        if (cbRead == 0 || cbReadTotal == cb )
            {
            // underlying stream is exhausted; or done
            return cbReadTotal;
            }

        }

    // finally try to fill the buffer as much as possible
    if ((cbAvailable = fillBuffer()) > 0)
        {
        // if there are less bytes available in the buffer we will not
        // fully comply with the request and just turn around with the
        // the number of bytes available.
        cb = cbAvailable < cb - cbReadTotal
            ? cbAvailable : cb - cbReadTotal;

        // copy 'cb' bytes from the buffer
        Array<octet_t>::copy(vab, m_iNext, hab, i + cbReadTotal, cb);
        m_iNext += cb;
        cbReadTotal += cb;
        }

    return cbReadTotal;
    }

void BufferedInputStream::close()
    {
    m_cb = m_iNext = 0;
    f_hIn->close();
    }

size32_t BufferedInputStream::available()
    {
    return f_hIn->available() + (m_cb - m_iNext);
    }

COH_CLOSE_NAMESPACE2
