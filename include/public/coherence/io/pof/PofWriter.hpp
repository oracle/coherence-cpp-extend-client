/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_WRITER_HPP
#define COH_POF_WRITER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDateTime.hpp"
#include "coherence/io/pof/RawDayTimeInterval.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/RawTimeInterval.hpp"
#include "coherence/io/pof/RawYearMonthInterval.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::Binary;
using coherence::util::Collection;
using coherence::util::Map;
using coherence::util::LongArray;


/**
* The PofWriter interface provides the capability of writing a set of
* non-primitive types ("user types") to a POF stream as an ordered sequence
* of indexed properties.
*
* The serialized format of a POF user type is as follows:
* <ul>
*   <li>Type Identifier</li>
*   <li>Version Identifier</li>
*   <li>[Property Index, Property Value]*</li>
*   <li>-1</li>
* </ul>
* The type identifier is an integer value greater than or equal to zero that
* identifies the non-primitive type. The type identifier has no explicit
* or self-describing meaning within the POF stream itself; in other words,
* the type identifier does not contain the actual class definition. Instead,
* the PofWriter and corresponding {@link PofReader} share a
* {@link PofContext} which contains the necessary meta-data, including type
* identifier to type mappings.
*
* The version identifier is used to support both backwards and forwards
* compatibility of serialized POF user types. Versioning of user types allows
* the addition of new properties to a user type, but not the replacement or
* removal of properties that existed in a previous version of the user type.
*
* When a version <i>v1</i> of a user type written by a PofWriter is read by
* a PofReader that supports version <i>v2</i> of the same user type, the
* PofReader returns default values for the additional properties of the User
* Type that exist in <i>v2</i> but do not exist in <i>v1</i>. Conversely,
* when a version <i>v2</i> of a user type written by a PofWriter is read by
* a PofReader that supports version <i>v1</i> of the same user type, the
* instance of user type <i>v1</i> must store those additional opaque
* properties for later encoding. The PofReader enables the user type to
* store off the opaque properties in binary form (see {@link
* PofReader#readRemainder PofReader::readRemainder}). When the user type
* is re-encoded, it must be done so using the version identifier
* <i>v2</i>, since it is including the unaltered <i>v2</i> properties. The
* opaque properties are subsequently included in the POF stream using the
* {@link #writeRemainder} method.
*
* Following the version identifier is an ordered sequence of index/value
* pairs, each of which is composed of a property index encoded as
* non-negative integer value whose value is greater than the previous
* property index, and a property value encoded as a POF value. The user type
* is finally terminated with an illegal property index of -1.
*
* Note: To read a property that was written using a PofWrite method, the
* corresponding read method on {@link PofReader} must be used. For example,
* if a property was written using {@link #writeByteArray},
* {@link PofReader#readByteArray PofReader::readByteArray} must be used to
* read the property. 
*
* @author jh  2008.01.18
*
* @see PofContext
* @see PofReader
*/
class COH_EXPORT PofWriter
    : public interface_spec<PofWriter>
    {
    // ----- primitive type support -----------------------------------------

    public:
        /**
        * Write a boolean property to the POF stream.
        *
        * @param iProp  the property index
        * @param f      the <tt>bool</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeBoolean(int32_t iProp, bool f) = 0;

        /**
        * Write an octet property to the POF stream.
        *
        * @param iProp  the property index
        * @param b      the <tt>octet_t</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeOctet(int32_t iProp, octet_t b) = 0;

        /**
        * Write a 16-bit Unicode character property (represented as a
        * <tt>wchar16_t</tt>) to the POF stream.
        *
        * @param iProp  the property index
        * @param ch     the <tt>wchar16_t</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeChar16(int32_t iProp, wchar16_t ch) = 0;

        /**
        * Write a 16-bit integer property to the POF stream.
        *
        * @param iProp  the property index
        * @param n      the <tt>int16_t</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeInt16(int32_t iProp, int16_t n) = 0;

        /**
        * Write a 32-bit integer property to the POF stream.
        *
        * @param iProp  the property index
        * @param n      the <tt>int32_t</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeInt32(int32_t iProp, int32_t n) = 0;

        /**
        * Write a 64-bit integer property to the POF stream.
        *
        * @param iProp  the property index
        * @param l      the <tt>int64_t</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeInt64(int32_t iProp, int64_t l) = 0;

        /**
        * Write a 32-bit floating-point property to the POF stream.
        *
        * @param iProp  the property index
        * @param fl     the <tt>float32_t</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeFloat32(int32_t iProp, float32_t fl) = 0;

        /**
        * Write a 64-bit floating-point property to the POF stream.
        *
        * @param iProp  the property index
        * @param dfl    the <tt>float64_t</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeFloat64(int32_t iProp, float64_t dfl) = 0;


    // ----- primitive array support ----------------------------------------

    public:
        /**
        * Write a boolean array property to the POF stream.
        *
        * @param iProp  the property index
        * @param vaf    the <tt>bool</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeBooleanArray(int32_t iProp, Array<bool>::View vaf) = 0;

        /**
        * Write an octect array property to the POF stream.
        *
        * @param iProp  the property index
        * @param vab    the <tt>octet_t</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeOctetArray(int32_t iProp, Array<octet_t>::View vab) = 0;

        /**
        * Write a 16-bit Unicode character array property to the POF stream.
        *
        * @param iProp  the property index
        * @param vach   the <tt>wchar16_t</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeChar16Array(int32_t iProp, Array<wchar16_t>::View vach) = 0;

        /**
        * Write a 16-bit integer array property to the POF stream.
        *
        * @param iProp  the property index
        * @param van    the <tt>int16_t</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeInt16Array(int32_t iProp, Array<int16_t>::View van) = 0;

        /**
        * Write a 32-bit integer array property to the POF stream.
        *
        * @param iProp  the property index
        * @param van    the <tt>int32_t</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeInt32Array(int32_t iProp, Array<int32_t>::View van) = 0;

        /**
        * Write a 64-bit integer array property to the POF stream.
        *
        * @param iProp  the property index
        * @param val    the <tt>int64_t</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeInt64Array(int32_t iProp, Array<int64_t>::View val) = 0;

        /**
        * Write a 32-bit floating-point array property to the POF stream.
        *
        * @param iProp  the property index
        * @param vafl   the <tt>float32_t</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeFloat32Array(int32_t iProp, Array<float32_t>::View vafl) = 0;

        /**
        * Write a 64-bit floating-point array property to the POF stream.
        *
        * @param iProp  the property index
        * @param vadfl  the <tt>float64_t</tt> array property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeFloat64Array(int32_t iProp, Array<float64_t>::View vadfl) = 0;


    // ----- object value support -------------------------------------------

    public:
        /**
        * Write a <tt>Binary</tt> property to the POF stream.
        *
        * @param iProp  the property index
        * @param vBin   the <tt>Binary</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeBinary(int32_t iProp, Binary::View vBin) = 0;

        /**
        * Write a <tt>String</tt> property to the POF stream.
        *
        * @param iProp  the property index
        * @param vs     the <tt>String</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeString(int32_t iProp, String::View vs) = 0;

        /**
        * Write a {@link RawDate} property to the POF stream.
        *
        * @param iProp  the property index
        * @param vDate  the <tt>RawDate</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeRawDate(int32_t iProp, RawDate::View vDate) = 0;

        /**
        * Write a {@link RawDateTime} property to the POF stream.
        *
        * @param iProp  the property index
        * @param vdt    the <tt>RawDateTime</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeRawDateTime(int32_t iProp, RawDateTime::View vdt) = 0;

        /**
        * Write a {@link RawDayTimeInterval} property to the POF stream.
        *
        * @param iProp      the property index
        * @param vInterval  the <tt>RawDayTimeInterval</tt> property value to
        *                   write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeRawDayTimeInterval(int32_t iProp,
                RawDayTimeInterval::View vInterval) = 0;

        /**
        * Write a {@link RawTime} property to the POF stream.
        *
        * @param iProp  the property index
        * @param vTime  the <tt>RawTime</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeRawTime(int32_t iProp, RawTime::View vTime) = 0;

        /**
        * Write a {@link RawTimeInterval} property to the POF stream.
        *
        * @param iProp      the property index
        * @param vInterval  the <tt>RawTimeInterval</tt> property value to
        *                   write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeRawTimeInterval(int32_t iProp,
                RawTimeInterval::View vInterval) = 0;

        /**
        * Write a {@link RawYearMonthInterval} property to the POF stream.
        *
        * @param iProp      the property index
        * @param vInterval  the <tt>RawYearMonthInterval</tt> property value
        *                   to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeRawYearMonthInterval(int32_t iProp,
                RawYearMonthInterval::View vInterval) = 0;

        /**
        * Write an <tt>Object</tt> property to the POF stream.
        *
        * The given object must be an instance of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for the object must be
        * obtainable from the {@link PofContext} associated with this
        * PofWriter.
        *
        * @param iProp  the property index
        * @param v      the <tt>Object</tt> property to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeObject(int32_t iProp, Object::View v) = 0;


    // ----- collection support ---------------------------------------------

    public:
        /**
        * Write an <tt>ObjectArray</tt> property to the POF stream.
        *
        * Each element of the given array must be an instance (or a
        * collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each element of the array
        * must be obtainable from the {@link PofContext} associated with this
        * PofWriter.
        *
        * @param iProp  the property index
        * @param va     the <tt>ObjectArray</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeObjectArray(int32_t iProp, ObjectArray::View va) = 0;

        /**
        * Write a uniform <tt>ObjectArray</tt> property to the POF stream.
        *
        * Each element of the given array must be an instance (or a
        * collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each element of the array
        * must be obtainable from the {@link PofContext} associated with this
        * PofWriter.
        *
        * Additionally, the type of each element must be equal to the
        * specified class.
        *
        * @param iProp   the property index
        * @param va      the <tt>ObjectArray</tt> property value to write
        * @param vClass  the class of all elements; must not be <tt>NULL</tt>
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::lang::IllegalArgumentException if the type of
        *         one or more elements of the array is not equal to the
        *         specified class
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeObjectArray(int32_t iProp, ObjectArray::View va,
                Class::View vClass) = 0;

        /**
        * Write a <tt>LongArray</tt> property to the POF stream.
        *
        * Each element of the given array must be an instance (or a
        * collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each element of the array
        * must be obtainable from the {@link PofContext} associated with this
        * PofWriter.
        *
        * @param iProp  the property index
        * @param vla    the <tt>LongArray</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeLongArray(int32_t iProp, LongArray::View vla) = 0;

        /**
        * Write a uniform <tt>LongArray</tt> property to the POF stream.
        *
        * Each element of the given array must be an instance (or a
        * collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each element of the array
        * must be obtainable from the {@link PofContext} associated with this
        * PofWriter.
        *
        * Additionally, the type of each element must be equal to the
        * specified class.
        *
        * @param iProp   the property index
        * @param vla     the <tt>LongArray</tt> property value to write
        * @param vClass  the class of all elements; must not be <tt>NULL</tt>
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::lang::IllegalArgumentException if the type of
        *         one or more elements of the array is not equal to the
        *         specified class
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeLongArray(int32_t iProp, LongArray::View vla,
                Class::View vClass) = 0;

        /**
        * Write a <tt>Collection</tt> property to the POF stream.
        *
        * Each element of the given <tt>Collection</tt> must be an instance
        * (or collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each element of the
        * <tt>Collection</tt> must be obtainable from the {@link PofContext}
        * associated with this PofWriter.
        *
        * @param iProp  the property index
        * @param vCol   the <tt>Collection</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeCollection(int32_t iProp, Collection::View vCol) = 0;

        /**
        * Write a uniform <tt>Collection</tt> property to the POF stream.
        *
        * Each element of the given <tt>Collection</tt> must be an instance
        * (or collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each element of the
        * <tt>Collection</tt> must be obtainable from the {@link PofContext}
        * associated with this PofWriter.
        *
        * Additionally, the type of each element must be equal to the
        * specified class.
        *
        * @param iProp   the property index
        * @param vCol    the <tt>Collection</tt> property value to write
        * @param vClass  the class of all elements; must not be <tt>NULL</tt>
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::lang::IllegalArgumentException if the type of
        *         one or more elements of the <tt>Collection</tt> is not
        *         equal to the specified class
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeCollection(int32_t iProp, Collection::View vCol,
                Class::View vClass) = 0;

        /**
        * Write a <tt>Map</tt> property to the POF stream.
        *
        * Each key and value of the given <tt>Map</tt> must be an instance
        * (or a collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each key and value of
        * the <tt>Map</tt> must be obtainable from the {@link PofContext}
        * associated with this PofWriter.
        *
        * @param iProp  the property index
        * @param vMap   the <tt>Map</tt> property value to write
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeMap(int32_t iProp, Map::View vMap) = 0;

        /**
        * Write a uniform key <tt>Map</tt> property to the POF stream.
        *
        * Each key and value of the given <tt>Map</tt> must be an instance
        * (or a collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each key and value of
        * the <tt>Map</tt> must be obtainable from the {@link PofContext}
        * associated with this PofWriter.
        *
        * Additionally, the type of each key must be equal to the specified
        * class.
        *
        * @param iProp      the property index
        * @param vMap       the <tt>Map</tt> property value to write
        * @param vClassKey  the class of all keys; must not be <tt>NULL</tt>
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::lang::IllegalArgumentException if the type of
        *         one or more keys of the <tt>Map</tt> is not equal to the
        *         specified class
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeMap(int32_t iProp, Map::View vMap,
                Class::View vClassKey) = 0;

        /**
        * Write a uniform <tt>Map</tt> property to the POF stream.
        *
        * Each key and value of the given <tt>Map</tt> must be an instance
        * (or a collection of instances) of one of the following:
        * <ul>
        *   <li>Boolean</li>
        *   <li>Octet</li>
        *   <li>Char16</li>
        *   <li>Int16</li>
        *   <li>Int32</li>
        *   <li>Int64</li>
        *   <li>Float32</li>
        *   <li>Float64</li>
        *   <li>Array<bool></li>
        *   <li>Array<octet_t></li>
        *   <li>Array<wchar16_t></li>
        *   <li>Array<int16_t></li>
        *   <li>Array<int32_t></li>
        *   <li>Array<int64_t></li>
        *   <li>Array<float32_t></li>
        *   <li>Array<float64_t></li>
        *   <li>Binary</li>
        *   <li>String</li>
        *   <li>{@link RawDate}</li>
        *   <li>{@link RawDateTime}</li>
        *   <li>{@link RawDayTimeInterval}</li>
        *   <li>{@link RawTime}</li>
        *   <li>{@link RawTimeInterval}</li>
        *   <li>{@link RawYearMonthInterval}</li>
        *   <li>Collection, with the same restrictions for all elements</li>
        *   <li>LongArray, with the same restrictions for all elements</li>
        *   <li>ObjectArray, with the same restrictions for all elements</li>
        * </ul>
        *
        * Otherwise, a {@link PofSerializer} for each key and value of
        * the <tt>Map</tt> must be obtainable from the {@link PofContext}
        * associated with this PofWriter.
        *
        * Additionally, the type of each key must be equal to the specified
        * class.
        *
        * @param iProp        the property index
        * @param vMap         the <tt>Map</tt> property value to write
        * @param vClassKey    the class of all keys; must not be
        *                     <tt>NULL</tt>
        * @param vClassValue  the class of all values; must not be
        *                     <tt>NULL</tt>
        *
        * @throws coherence::lang::IllegalArgumentException if the property
        *         index is invalid, or is less than or equal to the index of
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalArgumentException if the given
        *         property cannot be encoded into a POF stream
        * @throws coherence::lang::IllegalArgumentException if the type of
        *         one or more keys or values of the <tt>Map</tt> is not equal
        *         to the specified classes
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeMap(int32_t iProp, Map::View vMap,
                Class::View vClassKey, Class::View vClassValue) = 0;


    // ----- POF user type support ------------------------------------------

    public:
        /**
        * Return the PofContext object used by this PofWriter to serialize
        * user types to a POF stream.
        *
        * @return the PofContext object that contains user type meta-data
        */
        virtual PofContext::View getPofContext() const = 0;

        /**
        * Configure the PofContext object used by this PofWriter to serialize
        * user types to a POF stream.
        *
        * Note: this is an advanced method that should be used with care. For
        * example, if this method is being used to switch to another
        * PofContext mid-POF stream, it is important to eventually restore
        * the original PofContext. For example:
        * <pre>
        * PofContext::View vCtxOrig = hWriter->getPofContext();
        *
        * // switch to another PofContext
        * PofContext::View vCtxNew = ...;
        * hWriter->setContext(vCtxNew);
        *
        * // output POF data using the writer
        * ...
        *
        * // restore the original PofContext
        * hWriter->setPofContext(vCtxOrig);
        * </pre>
        *
        * @param vCtx  the new PofContext; must not be <tt>NULL</tt>
        */
        virtual void setPofContext(PofContext::View vCtx) = 0;

        /**
        * Determine the user type that is currently being written.
        *
        * @return the user type identifier, or -1 if the PofWriter is not
        *         currently writing a user type
        */
        virtual int32_t getUserTypeId() const = 0;

        /**
        * Determine the version identifier of the user type that is currently
        * being written.
        *
        * @return the integer version ID of the user type; always
        *         non-negative
        *
        * @throws coherence::lang::IllegalStateException if no user type is
        *         being parsed
        */
        virtual int32_t getVersionId() const = 0;

        /**
        * Set the version identifier of the user type that is currently being
        * written.
        *
        * @param nVersionId  the user type identifier; must be non-negative
        *
        * @throws coherence::lang::IllegalArgumentException if the given
        *         version ID is negative
        * @throws coherence::lang::IllegalStateException if no user type is
        *         being written
        */
        virtual void setVersionId(int32_t nVersionId) = 0;

        /**
        * Obtain a PofWriter that can be used to write a set of properties into
        * a single property of the current user type. The returned PofWriter is
        * only valid from the time that it is returned until the next call is
        * made to this PofWriter.
        *
        * @param iProp  the property index
        *
        * @return a PofWriter whose contents are nested into a single property
        *         of this PofWriter
        *
        * @throws coherence::lang::IllegalArgumentException if the property 
        *         index is invalid, or is less than or equal to the index of 
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalStateException if no user type is 
        *         being written
        * @throws coherence::io::IOException if an I/O error occurs
        *
        * @since Coherence 3.6
        */
        virtual PofWriter::Handle createNestedPofWriter(int32_t iProp) = 0;

        /**
        * Obtain a PofWriter that can be used to write a set of properties into
        * a single property of the current user type. The returned PofWriter is
        * only valid from the time that it is returned until the next call is
        * made to this PofWriter.
        *
        * @param iProp    the property index
        * @param nTypeId  the type identifier of the nested property
        *
        * @return a PofWriter whose contents are nested into a single property
        *         of this PofWriter
        *
        * @throws coherence::lang::IllegalArgumentException if the property 
        *         index is invalid, or is less than or equal to the index of 
        *         the previous property written to the POF stream
        * @throws coherence::lang::IllegalStateException if no user type is 
        *         being written
        * @throws coherence::io::IOException if an I/O error occurs
        *
        * @since Coherence 12.2.1
        */
        virtual PofWriter::Handle createNestedPofWriter(int32_t iProp, int32_t nTypeId) = 0;

        /**
        * Write the remaining properties to the POF stream, terminating the
        * writing of the currrent user type. As part of writing out a user
        * type, this method must be called by the PofSerializer that is
        * writing out the user type, or the POF stream will be corrupted.
        *
        * Calling this method terminates the current user type by writing
        * a -1 to the POF stream after the last indexed property. Subsequent
        * calls to the various <tt>writeXYZ</tt> methods of this interface
        * will fail after this method is called.
        *
        * @param vBinProps a buffer that contains zero or more indexed
        *                  properties in binary POF encoded form; may be
        *                  <tt>NULL</tt>
        *
        * @throws coherence::lang::IllegalStateException if no user type is
        *         being written
        * @throws coherence::io::IOException if an I/O error occurs
        */
        virtual void writeRemainder(Binary::View vBinProps) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_WRITER_HPP
