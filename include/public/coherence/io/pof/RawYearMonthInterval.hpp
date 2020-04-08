/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RAW_YEAR_MONTH_INTERVAL_HPP
#define COH_RAW_YEAR_MONTH_INTERVAL_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofIntrinsic.hpp"



COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* Raw POF year-month interval value.
*
* @author jh  2008.04.08
*/
class COH_EXPORT RawYearMonthInterval
    : public cloneable_spec<RawYearMonthInterval,
          extends<Object>,
          implements<PofIntrinsic> >
    {
    friend class factory<RawYearMonthInterval>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a raw POF year-month interval value.
        *
        * @param cYears  the number of years in the year-month interval
        * @param cMonths the number of months in the year-month interval
        *
        * @return the new RawYearMonthInterval
        */
        RawYearMonthInterval(int32_t cYears, int32_t cMonths);

        /**
        * Copy constructor.
        */
        RawYearMonthInterval(const RawYearMonthInterval& that);


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
        * Determine the number of years in the year-month interval.
        *
        * @return the number of years in the year-month interval
        */
        virtual int32_t getYears() const;

        /**
        * Determine the number of months in the year-month interval.
        *
        * @return the number of months in the year-month interval
        */
        virtual int32_t getMonths() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of years in the year-month interval.
        */
        int32_t m_cYears;

        /**
        * The number of months in the year-month interval.
        */
        int32_t m_cMonths;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RAW_YEAR_MONTH_INTERVAL_HPP
