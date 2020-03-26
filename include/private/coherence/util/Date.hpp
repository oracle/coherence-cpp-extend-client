/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DATE_HPP
#define COH_DATE_HPP

#include "coherence/lang.ns"



COH_OPEN_NAMESPACE2(coherence,util)


/**
* Immutable class that encapsulates date/time information. The default output
* format for Date is "yyyy-MM-dd hh:mm:ss.fff"
*
* @author js  2008.08.12
*/
class COH_EXPORT Date
    : public class_spec<Date>
    {
    friend class factory<Date>;


    // ----- factory methods ------------------------------------------------

    public:

        /**
        * Create a new Date, in local time, for the current time.
        *
        * @return the new Date instance for the current time
        */
        static View create();

        /**
        * Create a new Date, in local time, from milliseconds.
        *
        * @param lMillis  the time in milliseconds for which to obtain the
        *                 Date
        *
        * @return the new Date instance for the given time
        */
        static View create(int64_t lMillis);

        /**
         * Create a new Date.
         *
         * @param nYear     the year
         * @param nMonth    the month        (1-12)
         * @param nDay      the day          (1-31, depending on month)
         * @param nHours    the hours        (0-23)
         * @param nMinutes  the minutes      (0-59)
         * @param nSeconds  the seconds      (0-61, extra range is for leap
         *                  seconds in certain systems)
         * @param nMillis   the milliseconds (0-999)
         *
         * @return a new Date
         */
        static View create(int32_t nYear, int32_t nMonth, int32_t nDay,
                int32_t nHours, int32_t nMinutes, int32_t nSeconds,
                int32_t nMillis);


    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Create a new Date.
         *
         * @param nYear     the year
         * @param nMonth    the month        (1-12)
         * @param nDay      the day          (1-31, depending on month)
         * @param nHours    the hours        (0-23)
         * @param nMinutes  the minutes      (0-59)
         * @param nSeconds  the seconds      (0-61, extra range is for leap
         *                  seconds in certain systems)
         * @param nMillis   the milliseconds (0-999)
         *
         * @return a new immutable Date
         */
        Date(int32_t nYear, int32_t nMonth, int32_t nDay, int32_t nHours,
                int32_t nMinutes, int32_t nSeconds, int32_t nMillis);


    // -----  accessors -----------------------------------------------------

    public:
        /**
        * Get the year.
        *
        * @return the year
        */
        virtual int32_t getYear() const;

        /**
        * Get the month (1-12).
        *
        * @return the month
        */
        virtual int32_t getMonth() const;

        /**
        * Get the day (1-31).
        *
        * @return the day
        */
        virtual int32_t getDay() const;

        /**
        * Get the hour (0-23).
        *
        * @return the hour
        */
        virtual int32_t getHours() const;

        /**
        * Get the minutes (0-59).
        *
        * @return the minutes
        */
        virtual int32_t getMinutes() const;

        /**
        * Get the seconds (0-61) Note: The extra range(60-61) is for leap
        * seconds in certain systems.  The normal reange is 0-59.
        *
        * @return the seconds
        */
        virtual int32_t getSeconds() const;

        /**
        * Get the milliseconds (0-999).
        *
        * @return the milliseconds
        */
        virtual int32_t getMillis() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Output the date/time in the format "yyyy-MM-dd hh:mm:ss.fff".
        *
        * @param out  the output stream
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * @inheritDoc
        */
        virtual size32_t hashCode() const;

        /**
        * @inheritDoc
        */
        virtual bool equals(Object::View v) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The year.
        */
        const int32_t m_nYear;

        /**
        * The month.
        */
        const int32_t m_nMonth;

        /**
        * The day.
        */
        const int32_t m_nDay;

        /**
        * The hours.
        */
        const int32_t m_nHours;

        /**
        * The minutes.
        */
        const int32_t m_nMinutes;

        /**
        * The seconds.
        */
        const int32_t m_nSeconds;

        /**
        * The milliseconds.
        */
        const int32_t m_nMillis;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DATE_HPP

