/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/WritingPofHandler.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


// ----- constructors -------------------------------------------------------

WritingPofHandler::WritingPofHandler(WriteBuffer::BufferOutput::Handle hOut)
        : f_hOut(self(), hOut),
          m_hComplex(self()),
          m_nComplexId(0),
          m_fHasIdentity(false)
    {
    }

WritingPofHandler::Complex::Complex(
        WritingPofHandler::Complex::Handle hComplexCurrent,
        bool fEncodePosition)
        : m_fSparse(fEncodePosition),
          m_fUniform(false),
          m_nTypeId(0),
          f_hComplexOuter(self(), hComplexCurrent)
    {
    }

WritingPofHandler::Complex::Complex(
        WritingPofHandler::Complex::Handle hComplexCurrent,
        bool fEncodePosition, int32_t nUniformTypeId)
        : m_fSparse(fEncodePosition),
          m_fUniform(true),
          m_nTypeId(nUniformTypeId),
          f_hComplexOuter(self(), hComplexCurrent)
    {
    }

WritingPofHandler::ComplexMap::ComplexMap(
        WritingPofHandler::Complex::Handle hComplexCurrent,
        int32_t nUniformKeyTypeId)
        : super(hComplexCurrent,
                false, nUniformKeyTypeId),
          m_fKey(false),
          m_fUniformValue(false),
          m_nValueTypeId(0)
    {
    }

WritingPofHandler::ComplexMap::ComplexMap(
        WritingPofHandler::Complex::Handle hComplexCurrent,
        int32_t nUniformKeyTypeId, int32_t nUniformValTypeId)
        : super(hComplexCurrent,
                false, nUniformKeyTypeId),
          m_fKey(false),
          m_fUniformValue(true),
          m_nValueTypeId(nUniformValTypeId)
    {
    }


// ----- PofHandler interface -----------------------------------------------

void WritingPofHandler::registerIdentity(int32_t nId)
    {
    COH_ENSURE(!m_fHasIdentity || nId < 0);

    if (nId >= 0)
        {
        f_hOut->writeInt32(t_identity);
        f_hOut->writeInt32(nId);
        }

    m_fHasIdentity = true;
    }

void WritingPofHandler::onNullReference(int32_t iPos)
    {
    if (!isSkippable())
        {
        encodePosition(iPos);
        f_hOut->writeInt32(v_reference_null);
        }
    }

void WritingPofHandler::onIdentityReference(int32_t iPos, int32_t nId)
    {
    encodePosition(iPos);

    if (isTypeIdEncoded(t_reference))
        {
        f_hOut->writeInt32(t_reference);
        }
    f_hOut->writeInt32(nId);
    }

void WritingPofHandler::onInt16(int32_t iPos, int16_t n)
    {
    if (n != 0 || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_int16))
            {
            if (n >= -1 && n <= 22 && fCompressable)
                {
                f_hOut->writeInt32(encodeTinyInt32(n));
                return;
                }
            f_hOut->writeInt32(t_int16);
            }
        f_hOut->writeInt16(n);
        }
    }

void WritingPofHandler::onInt32(int32_t iPos, int32_t n)
    {
    if (n != 0 || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_int32))
            {
            if (n >= -1 && n <= 22 && fCompressable)
                {
                f_hOut->writeInt32(encodeTinyInt32(n));
                return;
                }
            f_hOut->writeInt32(t_int32);
            }
        f_hOut->writeInt32(n);
        }
    }

void WritingPofHandler::onInt64(int32_t iPos, int64_t n)
    {
    if (n != 0 || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_int64))
            {
            if (n >= -1 && n <= 22 && fCompressable)
                {
                f_hOut->writeInt32(encodeTinyInt32((int32_t) n));
                return;
                }
            f_hOut->writeInt32(t_int64);
            }
        f_hOut->writeInt64(n);
        }
    }

