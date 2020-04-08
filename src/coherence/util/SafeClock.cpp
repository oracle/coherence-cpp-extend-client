/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/SafeClock.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

SafeClock::SafeClock(int64_t ldtUnsafe)
    : m_ldtLastSafe(ldtUnsafe), m_ldtLastUnsafe(ldtUnsafe),
      m_lJitter(getDefaultJitterThreshold())
    {
    }

SafeClock::SafeClock(int64_t ldtUnsafe, int64_t lJitter)
    : m_ldtLastSafe(ldtUnsafe), m_ldtLastUnsafe(ldtUnsafe), m_lJitter(lJitter)
    {
    }


// ---- helper methods ----------------------------------------------------

int64_t SafeClock::updateSafeTimeMillis(int64_t ldtUnsafe)
    {
    if (m_lock.update(0, 1) == 0)
        {
        int64_t lJitter     = m_lJitter;
        int64_t ldtLastSafe = m_ldtLastSafe;
        int64_t lDelta      = ldtUnsafe - m_ldtLastUnsafe;
        int64_t ldtNewSafe  = ldtLastSafe;

        if (lDelta > 0)
            {
            // unsafe progressed
            if (ldtUnsafe >= ldtLastSafe)
                {
                // common case; unsafe is at or ahead of safe; sync clocks
                ldtNewSafe = ldtUnsafe;
                }
            else if (lDelta > lJitter && ldtLastSafe - ldtUnsafe <= lJitter)
                {
                // unsafe is behind safe and jumped; the jump brought it
                // very close (within jitter) to where it was before the
                // corresponding regression; this appears to be jitter, hold
                // safe and avoid recording anything about this bogus jump as
                // that could artificially push safe into the future
                m_lock.set(0); // unlock
                return ldtLastSafe;
                }
            else
                {
                // unsafe is behind safe and progressed; progress safe slowly
                // at half the measured delta or every other ms if delta is 1ms
                // allowing unsafe to eventually catch up
                ldtNewSafe += lDelta == 1 ? ldtUnsafe % 2 : lDelta / 2;
                }
            }
        else if (lDelta >= -lJitter)
            {
            // unsafe made an insignificant (within jitter) regression; or
            // didn't move at all; hold safe and avoid recording anything about
            // this bogus jump as that could artificially push safe into the future
            // Note: the same cases are handled in getSafeTimeMillis() but based
            // on synchronization ordering it may not be detected until here
            m_lock.set(0); // unlock
            return ldtLastSafe;
            }

        // except in the case of jitter we update our clocks
        m_ldtLastUnsafe = ldtUnsafe;
        m_ldtLastSafe = ldtNewSafe;
        m_lock.set(0); // unlock
        return ldtNewSafe;
        }
    else
        {
        // some other thread has locked the clock we have a few options
        // - block until they complete, but who likes global contention
        // - spin until they complete, but then we just waste CPU, and for what gain?
        // - pretend like time has not advanced, no worse then the above and we get to do useful work
        return m_ldtLastSafe; // note since we've attempted the CAS this is as good as a volatile read
        }
    }

int64_t SafeClock::getDefaultJitterThreshold()
    {
    static int64_t s_lThreshold = Integer64::parse(
            System::getProperty("coherence.safeclock.jitter", "16"));
    return s_lThreshold;
    }
COH_STATIC_INIT(SafeClock::getDefaultJitterThreshold());

COH_CLOSE_NAMESPACE2
