/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/RawDate.hpp"

#include "coherence/io/pof/PofHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

COH_REGISTER_CLASS(TypedBarrenClass<RawDate>::create());


// ----- constructors -------------------------------------------------------

RawDate::RawDate(int32_t nYear, int32_t nMonth, int32_t nDay)
    : m_nYear(nYear), m_nMonth(nMonth), m_nDay(nDay)
    {
    PofHelper::checkDate(nYear, nMonth, nDay);
    }

RawDate::RawDate(const struct tm& timeinfo)
    : m_nYear((int32_t) timeinfo.tm_year + 1900),
      m_nMonth((int32_t) timeinfo.tm_mon + 1),
      m_nDay((int32_t) timeinfo.tm_mday)
    {
    }

RawDate::RawDate(const RawDate& that)
    : super(that), m_nYear(that.m_nYear), m_nMonth(that.m_nMonth),
      m_nDay(that.m_nDay)
    {
    }


// ----- Object interface ---------------------------------------------------

bool RawDate::equals(Object::View v) const
    {
    RawDate::View vThat = cast<RawDate::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return this == vThat ||
              (this->getYear()  == vThat->getYear()
            && this->getMonth() == vThat->getMonth()
            && this->getDay()   == vThat->getDay());
    }

size32_t RawDate::hashCode() const
    {
    return (getYear() << 2) ^ (getMonth() << 1) ^ getDay();
    }

bool RawDate::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> RawDate::toString() const
    {
    return COH_TO_STRING(PofHelper::formatDate(getYear(), getMonth(), getDay()));
    }


// ----- accessors ----------------------------------------------------------

int32_t RawDate::getYear() const
    {
    return m_nYear;
    }

int32_t RawDate::getMonth() const
    {
    return m_nMonth;
    }

int32_t RawDate::getDay() const
    {
    return m_nDay;
    }

RawDate::operator struct tm() const
    {
    struct tm timeinfo = {0,}; // zero'd out date
    timeinfo.tm_year   = m_nYear  - 1900;
    timeinfo.tm_mon    = m_nMonth - 1;
    timeinfo.tm_mday   = m_nDay;

    // utilize mktime to fill in tm_wday and tm_yday fields
    mktime(&timeinfo);

    return timeinfo;
    }

COH_CLOSE_NAMESPACE3
