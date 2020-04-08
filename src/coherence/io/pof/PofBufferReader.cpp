/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PofBufferReader.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/io/pof/IdentityHolder.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/UserTypeReader.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LongArrayIterator.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/SparseArray.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include <limits>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::IOException;
using coherence::util::ArrayList;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::LongArrayIterator;
using coherence::util::ReadOnlyArrayList;
using coherence::util::SparseArray;
using coherence::util::StringHelper;


// ----- constructors -------------------------------------------------------

PofBufferReader::PofBufferReader(ReadBuffer::BufferInput::Handle hIn,
        PofContext::View vCtx)
        : f_hIn(self(), hIn), f_vCtx(self(), vCtx), f_hlaRefs(self())
    {
    }


// ----- PofReader interface ------------------------------------------------

bool PofBufferReader::readBoolean(int32_t iProp)
    {
    return readInt32(iProp) != 0;
    }

octet_t PofBufferReader::readOctet(int32_t iProp)
    {
    return (octet_t) readInt32(iProp);
    }

wchar16_t PofBufferReader::readChar16(int32_t iProp)
    {
    return (wchar16_t) readInt32(iProp);
    }

int16_t PofBufferReader::readInt16(int32_t iProp)
    {
    return (int16_t) readInt32(iProp);
    }

int32_t PofBufferReader::readInt32(int32_t iProp)
    {
    int32_t n = 0;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                n = readInt32(iProp);
                registerIdentity(nId, Integer32::valueOf(n));
                }
                break;

            case t_reference:
                {
                Number::View vNumber = cast<Number::View>(
                        lookupIdentity(f_hIn->readInt32()));
                if (NULL != vNumber)
                    {
                    n = vNumber->getInt32Value();
                    }
                }
                break;

            case v_reference_null:
            case v_int_0:
                break;

            default:
                n = readAsInt32(f_hIn, nType);
                break;
            }
        }
    complete(iProp);

    return n;
    }

int64_t PofBufferReader::readInt64(int32_t iProp)
    {
    int64_t n = 0;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                n = readInt64(iProp);
                registerIdentity(nId, Integer64::valueOf(n));
                }
                break;

            case t_reference:
                {
                Number::View vNumber = cast<Number::View>(
                        lookupIdentity(f_hIn->readInt32()));
                if (NULL != vNumber)
                    {
                    n = vNumber->getInt64Value();
                    }
                }
                break;

            case v_reference_null:
            case v_int_0:
                break;

            default:
                n = readAsInt64(f_hIn, nType);
                break;
            }
        }
    complete(iProp);

    return n;
    }

float32_t PofBufferReader::readFloat32(int32_t iProp)
    {
    float32_t fl = 0.0;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                fl = readFloat32(iProp);
                registerIdentity(nId, Float32::valueOf(fl));
                }
                break;

            case t_reference:
                {
                Number::View vNumber = cast<Number::View>(
                        lookupIdentity(f_hIn->readInt32()));
                if (NULL != vNumber)
                    {
                    fl = vNumber->getFloat32Value();
                    }
                }
                break;

            case v_reference_null:
            case v_int_0:
                break;

            default:
                fl = readAsFloat32(f_hIn, nType);
                break;
            }
        }
    complete(iProp);

    return fl;
    }

float64_t PofBufferReader::readFloat64(int32_t iProp)
    {
    float64_t dfl = 0.0;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                dfl = readFloat64(iProp);
                registerIdentity(nId, Float64::valueOf(dfl));
                }
                break;

            case t_reference:
                {
                Number::View vNumber = cast<Number::View>(
                        lookupIdentity(f_hIn->readInt32()));
                if (NULL != vNumber)
                    {
                    dfl = vNumber->getFloat64Value();
                    }
                }
                break;

            case v_reference_null:
            case v_int_0:
                break;

            default:
                dfl = readAsFloat64(f_hIn, nType);
                break;
            }
        }
    complete(iProp);

    return dfl;
    }

Array<bool>::Handle PofBufferReader::readBooleanArray(int32_t iProp)
    {
    Array<bool>::Handle haf = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                haf = readBooleanArray(iProp);
                registerIdentity(nId, haf);
                }
                break;

            case t_reference:
                haf = cast<Array<bool>::Handle>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case v_string_zero_length:
            case v_collection_empty:
                haf = getEmptyBooleanArray();
                break;

            case t_collection:
            case t_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                haf = Array<bool>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    haf[i] = readAsInt32(f_hIn, f_hIn->readInt32()) != 0;
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t   nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                haf = Array<bool>::create(cElements);
                switch (nElementType)
                    {
                    case t_boolean:
                    case t_int16:
                    case t_int32:
                    case t_int64:
                    case t_int128:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            haf[i] = f_hIn->readInt32() != 0;
                            }
                        break;

                    default:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            haf[i] = readAsInt32(f_hIn, nElementType) != 0;
                            }
                        break;
                    }
                }
                break;

            case t_sparse_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                haf = Array<bool>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    haf[validateIncomingSize(iElement)] =
                            readAsInt32(f_hIn, f_hIn->readInt32()) != 0;
                    }
                while (cElements-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                haf = Array<bool>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    haf[validateIncomingSize(iElement)] =
                            readAsInt32(f_hIn, nElementType) != 0;
                    }
                while (cElements-- > 0);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return haf;
    }

Array<octet_t>::Handle PofBufferReader::readOctetArray(int32_t iProp)
    {
    Array<octet_t>::Handle hab = NULL;

     if (advanceTo(iProp))
         {
         int32_t nType = f_hIn->readInt32();
         switch (nType)
             {
             case t_identity:
                 {
                 int32_t nId = f_hIn->readInt32();
                 hab = readOctetArray(iProp);
                 registerIdentity(nId, hab);
                 }
                 break;

             case t_reference:
                 {
                 Object::Holder oh   = lookupIdentity(f_hIn->readInt32());
                 Binary::View   vBin = cast<Binary::View>(oh, false);
                 if (NULL == vBin)
                     {
                     hab = cast<Array<octet_t>::Handle>(oh);
                     }
                 else
                     {
                     hab = cast<Array<octet_t>::Handle>(
                             vBin->toOctetArray()->clone());
                     }
                 }
                 break;

             case v_reference_null:
                 break;

             case v_string_zero_length:
             case v_collection_empty:
                 hab = getEmptyOctetArray();
                 break;

             case t_octet_string:
                 hab = Array<octet_t>::create(
                         validateIncomingSize(f_hIn->readInt32()));
                 f_hIn->read(hab);
                 break;

             case t_collection:
             case t_array:
                 {
                 size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                 hab = Array<octet_t>::create(cElements);
                 for (size32_t i = 0; i < cElements; ++i)
                     {
                     hab[i] = (octet_t) readAsInt32(f_hIn, f_hIn->readInt32());
                     }
                 }
                 break;

             case t_uniform_collection:
             case t_uniform_array:
                 {
                 int32_t nElementType = f_hIn->readInt32();
                 size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());

                 hab = Array<octet_t>::create(cElements);
                 if (t_octet == nElementType)
                     {
                     f_hIn->read(hab);
                     }
                 else
                     {
                     for (size32_t i = 0; i < cElements; ++i)
                         {
                         hab[i] = (octet_t) readAsInt32(f_hIn, nElementType);
                         }
                     }
                 }
                 break;

             case t_sparse_array:
                 {
                 size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                 hab = Array<octet_t>::create(cElements);
                 do
                     {
                     int32_t iElement = f_hIn->readInt32();
                     if (iElement < 0)
                         {
                         break;
                         }
                     hab[validateIncomingSize(iElement)] =
                             (octet_t) readAsInt32(f_hIn, f_hIn->readInt32());
                     }
                 while (cElements-- > 0);
                 }
                 break;

             case t_uniform_sparse_array:
                 {
                 int32_t nElementType = f_hIn->readInt32();
                 size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                 hab = Array<octet_t>::create(cElements);
                 do
                     {
                     int32_t iElement = f_hIn->readInt32();
                     if (iElement < 0)
                         {
                         break;
                         }
                     hab[validateIncomingSize(iElement)]
                                = t_octet == nElementType
                                    ? f_hIn->read()
                                    : (octet_t) readAsInt32(f_hIn, nElementType);
                     }
                 while (cElements-- > 0);
                 }
                 break;

             default:
                 COH_THROW_STREAM (IOException, "unable to convert type "
                         << nType << " to an array type");
             }
         }
     complete(iProp);

     return hab;
    }

