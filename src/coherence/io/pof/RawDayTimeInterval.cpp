/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/RawDayTimeInterval.hpp"

#include "coherence/io/pof/PofHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

COH_REGISTER_CLASS(TypedBarrenClass<RawDayTimeInterval>::create());


// ----- constructors -------------------------------------------------------

RawDayTimeInterval::RawDayTimeInterval(int32_t cDays, int32_t cHours,
        int32_t cMinutes, int32_t cSeconds, int32_t cNanos)
    : m_cDays(cDays),
      m_cHours(cHours),
      m_cMinutes(cMinutes),
      m_cSeconds(cSeconds),
      m_cNanos(cNanos)
    {
    PofHelper::checkDayTimeInterval(cDays, cHours, cMinutes, cSeconds, cNanos);
    }

RawDayTimeInterval::RawDayTimeInterval(const RawDayTimeInterval& that)
    : super(that),
      m_cDays(that.m_cDays),
      m_cHours(that.m_cHours),
      m_cMinutes(that.m_cMinutes),
      m_cSeconds(that.m_cSeconds),
      m_cNanos(that.m_cNanos)
    {
    }


// ----- Object interface ---------------------------------------------------

bool RawDayTimeInterval::equals(Object::View v) const
    {
    RawDayTimeInterval::View vThat = cast<RawDayTimeInterval::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return this == vThat ||
              (this->getDays()    == vThat->getDays()
            && this->getHours()   == vThat->getHours()
            && this->getMinutes() == vThat->getMinutes()
            && this->getSeconds() == vThat->getSeconds()
            && this->getNanos()   == vThat->getNanos());
    }

size32_t RawDayTimeInterval::hashCode() const
    {
    return (getDays() << 3) ^ (getHours() << 2) ^ (getMinutes() << 1)
            ^ getSeconds() ^ getNanos();
    }

bool RawDayTimeInterval::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> RawDayTimeInterval::toString() const
    {
    return COH_TO_STRING(
             "Days="    << getDays()
        << ", Hours="   << getHours()
        << ", Minutes=" << getMinutes()
        << ", Seconds=" << getSeconds()
        << ", Nanos="   << getNanos());
    }


// ----- accessors ----------------------------------------------------------

int32_t RawDayTimeInterval::getDays() const
    {
    return m_cDays;
    }

int32_t RawDayTimeInterval::getHours() const
    {
    return m_cHours;
    }

int32_t RawDayTimeInterval::getMinutes() const
    {
    return m_cMinutes;
    }

int32_t RawDayTimeInterval::getSeconds() const
    {
    return m_cSeconds;
    }

int32_t RawDayTimeInterval::getNanos() const
    {
    return m_cNanos;
    }

COH_CLOSE_NAMESPACE3
