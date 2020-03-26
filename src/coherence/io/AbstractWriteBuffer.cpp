/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/AbstractWriteBuffer.hpp"

#include "coherence/io/DelegatingWriteBuffer.hpp"
#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/UTFDataFormatException.hpp"
#include "coherence/util/Binary.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,io)

using coherence::util::Binary;


// ----- constants ----------------------------------------------------------

static const size32_t MIN_BUF = 64;


// ----- constructors -------------------------------------------------------

AbstractWriteBuffer::AbstractWriteBuffer()
        : m_habBuf(self())
    {
    }

AbstractWriteBuffer::AbstractWriteBuffer(const AbstractWriteBuffer& /*that*/)
        : m_habBuf(self())
    {
    }

AbstractWriteBuffer::AbstractBufferOutput::AbstractBufferOutput(
        AbstractWriteBuffer::Handle hBuf)
        : f_hBuf(self(), hBuf), m_ofWrite(0)
    {
    }

// ----- WriteBuffer interface ----------------------------------------------

size32_t AbstractWriteBuffer::getMaximumCapacity() const
    {
    return getCapacity();
    }

void AbstractWriteBuffer::write(size32_t ofDest, Array<octet_t>::View vabSrc)
    {
    write(ofDest, vabSrc, 0, vabSrc->length);
    }

void AbstractWriteBuffer::write(size32_t ofDest, ReadBuffer::View vBufSrc)
    {
    write(ofDest, vBufSrc, 0, vBufSrc->length());
    }

void AbstractWriteBuffer::write(size32_t ofDest, ReadBuffer::View vBufSrc,
        size32_t ofSrc, size32_t cbSrc)
    {
    // this is inefficient; sub-classes should override this
    write(ofDest, vBufSrc->toOctetArray(ofSrc, cbSrc), 0, cbSrc);
    }

void AbstractWriteBuffer::retain(size32_t of)
    {
    retain(of, length() - of);
    }

void AbstractWriteBuffer::clear()
    {
    retain(0, 0);
    }

WriteBuffer::Handle AbstractWriteBuffer::getWriteBuffer(size32_t of)
    {
    return getWriteBuffer(of, getMaximumCapacity() - of);
    }

WriteBuffer::Handle AbstractWriteBuffer::getWriteBuffer(size32_t of, size32_t cb)
    {
    if (of + cb > getMaximumCapacity())
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of
                << ", cb=" << cb << ", max=" << getMaximumCapacity());
        }

    return DelegatingWriteBuffer::create(this, of, cb);
    }

ReadBuffer::View AbstractWriteBuffer::getReadBuffer() const
    {
    Array<octet_t>::View vab = toOctetArray();
    return OctetArrayReadBuffer::create(vab, 0, vab->length, true);
    }

Array<octet_t>::View AbstractWriteBuffer::toOctetArray() const
    {
    return getUnsafeReadBuffer()->toOctetArray();
    }

Binary::View AbstractWriteBuffer::toBinary() const
    {
    return getUnsafeReadBuffer()->toBinary();
    }

WriteBuffer::BufferOutput::Handle AbstractWriteBuffer::getBufferOutput(size32_t of)
    {
    BufferOutput::Handle hOut = instantiateBufferOutput();
    hOut->setOffset(of);
    return hOut;
    }

WriteBuffer::BufferOutput::Handle AbstractWriteBuffer::getAppendingBufferOutput()
    {
    return getBufferOutput(length());
    }


// ----- BufferOutput interface ---------------------------------------------

WriteBuffer::Handle AbstractWriteBuffer::AbstractBufferOutput::getBuffer()
    {
    return f_hBuf;
    }

WriteBuffer::View AbstractWriteBuffer::AbstractBufferOutput::getBuffer() const
    {
    return f_hBuf;
    }

size32_t AbstractWriteBuffer::AbstractBufferOutput::getOffset() const
    {
    return m_ofWrite;
    }

void AbstractWriteBuffer::AbstractBufferOutput::setOffset(size32_t of)
    {
    if (of > f_hBuf->getMaximumCapacity())
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of
                << "min=0, max=" << f_hBuf->getMaximumCapacity());
        }

    m_ofWrite = of;
    }

void AbstractWriteBuffer::AbstractBufferOutput::write(octet_t b)
    {
    size32_t ofWrite = m_ofWrite;
    f_hBuf->write(ofWrite, b);
    m_ofWrite = ofWrite + 1;
    }

void AbstractWriteBuffer::AbstractBufferOutput::write(Array<octet_t>::View vab)
    {
    write(vab, 0, vab->length);
    }