Array<wchar16_t>::Handle PofBufferReader::readChar16Array(int32_t iProp)
    {
    Array<wchar16_t>::Handle hach = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                hach = readChar16Array(iProp);
                registerIdentity(nId, hach);
                }
                break;

            case t_reference:
                {
                Object::Holder oh = lookupIdentity(f_hIn->readInt32());
                String::View   vs = cast<String::View>(oh, false);
                if (NULL == vs)
                    {
                    hach = cast<Array<wchar16_t>::Handle>(oh);
                    }
                else
                    {
                    size32_t    cch = vs->length();
                    const char* ach = vs->getCString();

                    hach = Array<wchar16_t>::create(cch);
                    for (size32_t i = 0; i < cch; ++i)
                        {
                        hach[i] = String::next(ach);
                        }
                    }
                }
                break;

            case v_reference_null:
                break;

            case v_string_zero_length:
            case v_collection_empty:
                hach = getEmptyChar16Array();
                break;

            case t_octet_string:
                {
                size32_t cb = validateIncomingSize(f_hIn->readInt32());

                Array<octet_t>::Handle hab = Array<octet_t>::create(cb);
                f_hIn->read(hab);

                hach = Array<wchar16_t>::create(cb);
                for (size32_t of = 0; of < cb; ++cb)
                    {
                    hach[of] = (wchar16_t) (hab[of] & 0xFF);
                    }
                }
                break;

            case t_char_string:
                {
                String::View vs  = f_hIn->readString();
                size32_t     cch = vs->length();
                const char*  ach = vs->getCString();
                hach = Array<wchar16_t>::create(cch);
                for (size32_t i = 0; i < cch; ++i)
                    {
                    hach[i] = String::next(ach);
                    }
                }
                break;

            case t_collection:
            case t_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                hach = Array<wchar16_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    hach[i] = (wchar16_t) readAsInt32(f_hIn, f_hIn->readInt32());
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                hach = Array<wchar16_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    hach[i] = t_char == nElementType
                             ? f_hIn->readChar16()
                             : (wchar16_t) readAsInt32(f_hIn, nElementType);
                    }
                }
                break;

            case t_sparse_array:
                {
                size32_t  cElements = validateIncomingSize(f_hIn->readInt32());
                hach = Array<wchar16_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hach[validateIncomingSize(iElement)] =
                            (wchar16_t) readAsInt32(f_hIn, f_hIn->readInt32());
                    }
                while (cElements-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                hach = Array<wchar16_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hach[iElement] = t_char == nElementType
                            ? f_hIn->readChar16()
                            : (wchar16_t) readAsInt32(f_hIn, nElementType);
                    }
                while (cElements-- > 0);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return hach;
    }

Array<int16_t>::Handle PofBufferReader::readInt16Array(int32_t iProp)
    {
    Array<int16_t>::Handle han = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                han = readInt16Array(iProp);
                registerIdentity(nId, han);
                }
                break;

            case t_reference:
                han = cast<Array<int16_t>::Handle>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case v_collection_empty:
                han = getEmptyInt16Array();
                break;

            case t_collection:
            case t_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                han = Array<int16_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    han[i] = (int16_t) readAsInt32(f_hIn, f_hIn->readInt32());
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                han = Array<int16_t>::create(cElements);
                switch (nElementType)
                    {
                    case t_int16:
                    case t_int32:
                    case t_int64:
                    case t_int128:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            han[i] = (int16_t) f_hIn->readInt32();
                            }
                        break;

                    default:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            han[i] = (int16_t) readAsInt32(f_hIn, nElementType);
                            }
                        break;
                    }
                }
                break;

            case t_sparse_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                han = Array<int16_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    han[validateIncomingSize(iElement)] =
                            (int16_t) readAsInt32(f_hIn, f_hIn->readInt32());
                    }
                while (cElements-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                han = Array<int16_t>::create(cElements);
                switch (nElementType)
                    {
                    case t_int16:
                    case t_int32:
                    case t_int64:
                    case t_int128:
                        do
                            {
                            int32_t iElement = f_hIn->readInt32();
                            if (iElement < 0)
                                {
                                break;
                                }
                            han[validateIncomingSize(iElement)] =
                                    (int16_t) f_hIn->readInt32();
                            }
                        while (cElements-- > 0);
                        break;

                    default:
                        do
                            {
                            int32_t iElement = f_hIn->readInt32();
                            if (iElement < 0)
                                {
                                break;
                                }
                            han[validateIncomingSize(iElement)] =
                                    (int16_t) readAsInt32(f_hIn, nElementType);
                            }
                        while (cElements-- > 0);
                        break;
                    }
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return han;
    }

Array<int32_t>::Handle PofBufferReader::readInt32Array(int32_t iProp)
    {
    Array<int32_t>::Handle han = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                han = readInt32Array(iProp);
                registerIdentity(nId, han);
                }
                break;

            case t_reference:
                han = cast<Array<int32_t>::Handle>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case v_collection_empty:
                han = getEmptyInt32Array();
                break;

            case t_collection:
            case t_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                han = Array<int32_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    han[i] = readAsInt32(f_hIn, f_hIn->readInt32());
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                han = Array<int32_t>::create(cElements);
                switch (nElementType)
                    {
                    case t_int16:
                    case t_int32:
                    case t_int64:
                    case t_int128:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            han[i] = f_hIn->readInt32();
                            }
                        break;

                    default:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            han[i] = readAsInt32(f_hIn, nElementType);
                            }
                        break;
                    }
                }
                break;

            case t_sparse_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                han = Array<int32_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    han[validateIncomingSize(iElement)] =
                            readAsInt32(f_hIn, f_hIn->readInt32());
                    }
                while (cElements-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                han = Array<int32_t>::create(cElements);
                switch (nElementType)
                    {
                    case t_int16:
                    case t_int32:
                    case t_int64:
                    case t_int128:
                        do
                            {
                            int32_t iElement = f_hIn->readInt32();
                            if (iElement < 0)
                                {
                                break;
                                }
                            han[validateIncomingSize(iElement)] =
                                    f_hIn->readInt32();
                            }
                        while (cElements-- > 0);
                        break;

                    default:
                        do
                            {
                            int32_t iElement = f_hIn->readInt32();
                            if (iElement < 0)
                                {
                                break;
                                }
                            han[validateIncomingSize(iElement)] =
                                    readAsInt32(f_hIn, nElementType);
                            }
                        while (cElements-- > 0);
                        break;
                    }
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return han;
    }

