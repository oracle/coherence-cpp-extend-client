/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/Date.hpp"

#include "private/coherence/native/NativeTime.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::native::NativeTime;


// ----- factory methods ----------------------------------------------------

Date::View Date::create()
    {
    return NativeTime::instance()->createDate(System::currentTimeMillis());
    }

Date::View Date::create(int64_t lMillis)
    {
    return NativeTime::instance()->createDate(lMillis);
    }

Date::View Date::create(int32_t nYear, int32_t nMonth, int32_t nDay,
        int32_t nHours, int32_t nMinutes, int32_t nSeconds, int32_t nMillis)
    {
    return new Date(nYear, nMonth, nDay, nHours, nMinutes, nSeconds, nMillis);
    }


// ----- constructors -------------------------------------------------------

Date::Date(int32_t nYear, int32_t nMonth, int32_t nDay, int32_t nHours,
        int32_t nMinutes, int32_t nSeconds, int32_t nMillis)
        : m_nYear(nYear),
          m_nMonth(nMonth),
          m_nDay(nDay),
          m_nHours(nHours),
          m_nMinutes(nMinutes),
          m_nSeconds(nSeconds),
          m_nMillis(nMillis)
    {
    COH_ENSURE_PARAM_RELATION(nMonth, >=, 1);
    COH_ENSURE_PARAM_RELATION(nMonth, <=, 12);

    COH_ENSURE_PARAM_RELATION(nDay, >=, 1);
    COH_ENSURE_PARAM_RELATION(nDay, <=, 31);

    COH_ENSURE_PARAM_RELATION(nHours, >=, 0);
    COH_ENSURE_PARAM_RELATION(nHours, <=, 23);

    COH_ENSURE_PARAM_RELATION(nMinutes, >=, 0);
    COH_ENSURE_PARAM_RELATION(nMinutes, <=, 59);

    COH_ENSURE_PARAM_RELATION(nSeconds, >=, 0);
    COH_ENSURE_PARAM_RELATION(nSeconds, <=, 61);

    COH_ENSURE_PARAM_RELATION(nMillis, >=, 0);
    COH_ENSURE_PARAM_RELATION(nMillis, <=, 999);
    }


// ----- accessors ----------------------------------------------------------

int32_t Date::getYear() const
    {
    return m_nYear;
    }

int32_t Date::getMonth() const
    {
    return m_nMonth;
    }

int32_t Date::getDay() const
    {
    return m_nDay;
    }

int32_t Date::getHours() const
    {
    return m_nHours;
    }

int32_t Date::getMinutes() const
    {
    return m_nMinutes;
    }

int32_t Date::getSeconds() const
    {
    return m_nSeconds;
    }

int32_t Date::getMillis() const
    {
    return m_nMillis;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> Date::toString() const
    {
    int32_t nYear    = getYear();
    int32_t nMonth   = getMonth();
    int32_t nDay     = getDay();
    int32_t nHours   = getHours();
    int32_t nMinutes = getMinutes();
    int32_t nSeconds = getSeconds();
    int32_t nMillis  = getMillis();

    String::View vs = COH_TO_STRING(nYear << '-');
    if (nMonth < 10)
        {
        vs = COH_TO_STRING(vs << 0);
        }
    vs = COH_TO_STRING(vs << nMonth << '-');
    if (nDay < 10)
        {
        vs = COH_TO_STRING(vs << 0);
        }
    vs = COH_TO_STRING(vs << nDay << ' ');
    if (nHours < 10)
        {
        vs = COH_TO_STRING(vs << 0);
        }
    vs = COH_TO_STRING(vs << nHours << ':');
    if (nMinutes < 10)
        {
        vs = COH_TO_STRING(vs << 0);
        }
    vs = COH_TO_STRING(vs << nMinutes << ':');
    if (nSeconds < 10)
        {
        vs = COH_TO_STRING(vs << 0);
        }
    vs = COH_TO_STRING(vs << nSeconds << '.');
    if (nMillis < 10)
        {
        vs = COH_TO_STRING(vs << 0 << 0);
        }
    else if (nMillis < 100)
        {
        vs = COH_TO_STRING(vs << 0);
        }
    return vs = COH_TO_STRING(vs << nMillis);
    }

size32_t Date::hashCode() const
    {
    int32_t nHash = 7;

    nHash = 31 * nHash + m_nYear;
    nHash = 31 * nHash + m_nMonth;
    nHash = 31 * nHash + m_nDay;
    nHash = 31 * nHash + m_nHours;
    nHash = 31 * nHash + m_nMinutes;
    nHash = 31 * nHash + m_nSeconds;
    nHash = 31 * nHash + m_nMillis;

    return nHash;
    }

bool Date::equals(Object::View v) const
    {
    Date::View vThat = cast<Date::View>(v, false);

    if (NULL == vThat)
        {
        return false;
        }
    else if (this == vThat)
        {
        return true;
        }
    else
        {
        return m_nYear    == vThat->m_nYear    &&
               m_nMonth   == vThat->m_nMonth   &&
               m_nDay     == vThat->m_nDay     &&
               m_nHours   == vThat->m_nHours   &&
               m_nMinutes == vThat->m_nMinutes &&
               m_nSeconds == vThat->m_nSeconds &&
               m_nMillis  == vThat->m_nMillis;
        }
    }

COH_CLOSE_NAMESPACE2