void AbstractWriteBuffer::AbstractBufferOutput::write(Array<octet_t>::View vab,
        size32_t of, size32_t cb)
    {
    size32_t ofWrite = m_ofWrite;
    f_hBuf->write(ofWrite, vab, of, cb);
    m_ofWrite = ofWrite + cb;
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeBuffer(
        ReadBuffer::View vBuf)
    {
    writeBuffer(vBuf, 0, vBuf->length());
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeBuffer(
        ReadBuffer::View vBuf, size32_t of, size32_t cb)
    {
    size32_t ofWrite = m_ofWrite;
    f_hBuf->write(ofWrite, vBuf, of, cb);
    m_ofWrite = ofWrite + cb;
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeBoolean(bool f)
    {
    write(f ? (octet_t) 1 : (octet_t) 0);
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeChar16(wchar16_t ch)
    {
    Array<octet_t>::Handle hab = f_hBuf->tmpbuf();

    if (ch <= 0x007F)
        {
        // 1-octet format:  0xxx xxxx
        write((octet_t) ch);
        }
    else if (ch <= 0x07FF)
        {
        // 2-octet format:  110x xxxx, 10xx xxxx
        hab[0] = (octet_t) (0xC0 | ((ch >> 6) & 0x1F));
        hab[1] = (octet_t) (0x80 | ((ch     ) & 0x3F));
        write(hab, 0, 2);
        }
    else
        {
        // 3-octet format:  1110 xxxx, 10xx xxxx, 10xx xxxx
        hab[0] = (octet_t) (0xE0 | ((ch >> 12) & 0x0F));
        hab[1] = (octet_t) (0x80 | ((ch >>  6) & 0x3F));
        hab[2] = (octet_t) (0x80 | ((ch      ) & 0x3F));
        write(hab, 0, 3);
        }
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeString(String::View vs)
    {
    if (NULL == vs)
        {
        writeInt32(-1);
        }
    else
        {
        // retrieving a NUL terminated octet array from String is free
        Array<octet_t>::View vab = vs->getOctets();

        size32_t cb = vab->length - 1;
        writeInt32((int32_t) cb);

        if (cb > 0)
            {
            for (size32_t i = 0; i < cb; ++i)
                {
                // TODO: consider iterating over raw array, this is potentially
                // performance critical
                write(vab[i]);
                }
            }
        }
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeInt16(int16_t n)
    {
    Array<octet_t>::Handle hab = f_hBuf->tmpbuf();

    // first octet contains sign bit (bit 7 set if neg)
    octet_t  b  = 0;
    uint8_t  cb = 0;
    uint16_t un = n;
    if (n < 0)
        {
        b  = 0x40;
        un = ~un;
        }

    // first octet contains only 6 data bits
    b |= (octet_t) (un & 0x3F);
    un >>= 6;

    while (un != 0)
        {
        b |= 0x80;          // bit 8 is a continuation bit
        hab[cb++] = b;

        b = (octet_t) (un & 0x7F);
        un >>= 7;
        }

    if (cb == 0)
        {
        // one-octet format
        write(b);
        }
    else
        {
        hab[cb++] = b;
        write(hab, 0, cb);
        }
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeInt32(int32_t n)
    {
    Array<octet_t>::Handle hab = f_hBuf->tmpbuf();

    // first octet contains sign bit (bit 7 set if neg)
    octet_t  b  = 0;
    uint8_t  cb = 0;
    uint32_t un = n;
    if (n < 0)
        {
        b  = 0x40;
        un = ~un;
        }

    // first octet contains only 6 data bits
    b |= (octet_t) (un & 0x3F);
    un >>= 6;

    while (un != 0)
        {
        b |= 0x80;          // bit 8 is a continuation bit
        hab[cb++] = b;

        b = (octet_t) (un & 0x7F);
        un >>= 7;
        }

    if (cb == 0)
        {
        // one-octet format
        write(b);
        }
    else
        {
        hab[cb++] = b;
        write(hab, 0, cb);
        }
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeInt64(int64_t n)
    {
    Array<octet_t>::Handle hab = f_hBuf->tmpbuf();

    // first octet contains sign bit (bit 7 set if neg)
    octet_t  b  = 0;
    uint8_t  cb = 0;
    uint64_t l  = n;
    if (n < 0)
        {
        b = 0x40;
        l = ~l;
        }

    // first octet contains only 6 data bits
    b |= (octet_t) (l & 0x3F);
    l >>= 6;

    while (l != 0)
        {
        b |= 0x80;          // bit 8 is a continuation bit
        hab[cb++] = b;

        b = (octet_t) (l & 0x7F);
        l >>= 7;
        }

    if (cb == 0)
        {
        // one-octet format
        write(b);
        }
    else
        {
        hab[cb++] = b;
        write(hab, 0, cb);
        }
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeFloat32(float32_t fl)
    {
    Array<octet_t>::Handle hab = f_hBuf->tmpbuf();

    uint32_t n = Float32::float32ToBits(fl);

    hab[0] = (octet_t) (n >> 24);
    hab[1] = (octet_t) (n >> 16);
    hab[2] = (octet_t) (n >>  8);
    hab[3] = (octet_t) (n);

    write(hab, 0, 4);
    }

void AbstractWriteBuffer::AbstractBufferOutput::writeFloat64(float64_t dfl)
    {
    Array<octet_t>::Handle hab = f_hBuf->tmpbuf();

    uint64_t l = Float64::float64ToBits(dfl);

    // hi word
    uint32_t n = (uint32_t) (l >> 32);
    hab[0] = (octet_t) (n >> 24);
    hab[1] = (octet_t) (n >> 16);
    hab[2] = (octet_t) (n >>  8);
    hab[3] = (octet_t) (n);

    // lo word
    n = (uint32_t) l;
    hab[4] = (octet_t) (n >> 24);
    hab[5] = (octet_t) (n >> 16);
    hab[6] = (octet_t) (n >>  8);
    hab[7] = (octet_t) (n);

    write(hab, 0, 8);
    }


// ----- internal methods ---------------------------------------------------

WriteBuffer::BufferOutput::Handle AbstractWriteBuffer::instantiateBufferOutput()
    {
    return AbstractBufferOutput::create(this);
    }

Array<octet_t>::Handle AbstractWriteBuffer::tmpbuf()
    {
    Array<octet_t>::Handle hab = m_habBuf;
    if (NULL == hab)
        {
        m_habBuf = hab = Array<octet_t>::create(MIN_BUF);
        }
    return hab;
    }

Array<octet_t>::Handle AbstractWriteBuffer::tmpbuf(size32_t cb)
    {
    Array<octet_t>::Handle hab = m_habBuf;
    if (NULL == hab || hab->length < cb)
        {
        m_habBuf = hab = Array<octet_t>::create(std::max(cb, MIN_BUF));
        }
    return hab;
    }

COH_CLOSE_NAMESPACE2