Array<int64_t>::Handle PofBufferReader::readInt64Array(int32_t iProp)
    {
    Array<int64_t>::Handle han = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                han = readInt64Array(iProp);
                registerIdentity(nId, han);
                }
                break;

            case t_reference:
                han = cast<Array<int64_t>::Handle>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case v_collection_empty:
                han = getEmptyInt64Array();
                break;

            case t_collection:
            case t_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                han = Array<int64_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    han[i] = readAsInt64(f_hIn, f_hIn->readInt32());
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                han = Array<int64_t>::create(cElements);
                switch (nElementType)
                    {
                    case t_int16:
                    case t_int32:
                    case t_int64:
                    case t_int128:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            han[i] = f_hIn->readInt64();
                            }
                        break;

                    default:
                        for (size32_t i = 0; i < cElements; ++i)
                            {
                            han[i] = readAsInt64(f_hIn, nElementType);
                            }
                        break;
                    }
                }
                break;

            case t_sparse_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                han = Array<int64_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    han[validateIncomingSize(iElement)] =
                            readAsInt64(f_hIn, f_hIn->readInt32());
                    }
                while (cElements-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                han = Array<int64_t>::create(cElements);
                switch (nElementType)
                    {
                    case t_int16:
                    case t_int32:
                    case t_int64:
                    case t_int128:
                        do
                            {
                            int32_t iElement = f_hIn->readInt32();
                            if (iElement < 0)
                                {
                                break;
                                }
                            han[validateIncomingSize(iElement)] =
                                    f_hIn->readInt64();
                            }
                        while (cElements-- > 0);
                        break;

                    default:
                        do
                            {
                            int32_t iElement = f_hIn->readInt32();
                            if (iElement < 0)
                                {
                                break;
                                }
                            han[validateIncomingSize(iElement)] =
                                    readAsInt64(f_hIn, nElementType);
                            }
                        while (cElements-- > 0);
                        break;
                    }
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return han;
    }

Array<float32_t>::Handle PofBufferReader::readFloat32Array(int32_t iProp)
    {
    Array<float32_t>::Handle hafl = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                hafl = readFloat32Array(iProp);
                registerIdentity(nId, hafl);
                }
                break;

            case t_reference:
                hafl = cast<Array<float32_t>::Handle>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case v_collection_empty:
                hafl = getEmptyFloat32Array();
                break;

            case t_collection:
            case t_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                hafl = Array<float32_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    hafl[i] = readAsFloat32(f_hIn, f_hIn->readInt32());
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                hafl = Array<float32_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    hafl[i] = t_float32 == nElementType
                             ? f_hIn->readFloat32()
                             : readAsFloat32(f_hIn, nElementType);
                    }
                }
                break;

            case t_sparse_array:
                {
                int32_t cElements = f_hIn->readInt32();
                hafl = Array<float32_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hafl[validateIncomingSize(iElement)] =
                            readAsFloat32(f_hIn, f_hIn->readInt32());
                    }
                while (cElements-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                hafl = Array<float32_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hafl[validateIncomingSize(iElement)]
                                = t_float32 == nElementType
                                    ? f_hIn->readFloat32()
                                    : readAsFloat32(f_hIn, nElementType);
                    }
                while (cElements-- > 0);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return hafl;
    }

Array<float64_t>::Handle PofBufferReader::readFloat64Array(int32_t iProp)
    {
    Array<float64_t>::Handle hadfl = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                hadfl = readFloat64Array(iProp);
                registerIdentity(nId, hadfl);
                }
                break;

            case t_reference:
                hadfl = cast<Array<float64_t>::Handle>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case v_collection_empty:
                hadfl = getEmptyFloat64Array();
                break;

            case t_collection:
            case t_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                hadfl = Array<float64_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    hadfl[i] = readAsFloat64(f_hIn, f_hIn->readInt32());
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                hadfl = Array<float64_t>::create(cElements);
                for (size32_t i = 0; i < cElements; ++i)
                    {
                    hadfl[i] = t_float64 == nElementType
                              ? f_hIn->readFloat64()
                              : readAsFloat64(f_hIn, nElementType);
                    }
                }
                break;

            case t_sparse_array:
                {
                size32_t cElements = validateIncomingSize(f_hIn->readInt32());
                hadfl = Array<float64_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hadfl[validateIncomingSize(iElement)] =
                            readAsFloat64(f_hIn, f_hIn->readInt32());
                    }
                while (cElements-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cElements    = validateIncomingSize(f_hIn->readInt32());
                hadfl = Array<float64_t>::create(cElements);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hadfl[validateIncomingSize(iElement)]
                                 = t_float64 == nElementType
                                     ? f_hIn->readFloat64()
                                     : readAsFloat64(f_hIn, nElementType);
                    }
                while (cElements-- > 0);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return hadfl;
    }

Binary::View PofBufferReader::readBinary(int32_t iProp)
    {
    Binary::View vBin = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vBin = readBinary(iProp);
                registerIdentity(nId, vBin);
                }
                break;

            case t_reference:
                {
                Object::Holder       oh  = lookupIdentity(f_hIn->readInt32());
                Array<octet_t>::View vab = cast<Array<octet_t>::View>(oh, false);
                if (NULL == vab)
                    {
                    vBin = cast<Binary::View>(oh);
                    }
                else
                    {
                    vBin = Binary::create(vab, 0, vab->length);
                    }
                }
                break;

            case v_reference_null:
                break;

            case v_string_zero_length:
            case v_collection_empty:
                vBin = getEmptyBinary();
                break;

            case t_octet_string:
                {
                size32_t cb = validateIncomingSize(f_hIn->readInt32());
                size32_t of = f_hIn->getOffset();
                f_hIn->skip(cb);
                vBin = Binary::create(f_hIn->getBuffer()->toOctetArray(of, cb),
                        0, cb);
                }
                break;

            case t_collection:
            case t_array:
                {
                size32_t cb = validateIncomingSize(f_hIn->readInt32());

                Array<octet_t>::Handle hab = Array<octet_t>::create(cb);
                Array<octet_t>::View   vab = hab;
                for (size32_t i = 0; i < cb; ++i)
                    {
                    hab[i] = (octet_t) readAsInt32(f_hIn, f_hIn->readInt32());
                    }

                hab  = NULL;
                vBin = Binary::create(vab, 0, cb);
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t  nElementType = f_hIn->readInt32();
                size32_t cb = validateIncomingSize(f_hIn->readInt32());

                Array<octet_t>::Handle hab = Array<octet_t>::create(cb);
                Array<octet_t>::View   vab = hab;
                if (nElementType == t_octet)
                    {
                    f_hIn->read(hab);
                    }
                else
                    {
                    for (size32_t i = 0; i < cb; ++i)
                        {
                        hab[i] = (octet_t) readAsInt32(f_hIn, nElementType);
                        }
                    }

                hab  = NULL;
                vBin = Binary::create(vab, 0, cb);
                }
                break;

            case t_sparse_array:
                {
                size32_t cb = validateIncomingSize(f_hIn->readInt32());

                Array<octet_t>::Handle hab = Array<octet_t>::create(cb);
                Array<octet_t>::View   vab = hab;
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hab[validateIncomingSize(iElement)] =
                            (octet_t) readAsInt32(f_hIn, f_hIn->readInt32());
                    }
                while (cb-- > 0);

                hab  = NULL;
                vBin = Binary::create(vab, 0, cb);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t  nElementType = f_hIn->readInt32();
                size32_t cb = validateIncomingSize(f_hIn->readInt32());

                Array<octet_t>::Handle hab = Array<octet_t>::create(cb);
                Array<octet_t>::View   vab = hab;
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hab[validateIncomingSize(iElement)]
                            = nElementType == t_octet
                                ? f_hIn->read()
                                : (octet_t) readAsInt32(f_hIn, nElementType);
                    }
                while (cb-- > 0);

                hab  = NULL;
                vBin = Binary::create(vab, 0, cb);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a Binary type");
            }
        }
    complete(iProp);

    return vBin;
    }

