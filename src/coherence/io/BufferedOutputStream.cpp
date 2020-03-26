/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/BufferedOutputStream.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <iostream>

COH_OPEN_NAMESPACE2(coherence,io)

BufferedOutputStream::BufferedOutputStream(OutputStream::Handle hOut,
                                           size32_t cb)
    : f_hOut(self(), hOut), f_habBuffer(self(), Array<octet_t>::create(cb)),
      m_cb(0)
    {
    }

void BufferedOutputStream::write(octet_t b)
    {
    Array<octet_t>::Handle hab = f_habBuffer;
    if (m_cb == hab->length)
        {
        flushBuffer();
        }
    hab[m_cb++] = b;
    }

void BufferedOutputStream::write(Array<octet_t>::View vab,
        size32_t i, size32_t cb)
    {
    if (cb == npos)
        {
        cb = vab->length - i;
        }

    Array<octet_t>::Handle hab        = f_habBuffer;
    size32_t               cbBuffer   = hab->length;
    size32_t               cbBuffered = m_cb;
    size32_t               cbFree     = cbBuffer - cbBuffered;

    // primary goal is to minimize the number of flush and write calls
    // secondary goal is to maximize the size of each write
    if (cb < cbFree)
        {
        // input fits in existing buffer; just buffer the entire contents
        Array<octet_t>::copy(vab, i, hab, cbBuffered, cb); // buffer
        m_cb = cbBuffered + cb;
        }
    else if (cbBuffered == 0)
        {
        // nothing is buffered, and we dont' fit in the buffer; just write
        f_hOut->write(vab, i, cb); // write
        }
    else if (cb < cbFree + cbBuffer)
        {
        // 1 write is possible and we have buffered data; fill; flush; buffer
        Array<octet_t>::copy(vab, i, hab, cbBuffered, cbFree); // fill
        f_hOut->write(hab); // flush entire buffer
        Array<octet_t>::copy(vab, i + cbFree, hab, 0, cb - cbFree); // buffer
        m_cb = cb - cbFree;
        }
    else
        {
        // 2 write are be required; just flush; then write
        // note we don't fill the buffer as this would just be a needless array copy
        flushBuffer(); // partial flush
        f_hOut->write(vab, i, cb); // write
        }
    }

void BufferedOutputStream::flush()
    {
    flushBuffer();
    f_hOut->flush();
    }

void BufferedOutputStream::close()
    {
    flush();
    f_hOut->close();
    }

void BufferedOutputStream::flushBuffer()
    {
    if (m_cb > 0)
        {
        f_hOut->write(f_habBuffer, 0, m_cb);
        m_cb = 0;
        }
    }

COH_CLOSE_NAMESPACE2
