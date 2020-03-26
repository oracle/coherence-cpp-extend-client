/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/AbstractReadBuffer.hpp"

#include "coherence/io/EOFException.hpp"
#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/UTFDataFormatException.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/BinaryWriteBuffer.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include <algorithm>
#include <limits>

COH_OPEN_NAMESPACE2(coherence,io)

using coherence::util::Binary;
using coherence::util::BinaryWriteBuffer;
using coherence::util::StringHelper;


// ----- constants ----------------------------------------------------------

Array<octet_t>::Handle AbstractReadBuffer::getEmptyOctetArray()
    {
    static FinalHandle<Array<octet_t> > hab(System::common(),
            Array<octet_t>::create(0));
    return hab;
    }
COH_STATIC_INIT(AbstractReadBuffer::getEmptyOctetArray());

Binary::Handle AbstractReadBuffer::getEmptyBinary()
    {
    static FinalHandle<Binary> hBin(System::common(),
            Binary::create(cast<Array<octet_t>::View>(getEmptyOctetArray())));
    return hBin;
    }
COH_STATIC_INIT(AbstractReadBuffer::getEmptyBinary());

ReadBuffer::Handle AbstractReadBuffer::getEmptyReadBuffer()
    {
    static FinalHandle<ReadBuffer> hBuf(System::common(),
            OctetArrayReadBuffer::create(getEmptyOctetArray(), 0, 0));
    return hBuf;
    }
COH_STATIC_INIT(AbstractReadBuffer::getEmptyReadBuffer());


// ----- constructors -------------------------------------------------------

AbstractReadBuffer::AbstractBufferInput::AbstractBufferInput(
        AbstractReadBuffer::View vBuf)
        : f_vBuf(self(), vBuf), m_of(0)
    {
    }

// ----- ReadBuffer interface -----------------------------------------------

ReadBuffer::View AbstractReadBuffer::getReadBuffer(size32_t of, size32_t cb) const
    {
    checkBounds(of, cb);

    // optimize getting the "same" buffer
    if (of == 0 && cb == length())
        {
        return this;
        }

    // create a "sub" buffer
    return instantiateReadBuffer(of, cb);
    }

Binary::View AbstractReadBuffer::toBinary() const
    {
    return toBinary(0, length());
    }

Binary::View AbstractReadBuffer::toBinary(size32_t of, size32_t cb) const
    {
    checkBounds(of, cb);

    WriteBuffer::Handle hBuf = BinaryWriteBuffer::create(cb, cb);
    hBuf->write(0, this, of, cb);
    return hBuf->toBinary();
    }

Array<octet_t>::View AbstractReadBuffer::toOctetArray() const
    {
    return toOctetArray(0, length());
    }

Array<octet_t>::View AbstractReadBuffer::toOctetArray(size32_t of, size32_t cb) const
    {
    checkBounds(of, cb);

    Array<octet_t>::View vab;
    if (cb == 0)
        {
        vab = getEmptyOctetArray();
        }
    else
        {
        Array<octet_t>::Handle hab = Array<octet_t>::create(cb);
        read(of, of + cb, hab, 0);
        vab = hab;
        }

    return vab;
    }

ReadBuffer::BufferInput::Handle AbstractReadBuffer::getBufferInput() const
    {
    return instantiateBufferInput();
    }


// ----- BufferInput interface ----------------------------------------------

ReadBuffer::View AbstractReadBuffer::AbstractBufferInput::getBuffer() const
    {
    return f_vBuf;
    }

size32_t AbstractReadBuffer::AbstractBufferInput::getOffset() const
    {
    return m_of;
    }

void AbstractReadBuffer::AbstractBufferInput::setOffset(size32_t of)
    {
    if (of > getBuffer()->length())
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of
                << ", getBuffer()->length()=" << getBuffer()->length());
        }

    setOffsetInternal(of);
    }

