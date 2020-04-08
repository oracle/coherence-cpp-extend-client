/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TIMEOUT_BLOCK_HPP
#define COH_TIMEOUT_BLOCK_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Thread.hpp"

COH_OPEN_NAMESPACE2(coherence, lang)

/**
 * TimeoutBlock provides a mechanism for allowing a thread to interrupt itself if it doesn't return
 * to a specific call site within a given timeout.  TimeoutBlock instances are intended to be
 * used only with the COH_TIMEOUT_AFTER helper macro.  Once constructed a TimeoutBlock attempts to ensure that
 * the corresponding block completes within the specified timeout and if it does not the thread will
 * self-interrupt.  Exiting the timeout block will automatically clear any interrupt present on the thread
 * and in such a case an InterruptedException will be thrown.
 *
 * <pre>
 * try
 *     {
 *     COH_TIMEOUT_AFTER(5000)
 *         {
 *         doSomething();
 *         }
 *     } // this thread will self-interrupt if it doesn't reach this line within 5 seconds
 * catch (InterruptedException::View vEx)
 *     {
 *     // thread timed out or was otherwise interrupted
 *     }
 * </pre>
 *
 * Note that when catching the InterruptedException the preferred form is to surround the COH_TIMEOUT block with a try/catch
 * rather then embed the try/catch within the block.  This allows handling of cases where the thread gets interrupted but
 * the doSomething() method does not choose to throw the InterruptedException, in which case the TimeoutBlock destructor will
 * throw it, but this happens as the block terminates and thus would not be catchable within the block.
 *
 * Note that TimeoutBlock can only self-interrupt at interruptible points, and does not defend against
 * CPU bound loops for example.
 *
 * @author  mf 2015.03.03
 */
class COH_EXPORT TimeoutBlock
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
         * Specify a new timeout.
         *
         * This constructor variant allows the caller to override a parent timeout.  This is
         * rarely needed, and is roughly the equivalent of silently consuming a thread interrupt
         * without rethrowing the InterruptedException.
         *
         * @param cMillis         the new timeout.
         * @param fForceOverride  true if this timeout is allowed to extend a parent timeout.
         */
        TimeoutBlock(int64_t cMillis, bool fForceOverride = false);

        /**
        * Copy constructor for COH_TIMEOUT macros.
        */
        TimeoutBlock(const TimeoutBlock& that);

        /**
        * Destroy a TimeoutBlock object.
        *
        * This will automatically reset the timeout to any former value.
        */
        ~TimeoutBlock();


    // ----- operators ------------------------------------------------------

    public:
        /*
        * Boolean conversion for use in COH_TIMEOUT macros.
        *
        * @return false always
        */
        operator bool() const
            {
            return false;
            }

    private:
        /**
        * Blocked assignment operator.
        */
        const TimeoutBlock& operator=(const TimeoutBlock&);

        /**
        * Blocked dynamic allocation.
        */
        static void* operator new(size_t);


    // ----- data members ---------------------------------------------------

    protected:
        /**
         * This TimeoutBlock's timeout.
         */
        int64_t m_cMillisTimeout;

        /**
         * The original timeout before this instance changed it.
         */
        int64_t m_lTimeoutOrig;

        /**
         * The current thread, or NULL if this block is inactive.
         */
        mutable Thread::Handle m_hThread;
    };

COH_CLOSE_NAMESPACE2

/**
* Macro for making more readable timeout code blocks See the
* documentation of TimeoutBlock for a usage example.
*
* @see coherence::lang::TimeoutBlock
*/
#define COH_TIMEOUT_AFTER(CMILLIS) \
    if (coherence::lang::TimeoutBlock COH_UNIQUE_IDENTIFIER(_coh_timeout_) \
        = coherence::lang::TimeoutBlock(CMILLIS)) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else

/**
* Macro for making more readable timeout code blocks which is allowed to
* extend an already active timeout.
*
* This variant allows the caller to extend a parent timeout.  This is rarely
* needed, and is roughly the equivalent of silently consuming a thread interrupt
* without rethrowing the InterruptedException.  Use of this method should
* be extremely limited.
*
* @see coherence::lang::TimeoutBlock
*/
#define COH_TIMEOUT_OVERRIDE(CMILLIS) \
    if (coherence::lang::TimeoutBlock COH_UNIQUE_IDENTIFIER(_coh_timeout_) \
        = coherence::lang::TimeoutBlock(CMILLIS, /*fForceOverride*/ true)) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else

#endif // COH_TIMEOUT_BLOCK_HPP