void WritingPofHandler::onFloat32(int32_t iPos, float32_t fl)
    {
    if (!Float32::isZero(fl) || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_float32))
            {
            uint32_t nBits = Float32::float32ToBits(fl);
            if (0 == (nBits & 0x0000FFFF) && fCompressable)
                {
                switch (nBits >> 16)
                    {
                    case 0xFF80:
                        f_hOut->writeInt32(v_fp_neg_infinity);
                        return;

                    case 0x7F80:
                        f_hOut->writeInt32(v_fp_pos_infinity);
                        return;

                    case 0x7FC0:
                        f_hOut->writeInt32(v_fp_nan);
                        return;

                    case 0xBF80: // -1
                    case 0x0000: // 0
                    case 0x3F80: // 1
                    case 0x4000:
                    case 0x4040:
                    case 0x4080:
                    case 0x40A0:
                    case 0x40C0:
                    case 0x40E0:
                    case 0x4100:
                    case 0x4110:
                    case 0x4120:
                    case 0x4130:
                    case 0x4140:
                    case 0x4150:
                    case 0x4160:
                    case 0x4170:
                    case 0x4180:
                    case 0x4188:
                    case 0x4190:
                    case 0x4198:
                    case 0x41A0:
                    case 0x41A8:
                    case 0x41B0: // 22
                        f_hOut->writeInt32(encodeTinyInt32((int32_t) fl));
                        return;
                    }
                }
            f_hOut->writeInt32(t_float32);
            f_hOut->writeFloat32(fl);
            }
        else
            {
            f_hOut->writeFloat32(fl);
            }
        }
    }

void WritingPofHandler::onFloat64(int32_t iPos, float64_t dfl)
    {
    if (!Float64::isZero(dfl) || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_float64))
            {
            uint64_t nBits = Float64::float64ToBits(dfl);

            const static uint64_t MASK = COH_INT64(0x0000FFFFU, 0xFFFFFFFFU);
            if (0 == (nBits & MASK) && fCompressable)
                {
                switch ((int32_t) (nBits >> 48))
                    {
                    case 0xFFF0:
                        f_hOut->writeInt32(v_fp_neg_infinity);
                        return;

                    case 0x7FF0:
                        f_hOut->writeInt32(v_fp_pos_infinity);
                        return;

                    case 0x7FF8:
                        f_hOut->writeInt32(v_fp_nan);
                        return;

                    case 0xBFF0: // -1
                    case 0x0000: // 0
                    case 0x3FF0: // 1
                    case 0x4000:
                    case 0x4008:
                    case 0x4010:
                    case 0x4014:
                    case 0x4018:
                    case 0x401C:
                    case 0x4020:
                    case 0x4022:
                    case 0x4024:
                    case 0x4026:
                    case 0x4028:
                    case 0x402A:
                    case 0x402C:
                    case 0x402E:
                    case 0x4030:
                    case 0x4031:
                    case 0x4032:
                    case 0x4033:
                    case 0x4034:
                    case 0x4035:
                    case 0x4036: // 22
                        f_hOut->writeInt32(v_int_0 - (int32_t) dfl);
                        return;
                    }
                }
            f_hOut->writeInt32(t_float64);
            f_hOut->writeFloat64(dfl);
            }
        else
            {
            f_hOut->writeFloat64(dfl);
            }
        }
    }

void WritingPofHandler::onBoolean(int32_t iPos, bool f)
    {
    if (f || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_boolean))
            {
            if (fCompressable)
                {
                f_hOut->writeInt32(f ? v_boolean_true : v_boolean_false);
                return;
                }
            f_hOut->writeInt32(t_boolean);
            }

        f_hOut->writeInt32(f ? 1 : 0);
        }
    }

void WritingPofHandler::onOctet(int32_t iPos, octet_t b)
    {
    if (b != 0x00 || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_octet))
            {
            if (fCompressable)
                {
                // get rid of any extra bits
                if (b <= 22)
                    {
                    f_hOut->writeInt32(encodeTinyInt32(b));
                    return;
                    }
                else if (0xFF == b)
                    {
                    f_hOut->writeInt32(v_int_neg_1);
                    return;
                    }
                }

            f_hOut->writeInt32(t_octet);
            }
        f_hOut->write(b);
        }
    }

void WritingPofHandler::onOctetString(int32_t iPos, Binary::View vBin)
    {
    if (vBin->length() != 0 || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_octet_string))
            {
            if (0 == vBin->length() && fCompressable)
                {
                f_hOut->writeInt32(v_string_zero_length);
                return;
                }

            f_hOut->writeInt32(t_octet_string);
            }

        // WriteBuffer supports storing up to size32_t bytes but POF only
        // supports strings of length int32_t; validate this requirement
        f_hOut->writeInt32(validateOutgoingSize(vBin->length()));
        f_hOut->writeBuffer(vBin);
        }
    }

void WritingPofHandler::onChar(int32_t iPos, wchar16_t ch)
    {
    if (ch != 0 || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_char))
            {
            if (fCompressable)
                {
                if (ch <= 22)
                    {
                    f_hOut->writeInt32(encodeTinyInt32(ch));
                    return;
                    }
                else if (0xFFFF == ch)
                    {
                    f_hOut->writeInt32(v_int_neg_1);
                    return;
                    }
                }

            f_hOut->writeInt32(t_char);
            }
        f_hOut->writeChar16(ch);
        }
    }

