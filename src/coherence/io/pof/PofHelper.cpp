/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PofHelper.hpp"

#include "coherence/io/AbstractReadBuffer.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/PofIntrinsic.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/ReadOnlyMultiList.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/SparseArray.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"
#include "private/coherence/util/StringHelper.hpp"

#include <algorithm>
#include <sstream>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::AbstractReadBuffer;
using coherence::io::IOException;
using coherence::util::ArrayList;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::LongArray;
using coherence::util::Map;
using coherence::util::ReadOnlyArrayList;
using coherence::util::ReadOnlyMultiList;
using coherence::util::SafeHashMap;
using coherence::util::SimpleMapEntry;
using coherence::util::SparseArray;
using coherence::util::StringHelper;


// ----- constants ----------------------------------------------------------

Array<bool>::Handle PofHelper::getEmptyBooleanArray()
    {
    static FinalHandle<Array<bool> > haf(System::common(),
            Array<bool>::create(0));
    return haf;
    }
COH_STATIC_INIT(PofHelper::getEmptyBooleanArray());

Array<octet_t>::Handle PofHelper::getEmptyOctetArray()
    {
    return AbstractReadBuffer::getEmptyOctetArray();
    }

Array<wchar16_t>::Handle PofHelper::getEmptyChar16Array()
    {
    static FinalHandle<Array<wchar16_t> > hach(System::common(),
            Array<wchar16_t>::create(0));
    return hach;
    }
COH_STATIC_INIT(PofHelper::getEmptyChar16Array());

Array<int16_t>::Handle PofHelper::getEmptyInt16Array()
    {
    static FinalHandle<Array<int16_t> > han(System::common(),
            Array<int16_t>::create(0));
    return han;
    }
COH_STATIC_INIT(PofHelper::getEmptyInt16Array());

Array<int32_t>::Handle PofHelper::getEmptyInt32Array()
    {
    static FinalHandle<Array<int32_t> > han(System::common(),
            Array<int32_t>::create(0));
    return han;
    }
COH_STATIC_INIT(PofHelper::getEmptyInt32Array());

Array<int64_t>::Handle PofHelper::getEmptyInt64Array()
    {
    static FinalHandle<Array<int64_t> > han(System::common(),
            Array<int64_t>::create(0));
    return han;
    }
COH_STATIC_INIT(PofHelper::getEmptyInt64Array());

Array<float32_t>::Handle PofHelper::getEmptyFloat32Array()
    {
    static FinalHandle<Array<float32_t> > hafl(System::common(),
            Array<float32_t>::create(0));
    return hafl;
    }
COH_STATIC_INIT(PofHelper::getEmptyFloat32Array());

Array<float64_t>::Handle PofHelper::getEmptyFloat64Array()
    {
    static FinalHandle<Array<float64_t> > hadfl(System::common(),
            Array<float64_t>::create(0));
    return hadfl;
    }
COH_STATIC_INIT(PofHelper::getEmptyFloat64Array());

ObjectArray::Handle PofHelper::getEmptyObjectArray()
    {
    static FinalHandle<ObjectArray> hao(System::common(),
            ObjectArray::create(0));
    return hao;
    }
COH_STATIC_INIT(PofHelper::getEmptyObjectArray());

Binary::Handle PofHelper::getEmptyBinary()
    {
    return AbstractReadBuffer::getEmptyBinary();
    }

Collection::View PofHelper::getEmptyCollection()
    {
    static FinalView<Collection> vCol(System::common(),
            ReadOnlyMultiList::create(ObjectArray::create(0)));
    return vCol;
    }
