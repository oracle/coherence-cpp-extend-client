/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_READER_HPP
#define COH_POF_READER_HPP

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
using coherence::util::LongArray;
using coherence::util::Map;


/**
* The PofReader interface provides the capability of reading a set of
* non-primitive types ("user types") from a POF stream as an ordered
* sequence of indexed properties.
*
* See {@link PofWriter} for a complete description of the POF user type
* serialization format.
*
* @author jh  2008.01.18
*/
class COH_EXPORT PofReader
    : public interface_spec<PofReader>
    {
    // ----- primitive type support -----------------------------------------

    public:
        /**
        * Read a boolean property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>bool</tt> property value, or zero if no
        *         value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual bool readBoolean(int32_t iProp) = 0;

        /**
        * Read an octet property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>octet_t</tt> property value, or zero if no
        *         value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual octet_t readOctet(int32_t iProp) = 0;

        /**
        * Read a 16-bit Unicode character property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the 16-bit Unicode character property value as a
        *         <tt>wchar16_t</tt> value, or zero if no value was
        *         available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual wchar16_t readChar16(int32_t iProp) = 0;

        /**
        * Read a 16-bit integer property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>int16_t</tt> property value, or zero if no value
        *         was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual int16_t readInt16(int32_t iProp) = 0;

        /**
        * Read a 32-bit integer property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>int32_t</tt> property value, or zero if no value
        *         was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual int32_t readInt32(int32_t iProp) = 0;

        /**
        * Read a 64-bit integer property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>int64_t</tt> property value, or zero if no value
        *         was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual int64_t readInt64(int32_t iProp) = 0;

        /**
        * Read a a 32-bit floating-point property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>float32_t</tt> property value, or zero if no value
        *         was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual float32_t readFloat32(int32_t iProp) = 0;

        /**
        * Read a 64-bit floating-point property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>float64_t</tt> property value, or zero if no value
        *         was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual float64_t readFloat64(int32_t iProp) = 0;


    // ----- primitive array support ----------------------------------------

    public:
        /**
        * Read a boolean array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>bool</tt> array property value, or <tt>NULL</tt>
        *         if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<bool>::Handle readBooleanArray(int32_t iProp) = 0;

        /**
        * Read an octet array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>octet_t</tt> array property value, or <tt>NULL</tt>
        *         if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<octet_t>::Handle readOctetArray(int32_t iProp) = 0;

        /**
        * Read a 16-bit Unicode character array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>wchar16_t</tt> array property value, or
        *         <tt>NULL</tt> if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<wchar16_t>::Handle readChar16Array(int32_t iProp) = 0;

        /**
        * Read a 16-bit integer array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>int16_t</tt> array property value, or <tt>NULL</tt>
        *         if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<int16_t>::Handle readInt16Array(int32_t iProp) = 0;

        /**
        * Read a 32-bit integer array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>int32_t</tt> array property value, or <tt>NULL</tt>
        *         if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<int32_t>::Handle readInt32Array(int32_t iProp) = 0;

        /**
        * Read a 64-bit integer array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>int64_t</tt> array property value, or <tt>NULL</tt>
        *         if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<int64_t>::Handle readInt64Array(int32_t iProp) = 0;

        /**
        * Read a a 32-bit floating-point array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>float32_t</tt> array property value, or
        *         <tt>NULL</tt> if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<float32_t>::Handle readFloat32Array(int32_t iProp) = 0;

        /**
        * Read a a 64-bit floating-point array property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>float64_t</tt> array property value, or
        *         <tt>NULL</tt> if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Array<float64_t>::Handle readFloat64Array(int32_t iProp) = 0;


    // ----- object value support -------------------------------------------

    public:
        /**
        * Read a <tt>Binary</tt> property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>Binary</tt> property value, or <tt>NULL</tt> if no
        *         value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Binary::View readBinary(int32_t iProp) = 0;

        /**
        * Read a <tt>String</tt> property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>String</tt> property value, or <tt>NULL</tt> if no
        *         value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual String::View readString(int32_t iProp) = 0;

        /**
        * Read a {@link RawDate} property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>RawDate</tt> property value, or <tt>NULL</tt> if no
        *         value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual RawDate::View readRawDate(int32_t iProp) = 0;

        /**
        * Read a {@link RawTime} property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>RawTime</tt> property value, or <tt>NULL</tt> if no
        *         value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual RawTime::View readRawTime(int32_t iProp) = 0;

        /**
        * Read a {@link RawDateTime} property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>RawDateTime</tt> property value, or <tt>NULL</tt>
        *         if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual RawDateTime::View readRawDateTime(int32_t iProp) = 0;

        /**
        * Read a {@link RawYearMonthInterval} property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>RawYearMonthInterval</tt> property value, or
        *         <tt>NULL</tt> if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual RawYearMonthInterval::View readRawYearMonthInterval(
                int32_t iProp) = 0;

        /**
        * Read a {@link RawTimeInterval} property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>RawTimeInterval</tt> property value, or
        *         <tt>NULL</tt> if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual RawTimeInterval::View readRawTimeInterval(int32_t iProp) = 0;

        /**
        * Read a {@link RawDayTimeInterval} property from the POF stream.
        *
        * @param iProp  the property index to read
        *
        * @return the <tt>RawDayTimeInterval</tt> property value, or
        *         <tt>NULL</tt> if no value was available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual RawDayTimeInterval::View readRawDayTimeInterval(int32_t iProp) = 0;

        /**
        * Read a property of any type, including a user type, from the POF
        * stream.
        *
        * @param iProp  the property index to read
        *
        * @return the Object value, or <tt>NULL</tt> if no value was
        *         available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Object::Holder readObject(int32_t iProp) = 0;

        /**
        * Read an <tt>ObjectArray</tt> property from the POF stream.
        *
        * @param iProp  the property index to read
        * @param ha     the optional <tt>ObjectArray</tt> to use to store
        *               the values, or to use as a typed template for
        *               creating an array to store the values; may be
        *               <tt>NULL</tt>
        *
        * @return an <tt>ObjectArray</tt> of object values, or <tt>NULL</tt>
        *         if no <tt>ObjectArray</tt> is passed and there is no array
        *         data in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual ObjectArray::Handle readObjectArray(int32_t iProp,
                ObjectArray::Handle ha = NULL) = 0;


    // ----- collection support ---------------------------------------------

    public:
        /**
        * Read a <tt>LongArray</tt> of object values from the POF stream.
        *
        * @param iProp  the property index to read
        * @param hla    the optional <tt>LongArray</tt> to use to store the
        *               values; may be <tt>NULL</tt>
        *
        * @return a <tt>LongArray</tt> of object values, or <tt>NULL</tt> if
        *         no <tt>LongArray</tt> is passed and there is no key/value
        *         data available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual LongArray::View readLongArray(int32_t iProp,
                LongArray::Handle hla = NULL) = 0;

        /**
        * Read a <tt>Collection</tt> of object values from the POF stream.
        *
        * @param iProp  the property index to read
        * @param hCol   the optional <tt>Collection</tt> to use to store the
        *               values; may be <tt>NULL</tt>
        *
        * @return a <tt>Collection</tt> of object values, or <tt>NULL</tt> if
        *         no <tt>Collection</tt> is passed and there is no collection
        *         data available in the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Collection::View readCollection(int32_t iProp,
                Collection::Handle hCol = NULL) = 0;

        /**
        * Read a <tt>Map</tt> of key/value pairs from the POF stream.
        *
        * @param iProp  the property index to read
        * @param hMap   the optional <tt>Map</tt> to initialize; the handle
        *               may be <tt>NULL</tt>
        *
        * @return a <tt>Map</tt> of key/value pairs, or <tt>NULL</tt> if no
        *         <tt>Map</tt> is passed and there is no key/value data in
        *         the POF stream
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property
        * @throws IOException if an I/O error occurs
        */
        virtual Map::View readMap(int32_t iProp, Map::Handle hMap = NULL) = 0;


    // ----- POF user type support ------------------------------------------

    public:
        /**
        * Return the PofContext object used by this PofReader to deserialize
        * user types from a POF stream.
        *
        * @return the PofContext object that contains user type meta-data
        */
        virtual PofContext::View getPofContext() const = 0;

        /**
        * Configure the PofContext object used by this PofReader to
        * deserialize user types from a POF stream.
        *
        * Note: this is an advanced method that should be used with care. For
        * example, if this method is being used to switch to another
        * PofContext mid-POF stream, it is important to eventually restore
        * the original PofContext. For example:
        * <pre>
        * PofContext::View vCtxOrig = hReader->getPofContext();
        *
        * // switch to another PofContext
        * PofContext::View vCtxNew = ...;
        * hReader->setContext(vCtxNew);
        *
        * // read POF data using the reader
        * ...
        *
        * // restore the original PofContext
        * hReader->setPofContext(vCtxOrig);
        * </pre>
        *
        * @param vCtx  the new PofContext; must not be <tt>NULL</tt>
        */
        virtual void setPofContext(PofContext::View vCtx) = 0;

        /**
        * Determine the user type that is currently being parsed.
        *
        * @return the user type information, or -1 if the PofReader is not
        *         currently parsing a user type
        */
        virtual int32_t getUserTypeId() const = 0;

        /**
        * Determine the version identifier of the user type that is currently
        * being parsed.
        *
        * @return the integer version ID read from the POF stream; always
        *         non-negative
        *
        * @throws IllegalStateException if no user type is
        *         being parsed
        */
        virtual int32_t getVersionId() const = 0;

        /**
        * Register an identity for a newly created user type instance.
        *
        * If identity/reference types are enabled, an identity is used to
        * uniquely identify a user type instance within a POF stream. The
        * identity immediately proceeds the instance value in the POF stream and
        * can be used later in the stream to reference the instance.
        *
        * PofSerializer implementations must call this method with the user
        * type instance instantiated during deserialization prior to reading any
        * properties of the instance which are user type instances themselves.
        *
        * @param oh  the object to register the identity for
        *
        * @see PofSerializer::deserialize(PofReaderHandle)
        *
        * @since Coherence 3.7.1
        */
        virtual void registerIdentity(Object::Holder oh) = 0;

        /**
        * Obtain a PofReader that can be used to read a set of properties from a
        * single property of the current user type. The returned PofReader is
        * only valid from the time that it is returned until the next call is
        * made to this PofReader.
        *
        * @param iProp  the property index to read from
        *
        * @return a PofReader that reads its contents from  a single property of
        *         this PofReader
        *
        * @throws IllegalStateException if the POF
        *         stream has already advanced past the desired property,
        *         or if no user type is being parsed
        * @throws IOException if an I/O error occurs
        *
        * @since Coherence 3.6
        */
        virtual PofReader::Handle createNestedPofReader(int32_t iProp) = 0;

        /**
        * Read all remaining indexed properties of the current user type from
        * the POF stream. As part of reading in a user type, this method must
        * be called by the PofSerializer that is reading the user type, or
        * the read position within the POF stream will be corrupted.
        *
        * Subsequent calls to the various <tt>readXYZ</tt> methods of this
        * interface will fail after this method is called.
        *
        * @return a buffer containing zero or more indexed properties in
        *         binary POF encoded form, may be <tt>NULL</tt>
        *
        * @throws IllegalStateException if no user type is
        *         being parsed
        * @throws IOException if an I/O error occurs
        */
        virtual Binary::View readRemainder() = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_READER_HPP
