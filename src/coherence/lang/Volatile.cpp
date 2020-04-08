/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Volatile.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors -------------------------------------------------------

template<class T> Volatile<T>::Volatile(Object& oGuardian)
    : m_oGuardian(oGuardian), m_atomic(0, oGuardian._isEscaped())
    {
    }

template<class T> Volatile<T>::Volatile(Object& oGuardian, const Volatile& o)
    : m_oGuardian(oGuardian), m_atomic(o.m_atomic.get(), oGuardian._isEscaped())
    {
    }

// NOTE: fMutable is currently not used, it is a place holder for future
// potential optimizations, and provides symmetry with the other guardian
// based members
template<class T> Volatile<T>::Volatile(Object& oGuardian, const Volatile& o,
        bool /*fMutable*/)
    : m_oGuardian(oGuardian), m_atomic(o.m_atomic.get(), oGuardian._isEscaped())
    {
    }

template<class T> Volatile<T>::Volatile(Object& oGuardian, const T& value)
    : m_oGuardian(oGuardian), m_atomic((typename VolatileStorage<T>::cast_type) value, oGuardian._isEscaped())
    {
    }

// NOTE: fMutable is currently not used, it is a place holder for future
// potential optimizations, and provides symetry with the other guardian
// based members
template<class T> Volatile<T>::Volatile(Object& oGuardian, const T& value,
        bool /*fMutable*/)
    : m_oGuardian(oGuardian), m_atomic((typename VolatileStorage<T>::cast_type) value, oGuardian._isEscaped())
    {
    }

template<class T> Volatile<T>::~Volatile()
    {
    }


// ----- operators ----------------------------------------------------------

template<class T> Volatile<T>& Volatile<T>::operator=(const Volatile& value)
    {
    return operator=((T) value);
    }

template<class T> Volatile<T>& Volatile<T>::operator=(const T& value)
    {
    if (m_oGuardian._isEscaped())
        {
        m_atomic.set((typename VolatileStorage<T>::cast_type) value);
        }
    else
        {
        m_atomic.poke((typename VolatileStorage<T>::cast_type) value);
        }
    return *this;
    }

template<class T> Volatile<T>::operator const T() const
    {
    return (T) (m_oGuardian._isEscaped() ? m_atomic.get() : m_atomic.peek());
    }

template<> Volatile<bool>::operator const bool() const
    {
    // MSVC generates a performance warning when converting int32_t to bool
    // automatically
    return (m_oGuardian._isEscaped() ? m_atomic.get() : m_atomic.peek()) != 0;
    }


// ----- Volatile template instantiations -----------------------------------

#define COH_DEFINE_VOLATILE(type) \
    COH_STATIC_ASSERT(sizeof(type) <= sizeof(VolatileStorage<type>::cast_type)); \
    template class COH_EXPORT coherence::lang::Volatile<type>

COH_DEFINE_VOLATILE(bool);
COH_DEFINE_VOLATILE(char);
//COH_DEFINE_VOLATILE(octet_t);   // avoid duplicate instantiation
//COH_DEFINE_VOLATILE(wchar16_t); // avoid duplicate instantiation
//COH_DEFINE_VOLATILE(int8_t);    // avoid duplicate instantiation
//COH_DEFINE_VOLATILE(uint8_t);   // avoid duplicate instantiation
COH_DEFINE_VOLATILE(int16_t);
COH_DEFINE_VOLATILE(uint16_t);
COH_DEFINE_VOLATILE(int32_t);
COH_DEFINE_VOLATILE(uint32_t);
COH_DEFINE_VOLATILE(int64_t);
COH_DEFINE_VOLATILE(uint64_t);
COH_DEFINE_VOLATILE(float32_t);
COH_DEFINE_VOLATILE(float64_t);
//COH_DEFINE_VOLATILE(size32_t);  // avoid duplicate instantiation
//COH_DEFINE_VOLATILE(size64_t);  // avoid duplicate instantiation
COH_CLOSE_NAMESPACE2
