/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_VOLATILE_HPP
#define COH_VOLATILE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/native/NativeAtomic32.hpp"
#include "coherence/native/NativeAtomic64.hpp"


COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeAtomic32;
using coherence::native::NativeAtomic64;

template<class T>
class VolatileStorage
    {
    public:
        typedef int64_t cast_type;
        typedef NativeAtomic64 storage;
    };

#define COH_DEFINE_VOLATILE_STORAGE(TYPE, WIDTH) \
template<> \
class VolatileStorage<TYPE> \
    { \
    public: \
        typedef int##WIDTH##_t cast_type; \
        typedef NativeAtomic##WIDTH storage; \
    }

COH_DEFINE_VOLATILE_STORAGE(bool,        32);
COH_DEFINE_VOLATILE_STORAGE(char,        32);
//COH_DEFINE_VOLATILE_STORAGE(octet_t,   32); // avoid duplicate instantiation
//COH_DEFINE_VOLATILE_STORAGE(wchar16_t, 32); // avoid duplicate instantiation
//COH_DEFINE_VOLATILE_STORAGE(int8_t,    32); // avoid duplicate instantiation
//COH_DEFINE_VOLATILE_STORAGE(uint8_t,   32); // avoid duplicate instantiation
COH_DEFINE_VOLATILE_STORAGE(int16_t,     32);
COH_DEFINE_VOLATILE_STORAGE(uint16_t,    32);
COH_DEFINE_VOLATILE_STORAGE(int32_t,     32);
COH_DEFINE_VOLATILE_STORAGE(uint32_t,    32);
COH_DEFINE_VOLATILE_STORAGE(int64_t,     64);
COH_DEFINE_VOLATILE_STORAGE(uint64_t,    64);
COH_DEFINE_VOLATILE_STORAGE(float32_t,   64);
COH_DEFINE_VOLATILE_STORAGE(float64_t ,  64);
//COH_DEFINE_VOLATILE_STORAGE(size32_t,  32); // avoid duplicate instantiation
//COH_DEFINE_VOLATILE_STORAGE(size64_t,  64); // avoid duplicate instantiation

/**
* Template class wraps primitive data types with memory barriers, providing
* JSR-133 style volatiles.  Note that Weak|MemberHandle/View/Holder are naturally
* volatile and thus don't need to be supported by this class.
*
* Note: In the rare case that a Volatile is declared via the mutable
*       keyword, the Volatile must be informed of this fact by setting
*       fMutable to true during construction.
*
* @author mf  2008.02.07
*/
template<class T>
class COH_EXPORT Volatile
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * A primitive data type that is wrapped by this Volatile<T>
        */
        typedef T Type;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a new Volatile<T> with the default wrapped T value.
        *
        * @param oGuardian  the object which this data member is part of
        */
        Volatile(Object& oGuardian);

        /**
        * Copy-construct a Volatile<T>.
        *
        * @param oGuardian  the object which this data member is part of
        */
        Volatile(Object& oGuardian, const Volatile& o);

        /**
        * Copy-construct a Volatile<T>.
        *
        * @param oGuardian  the object which this data member is part of
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        Volatile(Object& oGuardian, const Volatile& o, bool fMutable);

        /**
        * Construct a new Volatile<T> with the specified wrapped T value.
        *
        * @param oGuardian  the object which this data member is part of
        */
        Volatile(Object& oGuardian, const T& value);

        /**
        * Construct a new Volatile<T> with the specified wrapped T value.
        *
        * @param oGuardian  the object which this data member is part of
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        Volatile(Object& oGuardian, const T& value, bool fMutable);

        /**
        * Destruct this Volatile<T> object.
        */
        ~Volatile();


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Assign this Volatile the value from another Volatile
        */
        Volatile& operator=(const Volatile& value);

        /**
        * Assign this Volatile a new value.
        */
        Volatile& operator=(const T& value);


    // ----- Volatile interface --------------------------------------------

    public:
        /**
        * Return the Volatile value
        *
        * @return The Volatile value
        */
        operator const T() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The object which this data member is part of.
        */
        const Object& m_oGuardian;

        /**
        * The volatile value wrapped in a union of NativeAtomic64.
        */
        typename VolatileStorage<T>::storage m_atomic;
    };

// ----- global operators and functions -------------------------------------

/**
* Output a human-readable description of the given Volatile<T> to the
* specified stream.
*
* @param out  the stream used to output the description
* @param o    the Volatile<T> value to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
            const coherence::lang::Volatile<T>& o)
    {
    out << (T) o;
    return out;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_Volatile_HPP