COH_STATIC_INIT(PofHelper::getEmptyCollection());


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Helper for getMaxDays per Month
    */
    Array<int32_t>::View _createMaxDaysPerMonth()
        {
        Array<int32_t>::Handle han = Array<int32_t>::create(12);
        han[0]  = 31;
        han[1]  = 29;
        han[2]  = 31;
        han[3]  = 30;
        han[4]  = 31;
        han[5]  = 30;
        han[6]  = 31;
        han[7]  = 31;
        han[8]  = 30;
        han[9]  = 31;
        han[10] = 30;
        han[11] = 31;
        return han;
        }

    /**
    * The maximum number of days in each month. Note February.
    */
    Array<int32_t>::View getMaxDaysPerMonth()
        {
        static FinalView<Array<int32_t> > van(System::common(),
                _createMaxDaysPerMonth());
        return van;
        }
    COH_STATIC_INIT(getMaxDaysPerMonth());

    /**
    * @return a Map containing Class name to POF type id mappings.
    */
    Map::View _createClassToPofTypeMap()
        {
        Map::Handle hMap = HashMap::create();

        // register primitive types
        hMap->put(Class::getTypeName(typeid(bool)),
                Integer32::valueOf(PofHelper::t_boolean));
        hMap->put(Class::getTypeName(typeid(Boolean)),
                Integer32::valueOf(PofHelper::t_boolean));
        hMap->put(Class::getTypeName(typeid(octet_t)),
                Integer32::valueOf(PofHelper::t_octet));
        hMap->put(Class::getTypeName(typeid(Octet)),
                Integer32::valueOf(PofHelper::t_octet));
        hMap->put(Class::getTypeName(typeid(wchar16_t)),
                Integer32::valueOf(PofHelper::t_char));
        hMap->put(Class::getTypeName(typeid(Character16)),
                Integer32::valueOf(PofHelper::t_char));
        hMap->put(Class::getTypeName(typeid(int16_t)),
                Integer32::valueOf(PofHelper::t_int16));
        hMap->put(Class::getTypeName(typeid(Integer16)),
                Integer32::valueOf(PofHelper::t_int16));
        hMap->put(Class::getTypeName(typeid(int32_t)),
                Integer32::valueOf(PofHelper::t_int32));
        hMap->put(Class::getTypeName(typeid(Integer32)),
                Integer32::valueOf(PofHelper::t_int32));
        hMap->put(Class::getTypeName(typeid(int64_t)),
                Integer32::valueOf(PofHelper::t_int64));
        hMap->put(Class::getTypeName(typeid(Integer64)),
                Integer32::valueOf(PofHelper::t_int64));
        hMap->put(Class::getTypeName(typeid(float32_t)),
                Integer32::valueOf(PofHelper::t_float32));
        hMap->put(Class::getTypeName(typeid(Float32)),
                Integer32::valueOf(PofHelper::t_float32));
        hMap->put(Class::getTypeName(typeid(float64_t)),
                Integer32::valueOf(PofHelper::t_float64));
        hMap->put(Class::getTypeName(typeid(Float64)),
                Integer32::valueOf(PofHelper::t_float64));

        // register primitive array types
        hMap->put(Class::getTypeName(typeid(Array<bool>)),
                Integer32::valueOf(PofHelper::t_uniform_array));
        hMap->put(Class::getTypeName(typeid(Array<octet_t>)),
                Integer32::valueOf(PofHelper::t_uniform_array));
        hMap->put(Class::getTypeName(typeid(Array<wchar16_t>)),
                Integer32::valueOf(PofHelper::t_uniform_array));
        hMap->put(Class::getTypeName(typeid(Array<int16_t>)),
                Integer32::valueOf(PofHelper::t_uniform_array));
        hMap->put(Class::getTypeName(typeid(Array<int32_t>)),
                Integer32::valueOf(PofHelper::t_uniform_array));
        hMap->put(Class::getTypeName(typeid(Array<int64_t>)),
                Integer32::valueOf(PofHelper::t_uniform_array));
        hMap->put(Class::getTypeName(typeid(Array<float32_t>)),
                Integer32::valueOf(PofHelper::t_uniform_array));
        hMap->put(Class::getTypeName(typeid(Array<float64_t>)),
                Integer32::valueOf(PofHelper::t_uniform_array));

        // register managed array type
        hMap->put(Class::getTypeName(typeid(ObjectArray)),
                Integer32::valueOf(PofHelper::t_array));

        // register Binary
        hMap->put(Class::getTypeName(typeid(Binary)),
                Integer32::create(PofHelper::t_octet_string));

        // register String
        hMap->put(Class::getTypeName(typeid(String)),
                Integer32::valueOf(PofHelper::t_char_string));

        // register "raw" types
        hMap->put(Class::getTypeName(typeid(RawDate)),
                Integer32::valueOf(PofHelper::t_date));
        hMap->put(Class::getTypeName(typeid(RawYearMonthInterval)),
                Integer32::valueOf(PofHelper::t_year_month_interval));
        hMap->put(Class::getTypeName(typeid(RawTime)),
                Integer32::valueOf(PofHelper::t_time));
        hMap->put(Class::getTypeName(typeid(RawTimeInterval)),
                Integer32::valueOf(PofHelper::t_time_interval));
        hMap->put(Class::getTypeName(typeid(RawDateTime)),
                Integer32::valueOf(PofHelper::t_datetime));
        hMap->put(Class::getTypeName(typeid(RawDayTimeInterval)),
                Integer32::valueOf(PofHelper::t_day_time_interval));

        // register collection types
        hMap->put(Class::getTypeName(typeid(HashSet)),
                Integer32::valueOf(PofHelper::t_collection));
        hMap->put(Class::getTypeName(typeid(ArrayList)),
                Integer32::valueOf(PofHelper::t_collection));

        // register map types
        hMap->put(Class::getTypeName(typeid(HashMap)),
                Integer32::valueOf(PofHelper::t_map));
        hMap->put(Class::getTypeName(typeid(SafeHashMap)),
                Integer32::valueOf(PofHelper::t_map));

        // register sparse array type
        hMap->put(Class::getTypeName(typeid(SparseArray)),
                Integer32::valueOf(PofHelper::t_sparse_array));

        return hMap;
        }

    /**
    * @return a Map containing Class name to POF type id mappings.
    */
    Map::View getClassToPofTypeMap()
        {
        static FinalView<Map> vMap(System::common(),
                _createClassToPofTypeMap());
        return vMap;
        }
    COH_STATIC_INIT(getClassToPofTypeMap());
    }


