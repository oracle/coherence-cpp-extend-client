/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RAW_TIME_INTERVAL_HPP
#define COH_RAW_TIME_INTERVAL_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofIntrinsic.hpp"



COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* Raw immutable POF time interval value.
*
* @author jh  2008.04.08
*/
class COH_EXPORT RawTimeInterval
    : public cloneable_spec<RawTimeInterval,
          extends<Object>,
          implements<PofIntrinsic> >
    {
    friend class factory<RawTimeInterval>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a raw POF time interval value.
        *
        * @param cHours    the number of hours in the time interval
        * @param cMinutes  the number of minutes in the time interval
        * @param cSeconds  the number of seconds in the time interval
        * @param cNanos    the number of nanoseconds in the time interval
        *
        * @return the new RawTimeInterval
        */
        RawTimeInterval(int32_t cHours, int32_t cMinutes, int32_t cSeconds,
                int32_t cNanos);

        /**
        * Copy constructor.
        */
        RawTimeInterval(const RawTimeInterval& that);


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
        * Determine the number of hours in the time interval.
        *
        * @return the number of hours in the time interval
        */
        virtual int32_t getHours() const;

        /**
        * Determine the number of minutes in the time interval.
        *
        * @return the number of minutes in the time interval
        */
        virtual int32_t getMinutes() const;

        /**
        * Determine the number of seconds in the time interval.
        *
        * @return the number of seconds in the time interval
        */
        virtual int32_t getSeconds() const;

        /**
        * Determine the number of nanoseconds in the time interval.
        *
        * @return the number of nanoseconds in the time interval
        */
        virtual int32_t getNanos() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of hours in the time interval.
        */
        int32_t m_cHours;

        /**
        * The number of minutes in the time interval.
        */
        int32_t m_cMinutes;

        /**
        * The number of seconds in the time interval.
        */
        int32_t m_cSeconds;

        /**
        * The number of nanoseconds in the time interval.
        */
        int32_t m_cNanos;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RAW_TIME_INTERVAL_HPP