String::View PofBufferReader::readString(int32_t iProp)
    {
    String::View vs = NULL;
    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vs = readString(iProp);
                registerIdentity(nId, vs);
                }
                break;

            case t_reference:
                {
                Object::Holder oh = lookupIdentity(f_hIn->readInt32());

                Array<octet_t>::View vab = cast<Array<octet_t>::View>(oh, false);
                if (NULL == vab)
                    {
                    Binary::View vBin = cast<Binary::View>(oh, false);
                    if (NULL == vBin)
                        {
                        vs = cast<String::View>(oh);
                        }
                    else
                        {
                        vs = String::create(vBin->toOctetArray());
                        }
                    }
                else
                    {
                    vs = String::create(vab);
                    }
                }
                break;

            case v_reference_null:
                break;

            case v_string_zero_length:
            case v_collection_empty:
                vs = StringHelper::getEmptyString();
                break;

            case t_octet_string:
                {
                size32_t cb = validateIncomingSize(f_hIn->readInt32());
                size32_t of = f_hIn->getOffset();
                f_hIn->skip(cb);
                vs = String::create(f_hIn->getBuffer()->toOctetArray(of, cb));
                }
                break;

            case t_char_string:
                vs = f_hIn->readString();
                break;

            case t_collection:
            case t_array:
                {
                size32_t cch  = validateIncomingSize(f_hIn->readInt32());

                Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(cch);
                for (size32_t i = 0; i < cch; ++i)
                    {
                    hach[i] = readAsChar16(f_hIn, f_hIn->readInt32());
                    }
                vs = String::create((Array<wchar16_t>::View) hach);
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cch          = validateIncomingSize(f_hIn->readInt32());

                Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(cch);
                for (size32_t i = 0; i < cch; ++i)
                    {
                    hach[i] = readAsChar16(f_hIn, nElementType);
                    }
                vs = String::create((Array<wchar16_t>::View) hach);
                }
                break;

            case t_sparse_array:
                {
                size32_t cch  = validateIncomingSize(f_hIn->readInt32());

                Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(cch);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hach[validateIncomingSize(iElement)] =
                            readAsChar16(f_hIn, f_hIn->readInt32());
                    }
                while (cch-- > 0);
                vs = String::create((Array<wchar16_t>::View) hach);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t nElementType = f_hIn->readInt32();
                size32_t  cch          = validateIncomingSize(f_hIn->readInt32());

                Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(cch);
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hach[validateIncomingSize(iElement)] =
                            readAsChar16(f_hIn, nElementType);
                    }
                while (cch-- > 0);
                vs = String::create((Array<wchar16_t>::View) hach);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a String type");
            }
        }
    complete(iProp);

    return vs;
    }

RawDate::View PofBufferReader::readRawDate(int32_t iProp)
    {
    RawDate::View vDate = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vDate = readRawDate(iProp);
                registerIdentity(nId, vDate);
                }
                break;

            case t_reference:
                {
                Object::Holder    oh  = lookupIdentity(f_hIn->readInt32());
                RawDateTime::View vdt = cast<RawDateTime::View>(oh, false);
                if (NULL == vdt)
                    {
                    vDate = cast<RawDate::View>(oh);
                    }
                else
                    {
                    vDate = vdt->getRawDate();
                    }
                }
                break;

            case v_reference_null:
                break;

            case t_date:
                vDate = readRawDate(f_hIn);
                break;

            case t_datetime:
                {
                // read the date portion
                vDate = readRawDate(f_hIn);

                // skip the time portion
                skipInts(f_hIn, 4);
                int32_t nZoneType = f_hIn->readInt32();
                if (2 == nZoneType)
                    {
                    skipInts(f_hIn, 2);
                    }
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a RawDate type");
            }
        }
    complete(iProp);

    return vDate;
    }

RawTime::View PofBufferReader::readRawTime(int32_t iProp)
    {
    RawTime::View vTime = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vTime = readRawTime(iProp);
                registerIdentity(nId, vTime);
                }
                break;

            case t_reference:
                {
                Object::Holder    oh  = lookupIdentity(f_hIn->readInt32());
                RawDateTime::View vdt = cast<RawDateTime::View>(oh, false);
                if (NULL == vdt)
                    {
                    vTime = cast<RawTime::View>(oh);
                    }
                else
                    {
                    vTime = vdt->getRawTime();
                    }
                }
                break;

            case v_reference_null:
                break;

            case t_datetime:
                // skip the date portion
                skipInts(f_hIn, 3);
                // fall through
            case t_time:
                vTime = readRawTime(f_hIn);
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a RawTime type");
            }
        }
    complete(iProp);

    return vTime;
    }

RawDateTime::View PofBufferReader::readRawDateTime(int32_t iProp)
    {
    RawDateTime::View vdt = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vdt = readRawDateTime(iProp);
                registerIdentity(nId, vdt);
                }
                break;

            case t_reference:
                {
                Object::Holder oh    = lookupIdentity(f_hIn->readInt32());
                RawDate::View  vDate = cast<RawDate::View>(oh, false);
                if (NULL == vDate)
                    {
                    vdt = cast<RawDateTime::View>(oh);
                    }
                else
                    {
                    vdt = RawDateTime::create(vDate,
                            RawTime::create(0, 0, 0, 0, false));
                    }
                }
                break;

            case v_reference_null:
                break;

            case t_date:
                vdt = RawDateTime::create(readRawDate(f_hIn),
                        RawTime::create(0, 0, 0, 0, false));
                break;

            case t_datetime:
                {
                RawDate::View vDate = readRawDate(f_hIn);
                RawTime::View vTime = readRawTime(f_hIn);
                vdt = RawDateTime::create(vDate, vTime);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a RawDateTime type");
            }
        }
    complete(iProp);

    return vdt;
    }

RawYearMonthInterval::View PofBufferReader::readRawYearMonthInterval(
        int32_t iProp)
    {
    RawYearMonthInterval::View vInterval = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vInterval = readRawYearMonthInterval(iProp);
                registerIdentity(nId, vInterval);
                }
                break;

            case t_reference:
                vInterval = cast<RawYearMonthInterval::View>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case t_year_month_interval:
                {
                int32_t cYears  = f_hIn->readInt32();
                int32_t cMonths = f_hIn->readInt32();
                vInterval = RawYearMonthInterval::create(cYears, cMonths);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a RawYearMonthInterval type");
            }
        }
    complete(iProp);

    return vInterval;
    }

RawTimeInterval::View PofBufferReader::readRawTimeInterval(int32_t iProp)
    {
    RawTimeInterval::View vInterval = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vInterval = readRawTimeInterval(iProp);
                registerIdentity(nId, vInterval);
                }
                break;

            case t_reference:
                vInterval = cast<RawTimeInterval::View>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case t_time_interval:
                {
                int32_t cHours   = f_hIn->readInt32();
                int32_t cMinutes = f_hIn->readInt32();
                int32_t cSeconds = f_hIn->readInt32();
                int32_t cNanos   = f_hIn->readInt32();
                vInterval = RawTimeInterval::create(cHours, cMinutes,
                        cSeconds, cNanos);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a RawTimeInterval type");
            }
        }
    complete(iProp);

    return vInterval;
    }

