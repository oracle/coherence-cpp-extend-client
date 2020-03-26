/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/TimeoutBlock.hpp"

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence, lang)

TimeoutBlock::TimeoutBlock(int64_t cMillis, bool fForceOverride)
    : m_hThread(Thread::currentThread())
    {
    m_lTimeoutOrig = m_hThread->getTimeout();

    if (m_lTimeoutOrig == Integer64::max_value) // orig is disabled (common)
        {
        m_cMillisTimeout = cMillis;
        m_hThread->setTimeout(-cMillis);
        }
    else if (m_lTimeoutOrig < 0) // orig is relative (common)
        {
        if (fForceOverride || cMillis < -m_lTimeoutOrig)
            {
            m_cMillisTimeout = cMillis;
            m_hThread->setTimeout(-cMillis);
            }
        else // we are not allowed to extend an existing timeout
            {
            m_cMillisTimeout = m_lTimeoutOrig;
            }
        }
    else // orig is timestamp
        {
        // TODO: we could avoid pulling the time here if we retained a ref to the prior Timeout object
        // rather then just it's timeout value.  In this case we'd have the absolute timeout and it's
        // relative value and could then compute our updated absolute from those.
        int64_t ldtTimeout = System::safeTimeMillis() + cMillis;
        if (fForceOverride || ldtTimeout < m_lTimeoutOrig)
            {
            m_cMillisTimeout = cMillis;
            m_hThread->setTimeout(ldtTimeout);
            }
        else // we are not allowed to extend an existing timeout
            {
            m_cMillisTimeout = m_lTimeoutOrig;
            }
        }
     }

TimeoutBlock:: TimeoutBlock(const TimeoutBlock& that)
    : m_cMillisTimeout(that.m_cMillisTimeout), m_lTimeoutOrig(that.m_lTimeoutOrig), m_hThread(that.m_hThread)
    {
    that.m_hThread = NULL; // deactivate the other block
    }

TimeoutBlock::~TimeoutBlock()
    {
    if (NULL != m_hThread)
        {
        // we must always restore the former timeout, even if it is expired
        if (m_lTimeoutOrig < 0) // orig was never realized
            {
            int64_t lTimeoutCurr = m_hThread->getTimeout();
            if (lTimeoutCurr < 0 ||             // we've yet to block
                lTimeoutCurr == Integer64::max_value) // timeout was disabled (note restore is suspect, but override has already violated orig)
                {
                // simply restore the orig value
                m_hThread->setTimeout(m_lTimeoutOrig);
                }
            else
                {
                // curr was realized, orig was not, adjust orig accordingly
                // and set it as new timeout
                m_hThread->setTimeout(lTimeoutCurr + (-m_lTimeoutOrig - m_cMillisTimeout));
                }
            }
        else // orig is realized, simply restore it
            {
            m_hThread->setTimeout(m_lTimeoutOrig);
            }

        // checking to see if the thread is interrupted here ensures that if the nested code within the
        // interrupt block were to suppress the InterruptedException (possibly from a timeout) that it
        // gets recreated here so the application is forced to deal with it
        // Note we don't just throw because of a timeout, as the general contract of a method which
        // throws InterruptedException is that it throws if the thread in interrupted, period
        // Note: we don't try to throw some derived exception such as InterruptedTimeoutException as
        // we can't ensure that all timeout points would actually result in that exception.  For instance
        // a timeout in LockSupport.park() will interrupt the thread by throw nothing, and some other code
        // could then detect the interrupt and throw a normal InterruptedException.  Overall the intent
        // here is to just make the timeout feature be indistinugisable from another thread interrupting
        // this the thread.
        if (m_hThread->isInterrupted() && // cheap because we hold the thread ref already
            Thread::interrupted())        // necessary in order to clear the ref, but only if we were in fact interrupted
            {
            COH_THROW(InterruptedException::create());
            }
        }
     }

COH_CLOSE_NAMESPACE2
