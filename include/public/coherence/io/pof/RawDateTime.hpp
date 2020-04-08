/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RAW_DATE_TIME_HPP
#define COH_RAW_DATE_TIME_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofIntrinsic.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawTime.hpp"


#include <time.h>

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* Raw immutable POF date-time value.
*
* @author jh  2008.04.08
*/
class COH_EXPORT RawDateTime
    : public cloneable_spec<RawDateTime,
          extends<Object>,
          implements<PofIntrinsic> >
    {
    friend class factory<RawDateTime>;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The boxed native type.
        */
        typedef struct tm BoxedType;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a date-time value.
        *
        * @param vDate  the date portion of the raw date-time value
        * @param vTime  the time portion of the raw date-time value
        *
        * @return the new RawDateTime
        */
        RawDateTime(RawDate::View vDate, RawTime::View vTime);

        /**
        * Construct a raw POF datetime value from a POSIX datetime struct.
        *
        * Note that the POSIX daylight savings flag is not preserved.
        *
        * @param timeinfo  the POSIX datetime
        * @param fUTC      true if the time value is UTC or false if the time
        *                  value does not have an explicit time zone
        *
        */
        RawDateTime(const struct tm& timeinfo, bool fUTC = false);

        /**
        * Copy constructor.
        */
        RawDateTime(const RawDateTime& that);


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
        * Obtain the date portion of the raw date-time value.
        *
        * @return the date portion of the raw date-time value
        */
        virtual RawDate::View getRawDate() const;

        /**
        * Obtain the time portion of the raw date-time value.
        *
        * @return the time portion of the raw date-time value
        */
        virtual RawTime::View getRawTime() const;

        /**
        * Convert the RawDateTime to a POSIX datetime struct.
        *
        * Note that RawTime's nanosecond and timezone related information are
        * not preserved.
        */
        virtual operator struct tm() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The date portion of the raw date-time value.
        */
        FinalView<RawDate> f_vDate;

        /**
        * The time portion of the raw date-time value.
        */
        FinalView<RawTime> f_vTime;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RAW_DATE_TIME_HPP
