/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/OctetArrayWriteBuffer.hpp"

#include "coherence/io/AbstractReadBuffer.hpp"
#include "coherence/io/OctetArrayReadBuffer.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,io)


// ----- constructors -------------------------------------------------------

OctetArrayWriteBuffer::OctetArrayWriteBuffer(Array<octet_t>::Handle hab)
    : m_hab(self(), hab),
      m_pab(hab->raw),
      m_cbab(hab->length),
      m_cb(0),
      m_cbMax(hab->length),
      m_hBufUnsafe(self(), AbstractReadBuffer::getEmptyReadBuffer(), true)
    {
    }

OctetArrayWriteBuffer::OctetArrayWriteBuffer(size32_t cbCap, size32_t cbMax)
    : m_hab(self(), Array<octet_t>::create(cbCap)),
      m_pab(m_hab->raw),
      m_cbab(cbCap),
      m_cb(0),
      m_cbMax(cbMax),
      m_hBufUnsafe(self(), AbstractReadBuffer::getEmptyReadBuffer(), true)
    {
    if (cbCap > cbMax)
        {
        COH_THROW(IllegalArgumentException::create());
        }
    }

OctetArrayWriteBuffer::OctetArrayWriteBuffer(const OctetArrayWriteBuffer& that)
    : super(that),
      m_hab(self(), Array<octet_t>::create(that.getCapacity())),
      m_pab(m_hab->raw),
      m_cbab(that.getCapacity()),
      m_cb(0),
      m_cbMax(that.getMaximumCapacity()),
      m_hBufUnsafe(self(), AbstractReadBuffer::getEmptyReadBuffer(), true)
    {
    if (that.length() > 0)
        {
        write(0, that.getUnsafeReadBuffer());
        }
    }

OctetArrayWriteBuffer::OctetArrayBufferOutput::OctetArrayBufferOutput(
        OctetArrayWriteBuffer::Handle hBuf)
        : super(hBuf), f_hBuf(self(), hBuf)
    {
    }

// ----- WriteBuffer interface ----------------------------------------------

size32_t OctetArrayWriteBuffer::length() const
    {
    return m_cb;
    }

size32_t OctetArrayWriteBuffer::getCapacity() const
    {
    return m_cbab;
    }

size32_t OctetArrayWriteBuffer::getMaximumCapacity() const
    {
    return m_cbMax;
    }

void OctetArrayWriteBuffer::write(size32_t ofDest, octet_t b)
    {
    checkBounds(ofDest, 1);

    octet_t*               ab  = m_pab; // for efficiency; but only safe because we've just checked bounds
    Array<octet_t>::Handle hab;
    if (ab == NULL)
        {
        // hBuf has escaped and technically m_hab and thus ab could be
        // destroyed by another thread if we don't hold a ref to it
        hab = m_hab;
        ab  = hab->raw;
        }

    ab[ofDest] = b;
    updateLength(ofDest + 1);
    }

void OctetArrayWriteBuffer::write(size32_t ofDest, Array<octet_t>::View vabSrc,
            size32_t ofSrc, size32_t cbSrc)
    {
    checkBounds(ofDest, cbSrc);
    Array<octet_t>::copy(vabSrc, ofSrc, m_hab, ofDest, cbSrc);
    updateLength(ofDest + cbSrc);
    }

void OctetArrayWriteBuffer::write(size32_t ofDest, ReadBuffer::View vBufSrc,
        size32_t ofSrc, size32_t cbSrc)
    {
    checkBounds(ofDest, cbSrc);
    vBufSrc->read(ofSrc, ofSrc + cbSrc, m_hab, ofDest);
    updateLength(ofDest + cbSrc);
    }

void OctetArrayWriteBuffer::retain(size32_t of, size32_t cb)
    {
    if (of + cb > m_cb)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of << ", cb="
                << cb << ", length()=" << m_cb);
        }

    if (of > 0 && cb > 0)
        {
        Array<octet_t>::copy(m_hab, of, m_hab, 0, cb);
        }

    m_cb         = cb;
    m_hBufUnsafe = AbstractReadBuffer::getEmptyReadBuffer();
    }

ReadBuffer::View OctetArrayWriteBuffer::getUnsafeReadBuffer() const
    {
    ReadBuffer::Handle hBuf = m_hBufUnsafe;
    if (AbstractReadBuffer::getEmptyReadBuffer() == hBuf)
        {
        m_hBufUnsafe = hBuf = OctetArrayReadBuffer::create(m_hab, 0, m_cb);
        }
    else
        {
        cast<OctetArrayReadBuffer::Handle>(hBuf)->updateLength(m_cb);
        }
    return hBuf;
    }


// ----- BufferOutput interface ---------------------------------------------

