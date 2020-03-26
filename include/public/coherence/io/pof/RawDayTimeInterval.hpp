/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RAW_DAY_TIME_INTERVAL_HPP
#define COH_RAW_DAY_TIME_INTERVAL_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofIntrinsic.hpp"



COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* Raw immutable POF day-time interval value.
*
* @author jh  2008.04.08
*/
class COH_EXPORT RawDayTimeInterval
    : public cloneable_spec<RawDayTimeInterval,
          extends<Object>,
          implements<PofIntrinsic> >
    {
    friend class factory<RawDayTimeInterval>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a raw POF day-time interval value.
        *
        * @param cDays     the number of days in the day-time interval
        * @param cHours    the number of hours in the day-time interval
        * @param cMinutes  the number of minutes in the day-time interval
        * @param cSeconds  the number of seconds in the day-time interval
        * @param cNanos    the number of nanoseconds in the day-time interval
        *
        * @return the new RawDayTimeInterval
        */
        RawDayTimeInterval(int32_t cDays, int32_t cHours, int32_t cMinutes,
                int32_t cSeconds, int32_t cNanos);

        /**
        * Copy constructor.
        */
        RawDayTimeInterval(const RawDayTimeInterval& that);


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
        * Determine the number of days in the day-time interval.
        *
        * @return the number of days in the day-time interval
        */
        virtual int32_t getDays() const;

        /**
        * Determine the number of hours in the day-time interval.
        *
        * @return the number of hours in the day-time interval
        */
        virtual int32_t getHours() const;

        /**
        * Determine the number of minutes in the day-time interval.
        *
        * @return the number of minutes in the day-time interval
        */
        virtual int32_t getMinutes() const;

        /**
        * Determine the number of seconds in the day-time interval.
        *
        * @return the number of seconds in the day-time interval
        */
        virtual int32_t getSeconds() const;

        /**
        * Determine the number of nanoseconds in the day-time interval.
        *
        * @return the number of nanoseconds in the day-time interval
        */
        virtual int32_t getNanos() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of days in the day-time interval.
        */
        int32_t m_cDays;

        /**
        * The number of hours in the day-time interval.
        */
        int32_t m_cHours;

        /**
        * The number of minutes in the day-time interval.
        */
        int32_t m_cMinutes;

        /**
        * The number of seconds in the day-time interval.
        */
        int32_t m_cSeconds;

        /**
        * The number of nanoseconds in the day-time interval.
        */
        int32_t m_cNanos;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RAW_DAY_TIME_INTERVAL_HPP
