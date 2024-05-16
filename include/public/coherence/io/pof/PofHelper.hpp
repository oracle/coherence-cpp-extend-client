/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_HELPER_HPP
#define COH_POF_HELPER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/pof/PofConstants.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDateTime.hpp"
#include "coherence/io/pof/RawDayTimeInterval.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/RawTimeInterval.hpp"
#include "coherence/io/pof/RawYearMonthInterval.hpp"
#include "coherence/util/AbstractMap.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Set.hpp"

#include <limits>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::ReadBuffer;
using coherence::util::AbstractMap;
using coherence::util::Binary;
using coherence::util::Collection;
using coherence::util::List;
using coherence::util::Set;


/**
* Collection of helper methods for POF streams.
*
* @author jh  2008.04.09
*/
class COH_EXPORT PofHelper
    : public abstract_spec<PofHelper,
        extends<PofConstants> >
    {
    public:
        /**
        * Return an identifier that represents the C++ type of the specified
        * object.
        *
        * @param v     an object to determine the type of
        * @param vCtx  the PofContext used to determine if the object is an
        *              instance of a valid user type; must not be NULL
        *
        * @return one of the {@link PofConstants} class <tt>C_*</tt>
        *         constants
        */
        static int32_t getClassTypeId(Object::View v, PofContext::View vCtx);

        /**
        * Return an indentifier that represents the POF type of the specified
        * class.
        *
        * @param vClass  the class; must not be NULL
        * @param vCtx    the PofContext used to determine the type identifier
        *                of a user type; must not be NULL
        *
        * @return one of the {@link PofConstants} class <tt>T_*</tt>
        *         constants
        *
        * @throws IllegalArgumentException if the user type associated with
        *         the given object is unknown to the specified PofContext
        */
        static int32_t getPofTypeId(Class::View vClass, PofContext::View vCtx);

        /**
        * Encode an int32_t value into one of the reserved single-byte
        * combined type and value indicators.
        *
        * @param n  an int32_t between -1 and 22 inclusive
        *
        * @return the int32_t value that the int32_t is encoded as
        */
        static int32_t encodeTinyInt32(int32_t n);

        /**
        * Decode an int32_t value from one of the reserved single-byte
        * combined type and value indicators.
        *
        * @param n  the int32_t value that the int32_t is encoded as
        *
        * @return an int32_t between -1 and 22 inclusive
        */
        static int32_t decodeTinyInt32(int32_t n);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to a wchar16_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as an char
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to a char value
        */
        static wchar16_t readAsChar16(ReadBuffer::BufferInput::Handle hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to a wchar16_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as an char
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to a char value
        */
        static wchar16_t readAsChar16(FinalHandle<ReadBuffer::BufferInput>& hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to an int32_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as an int32_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to an int32_t value
        */
        static int32_t readAsInt32(ReadBuffer::BufferInput::Handle hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to an int32_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as an int32_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to an int32_t value
        */
        static int32_t readAsInt32(FinalHandle<ReadBuffer::BufferInput>& hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to an int64_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as an int64_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to an int64_t value
        */
        static int64_t readAsInt64(ReadBuffer::BufferInput::Handle hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to an int64_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as an int64_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to an int64_t value
        */
        static int64_t readAsInt64(FinalHandle<ReadBuffer::BufferInput>& hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to a float32_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as a float32_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to a float32_t value
        */
        static float32_t readAsFloat32(ReadBuffer::BufferInput::Handle hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to a float32_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as a float32_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to a float32_t value
        */
        static float32_t readAsFloat32(FinalHandle<ReadBuffer::BufferInput>& hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to a float64_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as a float64_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to a float64_t value
        */
        static float64_t readAsFloat64(ReadBuffer::BufferInput::Handle hIn,
                int32_t nType);

        /**
        * Read a value of the specified encoding from the POF stream and
        * convert it to a float64_t.
        *
        * @param hIn    the POF stream containing the value
        * @param nType  the POF type of the value
        *
        * @return the POF value as a float64_t
        *
        * @throws IOException if an I/O error occurs reading the POF stream,
        *         or the POF value cannot be coerced to a float64_t value
        */
        static float64_t readAsFloat64(FinalHandle<ReadBuffer::BufferInput>& hIn,
                int32_t nType);

        /**
        * Read a RawDate value from the passed BufferInput.
        *
        * @param hIn  the BufferInput object to read from
        *
        * @return the RawDate value
        */
        static RawDate::View readRawDate(
                ReadBuffer::BufferInput::Handle hIn);

        /**
        * Read a RawDate value from the passed BufferInput.
        *
        * @param hIn  the BufferInput object to read from
        *
        * @return the RawDate value
        */
        static RawDate::View readRawDate(
                FinalHandle<ReadBuffer::BufferInput>& hIn);

        /**
        * Read a RawTime value from the passed BufferInput.
        *
        * @param hIn  the BufferInput object to read from
        *
        * @return the RawTime value
        */
        static RawTime::View readRawTime(
                ReadBuffer::BufferInput::Handle hIn);

        /**
        * Read a RawTime value from the passed BufferInput.
        *
        * @param hIn  the BufferInput object to read from
        *
        * @return the RawTime value
        */
        static RawTime::View readRawTime(
                FinalHandle<ReadBuffer::BufferInput>& hIn);

        /**
        * Within the POF stream, skip the next POF value.
        *
        * @param hIn the BufferInput containing the POF stream
        */
        static void skipValue(ReadBuffer::BufferInput::Handle hIn);

        /**
        * Within the POF stream, skip the next POF value of the specified
        * type.
        *
        * @param hIn    the BufferInput containing the POF stream
        * @param nType  the type of the value to skip
        */
        static void skipUniformValue(ReadBuffer::BufferInput::Handle hIn,
                int32_t nType);

        /**
        * Skip the specified number of integers in the passed POF stream.
        *
        * @param hIn  the BufferInput containing the POF stream
        * @param c    the number of integers to skip over
        */
        static void skipInts(ReadBuffer::BufferInput::Handle hIn, int32_t c);

        /**
        * Validate that the specified POF collection size as read from a POF
        * stream is smaller than the largest value of the size32_t type, and if
        * so, return the given size as a size32_t.
        *
        * @param cElements  the POF collection size
        *
        * @return the POF collection size as a size32_t
        *
        * @throws Exception if the given POF collection size is greater than
        *         the largest value of the size32_t type
        */
        inline static size32_t validateIncomingSize(int32_t cElements)
            {
            COH_ENSURE(cElements >= 0);

            size32_t nElements = (size32_t) cElements;
            if (((int32_t) nElements) != cElements)
                {
                COH_THROW_STREAM (IllegalStateException,
                        "incoming POF collection size overflow: " << cElements
                        << " > " << (std::numeric_limits<size32_t>::max)());
                }
            return nElements;
            }

        /**
        * Validate that the specified Collection size is smaller than the
        * largest value of the int32_t type, and if so, return the given size
        * as an in32_t.
        *
        * @param cElements  the Collection size
        *
        * @return the Collection size as a int32_t
        *
        * @throws Exception if the given Collection size is greater than the
        *         largest value of the int32_t type
        */
        inline static int32_t validateOutgoingSize(size32_t cElements)
            {
            int32_t nElements = (int32_t) cElements;
            if (((size32_t) nElements) != cElements || nElements < 0)
                {
                COH_THROW_STREAM (IllegalStateException,
                        "outgoing POF collection size overflow: " << cElements
                        << " > " << Integer32::max_value);
                }
            return nElements;
            }

        /**
        * Validate date information.
        *
        * @param nYear   the year number
        * @param nMonth  the month number
        * @param nDay    the day number
        */
        static void checkDate(int32_t nYear, int32_t nMonth, int32_t nDay);

        /**
        * Validate time information.
        *
        * @param nHour    the hour number
        * @param nMinute  the minute number
        * @param nSecond  the second number
        * @param nNano    the nanosecond number
        */
        static void checkTime(int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano);

        /**
        * Check the specified timezone offset.
        *
        * @param nHourOffset    the hour offset
        * @param nMinuteOffset  the minute offset
        */
        static void checkTimeZone(int32_t nHourOffset, int32_t nMinuteOffset);

        /**
        * Validate a time interval.
        *
        * @param cHours    the number of hours
        * @param cMinutes  the number of minutes
        * @param cSeconds  the number of seconds
        * @param cNanos    the number of nanoseconds
        */
        static void checkTimeInterval(int32_t cHours, int32_t cMinutes,
                int32_t cSeconds, int32_t cNanos);

        /**
        * Validate a day-time interval.
        *
        * See http://www.builderau.com.au/architect/database/soa/SQL_basics_Datetime_and_interval_data_types/0,39024547,20269031,00.htm
        *
        * @param cDays     the number of days
        * @param cHours    the number of hours
        * @param cMinutes  the number of minutes
        * @param cSeconds  the number of seconds
        * @param cNanos    the number of nanoseconds
        */
        static void checkDayTimeInterval(int32_t cDays, int32_t cHours,
                int32_t cMinutes, int32_t cSeconds, int32_t cNanos);

        /**
        * Validate a year-month interval.
        *
        * @param cYears   the number of years
        * @param cMonths  the number of months
        */
        static void checkYearMonthInterval(int32_t cYears, int32_t cMonths);

        /**
        * Format a date in the form YYYY-MM-DD.
        *
        * @param nYear   the year number
        * @param nMonth  the month number
        * @param nDay    the day number
        */
        static String::View formatDate(int32_t nYear, int32_t nMonth,
                int32_t nDay);

        /**
        * Format a time using the simplest applicable of the following
        * formats:
        * <ol>
        * <li><tt>HH:MM</tt></li>
        * <li><tt>HH:MM:SS</tt></li>
        * <li><tt>HH:MM:SS.MMM</tt></li>
        * <li><tt>HH:MM:SS.NNNNNNNNN</tt></li>
        * </ol>
        *
        * @param nHour    the hour number
        * @param nMinute  the minute number
        * @param nSecond  the second number
        * @param nNano    the nanosecond number
        * @param fUTC     true for UTC, false for no time zone
        *
        * @return a time String
        */
        static String::View formatTime(int32_t nHour, int32_t nMinute,
                int32_t nSecond, int32_t nNano, bool fUTC);

        /**
        * Format a time using the simplest applicable of the following
        * formats:
        * <ol>
        * <li><tt>HH:MM(+|-)HH:MM</tt></li>
        * <li><tt>HH:MM:SS(+|-)HH:MM</tt></li>
        * <li><tt>HH:MM:SS.MMM(+|-)HH:MM</tt></li>
        * <li><tt>HH:MM:SS.NNNNNNNNN(+|-)HH:MM</tt></li>
        * </ol>
        *
        * @param nHour          the hour number
        * @param nMinute        the minute number
        * @param nSecond        the second number
        * @param nNano          the nanosecond number
        * @param nHourOffset    the timezone offset in hours
        * @param nMinuteOffset  the timezone offset in minutes
        *
        * @return a time String
        */
        static String::View formatTime(int32_t nHour, int32_t nMinute,
                int32_t nSecond, int32_t nNano, int32_t nHourOffset,
                int32_t nMinuteOffset);

        /**
        * Write a date value to a BufferOutput object.
        *
        * @param hOut    the BufferOutput to write to
        * @param nYear   the year number as defined by ISO8601
        * @param nMonth  the month number between 1 and 12 inclusive as
        *                defined by ISO8601
        * @param nDay    the day number between 1 and 31 inclusive as defined
        *                by ISO8601
        *
        * @throws IOException thrown if the passed BufferOutput object throws
        *         an IOException while the value is being written to it
        */
        static void writeDate(WriteBuffer::BufferOutput::Handle hOut,
                int32_t nYear, int32_t nMonth, int32_t nDay);

        /**
        * Write a time value to a BufferOutput object.
        *
        * @param hOut          the BufferOutput to write to
        * @param nHour         the hour between 0 and 23 inclusive
        * @param nMinute       the minute value between 0 and 59 inclusive
        * @param nSecond       the second value between 0 and 59 inclusive
        *                      (and theoretically 60 for a leap-second)
        * @param nNano         the nanosecond value between 0 and 999999999
        *                      inclusive
        * @param nTimeZoneType 0 if the time value does not have an explicit
        *                      time zone, 1 if the time value is UTC and 2 if
        *                      the time zone has an explicit hour and minute
        *                      offset
        * @param nHourOffset   the timezone offset in hours from UTC, for
        *                      example 0 for BST, -5 for EST and 1 for CET
        * @param nMinuteOffset the timezone offset in minutes, for example 0
        *                      (in most cases) or 30
        *
        * @throws IOException thrown if the passed BufferOutput object throws
        *         an IOException while the value is being written to it
        */
        static void writeTime(WriteBuffer::BufferOutput::Handle hOut,
                int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano, int32_t nTimeZoneType, int32_t nHourOffset,
                int32_t nMinuteOffset);

        /**
        * Expand the passed array to contain the specified number of elements.
        *
        * @param haOld  the "template" array or null
        * @param cNew   the number of desired elements in the new array
        *
        * @return the old array, if it was big enough, or a new array of the 
        *         same type
        *
        * @since Coherence 12.1.2
        */
        static ObjectArray::Handle resizeArray(ObjectArray::Handle haOld, 
                size32_t cNew);


    // ----- inner class: WriteableEntrySetMap ------------------------------

    public:
        /**
        * Immutable Map implementation backed by a Set of Map::Entry objects.
        *
        * @author jh  2008.05.05
        */
        class WriteableEntrySetMap
            : public class_spec<WriteableEntrySetMap,
                extends<AbstractMap> >
            {
            friend class factory<WriteableEntrySetMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a new WriteableEntrySetMap that is backed by the
                * given Set of Map::Entry objects.
                *
                * @param vSetEntries  a Set of Map::Entry objects in the new
                *                     WriteableEntrySetMap; must not be NULL
                */
                WriteableEntrySetMap(Set::View vSetEntries);

            // ----- Map interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder put(Object::View vKey,
                        Object::Holder ohValue);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder remove(Object::View vKey);
                using Map::remove;

                /**
                * {@inheritDoc}
                */
                virtual void clear();

                /**
                * {@inheritDoc}
                */
                virtual Set::Handle entrySet();

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The backing Set of Map::Entry objects.
                */
                FinalView<Set> f_vSetEntries;
            };


    // ----- inner class: ReadableEntrySetMap -------------------------------

    public:
        /**
        * Map implementation backed by a List of Map.Entry objects.
        *
        * @author jh  2008.05.05
        */
        class ReadableEntrySetMap
            : public class_spec<ReadableEntrySetMap,
                extends<AbstractMap> >
            {
            friend class factory<ReadableEntrySetMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a new ReadableEntrySetMap.
                */
                ReadableEntrySetMap();

            // ----- Map interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder put(Object::View vKey,
                        Object::Holder ohValue);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder remove(Object::View vKey);
                using Map::remove;

                /**
                * {@inheritDoc}
                */
                virtual void clear();

                /**
                * {@inheritDoc}
                */
                virtual Set::Handle entrySet();

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The backing List of Map::Entry objects.
                */
                mutable FinalHandle<List> f_hListEntries;
            };


    // ----- constants ------------------------------------------------------

    public:
        /**
        * @return an empty array of bool.
        */
        static Array<bool>::Handle getEmptyBooleanArray();

        /**
        * @return an empty array of octet_t.
        */
        static Array<octet_t>::Handle getEmptyOctetArray();

        /**
        * @return an empty array of wchar16_t.
        */
        static Array<wchar16_t>::Handle getEmptyChar16Array();

        /**
        * @return an empty array of int16_t.
        */
        static Array<int16_t>::Handle getEmptyInt16Array();

        /**
        * @return an empty array of int32_t.
        */
        static Array<int32_t>::Handle getEmptyInt32Array();

        /**
        * @return an empty array of int64_t.
        */
        static Array<int64_t>::Handle getEmptyInt64Array();

        /**
        * @return an empty array of float32_t.
        */
        static Array<float32_t>::Handle getEmptyFloat32Array();

        /**
        * @return an empty array of float64_t.
        */
        static Array<float64_t>::Handle getEmptyFloat64Array();

        /**
        * @return an empty array of objects.
        */
        static ObjectArray::Handle getEmptyObjectArray();

        /**
        * A zero-length Binary.
        */
        static Binary::Handle getEmptyBinary();

        /**
        * @return an empty and immutable Collection.
        */
        static Collection::View getEmptyCollection();
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_HELPER_HPP
