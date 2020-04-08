/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SerializationHelper.hpp"

#include "coherence/io/EOFException.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/BinaryWriteBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::EOFException;
using coherence::io::IOException;
using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Return an array of Stats objects, indexed by the modulo of a
    * swizzled class hashcode.
    *
    * @return the stats array
    */
    static ObjectArray::Handle getStatsArray()
        {
        static FinalHandle<ObjectArray> haStats(
                System::common(), ObjectArray::create(6451));
        return haStats;
        }
    COH_STATIC_INIT(getStatsArray());
    }

// ----- SerializationHelper interface ----------------------------------

Binary::View SerializationHelper::decorateBinary(Binary::View vBinValue, int32_t nDecoration)
    {
    WriteBuffer::Handle               hBuf = BinaryWriteBuffer::create(6 + vBinValue->length());
    WriteBuffer::BufferOutput::Handle hOut = hBuf->getBufferOutput();

    hOut->write(fmt_ido);
    hOut->writeInt32(nDecoration);
    hOut->writeBuffer(vBinValue);

    return hBuf->toBinary();
    }

int32_t SerializationHelper::extractIntDecoration(Binary::View vBin)
    {
    try
        {
        ReadBuffer::BufferInput::Handle hIn = vBin->getBufferInput();
        hIn->read(); // skip the type
        return hIn->readInt32();
        }
    catch (IOException::View /* e */)
        {
        COH_THROW(IllegalArgumentException::create("invalid binary"));
        }
    }

bool SerializationHelper::isIntDecorated(Binary::View vBin)
    {
    try
        {
        return vBin->read(0) == fmt_ido;
        }
    catch (IndexOutOfBoundsException::View /* e */)
        {
        return false;
        }
    }

Binary::View SerializationHelper::removeIntDecoration(Binary::View vBin)
    {
    try
        {
        ReadBuffer::BufferInput::Handle hIn = vBin->getBufferInput();
        hIn->read();       // skip the type
        hIn->readInt32();  // skip the int decoration
        int of = hIn->getOffset();
        return vBin->toBinary(of, vBin->length() - of);
        }
    catch (IOException::View /* e */)
        {
        COH_THROW(IllegalArgumentException::create("invalid binary"));
        }
    }

Binary::View SerializationHelper::toBinary(Object::View v, Serializer::View vSerializer)
    {
    Stats::Handle       hStats = findStats(v);
    WriteBuffer::Handle hBuf   = hStats == NULL
            ? cast<WriteBuffer::Handle>(BinaryWriteBuffer::create(64))
            : hStats->instantiateBuffer();

    WriteBuffer::BufferOutput::Handle hOut = hBuf->getBufferOutput(0);
    hOut->write(fmt_ext);
    vSerializer->serialize(hOut, v);

    updateStats(v, hStats, hBuf->length());

    return hBuf->toBinary();
    }

Object::Holder SerializationHelper::fromBinary(Binary::View vBin, Serializer::View vSerializer)
    {
    ReadBuffer::BufferInput::Handle hIn = vBin->getBufferInput();

    octet_t nType = hIn->read();
    switch(nType)
        {
        case fmt_ido:
            // read is not symmetrical to the write;
            // it returns the decorated value itself!
            hIn->readInt32();
            nType = hIn->read();
            break;

        case fmt_bin_deco:
        case fmt_bin_ext_deco:
            int64_t nMask = nType == fmt_bin_deco ? hIn->read() : hIn->readInt64();
            if ((nMask & (1L << deco_value)) == 0L)
                {
                COH_THROW (EOFException::create("Decorated value is missing a value"));
                }

            int32_t cb = hIn->readInt32(); // packed length of the decorated Binary value
            hIn   = vBin->getReadBuffer(hIn->getOffset(), cb)->getBufferInput();
            nType = hIn->read();
            break;
        }

    if (nType == fmt_ext)
        {
        return vSerializer->deserialize(hIn);
        }

    COH_THROW (IOException::create("Illegal Binary format"));
    }

int32_t SerializationHelper::calculateStatsId(Object::View v)
    {
    if (v == NULL)
        {
        return 0;
        }
    int32_t n = Class::getClassName(v)->hashCode();
    return ((((uint32_t) n) >> 1) + (n & 0x01)) % getStatsArray()->length;
    }

SerializationHelper::Stats::Handle SerializationHelper::findStats(Object::View v)
    {
    ObjectArray::Handle haStats = getStatsArray();
    return cast<SerializationHelper::Stats::Handle>(haStats[calculateStatsId(v)]);
    }

void SerializationHelper::updateStats(Object::View v, SerializationHelper::Stats::Handle hStats, int32_t cb)
    {
    ObjectArray::Handle haStats = getStatsArray();
    if (hStats == NULL)
        {
        haStats[calculateStatsId(v)] = hStats = Stats::create();
        }
    hStats->update(cb);
    }

// ----- inner class: Stats -------------------------------------------------

SerializationHelper::Stats::Stats()
        : m_lAccum(self(), 0),
          m_lStats(self(), 0),
          m_ldtCreated(self(), System::safeTimeMillis())
    {
    }

void SerializationHelper::Stats::update(int32_t cb)
    {
    int32_t cbMax   = (int32_t) (((uint64_t) m_lStats) >> 32);
    int64_t lAccum  = m_lAccum;
    int32_t cItems  = (int32_t) (((uint64_t) lAccum) >> 48);
    int64_t cbTotal = lAccum & COH_INT64(0xFFFFU, 0xFFFFFFFFU);

    if (cItems > 0)
        {
        bool    fResetStats = false;   
        int32_t cbOldAvg    = (int32_t) (cbTotal / cItems);
        int64_t ldtNow      = 0;

        if (abs(cbOldAvg - cb) > (cb / 2))
            {
            // reset the stats because the current buffer size
            // is more than 50% larger than the requested size
            ldtNow = System::safeTimeMillis();
            fResetStats = true;
            }
        else if ((cItems & 0x3FF) == 0)
            {
            ldtNow = System::safeTimeMillis();
            if (ldtNow > m_ldtCreated + expiry_millis ||  // stats expiry
                    (cItems & 0xFFFF) == 0)               // cItems overflow
                {
                // reset the stats periodically
                fResetStats = true;
                }
            }
        if (fResetStats)
            {
            cbMax        = 0;
            lAccum       = 0L;
            cItems       = 0;
            m_ldtCreated = ldtNow;
            }
        }

    // accumulate the new total bytes (uses lowest 48 out of 64 bits)
    cbTotal = (lAccum + cb) & COH_INT64(0xFFFFU, 0xFFFFFFFFU);

    // recalculate the new average
    int32_t cbAvg = (int32_t) (cbTotal / ++cItems);

    // check for a new max size
    if (cb > cbMax)
        {
        cbMax = cb;
        }

    // the item count and total bytes are stored in a "volatile long"
    // so that they are accessed (and modified) atomically
    m_lAccum = ((int64_t) cItems << 48) | cbTotal;

    // the average and max are stored in a "volatile long" so that
    // they are subsequently accessed atomically
    m_lStats = ((int64_t) cbMax << 32) | cbAvg;
    }

WriteBuffer::Handle SerializationHelper::Stats::instantiateBuffer() const
    {
    int64_t lStats = m_lStats;
    int32_t cbMax  = (int32_t) (((uint64_t) lStats) >> 32);

    return BinaryWriteBuffer::create((cbMax + 0xF) & ~0xF);
    }

COH_CLOSE_NAMESPACE2
