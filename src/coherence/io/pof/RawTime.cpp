/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/RawTime.hpp"

#include "coherence/io/pof/PofHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

COH_REGISTER_CLASS(TypedBarrenClass<RawTime>::create());


// ----- file local helpers -------------------------------------------------

namespace
    {
    typedef enum
        {
        TZ_NONE   = 0, // the time value does not have an explicit time zone
        TZ_UTC    = 1, // the time value is in UTC
        TZ_OFFSET = 2  // the time value has an explicit time zone
        } TimeZone;
    }


// ----- constructors -------------------------------------------------------

RawTime::RawTime(int32_t nHour, int32_t nMinute, int32_t nSecond,
        int32_t nNano, bool fUTC)
        : m_nHour(nHour),
          m_nMinute(nMinute),
          m_nSecond(nSecond),
          m_nNano(nNano),
          m_nTimeZoneType(fUTC ? TZ_UTC : TZ_NONE),
          m_nHourOffset(0),
          m_nMinuteOffset(0)
    {
    PofHelper::checkTime(nHour, nMinute, nSecond, nNano);
    }

RawTime::RawTime(int32_t nHour, int32_t nMinute, int32_t nSecond,
        int32_t nNano, int32_t nHourOffset, int32_t nMinuteOffset)
    : m_nHour(nHour),
      m_nMinute(nMinute),
      m_nSecond(nSecond),
      m_nNano(nNano),
      m_nTimeZoneType(TZ_OFFSET),
      m_nHourOffset(nHourOffset),
      m_nMinuteOffset(nMinuteOffset)
    {
    PofHelper::checkTime(nHour, nMinute, nSecond, nNano);
    PofHelper::checkTimeZone(nHourOffset, nMinuteOffset);
    }

RawTime::RawTime(const struct tm& timeinfo, bool fUTC)
    : m_nHour((int32_t) timeinfo.tm_hour),
      m_nMinute((int32_t) timeinfo.tm_min),
      m_nSecond((int32_t) timeinfo.tm_sec),
      m_nNano(0),
      m_nTimeZoneType(fUTC ? TZ_UTC : TZ_NONE),
      m_nHourOffset(0),
      m_nMinuteOffset(0)
    {
    PofHelper::checkTime(m_nHour, m_nMinute, m_nSecond, 0);
    }

RawTime::RawTime(const RawTime& that)
    : super(that),
      m_nHour(that.m_nHour),
      m_nMinute(that.m_nMinute),
      m_nSecond(that.m_nSecond),
      m_nNano(that.m_nNano),
      m_nTimeZoneType(that.m_nTimeZoneType),
      m_nHourOffset(that.m_nHourOffset),
      m_nMinuteOffset(that.m_nMinuteOffset)
    {
    }


// ----- Object interface ---------------------------------------------------

bool RawTime::equals(Object::View v) const
    {
    RawTime::View vThat = cast<RawTime::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return this == vThat ||
              (this->getHour()         == vThat->getHour()
            && this->getMinute()       == vThat->getMinute()
            && this->getSecond()       == vThat->getSecond()
            && this->getNano()         == vThat->getNano()
            && this->isUTC()           == vThat->isUTC()
            && this->getHourOffset()   == vThat->getHourOffset()
            && this->getMinuteOffset() == vThat->getMinuteOffset());
    }

size32_t RawTime::hashCode() const
    {
    return (getHour() << 2) ^ (getMinute() << 1) ^ getSecond() ^ getNano();
    }

bool RawTime::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> RawTime::toString() const
    {
    if (hasTimezone() && !isUTC())
        {
        return COH_TO_STRING(PofHelper::formatTime(getHour(), getMinute(), getSecond(),
                getNano(), getHourOffset(), getMinuteOffset()));
        }
    else
        {
        return COH_TO_STRING(PofHelper::formatTime(getHour(), getMinute(), getSecond(),
                getNano(), isUTC()));
        }
    }


// ----- accessors ----------------------------------------------------------

int32_t RawTime::getHour() const
    {
    return m_nHour;
    }

int32_t RawTime::getMinute() const
    {
    return m_nMinute;
    }

int32_t RawTime::getSecond() const
    {
    return m_nSecond;
    }

int32_t RawTime::getNano() const
    {
    return m_nNano;
    }

bool RawTime::hasTimezone() const
    {
    return m_nTimeZoneType != TZ_NONE;
    }

bool RawTime::isUTC() const
    {
    return m_nTimeZoneType == TZ_UTC;
    }

int32_t RawTime::getHourOffset() const
    {
    return m_nHourOffset;
    }

int32_t RawTime::getMinuteOffset() const
    {
    return m_nMinuteOffset;
    }

RawTime::operator struct tm() const
    {
    struct tm timeinfo = {0,}; // zero'd out date
    timeinfo.tm_hour   = m_nHour;
    timeinfo.tm_min    = m_nMinute;
    timeinfo.tm_sec    = m_nSecond;
    timeinfo.tm_isdst  = -1; // indicates that this info is unknown
    return timeinfo;
    }

COH_CLOSE_NAMESPACE3