// ----- utility methods ----------------------------------------------------

int32_t PofHelper::getClassTypeId(Object::View v, PofContext::View vCtx)
    {
    COH_ENSURE(NULL != vCtx);
    if (NULL == v)
        {
        return c_null;
        }

    coh_class_id t = v->_getClassId();
    return    COH_CLASS_ID(String)           == t            ? c_string
            : COH_CLASS_ID(Integer32)        == t            ? c_int32
            : COH_CLASS_ID(Integer64)        == t            ? c_int64
            : COH_CLASS_ID(Float64)          == t            ? c_float64
            : COH_CLASS_ID(Integer16)        == t            ? c_int16
            : COH_CLASS_ID(Float32)          == t            ? c_float32
            : COH_CLASS_ID(Octet)            == t            ? c_octet
            : COH_CLASS_ID(Boolean)          == t            ? c_boolean
            : COH_CLASS_ID(Character16)      == t            ? c_char16
            : COH_CLASS_ID(Binary)           == t            ? c_binary
            : COH_CLASS_ID(Array<octet_t>)   == t            ? c_octet_array
            : COH_CLASS_ID(Array<int32_t>)   == t            ? c_int32_array
            : COH_CLASS_ID(Array<int64_t>)   == t            ? c_int64_array
            : COH_CLASS_ID(Array<float64_t>) == t            ? c_float64_array
            : COH_CLASS_ID(Array<wchar16_t>) == t            ? c_char16_array
            : COH_CLASS_ID(Array<bool>)      == t            ? c_boolean_array
            : COH_CLASS_ID(Array<int16_t>)   == t            ? c_int16_array
            : COH_CLASS_ID(Array<float32_t>) == t            ? c_float32_array
            : COH_CLASS_ID(ObjectArray)      == t            ? c_object_array
            // final POF-primitive types MUST be checked before customizable
            // serialization (isUserType); additionally, isUserType is a
            // potentially expensive call, and it is thus desirable to avoid
            // it where possible; similarly instanceof checks are significantly
            // more expensive then classid checks, and thus we hold off the check
            // for PortableObject, in order to benefit the serialization of the
            // above types
            : instanceof<PortableObject::View>(v) || vCtx->isUserType(v)
                                                             ? c_user_type
            // PofIntrinsic marker class used to limit the number of instanceof misses for types
            // which will fall-through to c_user_type
            : instanceof<PofIntrinsic::View>(v)
                ? instanceof<Map::View>(v)                   ? c_map
                : instanceof<Collection::View>(v)            ? c_collection
                : instanceof<LongArray::View>(v)             ? c_sparse_array
                : instanceof<RawDate::View>(v)               ? c_raw_date
                : instanceof<RawDateTime::View>(v)           ? c_raw_date_time
                : instanceof<RawDayTimeInterval::View>(v)    ? c_raw_day_time_interval
                : instanceof<RawTime::View>(v)               ? c_raw_time
                : instanceof<RawTimeInterval::View>(v)       ? c_raw_time_interval
                : instanceof<RawYearMonthInterval::View>(v)  ? c_raw_year_month_interval
                :                                              c_user_type
            :                                                  c_user_type;
    }

int32_t PofHelper::getPofTypeId(Class::View vClass, PofContext::View vCtx)
    {
    COH_ENSURE(NULL != vClass);
    COH_ENSURE(NULL != vCtx);

    Integer32::View vId = cast<Integer32::View>(
            getClassToPofTypeMap()->get(vClass->getName()));

    if (NULL == vId)
        {
        return vCtx->getUserTypeIdentifier(vClass);
        }
    else
        {
        return vId->getValue();
        }
    }

int32_t PofHelper::encodeTinyInt32(int32_t n)
    {
    COH_ENSURE(n >= -1 && n <= 22);
    return v_int_0 - n;
    }

int32_t PofHelper::decodeTinyInt32(int32_t n)
    {
    COH_ENSURE(n <= v_int_neg_1 && n >= v_int_22);
    return v_int_0 - n;
    }

template<class H>
wchar16_t pof_readAsChar16(H& hIn, int32_t nType)
    {
    switch (nType)
        {
        case PofConstants::t_octet:
            return (wchar16_t) hIn->read();

        case PofConstants::t_char:
            return hIn->readChar16();

        default:
            return (wchar16_t) PofHelper::readAsInt32(hIn, nType);
        }
    }

wchar16_t PofHelper::readAsChar16(ReadBuffer::BufferInput::Handle hIn,
        int32_t nType)
    {
    return pof_readAsChar16(hIn, nType);
    }

wchar16_t PofHelper::readAsChar16(FinalHandle<ReadBuffer::BufferInput>& hIn,
        int32_t nType)
    {
    return pof_readAsChar16(hIn, nType);
    }