void OctetArrayWriteBuffer::OctetArrayBufferOutput::writeChar16(wchar16_t ch)
    {
    size32_t ofb = m_ofWrite, ofOrig = ofb;

    f_hBuf->checkBounds(ofb, 3); // max length is 3

    octet_t*               ab  = f_hBuf->m_pab; // for efficiency; but only safe because we've just checked bounds
    Array<octet_t>::Handle hab;
    if (ab == NULL)
        {
        // hBuf has escaped and technically m_hab and thus ab could be
        // destroyed by another thread if we don't hold a ref to it
        hab = f_hBuf->m_hab;
        ab  = hab->raw;
        }

    if (ch <= 0x007F)
        {
        // 1-octet format:  0xxx xxxx
        ab[ofb++] = (octet_t) ch;
        }
    else if (ch <= 0x07FF)
        {
        // 2-octet format:  110x xxxx, 10xx xxxx
        ab[ofb++] = (octet_t) (0xC0 | ((ch >> 6) & 0x1F));
        ab[ofb++] = (octet_t) (0x80 | ((ch     ) & 0x3F));
        }
    else
        {
        // 3-octet format:  1110 xxxx, 10xx xxxx, 10xx xxxx
        ab[ofb++] = (octet_t) (0xE0 | ((ch >> 12) & 0x0F));
        ab[ofb++] = (octet_t) (0x80 | ((ch >>  6) & 0x3F));
        ab[ofb++] = (octet_t) (0x80 | ((ch      ) & 0x3F));
        }

    moveOffset(ofb - ofOrig);
    }

void OctetArrayWriteBuffer::OctetArrayBufferOutput::writeString(String::View vs)
    {
    if (NULL == vs)
        {
        writeInt64(-1);
        }
    else
        {
        Array<octet_t>::View vab = vs->getOctets();
        size32_t             cb  = vab->length - 1; // skip NUL

        writeInt64(cb);

        if (cb > 0)
            {
            size32_t of = m_ofWrite;

            f_hBuf->checkBounds(of, cb);

            Array<octet_t>::copy(vab, 0, f_hBuf->m_hab, of, cb);
            moveOffset(cb);
            }
        }
    }

void OctetArrayWriteBuffer::OctetArrayBufferOutput::writeInt16(int16_t n)
    {
    size32_t ofb = m_ofWrite, ofOrig = ofb;

    f_hBuf->checkBounds(ofb, 3); // max length is 3

    octet_t*               ab  = f_hBuf->m_pab; // for efficiency; but only safe because we've just checked bounds
    Array<octet_t>::Handle hab;
    if (ab == NULL)
        {
        // hBuf has escaped and technically m_hab and thus ab could be
        // destroyed by another thread if we don't hold a ref to it
        hab = f_hBuf->m_hab;
        ab  = hab->raw;
        }

    // first octet contains sign bit (bit 7 set if neg)
    octet_t  b  = 0;
    uint16_t un = n;
    if (n < 0)
        {
        b = 0x40;
        un = ~n;
        }

    // first octet contains only 6 data bits
    b |= (octet_t) (un & 0x3F);
    un >>= 6;

    while (un != 0)
        {
        b |= 0x80;          // bit 8 is a continuation bit
        ab[ofb++] = b;

        b = (octet_t) (un & 0x7F);
        un >>= 7;
        }

    ab[ofb++] = b;
    moveOffset(ofb - ofOrig);
    }

void OctetArrayWriteBuffer::OctetArrayBufferOutput::writeInt32(int32_t n)
    {
    size32_t ofb = m_ofWrite, ofOrig = ofb;

    f_hBuf->checkBounds(ofb, 5); // max length is 5

    octet_t*               ab  = f_hBuf->m_pab; // for efficiency; but only safe because we've just checked bounds
    Array<octet_t>::Handle hab;
    if (ab == NULL)
        {
        // hBuf has escaped and technically m_hab and thus ab could be
        // destroyed by another thread if we don't hold a ref to it
        hab = f_hBuf->m_hab;
        ab  = hab->raw;
        }

    // first octet contains sign bit (bit 7 set if neg)
    octet_t  b  = 0;
    uint32_t un = n;
    if (n < 0)
        {
        b = 0x40;
        un = ~n;
        }

    // first octet contains only 6 data bits
    b |= (octet_t) (un & 0x3F);
    un >>= 6;

    while (un != 0)
        {
        b |= 0x80;          // bit 8 is a continuation bit
        ab[ofb++] = b;

        b = (octet_t) (un & 0x7F);
        un >>= 7;
        }

    ab[ofb++] = b;
    moveOffset(ofb - ofOrig);
    }

void OctetArrayWriteBuffer::OctetArrayBufferOutput::writeInt64(int64_t n)
    {
    size32_t ofb = m_ofWrite, ofOrig = ofb;

    f_hBuf->checkBounds(ofb, 10); // max length is 10

    octet_t*               ab  = f_hBuf->m_pab; // for efficiency; but only safe because we've just checked bounds
    Array<octet_t>::Handle hab;
    if (ab == NULL)
        {
        // hBuf has escaped and technically m_hab and thus ab could be
        // destroyed by another thread if we don't hold a ref to it
        hab = f_hBuf->m_hab;
        ab  = hab->raw;
        }

    // first octet contains sign bit (bit 7 set if neg)
    octet_t  b = 0;
    uint64_t l = n;
    if (n < 0)
        {
        b = 0x40;
        l = ~n;
        }

    // first octet contains only 6 data bits
    b |= (octet_t) (l & 0x3F);
    l >>= 6;

    while (l != 0)
        {
        b |= 0x80;          // bit 8 is a continuation bit
        ab[ofb++] = b;

        b = (octet_t) (l & 0x7F);
        l >>= 7;
        }

    ab[ofb++] = b;
    moveOffset(ofb - ofOrig);
    }

