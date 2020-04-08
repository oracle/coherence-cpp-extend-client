/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/AbstractOctetArrayReadBuffer.hpp"

#include "coherence/io/EOFException.hpp"
#include "coherence/io/UTFDataFormatException.hpp"
#include "coherence/util/Binary.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,io)

using coherence::util::Binary;


// ----- constructors -------------------------------------------------------

AbstractOctetArrayReadBuffer::AbstractOctetArrayReadBuffer()
    : f_vab(self())
    {
    }

AbstractOctetArrayReadBuffer::AbstractOctetArrayReadBuffer(
        Array<octet_t>::View vab, size32_t of, size32_t cb)
    : f_vab(self(), vab)
    {
    resetRange(of, cb);
    }

AbstractOctetArrayReadBuffer::OctetArrayBufferInput::OctetArrayBufferInput(
        AbstractOctetArrayReadBuffer::View vBuf)
    : super(vBuf), f_vBuf(self(), vBuf), f_vab(self(), vBuf->f_vab)
    {
    }


// ----- ReadBuffer interface -----------------------------------------------

size32_t AbstractOctetArrayReadBuffer::length() const
    {
    return m_cb;
    }

octet_t AbstractOctetArrayReadBuffer::read(size32_t of) const
    {
    if (of < m_cb)
        {
        return f_vab->raw[m_of + of];
        }
    else
        {
        COH_THROW_STREAM(IndexOutOfBoundsException,
                "of=" << of << ", length()=" << m_cb);
        }
    }

void AbstractOctetArrayReadBuffer::read(size32_t ofBegin, size32_t ofEnd,
        Array<octet_t>::Handle habDest, size32_t ofDest) const
    {
    if (ofEnd > m_cb || ofBegin > ofEnd)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "ofBegin=" << ofBegin
                << ", ofEnd=" << ofEnd << ", length=" << m_cb);
        }

    Array<octet_t>::copy(f_vab, m_of + ofBegin, habDest, ofDest, ofEnd - ofBegin);
    }

Binary::View AbstractOctetArrayReadBuffer::toBinary() const
    {
    size32_t cb = m_cb;
    return cb == 0 ? getEmptyBinary()
                   : Binary::create(f_vab, m_of, cb);
    }

Binary::View AbstractOctetArrayReadBuffer::toBinary(size32_t of, size32_t cb) const
    {
    // validate parameters
    checkBounds(of, cb);

    return cb == 0 ? getEmptyBinary()
                   : Binary::create(f_vab, m_of + of, cb);
    }

Array<octet_t>::View AbstractOctetArrayReadBuffer::toOctetArray(size32_t of, size32_t cb) const
    {
    // validate parameters
    checkBounds(of, cb);
    if (cb == 0)
        {
        return getEmptyOctetArray();
        }

    Array<octet_t>::View vab = f_vab;

    // adjust offset based on what part of the underlying octet array this
    // buffer is "over"
    of += m_of;
    if (of == 0 && cb == vab->length)
        {
        return vab;
        }
    else
        {
        return vab->subArray(of, of + cb);
        }
    }


// ----- Object interface ---------------------------------------------------

bool AbstractOctetArrayReadBuffer::equals(Object::View v) const
    {
    if (instanceof<AbstractOctetArrayReadBuffer::View>(v))
        {
        AbstractOctetArrayReadBuffer::View that = cast<AbstractOctetArrayReadBuffer::View>(v);
        if (this == that)
            {
            return true;
            }

        size32_t cb = this->m_cb;
        return cb == that->m_cb &&
               this->f_vab->regionMatches(this->m_of, that->f_vab, that->m_of, cb);
        }
    else
        {
        return super::equals(v);
        }
    }


// ----- BufferInput interface ----------------------------------------------

octet_t AbstractOctetArrayReadBuffer::OctetArrayBufferInput::read()
    {
    size32_t of = getOffset();
    size32_t cb = f_vBuf->m_cb;
    if (of < cb)
        {
        // increment read position
        setOffsetInternal(of + 1);

        // return the octet from the previous read position
        return ((Array<octet_t>::View) f_vBuf->f_vab)[f_vBuf->m_of + of];
        }
    else
        {
        COH_THROW (EOFException::create());
        }
    }