RawDayTimeInterval::View PofBufferReader::readRawDayTimeInterval(int32_t iProp)
    {
    RawDayTimeInterval::View vInterval = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                vInterval = readRawDayTimeInterval(iProp);
                registerIdentity(nId, vInterval);
                }
                break;

            case t_reference:
                vInterval = cast<RawDayTimeInterval::View>(
                        lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
                break;

            case t_day_time_interval:
                {
                int32_t cDays    = f_hIn->readInt32();
                int32_t cHours   = f_hIn->readInt32();
                int32_t cMinutes = f_hIn->readInt32();
                int32_t cSeconds = f_hIn->readInt32();
                int32_t cNanos   = f_hIn->readInt32();
                vInterval = RawDayTimeInterval::create(cDays, cHours,
                        cMinutes, cSeconds, cNanos);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a RawDayTimeInterval type");
            }
        }
    complete(iProp);

    return vInterval;
    }

Object::Holder PofBufferReader::readObject(int32_t iProp)
    {
    Object::Holder oh = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                IdentityHolder::set(this, nId);
                oh = readObject(iProp);
                IdentityHolder::reset(this, nId, oh);
                }
                break;

            case t_reference:
                oh = lookupIdentity(f_hIn->readInt32());
                break;

            case v_reference_null:
                break;

            default:
                oh = readAsObject(nType);
                break;
            }
        }
    complete(iProp);

    return oh;
    }

ObjectArray::Handle PofBufferReader::readObjectArray(int32_t iProp,
        ObjectArray::Handle ha)
    {
    ObjectArray::Handle haResult = NULL;

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                int32_t nId = f_hIn->readInt32();
                haResult = readObjectArray(iProp, ha);
                registerIdentity(nId, haResult);
                }
                break;

            case t_reference:
                {
                Object::Holder oh = lookupIdentity(f_hIn->readInt32());

                Collection::View vCol = cast<Collection::View>(oh, false);
                if (NULL == vCol)
                    {
                    LongArray::View vla = cast<LongArray::View>(oh, false);
                    if (NULL == vla)
                        {
                        haResult = cast<ObjectArray::Handle>(oh);
                        }
                    else
                        {
                        int64_t cElements = vla->getLastIndex() + 1;
                        if (vla->getFirstIndex() < 0)
                            {
                            COH_THROW_STREAM (IndexOutOfBoundsException,
                                    "index=" << vla->getFirstIndex());
                            }
                        if (((uint64_t) cElements) > std::numeric_limits<size32_t>::max())
                            {
                            COH_THROW_STREAM (IndexOutOfBoundsException,
                                    "index=" << vla->getLastIndex());
                            }

                        haResult = resizeArray(ha, (size32_t) cElements);
                        for (LongArrayIterator::Handle hIter = vla->iterator();
                             hIter->hasNext(); )
                            {
                            haResult[(size32_t) hIter->getIndex()] =
                                    hIter->next();
                            }
                        }
                    }
                else
                    {
                    haResult = vCol->toArray();
                    }
                }
                break;

            case v_reference_null:
                break;

            case v_string_zero_length:
            case v_collection_empty:
                haResult = getEmptyObjectArray();
                break;

            default:
                haResult = readAsObjectArray(nType, ha);
                break;
            }
        }
    complete(iProp);

    return haResult;
    }

LongArray::View PofBufferReader::readLongArray(int32_t iProp,
        LongArray::Handle hla)
    {
    // do not default to NULL, since the caller is passing in a mutable
    // LongArray

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                if (NULL == hla)
                    {
                    hla = SparseArray::create();
                    }

                int32_t nId = f_hIn->readInt32();
                readLongArray(iProp, hla);
                registerIdentity(nId, hla);
                }
                break;

            case t_reference:
                {
                Object::Holder oh = lookupIdentity(f_hIn->readInt32());

                Collection::View vCol = cast<Collection::View>(oh, false);
                if (NULL == vCol)
                    {
                    hla = cast<LongArray::Handle>(oh);
                    }
                else
                    {
                    if (NULL == hla)
                        {
                        hla = SparseArray::create();
                        }

                    int32_t i = 0;
                    for (Iterator::Handle hIter = vCol->iterator();
                        hIter->hasNext(); )
                        {
                        hla->set(++i, hIter->next());
                        }
                    }
                }
                break;

            case v_reference_null:
            case v_string_zero_length:
            case v_collection_empty:
                break;

            case t_collection:
            case t_array:
                {
                if (NULL == hla)
                    {
                    hla = SparseArray::create();
                    }

                size32_t co = validateIncomingSize(f_hIn->readInt32());
                for (size32_t i = 0; i < co; ++i)
                    {
                    hla->set(i, readAsObject(f_hIn->readInt32()));
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                if (NULL == hla)
                    {
                    hla = SparseArray::create();
                    }

                int32_t nElementType = f_hIn->readInt32();

                size32_t  co = validateIncomingSize(f_hIn->readInt32());
                for (size32_t i = 0; i < co; ++i)
                    {
                    hla->set(i, readAsUniformObject(nElementType));
                    }
                }
                break;

            case t_sparse_array:
                {
                if (NULL == hla)
                    {
                    hla = SparseArray::create();
                    }

                size32_t co = validateIncomingSize(f_hIn->readInt32());
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hla->set(iElement, readAsObject(f_hIn->readInt32()));
                    }
                while (co-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                if (NULL == hla)
                    {
                    hla = SparseArray::create();
                    }

                int32_t nElementType = f_hIn->readInt32();

                size32_t co = validateIncomingSize(f_hIn->readInt32());
                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hla->set(iElement, readAsUniformObject(nElementType));
                    }
                while (co-- > 0);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to an array type");
            }
        }
    complete(iProp);

    return hla;
    }

Collection::View PofBufferReader::readCollection(int32_t iProp,
        Collection::Handle hCol)
    {
    // do not default to NULL, since the caller is passing in a mutable
    // Collection

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                bool fWrap = false;
                if (NULL == hCol)
                    {
                    hCol = ArrayList::create();
                    fWrap = true;
                    }

                int32_t nId = f_hIn->readInt32();
                readCollection(iProp, hCol);
                if (fWrap)
                    {
                    hCol = ReadOnlyArrayList::create(hCol->toArray())->getList();
                    }
                registerIdentity(nId, hCol);
                }
                break;

            case t_reference:
                {
                Object::Holder    oh = lookupIdentity(f_hIn->readInt32());
                ObjectArray::View va = cast<ObjectArray::View>(oh, false);

                if (NULL == va)
                    {
                    hCol = cast<Collection::Handle>(oh);
                    }
                else
                    {
                    Collection::Handle hCollData =
                        ReadOnlyArrayList::create(va)->getList();
                    if (NULL == hCol)
                        {
                        hCol = hCollData;
                        }
                    else
                        {
                        hCol->addAll(hCollData);
                        }
                    }
                }
                break;

            case v_reference_null:
            case v_string_zero_length:
            case v_collection_empty:
                break;

            case t_collection:
            case t_array:
                {
                if (NULL == hCol)
                    {
                    ObjectArray::Handle hao = readAsObjectArray(nType);
                    hCol = ReadOnlyArrayList::create(hao)->getList();
                    }
                else
                    {
                    size32_t c = validateIncomingSize(f_hIn->readInt32());
                    for (size32_t i = 0; i < c; ++i)
                        {
                        hCol->add(readAsObject(f_hIn->readInt32()));
                        }
                    }
                }
                break;

            case t_uniform_collection:
            case t_uniform_array:
                {
                if (NULL == hCol)
                    {
                    ObjectArray::Handle hao = readAsObjectArray(nType);
                    hCol = ReadOnlyArrayList::create(hao)->getList();
                    }
                else
                    {
                    int32_t  nElementType = f_hIn->readInt32();
                    size32_t c            = validateIncomingSize(f_hIn->readInt32());
                    for (size32_t i = 0; i < c; ++i)
                        {
                        hCol->add(readAsUniformObject(nElementType));
                        }
                    }
                }
                break;

            case t_sparse_array:
                {
                size32_t c = validateIncomingSize(f_hIn->readInt32());
                if (NULL == hCol)
                    {
                    hCol = ArrayList::create(c);
                    }

                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hCol->add(readAsObject(f_hIn->readInt32()));
                    }
                while (c-- > 0);
                }
                break;

            case t_uniform_sparse_array:
                {
                int32_t  nElementType = f_hIn->readInt32();
                size32_t c            = validateIncomingSize(f_hIn->readInt32());
                if (NULL == hCol)
                    {
                    hCol = ArrayList::create(c);
                    }

                do
                    {
                    int32_t iElement = f_hIn->readInt32();
                    if (iElement < 0)
                        {
                        break;
                        }
                    hCol->add(readAsUniformObject(nElementType));
                    }
                while (c-- > 0);
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a Collection type");
            }
        }
    complete(iProp);

    return hCol;
    }