void WritingPofHandler::onCharString(int32_t iPos, String::View vs)
    {
    if (vs->length() != 0 || !isSkippable())
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_char_string))
            {
            if (0 == vs->length() && fCompressable)
                {
                f_hOut->writeInt32(v_string_zero_length);
                return;
                }

            f_hOut->writeInt32(t_char_string);
            }

        // WriteBuffer supports storing up to size32_t bytes but POF only
        // supports strings of length int32_t; validate this requirement
        validateOutgoingSize(vs->length());
        f_hOut->writeString(vs);
        }
    }

void WritingPofHandler::onDate(int32_t iPos, int32_t nYear, int32_t nMonth,
        int32_t nDay)
    {
    encodePosition(iPos);

    WriteBuffer::BufferOutput::Handle hOut = f_hOut;
    if (isTypeIdEncoded(t_date))
        {
        hOut->writeInt32(t_date);
        }

    writeDate(hOut, nYear, nMonth, nDay);
    }

void WritingPofHandler::onYearMonthInterval(int32_t iPos, int32_t cYears,
        int32_t cMonths)
    {
    encodePosition(iPos);

    if (isTypeIdEncoded(t_year_month_interval))
        {
        f_hOut->writeInt32(t_year_month_interval);
        }

    f_hOut->writeInt32(cYears);
    f_hOut->writeInt32(cMonths);
    }

void WritingPofHandler::onTime(int32_t iPos, int32_t nHour, int32_t nMinute,
        int32_t nSecond, int32_t nNano, bool fUTC)
    {
    encodePosition(iPos);

    WriteBuffer::BufferOutput::Handle hOut = f_hOut;
    if (isTypeIdEncoded(t_time))
        {
        hOut->writeInt32(t_time);
        }

    writeTime(hOut, nHour, nMinute, nSecond, nNano, fUTC ? 1 : 0, 0, 0);
    }

void WritingPofHandler::onTime(int32_t iPos, int32_t nHour, int32_t nMinute,
        int32_t nSecond, int32_t nNano, int32_t nHourOffset,
        int32_t nMinuteOffset)
    {
    encodePosition(iPos);

    WriteBuffer::BufferOutput::Handle hOut = f_hOut;
    if (isTypeIdEncoded(t_time))
        {
        hOut->writeInt32(t_time);
        }

    writeTime(hOut, nHour, nMinute, nSecond, nNano, 2, nHourOffset,
            nMinuteOffset);
    }

void WritingPofHandler::onTimeInterval(int32_t iPos, int32_t cHours,
        int32_t cMinutes, int32_t cSeconds, int32_t cNanos)
    {
    encodePosition(iPos);

    if (isTypeIdEncoded(t_time_interval))
        {
        f_hOut->writeInt32(t_time_interval);
        }

    f_hOut->writeInt32(cHours);
    f_hOut->writeInt32(cMinutes);
    f_hOut->writeInt32(cSeconds);
    f_hOut->writeInt32(cNanos);
    }

void WritingPofHandler::onDateTime(int32_t iPos, int32_t nYear,
        int32_t nMonth, int32_t nDay, int32_t nHour, int32_t nMinute,
        int32_t nSecond, int32_t nNano, bool fUTC)
    {
    encodePosition(iPos);

    WriteBuffer::BufferOutput::Handle hOut = f_hOut;
    if (isTypeIdEncoded(t_datetime))
        {
        hOut->writeInt32(t_datetime);
        }

    writeDate(hOut, nYear, nMonth, nDay);
    writeTime(hOut, nHour, nMinute, nSecond, nNano, fUTC ? 1 : 0, 0, 0);
    }

void WritingPofHandler::onDateTime(int32_t iPos, int32_t nYear,
        int32_t nMonth, int32_t nDay, int32_t nHour, int32_t nMinute,
        int32_t nSecond, int32_t nNano, int32_t nHourOffset,
        int32_t nMinuteOffset)
    {
    encodePosition(iPos);

    WriteBuffer::BufferOutput::Handle hOut = f_hOut;
    if (isTypeIdEncoded(t_datetime))
        {
        hOut->writeInt32(t_datetime);
        }

    writeDate(hOut, nYear, nMonth, nDay);
    writeTime(hOut, nHour, nMinute, nSecond, nNano, 2, nHourOffset,
            nMinuteOffset);
    }

