/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/DelegatingWriteBuffer.hpp"

#include "coherence/io/AbstractReadBuffer.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,io)


// ----- constructors -------------------------------------------------------

DelegatingWriteBuffer::DelegatingWriteBuffer(WriteBuffer::Handle hBuf,
        size32_t of, size32_t cb)
    : f_hBuf(self(), hBuf), m_ofStart(of), m_cb(0), m_cbMax(cb)
    {
    }

DelegatingWriteBuffer::DelegatingWriteBuffer(
        const DelegatingWriteBuffer& that)
    : super(that),
      f_hBuf(self(), cast<WriteBuffer::Handle>(that.f_hBuf->clone())),
      m_ofStart(that.m_ofStart),
      m_cb(that.m_cb),
      m_cbMax(that.m_cbMax)
    {
    }

DelegatingWriteBuffer::DelegatingBufferOutput::DelegatingBufferOutput(
        DelegatingWriteBuffer::Handle hBuf)
    : super(hBuf),
      f_hOut(self(), hBuf->f_hBuf->getBufferOutput())
    {
    }


// ----- WriteBuffer interface ----------------------------------------------

size32_t DelegatingWriteBuffer::length() const
    {
    return m_cb;
    }

size32_t DelegatingWriteBuffer::getCapacity() const
    {
    // figure out the capacity by asking the delegatee for its capacity
    // and then subtracting this buffer's offset, but if that is beyond
    // our max capacity then the capacity is actually our max capacity
    return std::min(std::max((size32_t) 0, f_hBuf->getCapacity() - m_ofStart),
            m_cbMax);
    }

size32_t DelegatingWriteBuffer::getMaximumCapacity() const
    {
    return m_cbMax;
    }

void DelegatingWriteBuffer::write(size32_t ofDest, octet_t b)
    {
    checkBounds(ofDest, 1);
    f_hBuf->write(m_ofStart + ofDest, b);
    updateLength(ofDest + 1);
    }

void DelegatingWriteBuffer::write(size32_t ofDest, Array<octet_t>::View vabSrc,
        size32_t ofSrc, size32_t cbSrc)
    {
    checkBounds(ofDest, cbSrc);
    f_hBuf->write(m_ofStart + ofDest, vabSrc, ofSrc, cbSrc);
    updateLength(ofDest + cbSrc);
    }

void DelegatingWriteBuffer::write(size32_t ofDest, ReadBuffer::View vBufSrc,
        size32_t ofSrc, size32_t cbSrc)
    {
    checkBounds(ofDest, cbSrc);
    f_hBuf->write(m_ofStart + ofDest, vBufSrc, ofSrc, cbSrc);
    updateLength(ofDest + cbSrc);
    }

void DelegatingWriteBuffer::retain(size32_t of, size32_t cb)
    {
    if (of + cb > length())
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of << ", cb="
                << cb << ", length()=" << length());
        }
    else if (cb == 0)
        {
        clear();
        }
    else
        {
        // "slide" the desired octets to the start of this buffer
        if (of > 0)
            {
            size32_t ofStart = m_ofStart;
            f_hBuf->write(ofStart, f_hBuf->getUnsafeReadBuffer(), ofStart + of, cb);
            }

        // update length
        m_cb = cb;
        }
    }

void DelegatingWriteBuffer::clear()
    {
    m_cb = 0;
    }

ReadBuffer::View DelegatingWriteBuffer::getUnsafeReadBuffer() const
    {
    size32_t cb = m_cb;
    if (cb == 0)
        {
        return AbstractReadBuffer::getEmptyReadBuffer();
        }
    else
        {
        return f_hBuf->getUnsafeReadBuffer()->getReadBuffer(m_ofStart, cb);
        }
    }

Array<octet_t>::View DelegatingWriteBuffer::toOctetArray() const
    {
    size32_t cb = m_cb;
    if (cb == 0)
        {
        return AbstractReadBuffer::getEmptyOctetArray();
        }
    else
        {
        return f_hBuf->getUnsafeReadBuffer()->toOctetArray(m_ofStart, cb);
        }
    }


// ----- BufferOutput interface ---------------------------------------------

void DelegatingWriteBuffer::DelegatingBufferOutput::write(octet_t b)
    {
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, 1);
    f_hOut->write(b);
    moveOffset(1);
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::write(
        Array<octet_t>::View vab, size32_t of, size32_t cb)
    {
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, cb);
    f_hOut->write(vab, of, cb);
    moveOffset(cb);
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::writeBuffer(
        ReadBuffer::View vBuf)
    {
    size32_t cb = vBuf->length();
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, cb);
    f_hOut->writeBuffer(vBuf);
    moveOffset(cb);
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::writeBuffer(
        ReadBuffer::View vBuf, size32_t of, size32_t cb)
    {
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, cb);
    f_hOut->writeBuffer(vBuf, of, cb);
    moveOffset(cb);
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::writeBoolean(bool f)
    {
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, 1);
    f_hOut->writeBoolean(f);
    moveOffset(1);
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::writeChar16(wchar16_t ch)
    {
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, 2);
    f_hOut->writeChar16(ch);
    moveOffset(2);
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::writeFloat32(float32_t fl)
    {
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, 4);
    f_hOut->writeFloat32(fl);
    moveOffset(4);
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::writeFloat64(float64_t dfl)
    {
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->checkBounds(m_ofWrite, 8);
    f_hOut->writeFloat64(dfl);
    moveOffset(8);
    }


// ----- internal methods ---------------------------------------------------

WriteBuffer::BufferOutput::Handle DelegatingWriteBuffer::instantiateBufferOutput()
    {
    return DelegatingBufferOutput::create(this);
    }

void DelegatingWriteBuffer::checkBounds(size32_t of, size32_t cb) const
    {
    if (of + cb > m_cbMax)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of << ", cb="
                << cb << ", max=" << m_cbMax);
        }
    }

void DelegatingWriteBuffer::updateLength(size32_t cb)
    {
    if (cb > m_cb)
        {
        m_cb = cb;
        }
    }

void DelegatingWriteBuffer::DelegatingBufferOutput::moveOffset(size32_t cb)
    {
    size32_t of = m_ofWrite + cb;
    m_ofWrite = of;
    cast<DelegatingWriteBuffer::Handle>(f_hBuf)->updateLength(of);
    }

COH_CLOSE_NAMESPACE2