Map::View PofBufferReader::readMap(int32_t iProp, Map::Handle hMap)
    {
    // do not default to NULL, since the caller is passing in a mutable
    // Map

    if (advanceTo(iProp))
        {
        int32_t nType = f_hIn->readInt32();
        switch (nType)
            {
            case t_identity:
                {
                if (NULL == hMap)
                    {
                    hMap = HashMap::create();
                    }

                int32_t nId = f_hIn->readInt32();
                readMap(iProp, hMap);
                registerIdentity(nId, hMap);
                }
                break;

            case t_reference:
                hMap = cast<Map::Handle>(lookupIdentity(f_hIn->readInt32()));
                break;

            case v_reference_null:
            case v_string_zero_length:
            case v_collection_empty:
                break;

            case t_map:
                {
                size32_t cEntries = validateIncomingSize(f_hIn->readInt32());
                if (NULL == hMap)
                    {
                    hMap = HashMap::create(cEntries);
                    }

                for (size32_t i = 0; i < cEntries; ++i)
                    {
                    Object::Holder ohKey = readAsObject(f_hIn->readInt32());
                    Object::Holder ohVal = readAsObject(f_hIn->readInt32());
                    hMap->put(ohKey, ohVal);
                    }
                }
                break;

            case t_uniform_keys_map:
                {
                int32_t  nKeyType = f_hIn->readInt32();
                size32_t cEntries = validateIncomingSize(f_hIn->readInt32());
                if (NULL == hMap)
                    {
                    hMap = HashMap::create(cEntries);
                    }

                for (size32_t i = 0; i < cEntries; ++i)
                    {
                    Object::Holder ohKey = readAsUniformObject(nKeyType);
                    Object::Holder ohVal = readAsObject(f_hIn->readInt32());
                    hMap->put(ohKey, ohVal);
                    }
                }
                break;

            case t_uniform_map:
                {
                int32_t  nKeyType = f_hIn->readInt32();
                int32_t  nValType = f_hIn->readInt32();
                size32_t cEntries = validateIncomingSize(f_hIn->readInt32());
                if (NULL == hMap)
                    {
                    hMap = HashMap::create(cEntries);
                    }

                for (size32_t i = 0; i < cEntries; ++i)
                    {
                    Object::Holder ohKey = readAsUniformObject(nKeyType);
                    Object::Holder ohVal = readAsUniformObject(nValType);
                    hMap->put(ohKey, ohVal);
                    }
                }
                break;

            default:
                COH_THROW_STREAM (IOException, "unable to convert type "
                        << nType << " to a Map type");
            }
        }
    complete(iProp);

    return hMap;
    }

PofContext::View PofBufferReader::getPofContext() const
    {
    return f_vCtx;
    }

void PofBufferReader::setPofContext(PofContext::View vCtx)
    {
    if (NULL == vCtx)
        {
        COH_THROW (IllegalArgumentException::create(
                "PofContext cannot be NULL"));
        }
    initialize(f_vCtx, vCtx);
    }

int32_t PofBufferReader::getUserTypeId() const
    {
    return -1;
    }

int32_t PofBufferReader::getVersionId() const
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }

void PofBufferReader::registerIdentity(Object::Holder /*oh*/)
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }

PofReader::Handle PofBufferReader::createNestedPofReader(int32_t /*iProp*/)
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }

Binary::View PofBufferReader::readRemainder()
    {
    COH_THROW (IllegalStateException::create("not in a user type"));
    }


// ----- internal methods ---------------------------------------------------

bool PofBufferReader::advanceTo(int32_t iProp)
    {
    if (iProp > 0)
        {
        COH_THROW (IllegalStateException::create());
        }

    return true;
    }

void PofBufferReader::complete(int32_t /*iProp*/)
    {
    }

PofBufferReader::Handle PofBufferReader::getParentParser()
    {
    return NULL;
    }

PofBufferReader::View PofBufferReader::getParentParser() const
    {
    return NULL;
    }

LongArray::Handle PofBufferReader::ensureReferenceRegistry()
    {
    LongArray::Handle hla = f_hlaRefs;

    if (NULL == hla)
        {
        PofBufferReader::Handle hParent = getParentParser();
        hla = NULL == hParent
                ? (LongArray::Handle) SparseArray::create()
                : hParent->ensureReferenceRegistry();
        initialize(f_hlaRefs, hla);
        }

    return hla;
    }

void PofBufferReader::registerIdentity(int32_t nId, Object::Holder ohValue)
    {
    if (nId >= 0)
        {
        LongArray::Handle hla = ensureReferenceRegistry();
        Object::View      oh  = hla->get(nId);
        if (NULL != oh && oh != ohValue)
            {
            COH_THROW_STREAM (IOException, "duplicate identity: " << nId);
            }

        hla->set(nId, ohValue);
        }
    }

Object::Holder PofBufferReader::lookupIdentity(int32_t nId)
    {
    LongArray::Handle hla = ensureReferenceRegistry();
    if (!hla->exists(nId))
        {
        COH_THROW_STREAM (IOException, "missing identity: " << nId);
        }

    return hla->get(nId);
    }