void WritingPofHandler::onDayTimeInterval(int32_t iPos, int32_t cDays,
        int32_t cHours, int32_t cMinutes, int32_t cSeconds,
        int32_t cNanos)
    {
    encodePosition(iPos);

    if (isTypeIdEncoded(t_day_time_interval))
        {
        f_hOut->writeInt32(t_day_time_interval);
        }
    f_hOut->writeInt32(cDays);
    f_hOut->writeInt32(cHours);
    f_hOut->writeInt32(cMinutes);
    f_hOut->writeInt32(cSeconds);
    f_hOut->writeInt32(cNanos);
    }

void WritingPofHandler::beginCollection(int32_t iPos, size32_t cElements)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_collection))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(Complex::create((Complex::Handle) m_hComplex, false));
                return;
                }

            f_hOut->writeInt32(t_collection);
            }

        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    }

void WritingPofHandler::beginUniformCollection(int32_t iPos,
        size32_t cElements, int32_t nType)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_uniform_collection))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(Complex::create((Complex::Handle) m_hComplex, false, nType));
                return;
                }

            f_hOut->writeInt32(t_uniform_collection);
            }

        f_hOut->writeInt32(nType);
        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(Complex::create((Complex::Handle) m_hComplex, false, nType));
        }
    }

void WritingPofHandler::beginArray(int32_t iPos, size32_t cElements)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_array))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(Complex::create((Complex::Handle) m_hComplex, false));
                return;
                }

            f_hOut->writeInt32(t_array);
            }

        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    }

void WritingPofHandler::beginUniformArray(int32_t iPos, size32_t cElements,
        int32_t nType)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_uniform_array))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(Complex::create((Complex::Handle) m_hComplex, false, nType));
                return;
                }

            f_hOut->writeInt32(t_uniform_array);
            }

        f_hOut->writeInt32(nType);
        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(Complex::create((Complex::Handle) m_hComplex, false, nType));
        }
    }

void WritingPofHandler::beginSparseArray(int32_t iPos, size32_t cElements)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_sparse_array))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(Complex::create((Complex::Handle) m_hComplex, false));
                return;
                }

            f_hOut->writeInt32(t_sparse_array);
            }

        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(Complex::create((Complex::Handle) m_hComplex, true));
        }
    }

void WritingPofHandler::beginUniformSparseArray(int32_t iPos,
        size32_t cElements, int32_t nType)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_uniform_sparse_array))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(Complex::create((Complex::Handle) m_hComplex, false, nType));
                return;
                }

            f_hOut->writeInt32(t_uniform_sparse_array);
            }

        f_hOut->writeInt32(nType);
        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(Complex::create((Complex::Handle) m_hComplex, true, nType));
        }
    }

void WritingPofHandler::beginMap(int32_t iPos, size32_t cElements)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_map))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(Complex::create((Complex::Handle) m_hComplex, false));
                return;
                }

            f_hOut->writeInt32(t_map);
            }

        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    }

void WritingPofHandler::beginUniformKeysMap(int32_t iPos, size32_t cElements,
        int32_t nTypeKeys)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_uniform_keys_map))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(ComplexMap::create((Complex::Handle) m_hComplex, nTypeKeys));
                return;
                }

            f_hOut->writeInt32(t_uniform_keys_map);
            }

        f_hOut->writeInt32(nTypeKeys);
        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(ComplexMap::create((Complex::Handle) m_hComplex, nTypeKeys));
        }
    }

void WritingPofHandler::beginUniformMap(int32_t iPos, size32_t cElements,
        int32_t nTypeKeys, int32_t nTypeValues)
    {
    if (0 == cElements && isSkippable())
        {
        // dummy complex type (no contents, no termination)
        setComplex(Complex::create((Complex::Handle) m_hComplex, false));
        }
    else
        {
        bool fCompressable = isCompressable();
        encodePosition(iPos);

        if (isTypeIdEncoded(t_uniform_map))
            {
            if (0 == cElements && fCompressable)
                {
                f_hOut->writeInt32(v_collection_empty);
                setComplex(ComplexMap::create((Complex::Handle) m_hComplex, nTypeKeys,
                        nTypeValues));
                return;
                }

            f_hOut->writeInt32(t_uniform_map);
            }

        f_hOut->writeInt32(nTypeKeys);
        f_hOut->writeInt32(nTypeValues);
        f_hOut->writeInt32(validateOutgoingSize(cElements));

        setComplex(ComplexMap::create((Complex::Handle) m_hComplex, nTypeKeys, nTypeValues));
        }
    }

