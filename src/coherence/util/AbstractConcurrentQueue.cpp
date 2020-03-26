/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractConcurrentQueue.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructor --------------------------------------------------------

AbstractConcurrentQueue::AbstractConcurrentQueue()
        : m_nAtomicFlushState(flush_pending),
          m_iBatchSize(1), m_nElementCounter(0),
          f_hNotifier(self()), m_lStatsEmptied(0), m_lStatusFlushed(0),
          m_fSelfNotifier(true)
    {
    }

// ----- AbstractConcurrentQueue interface ----------------------------------

AbstractConcurrentQueue::FlushState AbstractConcurrentQueue::getFlushState() const
    {
    return (FlushState) m_nAtomicFlushState.get();
    }

bool AbstractConcurrentQueue::isFlushPending() const
    {
    return m_nAtomicFlushState.get() == flush_pending;
    }

void AbstractConcurrentQueue::setBatchSize(int32_t cBatch)
    {
    m_iBatchSize = cBatch;
    }

int32_t AbstractConcurrentQueue::getBatchSize() const
    {
    return m_iBatchSize;
    }

Object::Handle AbstractConcurrentQueue::getNotifier()
    {
    if (m_fSelfNotifier)
        {
        return this;
        }

    return f_hNotifier;
    }

void AbstractConcurrentQueue::setNotifier(Object::Handle hNotifier)
    {
    initialize(f_hNotifier, hNotifier);
    m_fSelfNotifier = false;
    }

void AbstractConcurrentQueue::waitForEntry(int64_t cMillis)
    {
    if (isEmpty())
        {
        Object::Handle hNotifier = getNotifier();
        if (NULL == hNotifier)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "blocking remove without a notifier"));
            }

        COH_SYNCHRONIZED(hNotifier)
            {
            if (isEmpty())
                {
                hNotifier->wait(cMillis);
                }
            }
        }
    }

int64_t AbstractConcurrentQueue::getStatsEmptied() const
    {
    return m_lStatsEmptied;
    }

int64_t AbstractConcurrentQueue::getStatsFlushed() const
    {
    return m_lStatusFlushed;
    }

void AbstractConcurrentQueue::checkFlush(int32_t cElements)
    {
    if (getNotifier() != NULL)
        {
        FlushState nFlushState;
        if (cElements == 1)
            {
            // queue was previously empty
            nFlushState = flush_pending;
            updateFlushState(nFlushState);
            }
        else
            {
            nFlushState = getFlushState();
            }

        switch(nFlushState)
            {
            case flush_pending:
                if (cElements % getBatchSize() == 0)
                    {
                    flush(true);
                    }
                break;

            case flush_explicit:
                // producer has started adding again before consumer fully
                // drained, exit the flush_explicit state as that may be used
                // as an indication that no more data is comming
                updateFlushStateConditionally(flush_explicit, flush_auto);
                break;

            case flush_auto:
                // noop
                break;
            }
        }
    }

void AbstractConcurrentQueue::flush(bool fAuto)
    {
    if (updateFlushState(fAuto ? flush_auto : flush_explicit) == flush_pending)
        {
        // transitioned from flush_pending
        Object::Handle hNotifier = getNotifier();
        if (hNotifier != NULL)
            {
            COH_SYNCHRONIZED(hNotifier)
                {
                setStatsFlushed(getStatsFlushed() + 1);
                hNotifier->notifyAll();
                }
            }
        }
    }

void AbstractConcurrentQueue::onAddElement()
    {
    checkFlush(m_nElementCounter.adjust(1));
    }

void AbstractConcurrentQueue::onEmpty()
    {
    setStatsEmptied(getStatsEmptied() + 1);
    }

AbstractConcurrentQueue::FlushState AbstractConcurrentQueue::updateFlushState(FlushState nState)
    {
    int32_t nAssume;
    int32_t nActual = m_nAtomicFlushState.peek();
    int32_t cNew    = nState;

    do
        {
        nAssume = nActual;
        nActual = m_nAtomicFlushState.update(nAssume, cNew);
        }
    while (nAssume != nActual);

    return (FlushState) nActual;
    }

AbstractConcurrentQueue::FlushState
        AbstractConcurrentQueue::updateFlushStateConditionally(FlushState nStateAssumed,
            FlushState nStateNew)
    {
    return (FlushState) m_nAtomicFlushState.update(nStateAssumed, nStateNew);
    }

void AbstractConcurrentQueue::setStatsEmptied(int64_t cEmpties)
    {
    m_lStatsEmptied = cEmpties;
    }

void AbstractConcurrentQueue::setStatsFlushed(int64_t cFlushed)
    {
    m_lStatusFlushed = cFlushed;
    }


// ----- Queue interface ----------------------------------------------------

void AbstractConcurrentQueue::flush()
    {
    flush(false);
    }

size32_t AbstractConcurrentQueue::size() const
    {
    return (size32_t) m_nElementCounter.get();
    }

Object::Holder AbstractConcurrentQueue::remove()
    {
    while (true)
        {
        Object::Holder h = removeNoWait();

        if (NULL != h)
            {
            return h;
            }

        waitForEntry(0);
        }
    }

COH_CLOSE_NAMESPACE2