Object::Holder PofBufferReader::readAsObject(int32_t nType)
    {
    Object::Holder oh = NULL;

    switch (nType)
        {
        case t_int16:
            oh = (Object::View) Integer16::valueOf(f_hIn->readInt16());
            break;

        case t_int32:
        case v_int_neg_1:
        case v_int_0:
        case v_int_1:
        case v_int_2:
        case v_int_3:
        case v_int_4:
        case v_int_5:
        case v_int_6:
        case v_int_7:
        case v_int_8:
        case v_int_9:
        case v_int_10:
        case v_int_11:
        case v_int_12:
        case v_int_13:
        case v_int_14:
        case v_int_15:
        case v_int_16:
        case v_int_17:
        case v_int_18:
        case v_int_19:
        case v_int_20:
        case v_int_21:
        case v_int_22:
            oh = (Object::View) Integer32::valueOf(readAsInt32(f_hIn, nType));
            break;

        case t_int64:
            oh = (Object::View) Integer64::valueOf(f_hIn->readInt64());
            break;

        case t_float32:
            oh = (Object::View) Float32::valueOf(f_hIn->readFloat32());
            break;

        case t_float64:
            oh = (Object::View) Float64::valueOf(f_hIn->readFloat64());
            break;

        case v_fp_pos_infinity:
            oh = (Object::View) Float64::valueOf(Float64::getPositiveInfinity());
            break;

        case v_fp_neg_infinity:
            oh = (Object::View) Float64::valueOf(Float64::getNegativeInfinity());
            break;

        case v_fp_nan:
            oh = (Object::View) Float64::valueOf(Float64::getNaN());
            break;

        case t_boolean:
            oh = (Object::View) Boolean::valueOf(f_hIn->readBoolean());
            break;

        case t_octet:
            oh = (Object::View) Octet::valueOf(f_hIn->read());
            break;

        case t_octet_string:
            {
            size32_t cb = validateIncomingSize(f_hIn->readInt32());
            size32_t of = f_hIn->getOffset();
            f_hIn->skip(cb);
            oh = Binary::create(f_hIn->getBuffer()->toOctetArray(of, cb), 0, cb);
            }
            break;

        case t_char:
            oh = (Object::View) Character16::valueOf(f_hIn->readChar16());
            break;

        case t_char_string:
            oh = f_hIn->readString();
            break;

        case t_date:
            oh = readRawDate(f_hIn);
            break;

        case t_time:
            oh = readRawTime(f_hIn);
            break;

        case t_datetime:
            {
            RawDate::View vDate = readRawDate(f_hIn);
            RawTime::View vTime = readRawTime(f_hIn);
            oh = (Object::View) RawDateTime::create(vDate, vTime);
            }
            break;

        case t_year_month_interval:
            {
            int32_t cYears  = f_hIn->readInt32();
            int32_t cMonths = f_hIn->readInt32();
            oh = (Object::View) RawYearMonthInterval::create(cYears, cMonths);
            }
            break;

        case t_time_interval:
            {
            int32_t cHours   = f_hIn->readInt32();
            int32_t cMinutes = f_hIn->readInt32();
            int32_t cSeconds = f_hIn->readInt32();
            int32_t cNanos   = f_hIn->readInt32();
            oh = (Object::View) RawTimeInterval::create(cHours, cMinutes,
                    cSeconds, cNanos);
            }
            break;

        case t_day_time_interval:
            {
            int32_t cDays    = f_hIn->readInt32();
            int32_t cHours   = f_hIn->readInt32();
            int32_t cMinutes = f_hIn->readInt32();
            int32_t cSeconds = f_hIn->readInt32();
            int32_t cNanos   = f_hIn->readInt32();
            oh = (Object::View) RawDayTimeInterval::create(cDays, cHours,
                    cMinutes, cSeconds, cNanos);
            }
            break;

        case t_collection:
        case t_uniform_collection:
            oh = ReadOnlyArrayList::create(readAsObjectArray(nType))->getList();
            break;

        case t_array:
            oh = readAsObjectArray(nType);
            break;

        case t_uniform_array:
            {
            int32_t  nElementType = f_hIn->readInt32();
            size32_t cElements    = validateIncomingSize(f_hIn->readInt32());
            switch (nElementType)
                {
                case t_boolean:
                    {
                    Array<bool>::Handle haf =
                            Array<bool>::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        haf[i] = f_hIn->readBoolean();
                        }
                    oh = haf;
                    }
                    break;

                case t_octet:
                    {
                    Array<octet_t>::Handle hab =
                            Array<octet_t>::create(cElements);
                    f_hIn->read(hab);
                    oh = hab;
                    }
                    break;

                case t_char:
                    {
                    Array<wchar16_t>::Handle hach =
                            Array<wchar16_t>::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        hach[i] = f_hIn->readChar16();
                        }
                    oh = hach;
                    }
                    break;

                case t_int16:
                    {
                    Array<int16_t>::Handle han =
                            Array<int16_t>::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        han[i] = f_hIn->readInt16();
                        }
                    oh = han;
                    }
                    break;

                case t_int32:
                    {
                    Array<int32_t>::Handle han =
                            Array<int32_t>::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        han[i] = f_hIn->readInt32();
                        }
                    oh = han;
                    }
                    break;

                case t_int64:
                    {
                    Array<int64_t>::Handle han =
                            Array<int64_t>::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        han[i] = f_hIn->readInt64();
                        }
                    oh = han;
                    }
                    break;

                case t_float32:
                    {
                    Array<float32_t>::Handle hafl =
                            Array<float32_t>::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        hafl[i] = f_hIn->readFloat32();
                        }
                    oh = hafl;
                    }
                    break;

                case t_float64:
                    {
                    Array<float64_t>::Handle hafl =
                            Array<float64_t>::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        hafl[i] = f_hIn->readFloat64();
                        }
                    oh = hafl;
                    }
                    break;

                default:
                    {
                    ObjectArray::Handle hao = ObjectArray::create(cElements);
                    for (size32_t i = 0; i < cElements; ++i)
                        {
                        hao[i] = readAsUniformObject(nElementType);
                        }
                    oh = hao;
                    }
                }
            }
            break;

        case t_sparse_array:
            {
            LongArray::Handle hla = SparseArray::create();
            size32_t cElements    = validateIncomingSize(f_hIn->readInt32());
            do
                {
                int32_t iElement = f_hIn->readInt32();
                if (iElement < 0)
                    {
                    break;
                    }
                hla->set(iElement, readAsObject(f_hIn->readInt32()));
                }
            while (cElements-- > 0);
            oh = (Object::View) hla;
            }
            break;

        case t_uniform_sparse_array:
            {
            int32_t  nElementType = f_hIn->readInt32();
            size32_t cElements    = validateIncomingSize(f_hIn->readInt32());
            switch (nElementType)
                {
                case t_boolean:
                    {
                    Array<bool>::Handle haf =
                            Array<bool>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        haf[validateIncomingSize(iElement)] =
                                f_hIn->readBoolean();
                        }
                    while (cElements-- > 0);
                    oh = haf;
                    }
                    break;

                case t_octet:
                    {
                    Array<octet_t>::Handle hab =
                            Array<octet_t>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        hab[validateIncomingSize(iElement)] =
                                f_hIn->read();
                        }
                    while (cElements-- > 0);
                    oh = hab;
                    }
                    break;

                case t_char:
                    {
                    Array<wchar16_t>::Handle hach =
                            Array<wchar16_t>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        hach[validateIncomingSize(iElement)] =
                                f_hIn->readChar16();
                        }
                    while (cElements-- > 0);
                    oh = hach;
                    }
                    break;

                case t_int16:
                    {
                    Array<int16_t>::Handle han =
                            Array<int16_t>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        han[validateIncomingSize(iElement)] =
                                f_hIn->readInt16();
                        }
                    while (cElements-- > 0);
                    oh = han;
                    }
                    break;

                case t_int32:
                    {
                    Array<int32_t>::Handle han =
                            Array<int32_t>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        han[validateIncomingSize(iElement)] =
                                f_hIn->readInt32();
                        }
                    while (cElements-- > 0);
                    oh = han;
                    }
                    break;

                case t_int64:
                    {
                    Array<int64_t>::Handle han =
                            Array<int64_t>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        han[validateIncomingSize(iElement)] =
                                f_hIn->readInt64();
                        }
                    while (cElements-- > 0);
                    oh = han;
                    }
                    break;

                case t_float32:
                    {
                    Array<float32_t>::Handle hfl =
                            Array<float32_t>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        hfl[validateIncomingSize(iElement)] =
                                f_hIn->readFloat32();
                        }
                    while (cElements-- > 0);
                    oh = hfl;
                    }
                    break;

                case t_float64:
                    {
                    Array<float64_t>::Handle hdfl =
                            Array<float64_t>::create(cElements);
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        hdfl[validateIncomingSize(iElement)] =
                                f_hIn->readFloat64();
                        }
                    while (cElements-- > 0);
                    oh = hdfl;
                    }
                    break;

                default:
                    {
                    LongArray::Handle hla = SparseArray::create();
                    do
                        {
                        int32_t iElement = f_hIn->readInt32();
                        if (iElement < 0)
                            {
                            break;
                            }
                        hla->set(iElement, readAsUniformObject(nElementType));
                        }
                    while (cElements-- > 0);
                    oh = hla;
                    }
                }
            }
            break;

        case t_map:
            {
            Map::Handle hMap     = HashMap::create();
            size32_t    cEntries = validateIncomingSize(f_hIn->readInt32());
            for (size32_t i = 0; i < cEntries; ++i)
                {
                Object::Holder ohKey = readAsObject(f_hIn->readInt32());
                Object::Holder ohVal = readAsObject(f_hIn->readInt32());
                hMap->put(ohKey, ohVal);
                }
            oh = (Object::View) hMap;
            }
            break;

        case t_uniform_keys_map:
            {
            Map::Handle hMap     = HashMap::create();
            int32_t     nKeyType = f_hIn->readInt32();
            size32_t    cEntries = validateIncomingSize(f_hIn->readInt32());
            for (size32_t i = 0; i < cEntries; ++i)
                {
                Object::Holder ohKey = readAsUniformObject(nKeyType);
                Object::Holder ohVal = readAsObject(f_hIn->readInt32());
                hMap->put(ohKey, ohVal);
                }
            oh = (Object::View) hMap;
            }
            break;

        case t_uniform_map:
            {
            Map::Handle hMap     = HashMap::create();
            int32_t     nKeyType = f_hIn->readInt32();
            int32_t     nValType = f_hIn->readInt32();
            size32_t    cEntries = validateIncomingSize(f_hIn->readInt32());
            for (size32_t i = 0; i < cEntries; ++i)
                {
                Object::Holder ohKey = readAsUniformObject(nKeyType);
                Object::Holder ohVal = readAsUniformObject(nValType);
                hMap->put(ohKey, ohVal);
                }
            oh = (Object::View) hMap;
            }
            break;

        case t_identity:
            {
            int32_t nId = f_hIn->readInt32();
            nType = f_hIn->readInt32();
            IdentityHolder::set(this, nId);
            oh = readAsObject(nType);
            IdentityHolder::reset(this, nId, oh);
            }
            break;

        case t_reference:
            oh = lookupIdentity(f_hIn->readInt32());
            break;

        case v_boolean_false:
            oh = (Object::View) Boolean::valueOf(false);
            break;

        case v_boolean_true:
            oh = (Object::View) Boolean::valueOf(true);
            break;

        case v_string_zero_length:
            oh = (Object::View) StringHelper::getEmptyString();
            break;

        case v_collection_empty:
            oh = getEmptyCollection();
            break;

        case v_reference_null:
            break;

        // REVIEW: COHCPP-180
        case t_int128:
        case t_float128:
        case t_decimal32:
        case t_decimal64:
        case t_decimal128:
            COH_THROW_STREAM (IOException, "unable to convert type " << nType
                    << " to a numeric type");

        default:
            {
            if (nType < 0)
                {
                COH_THROW_STREAM (IOException, "illegal type " << nType);
                }

            PofContext::View    vCtx    = getPofContext();
            PofSerializer::View vSer    = vCtx->getPofSerializer(nType);
            PofReader::Handle   hReader = UserTypeReader::create(this,
                    (ReadBuffer::BufferInput::Handle) f_hIn, vCtx, nType,
                    f_hIn->readInt32());
            oh = vSer->deserialize(hReader);
            }
            break;
        }

    return oh;
    }

