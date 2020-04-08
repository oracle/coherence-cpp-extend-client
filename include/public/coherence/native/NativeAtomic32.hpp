/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_ATOMIC32_HPP
#define COH_NATIVE_ATOMIC32_HPP

#include "coherence/lang/compatibility.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


/**
* @internal
*
* 32-bit atomic integer, providing JSR-133 volatile memory semantics.
*
* @author mf 2015.02.11
*/
class COH_EXPORT NativeAtomic32
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create a new Atomic32, as Atomic32 is not an Object
        * it is the responsibility of the caller to eventually
        * destroy this object.
        *
        * @param nValue   the initial value
        * @param fAtomic  true if the initial assignment needs synchronization
        *
        * @return a pointer to the new Atomic32
        */
        NativeAtomic32(int32_t nValue = 0, bool fAtomic = true)
            {
            if (fAtomic)
                {
                set(nValue);
                }
            else
                {
                poke(nValue);
                }
            }


    // ----- Atomic32 interface ---------------------------------------------

    public:
        /**
        * Return the current value.
        *
        * @return the current value
        */
        int32_t get() const;

        /**
        * Return the assumed value without performing any memory
        * synchronization. This value should not be trusted, but is suitable
        * as an assumed value to be passed to update, which will validate if
        * it is correct before allowing any associated change to occur.
        *
        * @return the assumed value
        */
        int32_t peek() const
            {
            return m_nAtomic;
            }

        /**
        * Unconditionally set the value in a thread-safe manor.
        *
        * @param  nValue the new value
        */
        void set(int32_t nValue);

        /**
        * Unconditionally (and in a non thread-safe manor) set the value.
        *
        * @param  nValue the new value
        */
        void poke(int32_t nValue)
            {
            m_nAtomic = nValue;
            }

        /**
        * Set the value so long as the current value matches the expected value.
        *
        * @param nAssume  the expected current value
        * @param nValue   the new value
        *
        * @return the prior actual value, if the returned value
        *         does is not equal to the supplied assumed
        *         value then update did not take place
        */
        int32_t update(int32_t nAssume, int32_t nValue);

        /**
        * Set the value so long as the current value matches the expected value.
        *
        * This version of the update method supports conditionally performing
        * the update in an unsafe manor. The unsafe update is performed
        * without any form of synchronization and should only be used when it
        * is known that the object is referenced by a single thread.
        *
        * @param nAssume  the expected current value
        * @param nValue   the new value
        * @param fAtomic  perform a safe update operation which can be used
        *                 across threads
        *
        * @return the prior actual value, if the returned value
        *         does is not equal to the supplied assumed
        *         value then update did not take place
        */
        int32_t update(int32_t nAssume, int32_t nValue, bool fAtomic)
            {
            if (fAtomic)
                {
                return update(nAssume, nValue);
                }

            int32_t nOld = peek();
            if (nOld == nAssume)
                {
                poke(nValue);
                }
            return nOld;
            }

        /**
        * Adjust the NativeAtomic32 by the specified value.
        *
        * @param c      the amount to adjust the atomic by
        * @param fSafe  to provide overflow protection
        *
        * @return the old value
        */
        int32_t postAdjust(int32_t c, bool fSafe = true)
            {
            int32_t cAssume;
            int32_t cActual = peek();
            do
                {
                cAssume = cActual;
                if (fSafe &&
                    ((c > 0 && cAssume > max_value - c) ||
                     (c < 0 && cAssume < min_value - c )))
                    {
                    // would overflow
                    coh_throw_illegal_argument("NativeAtomic32 overflow");
                    }
                cActual = update(cAssume, cAssume + c);
                }
            while (cAssume != cActual);
            return cAssume;
            }

        /**
        * Adjust the NativeAtomic32 by the specified value.
        *
        * @param c  the amount to adjust the atomic by
        * @param fSafe  to provide overflow protection
        *
        * @return the adjusted value
        */
        int32_t adjust(int32_t c, bool fSafe = true)
            {
            return postAdjust(c, fSafe) + c;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * A constant for the minimum representable int32_t value.
        *
        * Note: This is defined in Integer32.hpp, but we can not include
        *       Integer32.hpp, so define it here.
        */
        static const int32_t min_value = 0x80000000U;

        /**
        * A constant for the maximum representable int32_t value.
        *
        * Note: This is defined in Integer32.hpp, but we can not include
        *       Integer32.hpp, so define it here.
        */
        static const int32_t max_value = 0x7FFFFFFFU;

        /**
        * The atomically accessed value, in general this should be word aligned.
        */
        COH_ALIGN(4, int32_t, m_nAtomic);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_ATOMIC32_HPP