void OctetArrayWriteBuffer::OctetArrayBufferOutput::writeFloat32(float32_t fl)
    {
    size32_t of = m_ofWrite;

    f_hBuf->checkBounds(of, 4);

    octet_t*               ab  = f_hBuf->m_pab; // for efficiency; but only safe because we've just checked bounds
    Array<octet_t>::Handle hab;
    if (ab == NULL)
        {
        // hBuf has escaped and technically m_hab and thus ab could be
        // destroyed by another thread if we don't hold a ref to it
        hab = f_hBuf->m_hab;
        ab  = hab->raw;
        }

    uint32_t n = Float32::float32ToBits(fl);
    ab[of  ] = (octet_t) (n >> 24);
    ab[of+1] = (octet_t) (n >> 16);
    ab[of+2] = (octet_t) (n >>  8);
    ab[of+3] = (octet_t) (n);

    moveOffset(4);
    }

void OctetArrayWriteBuffer::OctetArrayBufferOutput::writeFloat64(float64_t dfl)
    {
    size32_t of = m_ofWrite;

    f_hBuf->checkBounds(of, 8);

    octet_t*               ab  = f_hBuf->m_pab; // for efficiency; but only safe because we've just checked bounds
    Array<octet_t>::Handle hab;
    if (ab == NULL)
        {
        // hBuf has escaped and technically m_hab and thus ab could be
        // destroyed by another thread if we don't hold a ref to it
        hab = f_hBuf->m_hab;
        ab  = hab->raw;
        }

    uint64_t l = Float64::float64ToBits(dfl);

    // hi word
    uint32_t n = (uint32_t) (l >> 32);
    ab[of  ] = (octet_t) (n >> 24);
    ab[of+1] = (octet_t) (n >> 16);
    ab[of+2] = (octet_t) (n >>  8);
    ab[of+3] = (octet_t) (n);

    // lo word
    n = (uint32_t) l;
    ab[of+4] = (octet_t) (n >> 24);
    ab[of+5] = (octet_t) (n >> 16);
    ab[of+6] = (octet_t) (n >>  8);
    ab[of+7] = (octet_t) (n);

    moveOffset(8);
    }


// ----- internal methods ---------------------------------------------------

WriteBuffer::BufferOutput::Handle OctetArrayWriteBuffer::instantiateBufferOutput()
    {
    return OctetArrayBufferOutput::create(this);
    }

void OctetArrayWriteBuffer::checkBounds(size32_t of, size32_t cb)
    {
    size32_t cbTotal = of + cb;
    if (cbTotal > m_cbMax)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of << ", cb="
                << cb << ", max=" << m_cbMax);
        }

    if (cbTotal > m_cbab)
        {
        grow(cbTotal);
        }
    }

void OctetArrayWriteBuffer::grow(size32_t cbCap)
    {
    Array<octet_t>::Handle hab = m_hab;

    // desired growth is 100% for "small" buffers and 50% for "huge"
    // minimum growth is 1KB
    size32_t       cbOld = hab->length;
    size32_t       cbAdd = std::max((size32_t) 1024, cbOld > 0x100000 ? cbOld >> 1 : cbOld);
    size32_t       cbNew = std::min(m_cbMax, std::max(cbCap + 1024, cbOld + cbAdd));
    if (cbNew > cbOld)
        {
        Array<octet_t>::Handle habNew = Array<octet_t>::copy(hab, 0, Array<octet_t>::create(cbNew), 0, m_cb);

        m_hab        = habNew;
        m_cbab       = cbNew;
        m_pab        = _isEscaped(/*fAttemptCapture*/ false) ? NULL : habNew->raw;
        m_hBufUnsafe = AbstractReadBuffer::getEmptyReadBuffer();
        }
    }

void OctetArrayWriteBuffer::updateLength(size32_t cb)
    {
    if (cb > m_cb)
        {
        m_cb = cb;
        }
    }

void OctetArrayWriteBuffer::OctetArrayBufferOutput::moveOffset(size32_t cb)
    {
    size32_t of = m_ofWrite + cb;
    m_ofWrite = of;
    f_hBuf->updateLength(of);
    }

void OctetArrayWriteBuffer::onEscape(bool fEscape) const
    {
    super::onEscape(fEscape);
    if (fEscape)
        {
        m_pab = NULL; // raw pointer optimization no longer provably safe
        }
    }

COH_CLOSE_NAMESPACE2