size32_t AbstractReadBuffer::AbstractBufferInput::available() const
    {
    return getBuffer()->length() - getOffset();
    }

void AbstractReadBuffer::AbstractBufferInput::skip(size32_t cb)
    {
    // get the current position within the buffer and determine the
    // remainder of the buffer in octets
    size32_t of    = getOffset();
    size32_t cbRem = getBuffer()->length() - of;

    // check for eof
    if (cbRem < cb)
        {
        setOffsetInternal(getBuffer()->length());
        COH_THROW (EOFException::create());
        }

    // do the skip
    setOffsetInternal(of + cb);
    }

octet_t AbstractReadBuffer::AbstractBufferInput::read()
    {
    try
        {
        size32_t of = getOffset();
        octet_t  b  = getBuffer()->read(of);
        setOffsetInternal(of + 1);
        return b;
        }
    catch (IndexOutOfBoundsException::View t)
        {
        COH_THROW (EOFException::create(t->getMessage()));
        }
    }

void AbstractReadBuffer::AbstractBufferInput::read(Array<octet_t>::Handle hab)
    {
    read(hab, 0, hab->length);
    }

void AbstractReadBuffer::AbstractBufferInput::read(Array<octet_t>::Handle hab,
        size32_t of, size32_t cb)
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

    ReadBuffer::View vBuf = getBuffer();

    // get the current position within the buffer and determine the
    // remainder of the buffer in octets
    size32_t ofIn  = getOffset();
    size32_t cbRem = vBuf->length() - ofIn;

    // check for eof
    if (cbRem < cb)
        {
        setOffsetInternal(vBuf->length());
        COH_THROW (EOFException::create());
        }

    // transfer the read data to the passed octet array
    size32_t ofEnd = ofIn + cb;
    vBuf->read(ofIn, ofEnd, hab, of);

    // update the read offset
    setOffsetInternal(ofEnd);
    }

ReadBuffer::View AbstractReadBuffer::AbstractBufferInput::readBuffer(size32_t cb)
    {
    ReadBuffer::View vBuf;
    size32_t of = getOffset();
    try
        {
        vBuf = getBuffer()->getReadBuffer(of, cb);
        }
    catch (IndexOutOfBoundsException::View t)
        {
        // pretend we read to the very end of the buffer and ran out of octets
        setOffsetInternal(getBuffer()->length());

        // report it as such, but keep the information contained in the
        // original exception
        COH_THROW (EOFException::create(t->getMessage()));
        }

    // adjust the read pointer past the ReadBuffer that we just read
    setOffsetInternal(of + cb);

    return vBuf;
    }

bool AbstractReadBuffer::AbstractBufferInput::readBoolean()
    {
    return read() != 0;
    }

wchar16_t AbstractReadBuffer::AbstractBufferInput::readChar16()
    {
    wchar16_t ch;

    uint16_t b = read();
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
            uint16_t b2 = read();
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
            uint16_t b2 = read();
            uint16_t b3 = read();
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

    return ch;
    }

String::View AbstractReadBuffer::AbstractBufferInput::readString()
    {
    int32_t cb = readInt32();
    if (cb < 0)
        {
        return NULL;
        }

    if (sizeof(int32_t) > sizeof(size32_t) &&
        cb > (int32_t) std::numeric_limits<size32_t>::max())
        {
        COH_THROW_STREAM(IOException,"size32_t overflow: cb=" << cb);
        }
    return readUTF(cb);
    }

int16_t AbstractReadBuffer::AbstractBufferInput::readInt16()
    {
    uint16_t b     = read();
    uint16_t n     = b & 0x3F;         // only 6 bits of data in first octet
    uint8_t  cBits = 6;
    bool     fNeg  = (b & 0x40) != 0;  // seventh bit is a sign bit

    while ((b & 0x80) != 0)             // eighth bit is the continuation bit
        {
        b      = read();
        n     |= ((b & 0x7F) << cBits);
        cBits += 7;
        }

    if (fNeg)
        {
        n = ~n;
        }

    return n;
    }

