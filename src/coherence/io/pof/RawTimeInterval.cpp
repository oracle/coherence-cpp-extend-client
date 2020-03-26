/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/RawTimeInterval.hpp"

#include "coherence/io/pof/PofHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

COH_REGISTER_CLASS(TypedBarrenClass<RawTimeInterval>::create());


// ----- constructors -------------------------------------------------------

RawTimeInterval::RawTimeInterval(int32_t cHours, int32_t cMinutes,
        int32_t cSeconds, int32_t cNanos)
    : m_cHours(cHours),
      m_cMinutes(cMinutes),
      m_cSeconds(cSeconds),
      m_cNanos(cNanos)
    {
    PofHelper::checkTimeInterval(cHours, cMinutes, cSeconds, cNanos);
    }

RawTimeInterval::RawTimeInterval(const RawTimeInterval& that)
    : super(that),
      m_cHours(that.m_cHours),
      m_cMinutes(that.m_cMinutes),
      m_cSeconds(that.m_cSeconds),
      m_cNanos(that.m_cNanos)
    {
    }


// ----- Object interface ---------------------------------------------------

bool RawTimeInterval::equals(Object::View v) const
    {
    RawTimeInterval::View vThat = cast<RawTimeInterval::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return this == vThat ||
              (this->getHours()   == vThat->getHours()
            && this->getMinutes() == vThat->getMinutes()
            && this->getSeconds() == vThat->getSeconds()
            && this->getNanos()   == vThat->getNanos());
    }

size32_t RawTimeInterval::hashCode() const
    {
    return (getHours() << 2) ^ (getMinutes() << 1) ^ getSeconds() ^ getNanos();
    }

bool RawTimeInterval::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> RawTimeInterval::toString() const
    {
    return COH_TO_STRING("Hours="     << getHours()
        << ", Minutes=" << getMinutes()
        << ", Seconds=" << getSeconds()
        << ", Nanos="   << getNanos());
    }


// ----- accessors ----------------------------------------------------------

int32_t RawTimeInterval::getHours() const
    {
    return m_cHours;
    }

int32_t RawTimeInterval::getMinutes() const
    {
    return m_cMinutes;
    }

int32_t RawTimeInterval::getSeconds() const
    {
    return m_cSeconds;
    }

int32_t RawTimeInterval::getNanos() const
    {
    return m_cNanos;
    }

COH_CLOSE_NAMESPACE3
