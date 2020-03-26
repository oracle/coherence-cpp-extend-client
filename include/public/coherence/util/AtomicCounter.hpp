/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ATOMIC_COUNTER_HPP
#define COH_ATOMIC_COUNTER_HPP

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic64.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::native::NativeAtomic64;


/**
* AtomicCounter allows for atomic updates to a "int64_t" value where possible
* without requiring synchronization.
*
* @author nsa 12.28.2007
*/
class COH_EXPORT AtomicCounter
    : public cloneable_spec<AtomicCounter>
    {
    friend class factory<AtomicCounter>;

    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create an AtomicCounter initialized with a specified value.
        *
        * @param cValue  the initial value of the Atomic64
        */
        AtomicCounter(int64_t cValue = 0);

        /**
        * Copy constructor.
        */
        AtomicCounter(const AtomicCounter& that);


    // ----- AtomicCounter interface ----------------------------------------

    public:
        /**
        * Incremenet the value by c, and return the new value.
        *
        * @param c  the amount to increment the counter by
        *
        * @return the new value
        */
        virtual int64_t increment(int64_t c = 1);

        /**
        * Incremenet the value by c, and return the original value.
        *
        * @param c  the amount to increment the counter by
        *
        * @return the original value
        */
        virtual int64_t postIncrement(int64_t c = 1);

        /**
        * Decrement the value by c, and return the new value.
        *
        * @param c  the amount to decrement the counter by
        *
        * @return the new value
        */
        virtual int64_t decrement(int64_t c = 1);

        /**
        * Decrement the value by c, and return the original value.
        *
        * @param c  the amount to decrement the counter by
        *
        * @return the original value
        */
        virtual int64_t postDecrement(int64_t c = 1);

        /**
        * Return the current value of the counter.
        *
        * @return the current value
        */
        virtual int64_t getCount() const;

        /**
        * Update the current value, only if it is equal to the assumed value.
        *
        * @param cAssume  the assumed old value
        * @param cNew     the new value
        *
        * @return the prior actual value, if the returned value does is not
        *         equal to the supplied assumed value then update did not take
        *         place
        */
        virtual int64_t update(int64_t cAssume, int64_t cNew);

        /**
        * Update the current value, and return the previous value.
        *
        * @param cNew  the new value
        *
        * @return the previous value just before the update went through
        */
        virtual int64_t setCount(int64_t cNew);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying native implementation.
        */
        NativeAtomic64 m_atomic;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ATOMIC_COUNTER_HPP