void AbstractOctetArrayReadBuffer::OctetArrayBufferInput::read(
        Array<octet_t>::Handle hab, size32_t of, size32_t cb)
    {
    // validate parameters
    size32_t cbDest = hab->length;
    if (of + cb > cbDest)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "hab->length=" << cbDest
                << ", of=" << of << ", cb=" << cb);
        }

    // avoid confusing "nothing requested" with "nothing remains"
    if (cb == 0)
        {
        return;
        }

    // get the current position within the buffer and determine the
    // remainder of the buffer in octets
    size32_t ofIn = getOffset();

    // check for eof
    if (f_vBuf->m_cb - ofIn < cb)
        {
        setOffsetInternal(f_vBuf->length());
        COH_THROW (EOFException::create());
        }

    // transfer the read data to the passed octet array
    size32_t ofSrc = f_vBuf->m_of + ofIn;
    Array<octet_t>::copy(f_vBuf->f_vab, ofSrc, hab, of, cb);

    // update the read offset
    setOffsetInternal(ofIn + cb);
    }

wchar16_t AbstractOctetArrayReadBuffer::OctetArrayBufferInput::readChar16()
    {
    size32_t ofRaw = f_vBuf->m_of;
    size32_t of    = ofRaw + getOffset();
    size32_t cb    = ofRaw + f_vBuf->m_cb;

    // check EOF
    if (of >= cb)
        {
        COH_THROW (EOFException::create());
        }

    wchar16_t ch;

    octet_t const* ab = f_vab->raw; // for efficiency; but only safe because we've just checked bounds

    uint16_t b = ab[of++];
    switch ((b & 0xF0) >> 4)
        {
        case 0x0: case 0x1: case 0x2: case 0x3:
        case 0x4: case 0x5: case 0x6: case 0x7:
            // 1-octet format:  0xxx xxxx
            ch = b;
            break;

        case 0xC: case 0xD:
            {
            // 2-octet format:  110x xxxx, 10xx xxxx
            uint16_t b2 = ab[of++];
            if ((b2 & 0xC0) != 0x80)
                {
                COH_THROW (UTFDataFormatException::create());
                }
            ch = ((b & 0x1F) << 6) | (b2 & 0x3F);
            break;
            }

        case 0xE:
            {
            // 3-octet format:  1110 xxxx, 10xx xxxx, 10xx xxxx
            uint16_t b2 = ab[of++];
            uint16_t b3 = ab[of++];
            if ((b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80)
                {
                COH_THROW (UTFDataFormatException::create());
                }
            ch = ((b & 0x0F) << 12) | ((b2 & 0x3F) <<  6) | (b3 & 0x3F);
            break;
            }

        default:
            COH_THROW_STREAM(UTFDataFormatException,
                    "illegal leading UTF octet: " << std::hex << b);
        }

    // increment read position
    setOffsetInternal(of - ofRaw);

    return ch;
    }

int16_t AbstractOctetArrayReadBuffer::OctetArrayBufferInput::readInt16()
    {
    size32_t ofRaw = f_vBuf->m_of;
    size32_t of    = ofRaw + getOffset();
    size32_t cb    = ofRaw + f_vBuf->m_cb;

    // check EOF
    if (of >= cb)
        {
        COH_THROW (EOFException::create());
        }

    octet_t const* ab = f_vab->raw; // for efficiency; but only safe because we've just checked bounds

    uint16_t b     = ab[of++];
    uint16_t n     = b & 0x3F;         // only 6 bits of data in first octet
    uint8_t  cBits = 6;
    bool     fNeg  = (b & 0x40) != 0;  // seventh bit is a sign bit

    while ((b & 0x80) != 0)            // eighth bit is the continuation bit
        {
        // check EOF
        if (of >= cb)
            {
            COH_THROW (EOFException::create());
            }

        b      = ab[of++];
        n     |= ((b & 0x7F) << cBits);
        cBits += 7;
        }

    if (fNeg)
        {
        n = ~n;
        }

    // increment read position
    setOffsetInternal(of - ofRaw);

    return n;
    }

int32_t AbstractOctetArrayReadBuffer::OctetArrayBufferInput::readInt32()
    {
    size32_t ofRaw = f_vBuf->m_of;
    size32_t of    = ofRaw + getOffset();
    size32_t cb    = ofRaw + f_vBuf->m_cb;

    // check EOF
    if (of >= cb)
        {
        COH_THROW (EOFException::create());
        }

    octet_t const* ab = f_vab->raw; // for efficiency; but only safe because we've just checked bounds

    uint32_t b     = ab[of++];
    uint32_t n     = b & 0x3F;         // only 6 bits of data in first octet
    uint8_t  cBits = 6;
    bool     fNeg  = (b & 0x40) != 0;  // seventh bit is a sign bit

    while ((b & 0x80) != 0)            // eighth bit is the continuation bit
        {
        // check EOF
        if (of >= cb)
            {
            COH_THROW (EOFException::create());
            }

        b      = ab[of++];
        n     |= ((b & 0x7F) << cBits);
        cBits += 7;
        }

    if (fNeg)
        {
        n = ~n;
        }

    // increment read position
    setOffsetInternal(of - ofRaw);

    return n;
    }

int64_t AbstractOctetArrayReadBuffer::OctetArrayBufferInput::readInt64()
    {
    size32_t ofRaw = f_vBuf->m_of;
    size32_t of    = ofRaw + getOffset();
    size32_t cb    = ofRaw + f_vBuf->m_cb;

    // check EOF
    if (of >= cb)
        {
        COH_THROW (EOFException::create());
        }

    octet_t const* ab = f_vab->raw; // for efficiency; but only safe because we've just checked bounds

    uint64_t b     = ab[of++];
    uint64_t l     = b & 0x3F;         // only 6 bits of data in first octet
    uint8_t  cBits = 6;
    bool     fNeg  = (b & 0x40) != 0;  // seventh bit is a sign bit

    while ((b & 0x80) != 0)            // eighth bit is the continuation bit
        {
        // check EOF
        if (of >= cb)
            {
            COH_THROW (EOFException::create());
            }

        b      = ab[of++];
        l     |= ((b & 0x7F) << cBits);
        cBits += 7;
        }

    if (fNeg)
        {
        l = ~l;
        }

    // increment read position
    setOffsetInternal(of - ofRaw);

    return l;
    }

float32_t AbstractOctetArrayReadBuffer::OctetArrayBufferInput::readFloat32()
    {
    // check for EOF
    size32_t ofAdd = getOffset();
    size32_t cb    = f_vBuf->m_cb;
    if (cb - ofAdd < 4)
        {
        // pretend we exhausted the stream trying to read the value
        setOffsetInternal(cb);
        COH_THROW (EOFException::create());
        }

    // increment read position
    setOffsetInternal(ofAdd + 4);

    // build and return the value
    octet_t const* ab = f_vab->raw; // for efficiency; but only safe because we've just checked bounds

    size32_t of = f_vBuf->m_of + ofAdd;
    uint32_t n  = ((ab[of    ]       ) << 24)
                | ((ab[of + 1] & 0xFF) << 16)
                | ((ab[of + 2] & 0xFF) << 8 )
                | ((ab[of + 3] & 0xFF)      );

    return Float32::bitsToFloat32(n);
    }

float64_t AbstractOctetArrayReadBuffer::OctetArrayBufferInput::readFloat64()
    {
    // check for EOF
    size32_t ofAdd = getOffset();
    size32_t cb    = f_vBuf->m_cb;
    if (cb - ofAdd < 8)
        {
        // pretend we exhausted the stream trying to read the value
        setOffsetInternal(cb);
        COH_THROW (EOFException::create());
        }

    // increment read position
    setOffsetInternal(ofAdd + 8);

    // build and return the value
    octet_t const* ab = f_vab->raw; // for efficiency; but only safe because we've just checked bounds

    size32_t of = f_vBuf->m_of + ofAdd;
    uint64_t n1 = ((ab[of    ]       ) << 24)
                | ((ab[of + 1] & 0xFF) << 16)
                | ((ab[of + 2] & 0xFF) << 8 )
                | ((ab[of + 3] & 0xFF)      );
    uint64_t n2 = ((ab[of + 4]       ) << 24)
                | ((ab[of + 5] & 0xFF) << 16)
                | ((ab[of + 6] & 0xFF) << 8 )
                | ((ab[of + 7] & 0xFF)      );

    return Float64::bitsToFloat64((n1 << 32) | (n2 & 0xFFFFFFFF));
    }


// ----- factory methods ----------------------------------------------------

ReadBuffer::BufferInput::Handle AbstractOctetArrayReadBuffer::instantiateBufferInput() const
    {
    return OctetArrayBufferInput::create(this);
    }


// ----- internal methods ---------------------------------------------------

void AbstractOctetArrayReadBuffer::resetRange(size32_t of, size32_t cb)
    {
    size32_t vabLength = f_vab->length;
    if (cb == npos)
        {
        if (of > vabLength)
            {
            COH_THROW_STREAM(IndexOutOfBoundsException,
                            "of=" << of << ", cb=" << cb << ", vab->length=" << f_vab->length);
            }
        cb = vabLength - of;
        }
    else if (of + cb > vabLength)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException,
                "of=" << of << ", cb=" << cb << ", vab->length=" << vabLength);
        }

    m_of = of;
    m_cb = cb;
    }

String::View AbstractOctetArrayReadBuffer::OctetArrayBufferInput::convertUTF(size32_t of,
        size32_t cb) const
    {
    return String::create(f_vBuf->f_vab, f_vBuf->m_of + of, cb);
    }

COH_CLOSE_NAMESPACE2
