/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AtomicCounter.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::native::NativeAtomic64;


// ----- constructor --------------------------------------------------------

AtomicCounter::AtomicCounter(int64_t cValue)
    : m_atomic(cValue)
    {
    }

AtomicCounter::AtomicCounter(const AtomicCounter& that)
    : super(that), m_atomic(that.m_atomic.get())
    {
    }


// ----- AtomicCounter interface --------------------------------------------

int64_t AtomicCounter::increment(int64_t c)
    {
    return m_atomic.postAdjust(c, false) + c;
    }

int64_t AtomicCounter::postIncrement(int64_t c)
    {
    return m_atomic.postAdjust(c, false);
    }

int64_t AtomicCounter::decrement(int64_t c)
    {
    return m_atomic.postAdjust(-c, false) - c;
    }

int64_t AtomicCounter::postDecrement(int64_t c)
    {
    return m_atomic.postAdjust(-c, false);
    }

int64_t AtomicCounter::getCount() const
    {
    return m_atomic.get();
    }

int64_t AtomicCounter::update(int64_t cAssume, int64_t cNew)
    {
    return m_atomic.update(cAssume, cNew);
    }

int64_t AtomicCounter::setCount(int64_t cNew)
    {
    // TODO NSA - Once NativeAtomic64::set has been updated to use the xchg
    // functionality, this method should be updated to take advantage of it.

    int64_t lAssume;
    int64_t lActual = m_atomic.peek();

    do
        {
        lAssume = lActual;
        lActual = m_atomic.update(lAssume, cNew);
        }
    while (lAssume != lActual);

    return lActual;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> AtomicCounter::toString() const
    {
    return COH_TO_STRING(m_atomic.get());
    }

COH_CLOSE_NAMESPACE2
