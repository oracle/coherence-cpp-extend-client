/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_HANDLER_HPP
#define COH_POF_HANDLER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::Binary;


/**
* This interface defines the handler for an event-driven approach to parsing
* (or assembling) a POF stream.
*
* @author jh  2008.01.23
*/
class COH_EXPORT PofHandler
    : public interface_spec<PofHandler>
    {
    // ----- PofHandler interface -------------------------------------------

    public:
        /**
        * This method is invoked when an identity is encountered in the POF
        * stream. The identity is used to uniquely identify the next value in
        * the POF stream, and can be later referenced by the
        * {@link #onIdentityReference} method.
        *
        * @param nId  if <tt>(nId >= 0)</tt>, then this is the identity
        *             encountered in the POF stream, otherwise it is an
        *             indicator that the following value <i>could</i> have
        *             been assigned an identifier but was not (i.e. that the
        *             subsequent value is of a referenceable data type)
        */
        virtual void registerIdentity(int32_t nId) = 0;

        /**
        * Specifies that a NULL value has been encountered in the POF stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        */
        virtual void onNullReference(int32_t iPos) = 0;

        /**
        * Specifies that a reference to a previously-identified value has
        * been encountered in the POF stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param nId   the identity of the previously encountered value, as
        *              was specified in a previous call to {@link
        *              #registerIdentity}
        */
        virtual void onIdentityReference(int32_t iPos, int32_t nId) = 0;

        /**
        * Report that a short integer value has been encountered in the POF
        * stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param n     the integer value as a short
        */
        virtual void onInt16(int32_t iPos, int16_t n) = 0;

        /**
        * Report that an integer value has been encountered in the POF stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param n     the integer value as an int32_t
        */
        virtual void onInt32(int32_t iPos, int32_t n) = 0;

        /**
        * Report that a long integer value has been encountered in the POF
        * stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param n     the integer value as a long
        */
        virtual void onInt64(int32_t iPos, int64_t n) = 0;

        /**
        * Report that a base-2 single-precision floating-point value has been
        * encountered in the POF stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param fl    the floating-point value as a float32_t
        */
        virtual void onFloat32(int32_t iPos, float32_t fl) = 0;

        /**
        * Report that a base-2 double-precision floating-point value has been
        * encountered in the POF stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param dfl   the floating-point value as a float64_t
        */
        virtual void onFloat64(int32_t iPos, float64_t dfl) = 0;

        /**
        * Report that a boolean value has been encountered in the POF stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param f     the boolean value
        */
        virtual void onBoolean(int32_t iPos, bool f) = 0;

        /**
        * Report that an octet value (a byte) has been encountered in the POF
        * stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param b     the octet value
        */
        virtual void onOctet(int32_t iPos, octet_t b) = 0;

        /**
        * Report that a octet string value has been encountered in the POF
        * stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param vBin  the octect string value as a Binary object
        */
        virtual void onOctetString(int32_t iPos, Binary::View vBin) = 0;

        /**
        * Report that a character value has been encountered in the POF
        * stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param ch    the character value as a wchar16_t
        */
        virtual void onChar(int32_t iPos, wchar16_t ch) = 0;

        /**
        * Report that a character string value has been encountered in the
        * POF stream.
        *
        * @param iPos  context-sensitive position information: property index
        *              within a user type, array index within an array,
        *              element counter within a collection, entry counter
        *              within a map, -1 otherwise
        * @param vs    the character string value as a String object
        */
        virtual void onCharString(int32_t iPos, String::View vs) = 0;

        /**
        * Report that a date value has been encountered in the POF stream.
        *
        * @param iPos    context-sensitive position information: property
        *                index within a user type, array index within an
        *                array, element counter within a collection, entry
        *                counter within a map, -1 otherwise
        * @param nYear   the year number as defined by ISO8601
        * @param nMonth  the month number between 1 and 12 inclusive as
        *                defined by ISO8601
        * @param nDay    the day number between 1 and 31 inclusive as defined
        *                by ISO8601
        */
        virtual void onDate(int32_t iPos, int32_t nYear, int32_t nMonth,
                int32_t nDay) = 0;

        /**
        * Report that a year-month interval value has been encountered in the
        * POF stream.
        *
        * @param iPos     context-sensitive position information: property
        *                 index within a user type, array index within an
        *                 array, element counter within a collection, entry
        *                 counter within a map, -1 otherwise
        * @param cYears   the number of years in the year-month interval
        * @param cMonths  the number of months in the year-month interval
        */
        virtual void onYearMonthInterval(int32_t iPos, int32_t cYears,
                int32_t cMonths) = 0;

        /**
        * Report that a time value has been encountered in the POF stream.
        *
        * @param iPos     context-sensitive position information: property
        *                 index within a user type, array index within an
        *                 array, element counter within a collection, entry
        *                 counter within a map, -1 otherwise
        * @param nHour    the hour between 0 and 23 inclusive
        * @param nMinute  the minute value between 0 and 59 inclusive
        * @param nSecond  the second value between 0 and 59 inclusive (and
        *                 theoretically 60 for a leap-second)
        * @param nNano    the nanosecond value between 0 and 999999999
        *                 inclusive
        * @param fUTC     true if the time value is UTC or false if the time
        *                 value does not have an explicit time zone
        */
        virtual void onTime(int32_t iPos, int32_t nHour, int32_t nMinute,
                int32_t nSecond, int32_t nNano, bool fUTC) = 0;

        /**
        * Report that a time value (with a timezone offset) has been
        * encountered in the POF stream.
        *
        * @param iPos           context-sensitive position information:
        *                       property index within a user type, array
        *                       index within an array, element counter within
        *                       a collection, entry counter within a map, -1
        *                       otherwise
        * @param nHour          the hour between 0 and 23 inclusive
        * @param nMinute        the minute value between 0 and 59 inclusive
        * @param nSecond        the second value between 0 and 59 inclusive
        *                       (and theoretically 60 for a leap-second)
        * @param nNano          the nanosecond value between 0 and 999999999
        *                       inclusive
        * @param nHourOffset    the timezone offset in hours from UTC, for
        *                       example 0 for BST, -5 for EST and 1 for CET
        * @param nMinuteOffset  the timezone offset in minutes, for example 0
        *                       (in most cases) or 30
        *
        * @see <a href="http://www.worldtimezone.com/faq.html">worldtimezone.com</a>
        */
        virtual void onTime(int32_t iPos, int32_t nHour, int32_t nMinute,
                int32_t nSecond, int32_t nNano, int32_t nHourOffset,
                int32_t nMinuteOffset) = 0;

        /**
        * Report that a time interval value has been encountered in the POF
        * stream.
        *
        * @param iPos      context-sensitive position information: property
        *                  index within a user type, array index within an
        *                  array, element counter within a collection, entry
        *                  counter within a map, -1 otherwise
        * @param cHours    the number of hours in the time interval
        * @param cMinutes  the number of minutes in the time interval, from 0
        *                  to 59 inclusive
        * @param cSeconds  the number of seconds in the time interval, from 0
        *                  to 59 inclusive
        * @param cNanos    the number of nanoseconds, from 0 to 999999999
        *                  inclusive
        */
        virtual void onTimeInterval(int32_t iPos, int32_t cHours,
                int32_t cMinutes, int32_t cSeconds, int32_t cNanos) = 0;

        /**
        * Report that a date-time value has been encountered in the POF
        * stream.
        *
        * @param iPos     context-sensitive position information: property
        *                 index within a user type, array index within an
        *                 array, element counter within a collection, entry
        *                 counter within a map, -1 otherwise
        * @param nYear    the year number as defined by ISO8601
        * @param nMonth   the month number between 1 and 12 inclusive as
        *                 defined by ISO8601
        * @param nDay     the day number between 1 and 31 inclusive as
        *                 defined by ISO8601
        * @param nHour    the hour between 0 and 23 inclusive
        * @param nMinute  the minute value between 0 and 59 inclusive
        * @param nSecond  the second value between 0 and 59 inclusive (and
        *                 theoretically 60 for a leap-second)
        * @param nNano    the nanosecond value between 0 and 999999999
        *                 inclusive
        * @param fUTC     true if the time value is UTC or false if the time
        *                 value does not have an explicit time zone
        */
        virtual void onDateTime(int32_t iPos, int32_t nYear, int32_t nMonth,
                int32_t nDay, int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano, bool fUTC) = 0;

        /**
        * Report that a date-time value (with a timezone offset) has been
        * encountered in the POF stream.
        *
        * @param iPos           context-sensitive position information:
        *                       property index within a user type, array
        *                       index within an array, element counter within
        *                       a collection, entry counter within a map, -1
        *                       otherwise
        * @param nYear          the year number as defined by ISO8601
        * @param nMonth         the month number between 1 and 12 inclusive
        *                       as defined by ISO8601
        * @param nDay           the day number between 1 and 31 inclusive as
        *                       defined by ISO8601
        * @param nHour          the hour between 0 and 23 inclusive
        * @param nMinute        the minute value between 0 and 59 inclusive
        * @param nSecond        the second value between 0 and 59 inclusive
        *                       (and theoretically 60 for a leap-second)
        * @param nNano          the nanosecond value between 0 and 999999999
        *                       inclusive
        * @param nHourOffset    the timezone offset in hours from UTC, for
        *                       example 0 for BST, -5 for EST and 1 for CET
        * @param nMinuteOffset  the timezone offset in minutes, for example 0
        *                       (in most cases) or 30
        */
        virtual void onDateTime(int32_t iPos, int32_t nYear, int32_t nMonth,
                int32_t nDay, int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano, int32_t nHourOffset, int32_t nMinuteOffset) = 0;

        /**
        * Report that a day-time interval value has been encountered in the POF
        * stream.
        *
        * @param iPos      context-sensitive position information: property
        *                  index within a user type, array index within an
        *                  array, element counter within a collection, entry
        *                  counter within a map, -1 otherwise
        * @param cDays     the number of days in the day-time interval
        * @param cHours    the number of hours in the day-time interval, from
        *                  0 to 23 inclusive
        * @param cMinutes  the number of minutes in the day-time interval,
        *                  from 0 to 59 inclusive
        * @param cSeconds  the number of seconds in the day-time interval,
        *                  from 0 to 59 inclusive
        * @param cNanos    the number of nanoseconds in the day-time interval,
        *                  from 0 to 999999999 inclusive
        */
        virtual void onDayTimeInterval(int32_t iPos, int32_t cDays,
                int32_t cHours, int32_t cMinutes, int32_t cSeconds,
                int32_t cNanos) = 0;

        /**
        * Report that a collection of values has been encountered in the POF
        * stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the <tt>cElements</tt> elements in the
        * collection, and the collection extent will then be terminated by a
        * call to {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of values (elements) in the
        *                   collection
        */
        virtual void beginCollection(int32_t iPos, size32_t cElements) = 0;

        /**
        * Report that a uniform collection of values has been encountered in
        * the POF stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the <tt>cElements</tt> elements in the
        * collection, and the collection extent will then be terminated by a
        * call to {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of values (elements) in the
        *                   collection
        * @param nType      the type identifier for all of the values in the
        *                   uniform collection
        */
        virtual void beginUniformCollection(int32_t iPos, size32_t cElements,
                int32_t nType) = 0;

        /**
        * Report that an array of values has been encountered in the POF
        * stream.
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the <tt>cElements</tt> elements in the
        * array, and the array extent will then be terminated by a call to
        * {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of values (elements) in the
        *                   array
        */
        virtual void beginArray(int32_t iPos, size32_t cElements) = 0;

        /**
        * Report that a uniform array of values has been encountered in the
        * POF stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the <tt>cElements</tt> elements in the
        * array, and the array extent will then be terminated by a call to
        * {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of values (elements) in the
        *                   array
        * @param nType      the type identifier for all of the values in the
        *                   uniform array
        */
        virtual void beginUniformArray(int32_t iPos, size32_t cElements,
                int32_t nType) = 0;

        /**
        * Report that a sparse array of values has been encountered in the
        * POF stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for present element in the sparse array (up to
        * <tt>cElements</tt> elements), and the array extent will then be
        * terminated by a call to {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of elements in the array, which
        *                   is greater than or equal to the number of values
        *                   in the sparse POF stream; in other words, the
        *                   number of values that will subsequently be
        *                   reported will not exceed this number
        */
        virtual void beginSparseArray(int32_t iPos, size32_t cElements) = 0;

        /**
        * Report that a uniform sparse array of values has been encountered
        * in the POF stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for present element in the sparse array (up to
        * <tt>cElements</tt> elements), and the array extent will then be
        * terminated by a call to {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of elements in the array, which
        *                   is greater than or equal to the number of values
        *                   in the sparse POF stream; in other words, the
        *                   number of values that will subsequently be
        *                   reported will not exceed this number
        * @param nType      the type identifier for all of the values in the
        *                   uniform sparse array
        */
        virtual void beginUniformSparseArray(int32_t iPos, size32_t cElements,
                int32_t nType) = 0;

        /**
        * Report that a map of key/value pairs has been encountered in the
        * POF stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the <tt>cElements</tt> elements in the
        * map, and the map extent will then be terminated by a call to
        * {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of key/value pairs (entries) in
        *                   the map
        */
        virtual void beginMap(int32_t iPos, size32_t cElements) = 0;

        /**
        * Report that a map of key/value pairs (with the keys being of a
        * uniform type) has been encountered in the POF stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the <tt>cElements</tt> elements in the
        * map, and the map extent will then be terminated by a call to
        * {@link #endComplexValue()}.
        *
        * @param iPos       context-sensitive position information: property
        *                   index within a user type, array index within an
        *                   array, element counter within a collection, entry
        *                   counter within a map, -1 otherwise
        * @param cElements  the exact number of key/value pairs (entries) in
        *                   the map
        * @param nTypeKeys  the type identifier for all of the keys in the
        *                   uniform-keys map
        */
        virtual void beginUniformKeysMap(int32_t iPos, size32_t cElements,
                int32_t nTypeKeys) = 0;

        /**
        * Report that a map of key/value pairs (with the keys being of a
        * uniform type and the values being of a uniform type) has been
        * encountered in the POF stream.
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the <tt>cElements</tt> elements in the
        * map, and the map extent will then be terminated by a call to
        * {@link #endComplexValue()}.
        *
        * @param iPos         context-sensitive position information:
        *                     property index within a user type, array index
        *                     within an array, element counter within a
        *                     collection, entry counter within a map, -1
        *                     otherwise
        * @param cElements    the exact number of key/value pairs (entries)
        *                     in the map
        * @param nTypeKeys    the type identifier for all of the keys in the
        *                     uniform map
        * @param nTypeValues  the type identifier for all of the values in
        *                     the uniform map
        */
        virtual void beginUniformMap(int32_t iPos, size32_t cElements,
                int32_t nTypeKeys, int32_t nTypeValues) = 0;

        /**
        * Report that a value of a "user type" has been encountered in the
        * POF stream. A user type is analogous to a "class", and a value of a
        * user type is analogous to an "object".
        *
        * This method call will be followed by a separate call to an "on" or
        * "begin" method for each of the property values in the user type,
        * and the user type will then be terminated by a call to
        * {@link #endComplexValue()}.
        *
        * @param iPos         context-sensitive position information:
        *                     property index within a user type, array index
        *                     within an array, element counter within a
        *                     collection, entry counter within a map, -1
        *                     otherwise
        * @param nUserTypeId  the user type identifier,
        *                     <tt>(nUserTypeId &gt;= 0)</tt>
        * @param nVersionId   the version identifier for the user data type
        *                     data in the POF stream, <tt>(nVersionId &gt;=
        *                     0)</tt>
        */
        virtual void beginUserType(int32_t iPos, int32_t nUserTypeId,
                int32_t nVersionId) = 0;

        /**
        * Signifies the termination of the current complex value. Complex
        * values are any of the collection, array, map and user types. For
        * each call to one of the "begin" methods, there will be a
        * corresponding call to this method, even if there were no contents
        * in the complex value.
        */
        virtual void endComplexValue() = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_HANDLER_HPP