template<class H>
int32_t pof_readAsInt32(H& hIn, int32_t nType)
    {
    switch (nType)
        {
        case PofConstants::t_boolean:
        case PofConstants::t_int16:
        case PofConstants::t_int32:
        case PofConstants::t_int64:
        case PofConstants::t_int128:
            return hIn->readInt32();

        case PofConstants::t_float32:
            return (int32_t) hIn->readFloat32();

        case PofConstants::t_float64:
            return (int32_t) hIn->readFloat64();

        case PofConstants::t_octet:
            return hIn->read();

        case PofConstants::t_char:
            return hIn->readChar16();

        case PofConstants::v_reference_null:
        case PofConstants::v_boolean_false:
        case PofConstants::v_int_0:
            return 0;

        case PofConstants::v_boolean_true:
        case PofConstants::v_int_1:
            return 1;

        case PofConstants::v_int_neg_1:
        case PofConstants::v_int_2:
        case PofConstants::v_int_3:
        case PofConstants::v_int_4:
        case PofConstants::v_int_5:
        case PofConstants::v_int_6:
        case PofConstants::v_int_7:
        case PofConstants::v_int_8:
        case PofConstants::v_int_9:
        case PofConstants::v_int_10:
        case PofConstants::v_int_11:
        case PofConstants::v_int_12:
        case PofConstants::v_int_13:
        case PofConstants::v_int_14:
        case PofConstants::v_int_15:
        case PofConstants::v_int_16:
        case PofConstants::v_int_17:
        case PofConstants::v_int_18:
        case PofConstants::v_int_19:
        case PofConstants::v_int_20:
        case PofConstants::v_int_21:
        case PofConstants::v_int_22:
            return PofHelper::decodeTinyInt32(nType);

        // REVIEW: COHCPP-180
        case PofConstants::t_float128:
        case PofConstants::t_decimal32:
        case PofConstants::t_decimal64:
        case PofConstants::t_decimal128:
        default:
            COH_THROW_STREAM (IOException, "unable to convert type " << nType
                    << " to a numeric type");
        }
    }

int32_t PofHelper::readAsInt32(ReadBuffer::BufferInput::Handle hIn,
        int32_t nType)
    {
    return pof_readAsInt32(hIn, nType);
    }

int32_t PofHelper::readAsInt32(FinalHandle<ReadBuffer::BufferInput>& hIn,
        int32_t nType)
    {
    return pof_readAsInt32(hIn, nType);
    }

template<class H>
int64_t pof_readAsInt64(H& hIn, int32_t nType)
    {
    switch (nType)
        {
        case PofConstants::t_int64:
        case PofConstants::t_int128:
            return hIn->readInt64();

        case PofConstants::t_float32:
            return (int64_t) hIn->readFloat32();

        case PofConstants::t_float64:
            return (int64_t) hIn->readFloat64();

        // REVIEW: COHCPP-180
        case PofConstants::t_float128:
        case PofConstants::t_decimal32:
        case PofConstants::t_decimal64:
        case PofConstants::t_decimal128:
        default:
            return pof_readAsInt32(hIn, nType);
        }
    }

int64_t PofHelper::readAsInt64(ReadBuffer::BufferInput::Handle hIn,
        int32_t nType)
    {
    return pof_readAsInt64(hIn, nType);
    }

int64_t PofHelper::readAsInt64(FinalHandle<ReadBuffer::BufferInput>& hIn,
        int32_t nType)
    {
    return pof_readAsInt64(hIn, nType);
    }

template<class H>
float32_t pof_readAsFloat32(H& hIn, int32_t nType)
    {
    switch (nType)
        {
        case PofConstants::t_int64:
            return (float32_t) hIn->readInt64();

        case PofConstants::t_float32:
            return hIn->readFloat32();

        case PofConstants::t_float64:
            return (float32_t) hIn->readFloat64();

        case PofConstants::v_fp_neg_infinity:
            return Float32::getNegativeInfinity();

        case PofConstants::v_fp_pos_infinity:
            return Float32::getPositiveInfinity();

        case PofConstants::v_fp_nan:
            return Float32::getNaN();

        // REVIEW: COHCPP-180
        case PofConstants::t_float128:
        case PofConstants::t_int128:
        case PofConstants::t_decimal32:
        case PofConstants::t_decimal64:
        case PofConstants::t_decimal128:
        default:
            return (float32_t) pof_readAsInt32(hIn, nType);
        }
    }

float32_t PofHelper::readAsFloat32(ReadBuffer::BufferInput::Handle hIn,
        int32_t nType)
    {
    return pof_readAsFloat32(hIn, nType);
    }

float32_t PofHelper::readAsFloat32(FinalHandle<ReadBuffer::BufferInput>& hIn,
        int32_t nType)
    {
    return pof_readAsFloat32(hIn, nType);
    }