int32_t AbstractReadBuffer::AbstractBufferInput::readInt32()
    {
    uint32_t b     = read();
    uint32_t n     = b & 0x3F;         // only 6 bits of data in first octet
    uint8_t  cBits = 6;
    bool     fNeg  = (b & 0x40) != 0;  // seventh bit is a sign bit

    while ((b & 0x80) != 0)             // eighth bit is the continuation bit
        {
        b      = read();
        n     |= ((b & 0x7F) << cBits);
        cBits += 7;
        }

    if (fNeg)
        {
        n = ~n;
        }

    return n;
    }

int64_t AbstractReadBuffer::AbstractBufferInput::readInt64()
    {
    uint64_t b     = read();
    uint64_t l     = b & 0x3F;         // only 6 bits of data in first octet
    uint8_t  cBits = 6;
    bool     fNeg  = (b & 0x40) != 0;  // seventh bit is a sign bit

    while ((b & 0x80) != 0)             // eighth bit is the continuation bit
        {
        b      = read();
        l     |= ((b & 0x7F) << cBits);
        cBits += 7;
        }

    if (fNeg)
        {
        l = ~l;
        }

    return l;
    }

float32_t AbstractReadBuffer::AbstractBufferInput::readFloat32()
    {
    uint32_t n = (((uint32_t) read()) << 24) |
                 (((uint32_t) read()) << 16) |
                 (((uint32_t) read()) <<  8) |
                  ((uint32_t) read());

    return Float32::bitsToFloat32(n);
    }

float64_t AbstractReadBuffer::AbstractBufferInput::readFloat64()
    {
    uint64_t l = (((uint32_t) read()) << 24) |
                 (((uint32_t) read()) << 16) |
                 (((uint32_t) read()) <<  8) |
                  ((uint32_t) read());

    l <<= 32;
    l |= (((uint32_t) read()) << 24) |
         (((uint32_t) read()) << 16) |
         (((uint32_t) read()) <<  8) |
          ((uint32_t) read());

    return Float64::bitsToFloat64(l);
    }


// ----- Object interface ---------------------------------------------------

bool AbstractReadBuffer::equals(Object::View v) const
    {
    ReadBuffer::View that = cast<ReadBuffer::View>(v, false);
    if (this == that)
        {
        return true;
        }
    else if (NULL == that)
        {
        return false;
        }
    else
        {
        return toOctetArray()->equals(that->toOctetArray());
        }
    }


// ----- internal methods ---------------------------------------------------

ReadBuffer::BufferInput::Handle AbstractReadBuffer::instantiateBufferInput() const
    {
    return AbstractBufferInput::create(this);
    }

void AbstractReadBuffer::checkBounds(size32_t of, size32_t cb) const
    {
    if (of + cb > length())
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "of=" << of << ", cb="
                << cb << ", length()=" << length());
        }
    }

String::View AbstractReadBuffer::AbstractBufferInput::readUTF(size32_t cb)
    {
    if (0 == cb)
        {
        return StringHelper::getEmptyString();
        }

    size32_t of    = getOffset();
    size32_t cbBuf = getBuffer()->length();
    if (of + cb > cbBuf)
        {
        // pretend to have read to end-of-stream
        setOffsetInternal(cbBuf);
        COH_THROW (EOFException::create());
        }
    else
        {
        // pretend to have read to the end of the binary
        setOffsetInternal(of + cb);
        }

    return convertUTF(of, cb);
    }

String::View AbstractReadBuffer::AbstractBufferInput::convertUTF(size32_t of,
        size32_t cb) const
    {
    return String::create(getBuffer()->toOctetArray(of, cb));
    }

void AbstractReadBuffer::AbstractBufferInput::setOffsetInternal(size32_t of)
    {
    m_of = of;
    }

COH_CLOSE_NAMESPACE2