Object::Holder PofBufferReader::readAsUniformObject(int32_t nType)
    {
    if (nType < 0)
        {
        return readAsObject(nType);
        }

    int32_t of  = f_hIn->getOffset();
    int32_t nId = -1;
    if (of == 0)
        {
        return readAsObject(nType);
        }

    int32_t nValue = f_hIn->readInt32();
    if (nValue == t_identity)
        {
        nId = f_hIn->readInt32();
        IdentityHolder::set(this, nId);
        }
    else
        {
        // it can only be reference if its data type supports reference
        // (a user defined object)
        if (nValue > 0 && nType >= 0)
            {
            Object::Holder oh = ensureReferenceRegistry()->get(nValue);
            if (NULL != oh)
                {
                // double check the object type
                int32_t nTypeId = PofHelper::getPofTypeId(SystemClassLoader::getInstance()->loadByName(Class::getClassName(oh)), getPofContext());
                if (nTypeId == nType)
                    {
                    return oh;
                    }
                }
            }
        f_hIn->setOffset(of);
        }

    Object::Holder oh = readAsObject(nType);
    if (nValue == t_identity)
        {
        IdentityHolder::reset(this, nId, oh);
        }
    return oh;
    }

ObjectArray::Handle PofBufferReader::readAsObjectArray(int32_t nType,
        ObjectArray::Handle ha)
    {
    ObjectArray::Handle haResult = NULL;

    switch (nType)
        {
        case v_reference_null:
            break;

        case v_string_zero_length:
        case v_collection_empty:
            haResult = getEmptyObjectArray();
            break;

        case t_collection:
        case t_array:
            {
            size32_t co = validateIncomingSize(f_hIn->readInt32());
            haResult = resizeArray(ha, co);
            for (size32_t i = 0; i < co; ++i)
                {
                haResult[i] = readAsObject(f_hIn->readInt32());
                }
            }
            break;

        case t_uniform_collection:
        case t_uniform_array:
            {
            int32_t   nElementType = f_hIn->readInt32();
            size32_t  co           = validateIncomingSize(f_hIn->readInt32());
            haResult = resizeArray(ha, co);
            for (size32_t i = 0; i < co; ++i)
                {
                haResult[i] = readAsUniformObject(nElementType);
                }
            }
            break;

        case t_sparse_array:
            {
            size32_t co = validateIncomingSize(f_hIn->readInt32());
            haResult = resizeArray(ha, co);
            do
                {
                int32_t iElement = f_hIn->readInt32();
                if (iElement < 0)
                    {
                    break;
                    }
                haResult[validateIncomingSize(iElement)] =
                        readAsObject(f_hIn->readInt32());
                }
            while (co-- > 0);
            }
            break;

        case t_uniform_sparse_array:
            {
            int32_t  nElementType = f_hIn->readInt32();
            size32_t co           = validateIncomingSize(f_hIn->readInt32());
            haResult = resizeArray(ha, co);
            do
                {
                int32_t iElement = f_hIn->readInt32();
                if (iElement < 0)
                    {
                    break;
                    }
                haResult[validateIncomingSize(iElement)] =
                        readAsUniformObject(nElementType);
                }
            while (co-- > 0);
            }
            break;

        default:
            COH_THROW_STREAM (IOException, "unable to convert type "
                    << nType << " to an array type");
        }

    return haResult;
    }

COH_CLOSE_NAMESPACE3