void WritingPofHandler::beginUserType(int32_t iPos, int32_t nUserTypeId,
        int32_t nVersionId)
	{
	beginUserType(iPos, -1, nUserTypeId, nVersionId);
	}

void WritingPofHandler::beginUserType(int32_t iPos, int32_t nId, int32_t nUserTypeId,
        int32_t nVersionId)
    {
    encodePosition(iPos);

    WriteBuffer::BufferOutput::Handle hOut = f_hOut;
    registerIdentity(nId);
    if (isTypeIdEncoded(nUserTypeId))
        {
        hOut->writeInt32(nUserTypeId);
        }
    hOut->writeInt32(nVersionId);

    setComplex(Complex::create((Complex::Handle) m_hComplex, true));
    }

void WritingPofHandler::endComplexValue()
    {
    Complex::Handle hComplex = m_hComplex;
    if (hComplex->isSparse())
        {
        f_hOut->writeInt32(-1);
        }

    setComplex(hComplex->pop());
    }


// ----- internal methods ---------------------------------------------------

bool WritingPofHandler::isSkippable() const
    {
    if (m_fHasIdentity)
        {
        return false;
        }

    Complex::View vComplex = m_hComplex;
    return NULL != vComplex && vComplex->isSparse();
    }

bool WritingPofHandler::isCompressable() const
    {
    return !m_fHasIdentity;
    }

void WritingPofHandler::encodePosition(int32_t iPos)
    {
    Complex::Handle hComplex = m_hComplex;
    if (NULL != hComplex)
        {
        hComplex->onValue(iPos);

        if (iPos >= 0 && hComplex->isSparse())
            {
            f_hOut->writeInt32(iPos);
            }
        }

    // once the position is encoded, the "has identity" flag is reset
    m_fHasIdentity = false;
    }

bool WritingPofHandler::isTypeIdEncoded(int32_t nTypeId) const
    {
    Complex::View vComplex = m_hComplex;

    // if the type is not being encoded, it must match the expected
    // uniform type
    COH_ENSURE(NULL == vComplex || !vComplex->isUniform()
           || vComplex->getUniformType() == nTypeId
           || nTypeId == t_reference);

    return NULL == vComplex || !vComplex->isUniform();
    }


// ----- Complex interface --------------------------------------------------

void WritingPofHandler::Complex::onValue(int32_t /*iPos*/)
    {
    }

bool WritingPofHandler::Complex::isUniform() const
    {
    return m_fUniform;
    }

int32_t WritingPofHandler::Complex::getUniformType() const
    {
    return m_nTypeId;
    }

bool WritingPofHandler::Complex::isSparse() const
    {
    return m_fSparse;
    }

WritingPofHandler::Complex::Handle WritingPofHandler::Complex::pop()
    {
    return f_hComplexOuter;
    }

WritingPofHandler::Complex::View WritingPofHandler::Complex::pop() const
    {
    return f_hComplexOuter;
    }


// ----- ComplexMap interface -----------------------------------------------

void WritingPofHandler::ComplexMap::onValue(int32_t /*iPos*/)
    {
    m_fKey = !m_fKey;
    }

bool WritingPofHandler::ComplexMap::isUniform() const
    {
    return m_fKey ? WritingPofHandler::Complex::isUniform()
                  : m_fUniformValue;
    }

int32_t WritingPofHandler::ComplexMap::getUniformType() const
    {
    return m_fKey ? WritingPofHandler::Complex::getUniformType()
                  : m_nValueTypeId;
    }


// ----- accessors ----------------------------------------------------------

WriteBuffer::BufferOutput::Handle WritingPofHandler::getBufferOutput()
    {
    return f_hOut;
    }

WriteBuffer::BufferOutput::View WritingPofHandler::getBufferOutput() const
    {
    return f_hOut;
    }

WritingPofHandler::Complex::Handle WritingPofHandler::getComplex()
    {
    return m_hComplex;
    }

WritingPofHandler::Complex::View WritingPofHandler::getComplex() const
    {
    return m_hComplex;
    }

size64_t WritingPofHandler::getComplexId() const
    {
    return m_nComplexId;
    }

void WritingPofHandler::setComplex(WritingPofHandler::Complex::Handle hComplex)
    {
    // see UserTypeWriter for rational behind caching the raw pointer
    m_hComplex   = hComplex;
    m_nComplexId = hComplex == NULL ? 0 : size64_t(get_pointer(hComplex));
    }

COH_CLOSE_NAMESPACE3
