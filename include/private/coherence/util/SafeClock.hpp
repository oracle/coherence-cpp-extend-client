/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_CLOCK_HPP
#define COH_SAFE_CLOCK_HPP

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic32.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::native::NativeAtomic32;

/**
* SafeClock maintains a "safe" time in milliseconds.
*
* Unlike the System::currentTimeMillis() this clock guarantees that the
* time never "goes back". More specifically, when queried twice on the same
* thread, the second query will never return a value that is less then the
* value returned by the first.
*
* If we detect the system clock moving backward, an attempt will be made to
* gradually compensate the safe clock (by slowing it down), so in the long run
* the safe time is the same as the system time.
*
* The SafeClock supports the concept of "clock jitter", which is a small time
* interval that the system clock could fluctuate by without a corresponding
* passage of wall time.
*
* @author mf  2009.12.09
*
* @since Coherence 3.6
*/
class COH_EXPORT SafeClock
    : public class_spec<SafeClock>
    {
    friend class factory<SafeClock>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new SafeClock with the default maximum expected jitter as
        * specified by the "coherence.safeclock.jitter" system
        * property.
        *
        * @param ldtUnsafe  the current unsafe time
        */
        SafeClock(int64_t ldtUnsafe);

        /**
        * Create a new SafeClock with the specified jitter threshold.
        *
        * @param ldtUnsafe  the current unsafe time
        * @param lJitter    the maximum expected jitter in the underlying
        *                   system clock
        */
        SafeClock(int64_t ldtUnsafe, int64_t lJitter);


    // ----- SafeClock interface --------------------------------------------

    public:
        /**
        * Returns a "safe" current time in milliseconds.
        *
        * @param ldtUnsafe  the current unsafe time
        *
        * @return the difference, measured in milliseconds, between the corrected
        *         current time and midnight, January 1, 1970 UTC.
        */
        int64_t getSafeTimeMillis(int64_t ldtUnsafe)
            {
            // optimization for heavy concurrent load: if no time has passed, or
            // time jumped back within the expected jitter just return the last
            // time and avoid synchronization; keep short to encourage compiler
            // optimizations
            int64_t lDelta = ldtUnsafe - m_ldtLastUnsafe;

            return lDelta == 0 || (lDelta < 0 && lDelta >= -m_lJitter)
                    ? m_ldtLastSafe // common case during heavy load
                    : updateSafeTimeMillis(ldtUnsafe);
            }

        /**
        * Returns the last "safe" time as computed by a previous call to
        * the {@link #getSafeTimeMillis} method.
        * <p/>
        * Note: Since the underlying field is non-volatile, the returned value is
        * only guaranteed to be no less than the last value returned by
        * getSafeTimeMillis() call on the same thread.
        *
        * @return the last "safe" time in milliseconds
        */
        int64_t getLastSafeTimeMillis() const
            {
            return m_ldtLastSafe;
            }


    // ---- helper methods --------------------------------------------------

    protected:
        /**
        * Updates and returns a "safe" current time in milliseconds based on the
        * "unsafe" time.
        *
        * @param ldtUnsafe  the unsafe current time in milliseconds
        *
        * @return the corrected safe time
        */
        virtual int64_t updateSafeTimeMillis(int64_t ldtUnsafe);

    public:
        /**
        * Get the default jitter threshold.
        */
        static int64_t getDefaultJitterThreshold();


    // ---- data members ----------------------------------------------------

    protected:
        /**
        * The last known safe time value.
        */
        int64_t m_ldtLastSafe;

        /**
        * The last recorded unsafe time value.
        */
        int64_t m_ldtLastUnsafe;

        /**
        * The maximum expected jitter exposed by the underlying unsafe clock.
        */
        const int64_t m_lJitter;

        /**
         * Lock protecting clock updates.
         */
        NativeAtomic32 m_lock;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SAFE_CLOCK_HPP
