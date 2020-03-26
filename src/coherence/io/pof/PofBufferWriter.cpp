/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PofBufferWriter.hpp"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/pof/UserTypeWriter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LongArrayIterator.hpp"

#include <limits>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;
using coherence::io::IOException;
using coherence::util::Iterator;
using coherence::util::LongArrayIterator;


// ----- constructors -------------------------------------------------------

PofBufferWriter::PofBufferWriter(WriteBuffer::BufferOutput::Handle hOut,
        PofContext::View vCtx)
        : f_hOut(self(), hOut),
          f_vCtx(self(), vCtx),
          f_hHandler(self(), WritingPofHandler::create(hOut)),
          f_hRefs(self(), NULL)
    {
    COH_ENSURE(NULL != hOut);
    COH_ENSURE(NULL != vCtx);
    }

PofBufferWriter::PofBufferWriter(WritingPofHandler::Handle hHandler,
        PofContext::View vCtx)
        : f_hOut(self(), NULL == hHandler
                ? WriteBuffer::BufferOutput::Handle(NULL)
                : hHandler->getBufferOutput()),
          f_vCtx(self(), vCtx),
          f_hHandler(self(), hHandler),
          f_hRefs(self(), NULL)
    {
    COH_ENSURE(NULL != hHandler);
    COH_ENSURE(NULL != vCtx);
    }


// ----- PofWriter interface ------------------------------------------------

void PofBufferWriter::writeBoolean(int32_t iProp, bool f)
    {
    beginProperty(iProp);
    f_hHandler->onBoolean(iProp, f);
    endProperty(iProp);
    }

void PofBufferWriter::writeOctet(int32_t iProp, octet_t b)
    {
    beginProperty(iProp);
    f_hHandler->onOctet(iProp, b);
    endProperty(iProp);
    }

void PofBufferWriter::writeChar16(int32_t iProp, wchar16_t ch)
    {
    beginProperty(iProp);
    f_hHandler->onChar(iProp, ch);
    endProperty(iProp);
    }

void PofBufferWriter::writeInt16(int32_t iProp, int16_t n)
    {
    beginProperty(iProp);
    f_hHandler->onInt16(iProp, n);
    endProperty(iProp);
    }

void PofBufferWriter::writeInt32(int32_t iProp, int32_t n)
    {
    beginProperty(iProp);
    f_hHandler->onInt32(iProp, n);
    endProperty(iProp);
    }

void PofBufferWriter::writeInt64(int32_t iProp, int64_t l)
    {
    beginProperty(iProp);
    f_hHandler->onInt64(iProp, l);
    endProperty(iProp);
    }

void PofBufferWriter::writeFloat32(int32_t iProp, float32_t fl)
    {
    beginProperty(iProp);
    f_hHandler->onFloat32(iProp, fl);
    endProperty(iProp);
    }

void PofBufferWriter::writeFloat64(int32_t iProp, float64_t dfl)
    {
    beginProperty(iProp);
    f_hHandler->onFloat64(iProp, dfl);
    endProperty(iProp);
    }

