/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_THREAD_GATE_HPP
#define COH_THREAD_GATE_HPP

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic32.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::native::NativeAtomic32;


/**
* Use this class in cases that large numbers of threads can operate
* concurrently with an additional requirement that all threads be blocked for
* certain operations.  The algorithm is based on a gate concept, allowing
* threads in (enter) and out (exit), but occasionally shutting the gate (close)
* such that other threads cannot enter and exit.  However, since threads may
* "be inside", the gate cannot fully close until they leave (exit).  Once all
* threads are out, the gate is closed, and can be re-opened (open) or
* permanently closed (destroy).
*
* Each call to enter requires a corresponding call to exit, similar to the
* implementation of the COH_SYNCHRONIZED macro that calls Object::lock at the
* the beginning of the synchronized portion and protects the synchronized
* portion with a try..catch construct that ensures the execution of a
* Object::unlock call.  For example, the following would ensure proper clean-up
* using a ThreadGate:
* </p>
* The enter/exit calls can be nested; the same thread can invoke enter multiple
* times as long as exit is invoked a corresponding number of times. The
* close/open calls work in the same manner.  Lastly, the thread that closes the
* gate may continue to enter/exit the gate even when it is closed since that
* thread has exclusive control of the gate.
* <p>
* To make usage of these calls easier, the following macros have been defined:
* <ul>
*     <li> COH_GATE_ENTER enters and automatically exits the gate
*     <li> COH_GATE_CLOSE closes and automatically opens the gate
*     <li> COH_GATE_BAR   bars entry and automatically opens the gate
* </ul>
* <pre>
* COH_GATE_ENTER (hGate) // read lock acquired
*   {
*   // read operations go here
*   // ...
*   // ...
*   } // read lock released
* // outside of sync block
*
* COH_GATE_CLOSE (hGate) // write lock acquired
*   {
*   // write operations go here
*   // ...
*   // ...
*   } // write lock released
* // outside of sync block
*
* COH_GATE_BAR (hGate) // additional readers are blocked
*   {
*   // non-synchronized ops
*   COH_GATE_CLOSE (hGate) // write lock acquired
*      {
*      // ...
*      // ...
*      } // inner-write lock released
*   // gate still locked
*   } // gate opened
* // outside of sync block
* </pre>
*
* @author nsa  2008.01.03
*/
class COH_EXPORT ThreadGate
    : public class_spec<ThreadGate>
    {
    friend class factory<ThreadGate>;

    // ----- enum: State ----------------------------------------------------

    public:
    /**
    * State identifiers
    */
    typedef enum
        {
        gate_open      = 0,
        gate_closing   = 1,
        gate_closed    = 2,
        gate_destroyed = 3
        } Status;


    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new ThreadGate.
        */
        ThreadGate();


    // ----- ThreadGate interface -------------------------------------------

    public:
        /**
        * Bar entry of the thread gate by other threads, but do not wait for
        * the gate to close. When all other threads have exited, the thread
        * gate will be closeable by the thread which barred entry. Each
        * successful invocation of this method must ultimately have a
        * corresponding invocation of the open() method (assuming the thread
        * gate is not destroyed), even if the calling thread does not
        * subsequently close the gate.
        *
        * @param cMillis  maximum number of milliseconds to wait; pass
        *                 infinite for forever or immediate for no wait
        *
        * @return true iff entry into the gate was successfully barred by the
        *              calling thread
        */
        virtual bool barEntry(int64_t cMillis = infinite);

        /**
        * Close the thread gate. A thread uses this method to obtain exclusive
        * access to the resource represented by the thread gate. Each
        * invocation of this method must ultimately have a corresponding
        * invocation of the open() method.
        *
        * @param cMillis  maximum number of milliseconds to wait; pass
        *                 infinite for forever or immediate for no wait
        *
        * @return true iff the entry into the thread gate was successfully
        *              barred by the calling thread and no other threads
        *              remain in the gate.
        */
        virtual bool close(int64_t cMillis = infinite);

        /**
        * Destroy the thread gate. This method can only be invoked if the
        * gate is already closed by the calling thread.
        */
        virtual void destroy();

        /**
        * Enter the thread gate. A thread uses this method to obtain
        * non-exclusive access to the resource represented by the thread gate.
        * Each successful invocation of this method must ultimately have a
        * corresponding invocation of the exit() method.
        *
        * @param cMillis  maximum number of milliseconds to wait; pass
        *                 infinite for forever or immediate for no wait
        *
        * @return true iff the calling thread successfully entered the gate
        */
        virtual bool enter(int64_t cMillis = infinite);

        /**
        * Exit the gate. A thread must invoke this method corresponding to
        * each successful invocation of the enter method.
        */
        virtual void exit();

        /**
        * After entry into the ThreadGate is restricted by a call to
        * barEntry() or close(), it can be re-opened by calling this method.
        * Only the thread that called barEntry() or close() can call open().
        */
        virtual void open();

        /**
        * Return the number of entered threads.
        *
        * @return the number of entered threads
        */
        virtual int32_t getActiveCount() const;

        /**
        * Determine if the current thread has entered and not exited. This is
        * useful for detecting re-entrancy.
        *
        * @return true if the current thread has entered and not exited.
        */
        virtual bool isActiveThread() const;

        /**
        * Return the number of unmatched completed close/barEntry calls.
        *
        * @return the number of unmatched completed close/barEntry calls.
        */
        virtual int32_t getCloseCount() const;

        /**
        * Return the current thread gate status.
        *
        * @return the current thread gate status
        */
        virtual Status getStatus() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- inner class: Counter -------------------------------------------

    public:
        /**
        * Counter is a mutable integer class.
        */
        class Counter
            : public class_spec<Counter>
            {
            friend class factory<Counter>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a Counter initialized with a specified value.
                *
                * @param c  the initial value of the Counter
                */
                Counter(int32_t c = 0);

            // ----- Counter interface ----------------------------------

            public:
                /**
                * Return the count.
                *
                * @return the count
                */
                virtual int32_t getCount() const;

                /**
                * Set the count.
                *
                * @param c  the value to set the count to.
                */
                virtual void setCount(int32_t c);

                /**
                * Increment the count by 1.
                *
                * @return the incremented count.
                */
                virtual int32_t increment();

                /**
                * Decrement the count.
                *
                * @return the decremented count.
                */
                virtual int32_t decrement();

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- data members ---------------------------------------

            private:
                /**
                * The count.
                */
                int32_t m_c;
            };


    // ----- inner class: GateBlock -----------------------------------------

    public:
        /**
        * The EnterBlock class allows for easy creation of ThreadGate::enter
        * code. The EnterBlock object will ensure that the gate is entered
        * and exited as a part of starting and ending the code block. It's
        * used by the COH_GATE_ENTER macro
        */
        class GateBlock
            {
            // ----- enums ----------------------------------------------
            public:
                /**
                * The type of GateBlock that will be constructed
                */
                typedef enum
                    {
                    bar,
                    close,
                    enter
                    } BlockType;

            // ----- constructor ----------------------------------------

            public:
                /**
                * Construct a new EnterBlock object entering the supplied
                * gate.
                *
                * @param nType  the type of GateBlock to construct
                * @param hGate  the gate to enter
                * @param lTime  maximum number of milliseconds to wait; pass
                *               infinite for forever or immediate for no wait
                */
                GateBlock(BlockType nType, ThreadGate::Handle hGate,
                        int64_t lTime);

                /**
                * Copy constructor used by the COH_GATE_ENTERED macro.
                *
                * The new block takes over ownership of the gate.
                */
                GateBlock(const GateBlock& that);

                /**
                * Destroy an EnterBlock object, exiting the associated gate
                */
                ~GateBlock();

            // ----- GateBlock interface --------------------------------

            public:
                /**
                * Boolean conversion for the COH_GATE_ENTER macro
                *
                * @return true always
                *
                * @throw IllegalStateException if the gate is null
                */
                operator bool() const;

            private:
                /**
                * Blocked assignment operator.
                */
                const GateBlock& operator=(const GateBlock&);

                /**
                * Blocked dynamic allocation.
                */
                static void* operator new(size_t);

            // ----- data members ---------------------------------------

            protected:
                /**
                * Gate used by the EnterBlock
                */
                mutable ThreadGate::Handle m_hGate; // on stack

                /**
                * The operation requested when the GateBlock is entered.
                */
                BlockType m_nType;
            };

    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Specify the number of unmatched completed close/barEntry calls. The
        * caller must have the gate closed/closing.
        *
        * @param cClose  the number of unmatched completed close/barEntry
        *                calls.
        */
        virtual void setCloseCount(int32_t cClose);

        /**
        * Return the thread that is closing the gate.
        *
        * @return the thread that is closing the gate
        */
        virtual Thread::View getClosingThread() const;

        /**
        * Specify the thread id that is closing the gate. The caller must be
        * synchronized on the thread gate.
        *
        * @param vThread  the thread id that is closing the gate.
        */
        virtual void setClosingThread(Thread::View vThread);

        /**
        * Update the current thread gate status, without changing the active
        * count. The caller must hold synchronization on the ThreadGate.
        *
        * @param nStatus  the new status
        *
        * @return the old status
        */
        virtual Status updateStatus(Status nStatus);

        /**
        * Wait up to the specified number of milliseconds for notification.
        * Caller must be synchronized on this gate.
        *
        * @param cMillis  the number of milliseconds to wait;; pass
        *                 infinite for forever or immediate for no wait
        *
        * @return the remaining wait time in milliseconds
        */
        virtual int64_t doWait(int64_t cMillis);

    private:
        /**
        * Perform cleanup related to returning from the close() method.
        *
        * @param  fReenter  true iff the calling thread needs to reenter the
        *                   gate
        * @param  fReopen   true iff the calling thread needs to reopen the
        *                   gate
        */
        void closeFinally(bool fReenter, bool fReopen);

        /**
        * Perform cleanup related to returning from the enter() method.
        *
        * @param hCounter  the thread-local counter
        * @param fSuccess  true iff the gate was successfully entered
        */
        void enterFinally(Counter::Handle hCounter, bool fSuccess);

        /**
        * Return the thread local counter object.
        *
        * @return the thread local counter object
        */
        Counter::Handle getCounter() const;


    // ----- data members ---------------------------------------------------

    public:
        /**
        * The maximum number of threads allowed in the gate at one time.
        */
        static const int32_t max_enters = 0x3FFFFFFF;

        /**
        * The constant representing wait forever.
        */
        static const int64_t infinite = int64_t(-1);

        /**
        * The constant representing not to wait.
        */
        static const int64_t immediate = int64_t(0);

    private:
        /**
        * The state of the ThreadGate. This counter is overloaded for two
        * responsibilities: It tracks the number of threads that have
        * entered the gate, and the status of the gate (OPEN, CLOSING
        * CLOSED or DESTROYED).
        */
        NativeAtomic32 m_nAtomicState;

        /**
        * Number of unmatched completed close/barEntry calls.
        */
        int32_t m_cClose;

        /**
        * The id of the thread that's currently closing the gate.
        */
        MemberView<Thread> m_vClosingThread;

        /**
        * Count of unmatched enter calls per thread.
        */
        mutable FinalHandle<ThreadLocalReference> f_htlcEnters;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Macro for more readable ThreadGate blocks. See the ThreadGate docs for
* more details.
*/
#define COH_GATE_ENTER(GATE, TIME) \
    if (coherence::util::ThreadGate::GateBlock COH_UNIQUE_IDENTIFIER(gateblock) =  \
            coherence::util::ThreadGate::GateBlock(                         \
                coherence::util::ThreadGate::GateBlock::enter, GATE, TIME))

/**
* Macro for more readable ThreadGate blocks. See the ThreadGate docs for
* more details.
*/
#define COH_GATE_BAR(GATE, TIME) \
    if (coherence::util::ThreadGate::GateBlock COH_UNIQUE_IDENTIFIER(gateblock) =  \
            coherence::util::ThreadGate::GateBlock(                         \
                coherence::util::ThreadGate::GateBlock::bar, GATE, TIME))
/**
* Macro for more readable ThreadGate blocks. See the ThreadGate docs for
* more details.
*/
#define COH_GATE_CLOSE(GATE, TIME) \
    if (coherence::util::ThreadGate::GateBlock COH_UNIQUE_IDENTIFIER(gateblock) =  \
            coherence::util::ThreadGate::GateBlock(                         \
                coherence::util::ThreadGate::GateBlock::close, GATE, TIME))

COH_CLOSE_NAMESPACE2

#endif // COH_THREAD_GATE_HPP
