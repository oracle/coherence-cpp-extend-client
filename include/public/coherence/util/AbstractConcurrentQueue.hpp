/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_CONCURRENT_QUEUE
#define COH_ABSTRACT_CONCURRENT_QUEUE

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic32.hpp"
#include "coherence/util/Queue.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::native::NativeAtomic32;

/**
* The ConcurrentQueue provides a means to efficiently (and in a thread-safe
* manner) queue elements with minimal contention.
*
* Note: The ConcurrentQueue does not support null entries.
*
* @author nsa 2008.01.19
*/
class COH_EXPORT AbstractConcurrentQueue
    : public abstract_spec<AbstractConcurrentQueue,
        extends<Object>,
        implements<Queue> >
    {
    // ----- enums ----------------------------------------------------------

    public:
        /**
        * The FlushState values are used to indicate the state of the Queue
        * with regards to flushing:
        *
        * <ul>
        *   <li>
        *     flush_pending:  Indicates that a flush is pending. At
        *                     some point the queue will flush itself
        *                     automatically or the consumer will explicitly
        *                     flush the queue.
        *   </li>
        *   <li>
        *     flush_auto:     Indicates that no flush is pending as the queue
        *                     has been auto flushed. This state will not be
        *                     reset to flush_pending until the queue has been
        *                     cleared and the producer has added a new
        *                     element.
        *   </li>
        *   <li>
        *     flush_explicit: Indicates that no flush is pending as the queue
        *                     has been explicitly flushed. This state will
        *                     not be reset to flush_pending until the queue
        *                     has been cleared and the producer has added a
        *                     new element, or if the producer calls flush
        *                     multiple times before the queue has
        *                     been cleared.
        *   </li>
        * </ul>
        */
        typedef enum
            {
            flush_pending = 0,
            flush_auto = 1,
            flush_explicit = 2
            } FlushState;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractConcurrentQueue();


    // ----- AbstractConcurrentQueue interface ------------------------------

    public:
        /**
        * Return the current flush state.
        */
        virtual FlushState getFlushState() const;

        /**
        * Return whether a flush is pending or not.
        *
        * @return true if a flush is pending
        */
        virtual bool isFlushPending() const;

        /**
        * Set the batch size of the queue.
        *
        * @param cBatch  the batch size to set
        */
        virtual void setBatchSize(int32_t cBatch);

        /**
        * Return the batch size of the queue.
        *
        * @return the batch size of the queue
        */
        virtual int32_t getBatchSize() const;

        /**
        * Return the object used for notifications on this queue.
        *
        * @return the object used for notifications on this queue.
        */
        virtual Object::Handle getNotifier();

        /**
        * Set the object used for notifications on this queue.
        *
        * @param hNotifier  the object used for notifications on this queue
        */
        virtual void setNotifier(Object::Handle hNotifier);

        /**
        * Wait for the queue to contain at least one entry.
        *
        * Note: By the time the method returns the entry may have already
        * been removed by another thread.
        *
        * @param cMillis the number of milliseconds to wait before returing
        *                without having been notified, or 0 to wait until
        *                notified
        */
        virtual void waitForEntry(int64_t cMillis);

        /**
        * Return the total number of times the queue has been emptied.
        *
        * @return the total number of times the queue has been emptied.
        */
        virtual int64_t getStatsEmptied() const;

        /**
        * Return the total number of times the queue has been flushed.
        *
        * @return the total number of times the queue has been flushed
        */
        virtual int64_t getStatsFlushed() const;


    // ----- internal helpers -----------------------------------------------

    protected:
        /**
        * Check whether or not the flush (notify) is necessary. This method
        * is always called when a new item is added to the queue.
        *
        * @param cElements  the number of elements in the queue after the
        *                   addition
        */
        virtual void checkFlush(int32_t cElements);

        /**
        * Flush the queue.
        *
        * @param fAuto  iff the flush was invoked automatically based on the
        *               notification batch size
        *
        */
        virtual void flush(bool fAuto);

        /**
        * Event called each time an element is added to the queue.
        */
        virtual void onAddElement();

        /**
        * Event called when the queue becomes empty.
        */
        virtual void onEmpty();

        /**
        * Set the flush state and return the previous state.
        *
        * @param nState  the state to set
        *
        * @return the previous flush state
        */
        virtual FlushState updateFlushState(FlushState nState);

        /**
        * Set the flush state iff the assumed state is correct, return the
        * previous flushState
        *
        * @param nStateAssumed  the assumed current value of the flush state
        * @param nStateNew      the new flush state value
        *
        * @return FlushState  the old flush state value
        */
        virtual FlushState updateFlushStateConditionally(FlushState nStateAssumed,
                FlushState nStateNew);

        /**
        * Set the total number of times the queue has been emptied.
        *
        * @param cEmpties  the total number of times the queue has been
        *                  flushed.
        */
        virtual void setStatsEmptied(int64_t cEmpties);

        /**
        * Set the total number of times the queue has been flushed.
        *
        * @param cFlushed  the total number of times the queue has been
        *                  flushed
        */
        virtual void setStatsFlushed(int64_t cFlushed);


    // ----- Queue interface ------------------------------------------------

    public:
        /**
        * @{inheritDoc}
        */
        virtual void flush();

        /**
        * @{inheritDoc}
        */
        virtual size32_t size() const;

        /**
        *  @{inheritDoc}
        */
        virtual Object::Holder remove();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The AtomicLong used to maintain the FlushState.  See
        * getFlushState() and setFlushState() helper methods.
        */
        NativeAtomic32 m_nAtomicFlushState;

        /**
        * The queue size at which to auto-flush the queue during an add
        * operation.  If the BatchSize is greater then one, the caller must
        * externally call flush() when it has finished adding elements in
        * order to ensure that they may be processed by any waiting consumer
        * thread.
        */
        int32_t m_iBatchSize;

        /**
        * A counter for maintaining the size of the queue.
        */
        NativeAtomic32 m_nElementCounter;

        /**
        * The monitor on which notifications related to a queue addition
        * will be performed.  The default value is the Queue itself.  The
        * Notifier should not be changed while the queue is in use.  If the
        * Notifier is null then notification will be disabled.
        */
        FinalHandle<Object> f_hNotifier;

        /**
        * The total number of times the queue transitioned to the empty
        * state.
        */
        int64_t m_lStatsEmptied;

        /**
        * The total number of times the queue has been flushed.
        */
        int64_t m_lStatusFlushed;

        /**
        * Indicate whether the notifier references itself
        */
        bool m_fSelfNotifier;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ABSTRACT_CONCURRENT_QUEUE