template<class H>
float64_t pof_readAsFloat64(H& hIn, int32_t nType)
    {
    switch (nType)
        {
        case PofConstants::t_int64:
            return (float64_t) hIn->readInt64();

        case PofConstants::t_float32:
            return (float64_t) hIn->readFloat32();

        case PofConstants::t_float64:
            return hIn->readFloat64();

        case PofConstants::v_fp_neg_infinity:
            return Float64::getNegativeInfinity();

        case PofConstants::v_fp_pos_infinity:
            return Float64::getPositiveInfinity();

        case PofConstants::v_fp_nan:
            return Float64::getNaN();

        // REVIEW: COHCPP-180
        case PofConstants::t_float128:
        case PofConstants::t_int128:
        case PofConstants::t_decimal32:
        case PofConstants::t_decimal64:
        case PofConstants::t_decimal128:
        default:
            return pof_readAsInt32(hIn, nType);
        }
    }

float64_t PofHelper::readAsFloat64(ReadBuffer::BufferInput::Handle hIn,
        int32_t nType)
    {
    return pof_readAsFloat64(hIn, nType);
    }

float64_t PofHelper::readAsFloat64(FinalHandle<ReadBuffer::BufferInput>& hIn,
        int32_t nType)
    {
    return pof_readAsFloat64(hIn, nType);
    }

template<class H>
RawDate::View pof_readRawDate(H& hIn)
    {
    int32_t nYear  = hIn->readInt32();
    int32_t nMonth = hIn->readInt32();
    int32_t nDay   = hIn->readInt32();

    return RawDate::create(nYear, nMonth, nDay);
    }

RawDate::View PofHelper::readRawDate(ReadBuffer::BufferInput::Handle hIn)
    {
    return pof_readRawDate(hIn);
    }

RawDate::View PofHelper::readRawDate(FinalHandle<ReadBuffer::BufferInput>& hIn)
    {
    return pof_readRawDate(hIn);
    }

template<class H>
RawTime::View pof_readRawTime(H& hIn)
    {
    RawTime::View vTime;

    int32_t nHour     = hIn->readInt32();
    int32_t nMinute   = hIn->readInt32();
    int32_t nSecond   = hIn->readInt32();
    int32_t nFraction = hIn->readInt32();
    int32_t nNanos    = nFraction <= 0 ? -nFraction : nFraction * 1000000;

    int32_t nZoneType = hIn->readInt32();
    if (nZoneType == 2)
        {
        int32_t nHourOffset   = hIn->readInt32();
        int32_t nMinuteOffset = hIn->readInt32();
        vTime = RawTime::create(nHour, nMinute, nSecond, nNanos, nHourOffset,
                nMinuteOffset);
        }
    else
        {
        COH_ENSURE(0 == nZoneType || 1 == nZoneType);
        bool fUTC = 1 == nZoneType;
        vTime = RawTime::create(nHour, nMinute, nSecond, nNanos, fUTC);
        }

    return vTime;
    }

RawTime::View PofHelper::readRawTime(ReadBuffer::BufferInput::Handle hIn)
    {
    return pof_readRawTime(hIn);
    }

RawTime::View PofHelper::readRawTime(FinalHandle<ReadBuffer::BufferInput>& hIn)
    {
    return pof_readRawTime(hIn);
    }

void PofHelper::skipValue(ReadBuffer::BufferInput::Handle hIn)
    {
    int32_t nType = hIn->readInt32();
    if (nType == t_identity)
        {
        skipInts(hIn, 1);
        nType = hIn->readInt32();
        }

    skipUniformValue(hIn, nType);
    }

