/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RAW_DATE_HPP
#define COH_RAW_DATE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofIntrinsic.hpp"


#include <time.h>

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* Raw immutable POF date value.
*
* @author jh  2008.04.08
*/
class COH_EXPORT RawDate
    : public cloneable_spec<RawDate,
          extends<Object>,
          implements<PofIntrinsic> >
    {
    friend class factory<RawDate>;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The boxed native type.
        */
        typedef struct tm BoxedType;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a raw POF date value.
        *
        * @param nYear   the year number as defined by ISO8601
        * @param nMonth  the month number between 1 and 12 inclusive as
        *                defined by ISO8601
        * @param nDay    the day number between 1 and 31 inclusive as defined
        *                by ISO8601
        *
        * @return the new RawDate
        */
        RawDate(int32_t nYear, int32_t nMonth, int32_t nDay);

        /**
        * Construct a raw POF date value from a POSIX datetime struct.
        */
        RawDate(const struct tm& timeinfo);

        /**
        * Copy constructor.
        */
        RawDate(const RawDate& that);


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
        * Determine the date's year value.
        *
        * @return the year number as defined by ISO8601
        */
        virtual int32_t getYear() const;

        /**
        * Determine the date's month value.
        *
        * @return the month number between 1 and 12 inclusive as defined by
        *         ISO8601
        */
        virtual int32_t getMonth() const;

        /**
        * Determine the date's day value.
        *
        * @return the day number between 1 and 31 inclusive as defined by
        *         ISO8601
        */
        virtual int32_t getDay() const;

        /**
        * Convert the RawDate to a POSIX datetime struct.
        */
        virtual operator struct tm() const;

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The year number.
        */
        int32_t m_nYear;

        /**
        * The month number.
        */
        int32_t m_nMonth;

        /**
        * The day number.
        */
        int32_t m_nDay;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RAW_DATE_HPP
