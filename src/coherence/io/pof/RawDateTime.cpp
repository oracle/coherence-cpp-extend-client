/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/RawDateTime.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

COH_REGISTER_CLASS(TypedBarrenClass<RawDateTime>::create());


// ----- constructors -------------------------------------------------------

RawDateTime::RawDateTime(RawDate::View vDate, RawTime::View vTime)
        : f_vDate(self(), vDate), f_vTime(self(), vTime)
    {
    if (NULL == vDate || NULL == vTime)
        {
        COH_THROW (IllegalArgumentException::create("date and time required"));
        }
    }

RawDateTime::RawDateTime(const struct tm& tm, bool fUTC)
    : f_vDate(self(), RawDate::create(tm)),
      f_vTime(self(), RawTime::create(tm, fUTC))
    {
    }

RawDateTime::RawDateTime(const RawDateTime& that)
    : super(that), f_vDate(self(), cast<RawDate::View>(that.f_vDate->clone())),
      f_vTime(self(), cast<RawTime::View>(that.f_vTime->clone()))
    {
    }


// ----- Object interface ---------------------------------------------------

bool RawDateTime::equals(Object::View v) const
    {
    RawDateTime::View vThat = cast<RawDateTime::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return this == vThat ||
              (this->getRawDate()->equals(vThat->getRawDate())
            && this->getRawTime()->equals(vThat->getRawTime()));
    }

size32_t RawDateTime::hashCode() const
    {
    return getRawDate()->hashCode() ^ getRawTime()->hashCode();
    }

bool RawDateTime::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> RawDateTime::toString() const
    {
    return COH_TO_STRING(getRawDate() << ' ' << getRawTime());
    }


// ----- accessors ----------------------------------------------------------

RawDate::View RawDateTime::getRawDate() const
    {
    return f_vDate;
    }

RawTime::View RawDateTime::getRawTime() const
    {
    return f_vTime;
    }

RawDateTime::operator struct tm() const
    {
    struct tm tmDateTime = *f_vTime;
    struct tm tmDate     = *f_vDate;

    // transfer over date fields
    tmDateTime.tm_year = tmDate.tm_year;
    tmDateTime.tm_mon  = tmDate.tm_mon;
    tmDateTime.tm_mday = tmDate.tm_mday;
    tmDateTime.tm_wday = tmDate.tm_wday;
    tmDateTime.tm_yday = tmDate.tm_yday;

    return tmDateTime;
    }

COH_CLOSE_NAMESPACE3