void PofBufferWriter::writeBooleanArray(int32_t iProp,
        Array<bool>::View vaf)
    {
    beginProperty(iProp);
    if (NULL == vaf)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = vaf->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, t_boolean);
        for (size32_t i = 0; i < cElements; ++i)
            {
            f_hHandler->onBoolean(validateOutgoingSize(i), vaf[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeOctetArray(int32_t iProp,
        Array<octet_t>::View vab)
    {
    beginProperty(iProp);
    if (NULL == vab)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, vab->length, t_octet);

        f_hOut->write(vab);

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeChar16Array(int32_t iProp,
        Array<wchar16_t>::View vach)
    {
    beginProperty(iProp);
    if (NULL == vach)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = vach->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, t_char);
        for (size32_t i = 0; i < cElements; ++i)
            {
            f_hHandler->onChar(validateOutgoingSize(i), vach[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeInt16Array(int32_t iProp,
        Array<int16_t>::View van)
    {
    beginProperty(iProp);
    if (NULL == van)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = van->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, t_int16);
        for (size32_t i = 0; i < cElements; ++i)
            {
            f_hHandler->onInt16(validateOutgoingSize(i), van[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeInt32Array(int32_t iProp,
        Array<int32_t>::View van)
    {
    beginProperty(iProp);
    if (NULL == van)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = van->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, t_int32);
        for (size32_t i = 0; i < cElements; ++i)
            {
            f_hHandler->onInt32(validateOutgoingSize(i), van[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeInt64Array(int32_t iProp,
        Array<int64_t>::View val)
    {
    beginProperty(iProp);
    if (NULL == val)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = val->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, t_int64);
        for (size32_t i = 0; i < cElements; ++i)
            {
            f_hHandler->onInt64(validateOutgoingSize(i), val[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeFloat32Array(int32_t iProp,
        Array<float32_t>::View vafl)
    {
    beginProperty(iProp);
    if (NULL == vafl)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = vafl->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, t_float32);
        for (size32_t i = 0; i < cElements; ++i)
            {
            f_hHandler->onFloat32(validateOutgoingSize(i), vafl[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeFloat64Array(int32_t iProp,
        Array<float64_t>::View vadfl)
    {
    beginProperty(iProp);
    if (NULL == vadfl)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = vadfl->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, t_float64);
        for (size32_t i = 0; i < cElements; ++i)
            {
            f_hHandler->onFloat64(validateOutgoingSize(i), vadfl[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeBinary(int32_t iProp, Binary::View vBin)
    {
    beginProperty(iProp);
    if (NULL == vBin)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);
        f_hHandler->onOctetString(iProp, vBin);
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeString(int32_t iProp, String::View vs)
    {
    beginProperty(iProp);
    if (NULL == vs)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);
        f_hHandler->onCharString(iProp, vs);
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeRawDate(int32_t iProp, RawDate::View vDate)
    {
    beginProperty(iProp);
    if (NULL == vDate)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);
        f_hHandler->onDate(iProp,
                vDate->getYear(),
                vDate->getMonth(),
                vDate->getDay());
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeRawDateTime(int32_t iProp, RawDateTime::View vdt)
    {
    beginProperty(iProp);
    if (NULL == vdt)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);

        RawDate::View vDate = vdt->getRawDate();
        RawTime::View vTime = vdt->getRawTime();

        if (vTime->hasTimezone())
            {
            if (vTime->isUTC())
                {
                f_hHandler->onDateTime(iProp,
                        vDate->getYear(),
                        vDate->getMonth(),
                        vDate->getDay(),
                        vTime->getHour(),
                        vTime->getMinute(),
                        vTime->getSecond(),
                        vTime->getNano(),
                        true);
                }
            else
                {
                f_hHandler->onDateTime(iProp,
                        vDate->getYear(),
                        vDate->getMonth(),
                        vDate->getDay(),
                        vTime->getHour(),
                        vTime->getMinute(),
                        vTime->getSecond(),
                        vTime->getNano(),
                        vTime->getHourOffset(),
                        vTime->getMinuteOffset());
                }
            }
        else
            {
            f_hHandler->onDateTime(iProp,
                    vDate->getYear(),
                    vDate->getMonth(),
                    vDate->getDay(),
                    vTime->getHour(),
                    vTime->getMinute(),
                    vTime->getSecond(),
                    vTime->getNano(),
                    false);
            }
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeRawDayTimeInterval(int32_t iProp,
        RawDayTimeInterval::View vInterval)
    {
    beginProperty(iProp);
    if (NULL == vInterval)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);
        f_hHandler->onDayTimeInterval(iProp,
                vInterval->getDays(),
                vInterval->getHours(),
                vInterval->getMinutes(),
                vInterval->getSeconds(),
                vInterval->getNanos());
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeRawTime(int32_t iProp, RawTime::View vTime)
    {
    beginProperty(iProp);
    if (NULL == vTime)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);

        if (vTime->hasTimezone())
            {
            if (vTime->isUTC())
                {
                f_hHandler->onTime(iProp,
                        vTime->getHour(),
                        vTime->getMinute(),
                        vTime->getSecond(),
                        vTime->getNano(),
                        true);
                }
            else
                {
                f_hHandler->onTime(iProp,
                        vTime->getHour(),
                        vTime->getMinute(),
                        vTime->getSecond(),
                        vTime->getNano(),
                        vTime->getHourOffset(),
                        vTime->getMinuteOffset());
                }
            }
        else
            {
            f_hHandler->onTime(iProp,
                    vTime->getHour(),
                    vTime->getMinute(),
                    vTime->getSecond(),
                    vTime->getNano(),
                    false);
            }
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeRawTimeInterval(int32_t iProp,
        RawTimeInterval::View vInterval)
    {
    beginProperty(iProp);
    if (NULL == vInterval)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);
        f_hHandler->onTimeInterval(iProp,
                vInterval->getHours(),
                vInterval->getMinutes(),
                vInterval->getSeconds(),
                vInterval->getNanos());
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeRawYearMonthInterval(int32_t iProp,
        RawYearMonthInterval::View vInterval)
    {
    beginProperty(iProp);
    if (NULL == vInterval)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        f_hHandler->registerIdentity(-1);
        f_hHandler->onYearMonthInterval(iProp,
                vInterval->getYears(),
                vInterval->getMonths());
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeObject(int32_t iProp, Object::View v)
    {
    // indicate to the handler that the subsequent value is of a
    // identifiable reference type
    f_hHandler->registerIdentity(-1);

    switch (getClassTypeId(v, getPofContext()))
        {
        case c_null:
            beginProperty(iProp);
            f_hHandler->onNullReference(iProp);
            endProperty(iProp);
            break;

        case c_boolean:
            writeBoolean(iProp, (cast<Boolean::View>(v))->getValue());
            break;

        case c_octet:
            writeOctet(iProp, (cast<Octet::View>(v))->getValue());
            break;

        case c_char16:
            writeChar16(iProp, (cast<Character16::View>(v))->getValue());
            break;

        case c_int16:
            writeInt16(iProp, (cast<Integer16::View>(v))->getValue());
            break;

        case c_int32:
            writeInt32(iProp, (cast<Integer32::View>(v))->getValue());
            break;

        case c_int64:
            writeInt64(iProp, (cast<Integer64::View>(v))->getValue());
            break;

        case c_float32:
            writeFloat32(iProp, (cast<Float32::View>(v))->getValue());
            break;

        case c_float64:
            writeFloat64(iProp, (cast<Float64::View>(v))->getValue());
            break;

        case c_binary:
            writeBinary(iProp, cast<Binary::View>(v));
            break;

        case c_string:
            writeString(iProp, cast<String::View>(v));
            break;

        case c_raw_date:
            writeRawDate(iProp, cast<RawDate::View>(v));
            break;

        case c_raw_date_time:
            writeRawDateTime(iProp, cast<RawDateTime::View>(v));
            break;

        case c_raw_day_time_interval:
            writeRawDayTimeInterval(iProp, cast<RawDayTimeInterval::View>(v));
            break;

        case c_raw_time:
            writeRawTime(iProp, cast<RawTime::View>(v));
            break;

        case c_raw_time_interval:
            writeRawTimeInterval(iProp, cast<RawTimeInterval::View>(v));
            break;

        case c_raw_year_month_interval:
            writeRawYearMonthInterval(iProp, cast<RawYearMonthInterval::View>(v));
            break;

        case c_boolean_array:
            writeBooleanArray(iProp, cast<Array<bool>::View>(v));
            break;

        case c_octet_array:
            writeOctetArray(iProp, cast<Array<octet_t>::View>(v));
            break;

        case c_char16_array:
            writeChar16Array(iProp, cast<Array<wchar16_t>::View>(v));
            break;

        case c_int16_array:
            writeInt16Array(iProp, cast<Array<int16_t>::View>(v));
            break;

        case c_int32_array:
            writeInt32Array(iProp, cast<Array<int32_t>::View>(v));
            break;

        case c_int64_array:
            writeInt64Array(iProp, cast<Array<int64_t>::View>(v));
            break;

        case c_float32_array:
            writeFloat32Array(iProp, cast<Array<float32_t>::View>(v));
            break;

        case c_float64_array:
            writeFloat64Array(iProp, cast<Array<float64_t>::View>(v));
            break;

        case c_object_array:
            writeObjectArray(iProp, cast<ObjectArray::View>(v));
            break;

        case c_sparse_array:
            writeLongArray(iProp, cast<LongArray::View>(v));
            break;

        case c_collection:
            writeCollection(iProp, cast<Collection::View>(v));
            break;

        case c_map:
            writeMap(iProp, cast<Map::View>(v));
            break;

        case c_user_type:
        default:
            writeUserType(iProp, v);
        }
    }

void PofBufferWriter::writeUserType(int32_t iProp, Object::View v)
    {
    bool fEvolvableOld = isEvolvable();
    bool fEvolvable    = fEvolvableOld || instanceof<Evolvable::View>(v);

    setEvolvable(fEvolvable);
    beginProperty(iProp);

    WritingPofHandler::Handle hHandler = f_hHandler;
    int32_t                   iRef     = -1;
    bool                      fRef     = false;
    ReferenceLibrary::Handle  hRefs    = f_hRefs;

    // COH-5065: due to the complexity of maintaining references
    // in future data, we won't support them for Evolvable objects
    if (NULL != hRefs && !fEvolvable)
        {
        iRef = hRefs->getIdentity(v);
        if (iRef < 0)
            {
            iRef = hRefs->registerReference(v);
            }
        else
            {
            fRef = true;
            }
        }

    if (fRef)
        {
        hHandler->onIdentityReference(iProp, iRef);
        }
    else
        {
        PofContext::View vCtx = getPofContext();

        // resolve the user type identifier
        int32_t nTypeId = vCtx->getUserTypeIdentifier(v);

        // create a new PofWriter for the user type
        UserTypeWriter::Handle hWriter = UserTypeWriter::create(this,
                hHandler, vCtx, nTypeId, iProp, iRef);
        if (NULL != hRefs && !fEvolvable)
            {
            hWriter->enableReference();
            }

        // serialize the object using a PofSerializer
        vCtx->getPofSerializer(nTypeId)->serialize(hWriter, v);

        // notify the nested PofWriter that it is closing
        hWriter->closeNested();
        }

    endProperty(iProp);
    setEvolvable(fEvolvableOld);
    }

void PofBufferWriter::writeObjectArray(int32_t iProp, ObjectArray::View va)
    {
    beginProperty(iProp);
    if (NULL == va)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = va->length;
        f_hHandler->registerIdentity(-1);
        f_hHandler->beginArray(iProp, cElements);
        for (size32_t i = 0; i < cElements; ++i)
            {
            writeObject(validateOutgoingSize(i), va[i]);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeObjectArray(int32_t iProp, ObjectArray::View va,
        Class::View vClass)
    {
    // COH-3370: uniform arrays cannot contain null values
    for (size32_t i = 0, c = NULL == va ? 0 : va->length; i < c; ++i)
        {
        if (NULL == va[i])
            {
            writeObjectArray(iProp, va);
            return;
            }
        }

    beginProperty(iProp);
    if (NULL == va)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        ClassLoader::View vLoader = SystemClassLoader::getInstance();

        int32_t nTypeId   = getPofTypeId(vClass, getPofContext());
        size32_t  cElements = va->length;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformArray(iProp, cElements, nTypeId);
        for (size32_t i = 0; i < cElements; ++i)
            {
            Object::View v = va[i];
            assertEqual(vClass, vLoader->loadByName(Class::getClassName(v)));
            writeObject(validateOutgoingSize(i), v);
            }
        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeLongArray(int32_t iProp, LongArray::View vla)
    {
    beginProperty(iProp);
    if (NULL == vla)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        int64_t nSize     = vla->getLastIndex() + 1;
        size32_t  cElements = vla->getSize();
        size32_t  cWritten  = 0;

        if (cElements > 0 && (vla->getFirstIndex() < 0 ||
                nSize > Integer32::max_value))
            {
            COH_THROW_STREAM (IndexOutOfBoundsException,
                    "cannot encode LongArray["
                    << vla->getFirstIndex() << ", " << vla->getLastIndex()
                    << "] as a POF sparse array");
            }

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginSparseArray(iProp, (size32_t) nSize);
        for (LongArrayIterator::Handle hIter = vla->iterator();
             hIter->hasNext(); ++cWritten)
            {
            Object::View v = hIter->next();
            int32_t      n = (int32_t) hIter->getIndex();
            writeObject(n, v);
            }

        // check for under/overflow
        if (cWritten != cElements)
            {
            COH_THROW_STREAM (IOException, "expected to write " << cElements
                    << " objects but actually wrote " << cWritten);
            }

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeLongArray(int32_t iProp, LongArray::View vla,
        Class::View vClass)
    {
    // COH-3370: uniform arrays cannot contain null values
    if (NULL != vla && vla->contains(NULL))
        {
        writeLongArray(iProp, vla);
        return;
        }

    beginProperty(iProp);
    if (NULL == vla)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        ClassLoader::View vLoader = SystemClassLoader::getInstance();

        int64_t nSize     = vla->getLastIndex() + 1;
        int32_t nTypeId   = getPofTypeId(vClass, getPofContext());
        size32_t  cElements = vla->getSize();
        size32_t  cWritten  = 0;

        if (cElements > 0 && (vla->getFirstIndex() < 0 ||
                nSize > Integer32::max_value))
            {
            COH_THROW_STREAM (IndexOutOfBoundsException,
                    "cannot encode LongArray["
                    << vla->getFirstIndex() << ", " << vla->getLastIndex()
                    << "] as a POF sparse array");
            }

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformSparseArray(iProp, (size32_t) nSize, nTypeId);
        for (LongArrayIterator::Handle hIter = vla->iterator();
             hIter->hasNext(); ++cWritten)
            {
            Object::View v = hIter->next();
            int32_t      n = (int32_t) hIter->getIndex();
            assertEqual(vClass, vLoader->loadByName(Class::getClassName(v)));
            writeObject(n, v);
            }

        // check for under/overflow
        if (cWritten != cElements)
            {
            COH_THROW_STREAM (IOException, "expected to write " << cElements
                    << " objects but actually wrote " << cWritten);
            }

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeCollection(int32_t iProp, Collection::View vCol)
    {
    beginProperty(iProp);
    if (NULL == vCol)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = vCol->size();
        size32_t cWritten  = 0;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginCollection(iProp, cElements);
        for (Iterator::Handle hIter = vCol->iterator(); hIter->hasNext();
             ++cWritten)
            {
            writeObject(validateOutgoingSize(cWritten), hIter->next());
            }

        // check for under/overflow
        if (cWritten != cElements)
            {
            COH_THROW_STREAM (IOException, "expected to write " << cElements
                    << " objects but actually wrote " << cWritten);
            }

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeCollection(int32_t iProp, Collection::View vCol,
        Class::View vClass)
    {
    // COH-3370: uniform collections cannot contain null values
    if (NULL != vCol)
        {
        if (vCol->contains(NULL))
            {
            writeCollection(iProp, vCol);
            return;
            }
        }

    beginProperty(iProp);
    if (NULL == vCol)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        ClassLoader::View vLoader = SystemClassLoader::getInstance();

        int32_t nTypeId   = getPofTypeId(vClass, getPofContext());
        size32_t  cElements = vCol->size();
        size32_t  cWritten  = 0;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformCollection(iProp, cElements, nTypeId);
        for (Iterator::Handle hIter = vCol->iterator(); hIter->hasNext();
             ++cWritten)
            {
            Object::View v = hIter->next();
            assertEqual(vClass, vLoader->loadByName(Class::getClassName(v)));
            writeObject(validateOutgoingSize(cWritten), v);
            }

        // check for under/overflow
        if (cWritten != cElements)
            {
            COH_THROW_STREAM (IOException, "expected to write " << cElements
                    << " objects but actually wrote " << cWritten);
            }

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeMap(int32_t iProp, Map::View vMap)
    {
    beginProperty(iProp);
    if (NULL == vMap)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        size32_t cElements = vMap->size();
        size32_t cWritten  = 0;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginMap(iProp, cElements);
        for (Iterator::Handle hIter = vMap->entrySet()->iterator();
             hIter->hasNext(); ++cWritten)
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
            Object::View     vKey   = vEntry->getKey();
            Object::View     vValue = vEntry->getValue();

            writeObject(0, vKey);   // index is ignored
            writeObject(0, vValue); // index is ignored
            }

        // check for under/overflow
        if (cWritten != cElements)
            {
            COH_THROW_STREAM (IOException, "expected to write " << cElements
                    << " objects but actually wrote " << cWritten);
            }

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeMap(int32_t iProp, Map::View vMap,
        Class::View vClassKey)
    {
    // COH-3370: uniform maps cannot contain null keys
    if (NULL != vMap)
        {
        if (vMap->containsKey(NULL))
            {
            writeMap(iProp, vMap);
            return;
            }
        }

    beginProperty(iProp);
    if (NULL == vMap)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        ClassLoader::View vLoader = SystemClassLoader::getInstance();

        int32_t nTypeId   = getPofTypeId(vClassKey, getPofContext());
        size32_t  cElements = vMap->size();
        size32_t  cWritten  = 0;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformKeysMap(iProp, cElements, nTypeId);
        for (Iterator::Handle hIter = vMap->entrySet()->iterator();
             hIter->hasNext(); ++cWritten)
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
            Object::View     vKey   = vEntry->getKey();
            Object::View     vValue = vEntry->getValue();

            assertEqual(vClassKey, vLoader->loadByName(Class::getClassName(vKey)));
            writeObject(0, vKey);   // index is ignored
            writeObject(0, vValue); // index is ignored
            }

        // check for under/overflow
        if (cWritten != cElements)
            {
            COH_THROW_STREAM (IOException, "expected to write " << cElements
                    << " objects but actually wrote " << cWritten);
            }

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

void PofBufferWriter::writeMap(int32_t iProp, Map::View vMap,
        Class::View vClassKey, Class::View vClassValue)
    {
    // COH-3370: uniform maps cannot contain null keys or values
    if (NULL != vMap)
        {
        for (Iterator::Handle hIter = vMap->entrySet()->iterator();
                hIter->hasNext(); )
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
            if (NULL == vEntry->getKey() || NULL == vEntry->getValue())
                {
                writeMap(iProp, vMap);
                return;
                }
            }
        }

    beginProperty(iProp);
    if (NULL == vMap)
        {
        f_hHandler->onNullReference(iProp);
        }
    else
        {
        ClassLoader::View vLoader = SystemClassLoader::getInstance();
        PofContext::View  vCtx    = getPofContext();

        int32_t nTypeIdKey   = getPofTypeId(vClassKey, vCtx);
        int32_t nTypeIdValue = getPofTypeId(vClassValue, vCtx);
        size32_t  cElements    = vMap->size();
        size32_t  cWritten     = 0;

        f_hHandler->registerIdentity(-1);
        f_hHandler->beginUniformMap(iProp, cElements, nTypeIdKey, nTypeIdValue);
        for (Iterator::Handle hIter = vMap->entrySet()->iterator();
             hIter->hasNext(); ++cWritten)
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
            Object::View     vKey   = vEntry->getKey();
            Object::View     vValue = vEntry->getValue();

            assertEqual(vClassKey,   vLoader->loadByName(Class::getClassName(vKey)));
            assertEqual(vClassValue, vLoader->loadByName(Class::getClassName(vValue)));
            writeObject(0, vKey);   // index is ignored
            writeObject(0, vValue); // index is ignored
            }

        // check for under/overflow
        if (cWritten != cElements)
            {
            COH_THROW_STREAM (IOException, "expected to write " << cElements
                    << " objects but actually wrote " << cWritten);
            }

        f_hHandler->endComplexValue();
        }
    endProperty(iProp);
    }

PofContext::View PofBufferWriter::getPofContext() const
    {
    return f_vCtx;
    }

void PofBufferWriter::setPofContext(PofContext::View vCtx)
    {
    if (NULL == vCtx)
        {
        COH_THROW (IllegalArgumentException::create(
                "PofContext cannot be NULL"));
        }
    initialize(f_vCtx, vCtx);
    }

int32_t PofBufferWriter::getUserTypeId() const
    {
    return -1;
    }

int32_t PofBufferWriter::getVersionId() const
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }

void PofBufferWriter::setVersionId(int32_t /*nVersionId*/)
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }

PofWriter::Handle PofBufferWriter::createNestedPofWriter(int32_t /*iProp*/)
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }

PofWriter::Handle PofBufferWriter::createNestedPofWriter(int32_t /*iProp*/, int32_t /*nTypeId*/)
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }

void PofBufferWriter::writeRemainder(Binary::View /*vBinProps*/)
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }


// ----- internal methods ---------------------------------------------------

PofBufferWriter::Handle PofBufferWriter::getParentWriter()
    {
    return NULL;
    }

PofBufferWriter::View PofBufferWriter::getParentWriter() const
    {
    return NULL;
    }

void PofBufferWriter::beginProperty(int32_t iProp)
    {
    if (iProp > 0 && NULL == f_hHandler->getComplex())
        {
        COH_THROW (IllegalArgumentException::create("not in a complex type"));
        }
    }

void PofBufferWriter::endProperty(int32_t /*iProp*/)
    {
    }

void PofBufferWriter::assertEqual(Class::View vClass, Class::View vClassTest)
    {
    if (!vClass->equals(vClassTest))
        {
        COH_THROW_STREAM (IllegalArgumentException, "illegal class \""
                << vClassTest->getName() << "\"; expected \""
                << vClass->getName() << '"');
        }
    }


// ----- accessors ----------------------------------------------------------

void PofBufferWriter::enableReference()
    {
    if (f_hRefs == NULL)
        {
        initialize(f_hRefs, ReferenceLibrary::create());
        }
    }

bool PofBufferWriter::isReferenceEnabled()
    {
    return NULL != f_hRefs;
    }

WriteBuffer::BufferOutput::Handle PofBufferWriter::getBufferOutput()
    {
    return f_hOut;
    }

WriteBuffer::BufferOutput::View PofBufferWriter::getBufferOutput() const
    {
    return f_hOut;
    }

WritingPofHandler::Handle PofBufferWriter::getPofHandler()
    {
    return f_hHandler;
    }

WritingPofHandler::View PofBufferWriter::getPofHandler() const
    {
    return f_hHandler;
    }

bool PofBufferWriter::isEvolvable()
    {
    return m_fEvolvable;
    }

void PofBufferWriter::setEvolvable(bool fEvolvable)
    {
    m_fEvolvable = fEvolvable;
    }

COH_CLOSE_NAMESPACE3