void PofHelper::skipUniformValue(ReadBuffer::BufferInput::Handle hIn,
        int32_t nType)
    {
    switch (nType)
        {
        case t_int16:                  // int16
        case t_int32:                  // int32
        case t_int64:                  // int64
        case t_int128:                 // int128
        case t_reference:              // reference
        case t_boolean:                // boolean
            skipInts(hIn, 1);
            break;

        case t_year_month_interval:    // year-month-interval
            skipInts(hIn, 2);
            break;

        case t_date:                   // date
            skipInts(hIn, 3);
            break;

        case t_time_interval:          // time-interval
            skipInts(hIn, 4);
            break;

        case t_day_time_interval:      // day-time-interval
            skipInts(hIn, 5);
            break;

        case t_float32:                // float32
            hIn->skip(4);
            break;

        case t_float64:                // float64
            hIn->skip(8);
            break;

        case t_float128:               // float128*
            hIn->skip(16);
            break;

        case t_decimal32:              // decimal32
        case t_decimal64:              // decimal64
        case t_decimal128:             // decimal128
            skipInts(hIn, 2);
            break;

        case t_octet:                  // octet
            hIn->skip(1);
            break;

        case t_char:                   // char
            switch (hIn->read() & 0xF0)
                {
                case 0xC0: case 0xD0:
                    hIn->skip(1);
                    break;

                case 0xE0:
                    hIn->skip(2);
                    break;
                }
            break;

        case t_octet_string:           // octet-string
        case t_char_string:            // char-string
            {
            int32_t cb = hIn->readInt32();

            if (cb == v_reference_null)
                {
                break;
                }

            hIn->skip(validateIncomingSize(cb));
            }
            break;

        case t_datetime:               // datetime
            skipInts(hIn, 3);
            // fall through (datetime ends with a time)
        case t_time:                   // time
            {
            skipInts(hIn, 4);
            int32_t nZoneType = hIn->readInt32();
            if (nZoneType == 2)
                {
                skipInts(hIn, 2);
                }
            }
            break;

        case t_collection:             // collection
        case t_array:                  // array
            for (size32_t i = 0, c = validateIncomingSize(hIn->readInt32());
                    i < c; ++i)
                {
                skipValue(hIn);
                }
            break;

        case t_uniform_collection:     // uniform-collection
        case t_uniform_array:          // uniform-array
            {
            int32_t nElementTypeId = hIn->readInt32();
            for (size32_t i = 0, c = validateIncomingSize(hIn->readInt32());
                    i < c; ++i)
                {
                skipUniformValue(hIn, nElementTypeId);
                }
            }
            break;

        case t_sparse_array:           // sparse-array
            for (size32_t i = 0, c = validateIncomingSize(hIn->readInt32());
                    i < c; ++i)
                {
                int32_t iPos = hIn->readInt32();
                if (iPos < 0)
                    {
                    break;
                    }
                skipValue(hIn);
                }
            break;

        case t_uniform_sparse_array:   // uniform-sparse-array
            {
            int32_t nElementTypeId = hIn->readInt32();
            for (size32_t i = 0, c = validateIncomingSize(hIn->readInt32());
                    i < c; ++i)
                {
                int32_t iPos = hIn->readInt32();
                if (iPos < 0)
                    {
                    break;
                    }
                skipUniformValue(hIn, nElementTypeId);
                }
            }
            break;

        case t_map:                    // map
            for (size32_t i = 0, c = validateIncomingSize(hIn->readInt32());
                    i < c; ++i)
                {
                skipValue(hIn); // key
                skipValue(hIn); // value
                }
            break;

        case t_uniform_keys_map:       // uniform-keys-map
            {
            int32_t nKeyTypeId = hIn->readInt32();
            for (size32_t i = 0, c = validateIncomingSize(hIn->readInt32());
                    i < c; ++i)
                {
                skipUniformValue(hIn, nKeyTypeId);
                skipValue(hIn);
                }
            }
            break;

        case t_uniform_map:            // uniform-map
            {
            int32_t nKeyTypeId   = hIn->readInt32();
            int32_t nValueTypeId = hIn->readInt32();
            for (size32_t i = 0, c = validateIncomingSize(hIn->readInt32());
                    i < c; ++i)
                {
                skipUniformValue(hIn, nKeyTypeId);
                skipUniformValue(hIn, nValueTypeId);
                }
            }
            break;

        case v_boolean_false:          // boolean:false
        case v_boolean_true:           // boolean:true
        case v_string_zero_length:     // string:zero-length
        case v_collection_empty:       // collection:empty
        case v_reference_null:         // reference:null
        case v_fp_pos_infinity:        // floating-point:+infinity
        case v_fp_neg_infinity:        // floating-point:-infinity
        case v_fp_nan:                 // floating-point:NaN
        case v_int_neg_1:              // int:-1
        case v_int_0:                  // int:0
        case v_int_1:                  // int:1
        case v_int_2:                  // int:2
        case v_int_3:                  // int:3
        case v_int_4:                  // int:4
        case v_int_5:                  // int:5
        case v_int_6:                  // int:6
        case v_int_7:                  // int:7
        case v_int_8:                  // int:8
        case v_int_9:                  // int:9
        case v_int_10:                 // int:10
        case v_int_11:                 // int:11
        case v_int_12:                 // int:12
        case v_int_13:                 // int:13
        case v_int_14:                 // int:14
        case v_int_15:                 // int:15
        case v_int_16:                 // int:16
        case v_int_17:                 // int:17
        case v_int_18:                 // int:18
        case v_int_19:                 // int:19
        case v_int_20:                 // int:20
        case v_int_21:                 // int:21
        case v_int_22:                 // int:22
            break;

        default:
            if (nType >= 0)
                {
                // user type
                skipInts(hIn, 1); // version id
                while (hIn->readInt32() >= 0)
                    {
                    skipValue(hIn);
                    }
                }
            else
                {
                COH_THROW_STREAM (IllegalStateException, "type=" << nType);
                }
            break;
        }
    }

void PofHelper::skipInts(ReadBuffer::BufferInput::Handle hIn, int32_t c)
    {
    while (c-- > 0)
        {
        while ((hIn->read() & 0x80) != 0) {}
        }
    }

