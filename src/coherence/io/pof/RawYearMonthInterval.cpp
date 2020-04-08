/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/RawYearMonthInterval.hpp"

#include "coherence/io/pof/PofHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

COH_REGISTER_CLASS(TypedBarrenClass<RawYearMonthInterval>::create());


// ----- constructors -------------------------------------------------------

RawYearMonthInterval::RawYearMonthInterval(int32_t cYears, int32_t cMonths)
    : m_cYears(cYears), m_cMonths(cMonths)
    {
    PofHelper::checkYearMonthInterval(cYears, cMonths);
    }

RawYearMonthInterval::RawYearMonthInterval(const RawYearMonthInterval& that)
    : super(that), m_cYears(that.m_cYears), m_cMonths(that.m_cMonths)
    {
    }


// ----- Object interface ---------------------------------------------------

bool RawYearMonthInterval::equals(Object::View v) const
    {
    RawYearMonthInterval::View vThat = cast<RawYearMonthInterval::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return this == vThat ||
              (this->getYears()  == vThat->getYears()
            && this->getMonths() == vThat->getMonths());
    }

size32_t RawYearMonthInterval::hashCode() const
    {
    return getYears() ^ getMonths();
    }

bool RawYearMonthInterval::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> RawYearMonthInterval::toString() const
    {
    return COH_TO_STRING("Years=" << getYears() << ", Months=" << getMonths());
    }


// ----- accessors ----------------------------------------------------------

int32_t RawYearMonthInterval::getYears() const
    {
    return m_cYears;
    }

int32_t RawYearMonthInterval::getMonths() const
    {
    return m_cMonths;
    }

COH_CLOSE_NAMESPACE3
