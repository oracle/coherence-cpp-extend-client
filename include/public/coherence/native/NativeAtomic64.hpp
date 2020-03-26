/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_ATOMIC64_HPP
#define COH_NATIVE_ATOMIC64_HPP

#include "coherence/lang/compatibility.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


/**
* @internal
*
* 64-bit atomic integer, providing JSR-133 volatile memory semantics.
*
* @author mf 2007.12.01
*/
class COH_EXPORT NativeAtomic64
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create a new Atomic64, as Atomic64 is not an Object
        * it is the responsibility of the caller to eventually
        * destroy this object.
        *
        * @param lValue   the initial value
        * @param fAtomic  true if the initial assignment needs synchronization
        *
        * @return a pointer to the new Atomic64
        */
        NativeAtomic64(int64_t lValue = 0, bool fAtomic = true)
            {
            if (fAtomic)
                {
                set(lValue);
                }
            else
                {
                poke(lValue);
                }
            }


    // ----- Atomic64 interface ---------------------------------------------

    public:
        /**
        * Return the current value.
        *
        * @return the current value
        */
        int64_t get() const;

        /**
        * Return the assumed value without performing any memory
        * synchronization. This value should not be trusted, but is suitable
        * as an assumed value to be passed to update, which will validate if
        * it is correct before allowing any associated change to occur.
        *
        * @return the assumed value
        */
        int64_t peek() const
            {
            return m_lAtomic;
            }

        /**
        * Unconditionally set the value in a thread-safe manor.
        *
        * @param  lValue the new value
        */
        void set(int64_t lValue);

        /**
        * Unconditionally (and in a non thread-safe manor) set the value.
        *
        * @param  lValue the new value
        */
        void poke(int64_t lValue)
            {
            m_lAtomic = lValue;
            }

        /**
        * Set the value so long as the current value matches the expected value.
        *
        * @param lAssume  the expected current value
        * @param lValue   the new value
        *
        * @return the prior actual value, if the returned value
        *         does is not equal to the supplied assumed
        *         value then update did not take place
        */
        int64_t update(int64_t lAssume, int64_t lValue);

        /**
        * Set the value so long as the current value matches the expected value.
        *
        * This version of the update method supports conditionally performing
        * the update in an unsafe manor. The unsafe update is performed
        * without any form of synchronization and should only be used when it
        * is known that the object is referenced by a single thread.
        *
        * @param lAssume  the expected current value
        * @param lValue   the new value
        * @param fAtomic  perform a safe update operation which can be used
        *                 across threads
        *
        * @return the prior actual value, if the returned value
        *         does is not equal to the supplied assumed
        *         value then update did not take place
        */
        int64_t update(int64_t lAssume, int64_t lValue, bool fAtomic)
            {
            if (fAtomic)
                {
                return update(lAssume, lValue);
                }

            int64_t lOld = peek();
            if (lOld == lAssume)
                {
                poke(lValue);
                }
            return lOld;
            }

        /**
        * Adjust the NativeAtomic64 by the specified value.
        *
        * @param c      the amount to adjust the atomic by
        * @param fSafe  to provide overflow protection
        *
        * @return the old value
        */
        int64_t postAdjust(int64_t c, bool fSafe = true)
            {
            int64_t cAssume;
            int64_t cActual = peek();
            do
                {
                cAssume = cActual;
                if (fSafe &&
                    ((c > 0 && cAssume > max_value - c) ||
                     (c < 0 && cAssume < min_value - c )))
                    {
                    // would overflow
                    coh_throw_illegal_argument("NativeAtomic64 overflow");
                    }
                cActual = update(cAssume, cAssume + c);
                }
            while (cAssume != cActual);
            return cAssume;
            }

        /**
        * Adjust the NativeAtomic64 by the specified value.
        *
        * @param c  the amount to adjust the atomic by
        * @param fSafe  to provide overflow protection
        *
        * @return the adjusted value
        */
        int64_t adjust(int64_t c, bool fSafe = true)
            {
            return postAdjust(c, fSafe) + c;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * A constant for the minimum representable int64_t value.
        * 
        * Note: This is defined in Integer64.hpp, but we can not include
        *       Integer64.hpp, so define it here.
        */
        static const int64_t min_value = COH_INT64(0x80000000U, 0x0);

        /**
        * A constant for the maximum representable int64_t value.
        * 
        * Note: This is defined in Integer64.hpp, but we can not include
        *       Integer64.hpp, so define it here.
        */
        static const int64_t max_value = COH_INT64(0x7FFFFFFFU, 0xFFFFFFFFU);

        /**
        * The atomically accessed value, in general this should be word aligned.
        */
        COH_ALIGN(8, int64_t, m_lAtomic);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_ATOMIC64_HPP