void PofHelper::checkDate(int32_t nYear, int32_t nMonth, int32_t nDay)
    {
    if (nMonth < 1 || nMonth > 12)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "month is out of range: " << nMonth);
        }

    if (nDay < 1 || nDay > getMaxDaysPerMonth()[nMonth-1])
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "day is out of range: " << nDay);
        }

    if (nMonth == 2 && nDay == 29 && (nYear % 4 != 0
            || (nYear % 100 == 0 && nYear % 400 != 0)))
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "not a leap year: " << nYear);
        }
    }

void PofHelper::checkTime(int32_t nHour, int32_t nMinute, int32_t nSecond,
        int32_t nNano)
    {
    if (nHour < 0 || nHour > 23)
        {
        if (nHour == 24 && nMinute == 0 && nSecond == 0 && nNano == 0)
            {
            COH_THROW_STREAM (IllegalArgumentException,
                    "end-of-day midnight (24:00:00.0) is supported by \
                    ISO8601, but use 00:00:00.0 instead");
            }
        else
            {
            COH_THROW_STREAM (IllegalArgumentException,
                    "hour is out of range: " << nHour);
            }
        }

    if (nMinute < 0 || nMinute > 59)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "minute is out of range: " << nMinute);
        }

    // 60 is allowed for a leap-second
    if (nSecond < 0 || (nSecond == 60 && nNano > 0) || nSecond > 60)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "second is out of range: " << nSecond);
        }

    if (nNano < 0 || nNano > 999999999)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "nanosecond is out of range: " << nNano);
        }
    }

void PofHelper::checkTimeZone(int32_t nHourOffset, int32_t nMinuteOffset)
    {
    // technically this is reasonable, but in reality iw should never be
    // over 14; unfortunately, countries keep changing theirs for silly
    // reasons; see http://www.worldtimezone.com/faq.html
    if (nHourOffset < -23 || nHourOffset > 23)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "invalid hour offset: " << nHourOffset);
        }

    // The minute offset should be 0, 15, 30 or 45 for standard timezones, but for
    // non-standard timezones, the minute offset could be any number between 0 and 59.
    // For example, Hong Kong switched from local mean time to standard time in 1904,
    // so prior to 1904, the minute offset was 36.See http://en.wikipedia.org/wiki/Standard_time
    if (nMinuteOffset < 0 || nMinuteOffset > 59)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "invalid minute offset: " << nMinuteOffset);
        }
    }

void PofHelper::checkTimeInterval(int32_t cHours, int32_t cMinutes,
        int32_t cSeconds, int32_t cNanos)
    {
    // duration is allowed to be negative
    if (0 == cHours)
        {
        if (0 == cMinutes)
            {
            if (0 == cSeconds)
                {
                cNanos = abs(cNanos);
                }
            else
                {
                cSeconds = abs(cSeconds);
                }
            }
        else
            {
            cMinutes = abs(cMinutes);
            }
        }
    else
        {
        cHours = abs(cHours);
        }

    // apply the same rules as limit the time values themselves
    checkTime(cHours, cMinutes, cSeconds, cNanos);
    }

void PofHelper::checkDayTimeInterval(int32_t cDays, int32_t cHours,
        int32_t cMinutes, int32_t cSeconds, int32_t cNanos)
    {
    if (0 == cDays)
        {
        checkTimeInterval(cHours, cMinutes, cSeconds, cNanos);
        }
    else
        {
        // number of days is permitted to be any value

        // apply the same rules as limit the time values themselves
        checkTime(cHours, cMinutes, cSeconds, cNanos);
        }
    }

void PofHelper::checkYearMonthInterval(int32_t cYears, int32_t cMonths)
    {
    if (0 == cYears)
        {
        if (cMonths < -11 || cMonths > 11)
            {
            COH_THROW_STREAM (IllegalStateException,
                    "month interval is out of range: " << cMonths);
            }
        }
    }

String::View PofHelper::formatDate(int32_t nYear, int32_t nMonth, int32_t nDay)
    {
    return COH_TO_STRING(
            StringHelper::toDecString(nYear,
                std::max((int32_t) 4, StringHelper::getMaxDecDigits(nYear)))  << "-" <<
            StringHelper::toDecString(nMonth,
                std::max((int32_t) 2, StringHelper::getMaxDecDigits(nMonth))) << "-" <<
            StringHelper::toDecString(nDay,
                std::max((int32_t) 2, StringHelper::getMaxDecDigits(nDay))));
    }

String::View PofHelper::formatTime(int32_t nHour, int32_t nMinute,
        int32_t nSecond, int32_t nNano, bool fUTC)
    {
    String::View vs = COH_TO_STRING(StringHelper::toDecString(nHour,
            std::max((int32_t) 2, StringHelper::getMaxDecDigits(nHour))) << ":"
       << StringHelper::toDecString(nMinute,
            std::max((int32_t) 2, StringHelper::getMaxDecDigits(nMinute))));

    if (nSecond != 0 || nNano != 0)
        {
        vs = COH_TO_STRING (vs << ":" << StringHelper::toDecString(nSecond, std::max((int32_t) 2,
                StringHelper::getMaxDecDigits(nSecond))));

        if (nNano != 0)
            {
            vs = COH_TO_STRING(vs << '.');
            if (nNano % 1000000 == 0)
                {
                vs = COH_TO_STRING(vs << StringHelper::toDecString(nNano / 1000000, 3));
                }
            else
                {
                vs = COH_TO_STRING(vs << StringHelper::toDecString(nNano, 9));
                }
            }
        }

    if (fUTC)
        {
        vs = COH_TO_STRING(vs << 'Z');
        }

    return vs;
    }

