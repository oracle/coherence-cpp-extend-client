/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RAW_TIME_HPP
#define COH_RAW_TIME_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofIntrinsic.hpp"


#include <time.h>

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* Raw immutable POF time value.
*
* @author jh  2008.04.08
*/
class COH_EXPORT RawTime
    : public cloneable_spec<RawTime,
          extends<Object>,
          implements<PofIntrinsic> >
    {
    friend class factory<RawTime>;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The boxed native type.
        */
        typedef struct tm BoxedType;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a raw POF time value.
        *
        * @param nHour   the hour between 0 and 23 inclusive
        * @param nMinute the minute value between 0 and 59 inclusive
        * @param nSecond the second value between 0 and 59 inclusive (and
        *                theoretically 60 for a leap-second)
        * @param nNano   the nanosecond value between 0 and 999999999
        *                inclusive
        * @param fUTC    true if the time value is UTC or false if the time
        *                value does not have an explicit time zone
        */
        RawTime(int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano, bool fUTC);

        /**
        * Construct a raw POF time value with a timezone.
        *
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
        RawTime(int32_t nHour, int32_t nMinute, int32_t nSecond,
                int32_t nNano, int32_t nHourOffset, int32_t nMinuteOffset);

        /**
        * Construct a raw POF time value from a POSIX datetime struct.
        *
        * Note that the POSIX daylight savings flag is not preserved.
        *
        * @param timeinfo  the POSIX time
        * @param fUTC      true if the time value is UTC or false if the time
        *                  value does not have an explicit time zone
        */
        RawTime(const struct tm& timeinfo, bool fUTC = false);

        /**
        * Copy constructor.
        */
        RawTime(const RawTime& that);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isImmutable() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Determine the time's hour value.
        *
        * @return the hour between 0 and 23 inclusive
        */
        virtual int32_t getHour() const;

        /**
        * Determine the time's minute value.
        *
        * @return the minute value between 0 and 59 inclusive
        */
        virtual int32_t getMinute() const;

        /**
        * Determine the time's second value.
        *
        * @return the second value between 0 and 59 inclusive (and possibly
        *         60 for a leap-second)
        */
        virtual int32_t getSecond() const;

        /**
        * Determine the time's nanosecond value.
        *
        * @return the nanosecond value between 0 and 999999999 inclusive
        */
        virtual int32_t getNano() const;

        /**
        * Determine if the time value has an explicit timezone. A time value
        * without an explicit timezone is assumed to be in some conventional
        * local timezone, according to ISO8601.
        *
        * @return true iff the time has an explicit timezone
        */
        virtual bool hasTimezone() const;

        /**
        * Determine if the time value uses UTC.
        *
        * @return true if the time value is a UTC value
        */
        virtual bool isUTC() const;

        /**
        * Determine the timezone's hour offset value.
        *
        * @return the hour offset of the timezeone, or zero if there is no
        *         explicit timezone or the time is UTC
        */
        virtual int32_t getHourOffset() const;

        /**
        * Determine the timezone's minute offset value.
        *
        * @return the minute offset of the timezeone, or zero if there is no
        *         explicit timezone or the time is UTC
        */
        virtual int32_t getMinuteOffset() const;

        /**
        * Convert the RawTime to a POSIX datetime struct.
        *
        * Note that the nanosecond and timezone related information are not
        * preserved.
        *
        */
        virtual operator struct tm() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The hour number.
        */
        int32_t m_nHour;

        /**
        * The minute number.
        */
        int32_t m_nMinute;

        /**
        * The second number.
        */
        int32_t m_nSecond;

        /**
        * The nanosecond number.
        */
        int32_t m_nNano;

        /**
        * The timezone indicator, one of the TZ_ enumerated constants.
        */
        int32_t m_nTimeZoneType;

        /**
        * The hour offset of the time's timezone.
        */
        int32_t m_nHourOffset;

        /**
        * The minute offset of the time's timezone.
        */
        int32_t m_nMinuteOffset;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RAW_TIME_HPP