String::View PofHelper::formatTime(int32_t nHour, int32_t nMinute,
        int32_t nSecond, int32_t nNano, int32_t nHourOffset,
        int32_t nMinuteOffset)
    {
    String::View vs = formatTime(nHour, nMinute, nSecond, nNano, false);

    if (nHourOffset < 0)
        {
        vs = COH_TO_STRING(vs << '-');
        nHourOffset = -nHourOffset;
        }
    else
        {
        vs = COH_TO_STRING(vs << '+');
        }

    vs = COH_TO_STRING(vs << StringHelper::toDecString(nHourOffset,
            std::max((int32_t) 2, StringHelper::getMaxDecDigits(nHourOffset))) << ":"
       << StringHelper::toDecString(nMinuteOffset,
            std::max((int32_t) 2, StringHelper::getMaxDecDigits(nMinuteOffset))));

    return vs;
    }

void PofHelper::writeDate(WriteBuffer::BufferOutput::Handle hOut,
        int32_t nYear, int32_t nMonth, int32_t nDay)
    {
    hOut->writeInt32(nYear);
    hOut->writeInt32(nMonth);
    hOut->writeInt32(nDay);
    }

void PofHelper::writeTime(WriteBuffer::BufferOutput::Handle hOut,
        int32_t nHour, int32_t nMinute, int32_t nSecond, int32_t nNano,
        int32_t nTimeZoneType, int32_t nHourOffset, int32_t nMinuteOffset)
    {
    int32_t nFraction = 0;
    if (nNano != 0)
        {
        nFraction = nNano % 1000000 == 0 ? nNano / 1000000 : -nNano;
        }

    hOut->writeInt32(nHour);
    hOut->writeInt32(nMinute);
    hOut->writeInt32(nSecond);
    hOut->writeInt32(nFraction);
    hOut->write((octet_t) nTimeZoneType);

    if (nTimeZoneType == 2)
        {
        hOut->writeInt32(nHourOffset);
        hOut->writeInt32(nMinuteOffset);
        }
    }

ObjectArray::Handle PofHelper::resizeArray(ObjectArray::Handle haOld, size32_t cNew)
    {
    ObjectArray::Handle haNew;

    if (haOld == NULL)
        {
        haNew = ObjectArray::create(cNew);
        }
    else
        {
        if (cNew > haOld->length)
            {
            haNew = ObjectArray::create(cNew);;
            }
        else
            {
            haNew = haOld;
            }
        }

    return haNew;
    }


// ----- WriteableEntrySetMap inner class -----------------------------------

PofHelper::WriteableEntrySetMap::WriteableEntrySetMap(Set::View vSetEntries)
        : f_vSetEntries(self(), vSetEntries)
    {
    if (NULL == vSetEntries)
        {
        COH_THROW (IllegalArgumentException::create());
        }
    }

Object::Holder PofHelper::WriteableEntrySetMap::put(Object::View /*vKey*/,
        Object::Holder /*ohValue*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

Object::Holder PofHelper::WriteableEntrySetMap::remove(Object::View /*vKey*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

void PofHelper::WriteableEntrySetMap::clear()
    {
    COH_THROW (UnsupportedOperationException::create());
    }

Set::Handle PofHelper::WriteableEntrySetMap::entrySet()
    {
    COH_THROW (UnsupportedOperationException::create());
    }

Set::View PofHelper::WriteableEntrySetMap::entrySet() const
    {
    return f_vSetEntries;
    }


// ----- ReadableEntrySetMap inner class ------------------------------------

PofHelper::ReadableEntrySetMap::ReadableEntrySetMap()
        : f_hListEntries(self(), ArrayList::create())
    {
    }

Object::Holder PofHelper::ReadableEntrySetMap::put(Object::View vKey,
        Object::Holder ohValue)
    {
    f_hListEntries->add(SimpleMapEntry::create(vKey, ohValue));
    return NULL;
    }

Object::Holder PofHelper::ReadableEntrySetMap::remove(Object::View /*vKey*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

void PofHelper::ReadableEntrySetMap::clear()
    {
    f_hListEntries->clear();
    }

Set::Handle PofHelper::ReadableEntrySetMap::entrySet()
    {
    return ReadOnlyArrayList::create(f_hListEntries->toArray());
    }

Set::View PofHelper::ReadableEntrySetMap::entrySet() const
    {
    return ReadOnlyArrayList::create(f_hListEntries->toArray());
    }

COH_CLOSE_NAMESPACE3
