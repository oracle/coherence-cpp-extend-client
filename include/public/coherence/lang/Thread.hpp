/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_THREAD_HPP
#define COH_THREAD_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/MemberView.hpp"
#include "coherence/lang/Runnable.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/ThreadGroup.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/Volatile.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

class TimeoutBlock;

/**
* An Object representing a thread of execution.
*
* @author mf 2007.12.10
*/
class COH_EXPORT Thread
    : public class_spec<Thread,
        extends<Object>,
        implements<Runnable> >
    {
    friend class factory<Thread>;

    // ----- State definition -----------------------------------------------

    public:
        typedef enum
            {
            state_initial     = 0, // the thread has not yet started
            state_runnable    = 1, // the thread has started
            state_interrupted = 2, // the thread has been interrupted
            state_terminated  = 3  // the thread has completed execution
            } State;


   // ----- Maintenance definition ------------------------------------------

    protected:
        /**
         * Defines various thread maintenance tasks.
         */
        typedef enum
            {
            none          = 0,
            collect_stack = 1,
            compact_tls   = 2
            } Maintenance;


    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a Thread.
        *
        * @param hRunnable    the Runnable to execute
        * @param vsName       the thread's name, or NULL for default
        * @param hGroup       the thread's group, or NULL for parent's
        * @param cBytesStack  the stack size, or 0 for default
        *
        * @return the handle to the new Thread
        */
        Thread(Runnable::Handle hRunnable,
               String::View vsName = String::null_string,
               ThreadGroup::Handle hGroup = NULL,
               size32_t cBytesStack = 0);


    // ----- Runnable interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- Thread interface -----------------------------------------------

    public:
        /**
        * Return the Thread's unique ID.
        *
        * No two live thread's will have the same ID and no thread will have
        * an id of 0.
        *
        * @return the Thread's unique ID.
        */
        virtual int64_t getId() const;

        /**
        * Return the Thread's current state.
        *
        * @return the Thread's current state
        */
        virtual State getState() const;

        /**
        * Return the Thread's name.
        *
        * @return the Thread's name
        */
        virtual String::View getName() const;

        /**
        * Return this thread's group, or NULL if the thread has terminated.
        *
        * @return this thread's group
        */
        virtual ThreadGroup::Handle getThreadGroup();

        /**
        * Return this thread's group, or NULL if the thread has terminated.
        *
        * @return this thread's group
        */
        virtual ThreadGroup::View getThreadGroup() const;

        /**
        * Tests if this thread is alive. A thread is alive if it has
        * been started and has not yet died.
        *
        * @return  <code>true</code> if this thread is alive;
        *          <code>false</code> otherwise.
        */
        virtual bool isAlive() const;

        /**
        * Begin executing the Thread's runnable on a new system thread.
        *
        * This method may only be called once per Thread instance.
        *
        * @throw IllegalStateException if a called on a Thread which is not
        *        in the state state_initial.
        */
        virtual void start();

        /**
        * Request that the Thread stop executing.
        */
        virtual void interrupt();

        /**
        * Return true iff the thread is interrupted.
        *
        * Note, Unlike Thread::interrupted() this does not check if the thread has timed-out.
        *
        * @return true iff the thread is interrupted
        */
        virtual bool isInterrupted() const;

        /**
        * Wait for the Thread to reach the state_terminated state.
        */
        virtual void join() const;

        /**
        * Wait for the Thread to reach the state_terminated state.
        *
        * @param cMillis  the maximum duration to wait, or zero for infinite
        */
        virtual void join(int64_t cMillis) const;

        /**
        * Set the Thread's name.
        *
        * @param vsName  the Thread's name
        */
        virtual void setName(String::View vsName);

    protected:
        /**
        * Called automatically on the thread prior to termination.
        *
        * Any class extending this Thread and overriding this method much
        * call this implementation to ensure that join() operations complete.
        */
        virtual void onExit();

        /**
        * Instruct the thread to perform maintenance.
        */
        virtual void scheduleMaintenance(Maintenance nMaintenance) const;


    // ----- static helper methods ------------------------------------------

    public:
        /**
        * Return the Thread on which the caller is executing.
        *
        * @return the caller's thread
        */
        static Thread::Handle currentThread();

        /**
        * Return a stack trace for this thread.
        *
        * @param cTrim  the number of frames to trim from the top of the stack
        *
        * @return an array of StackTraceElements describing as much of the
        *         thread's stack as can be determined
        */
        static ObjectArray::Handle getStackTrace(size32_t cTrim = 0);

        /**
        * Format a stack trace.
        *
        * @param vaStack  the array of StackTraceElements to output, or NULL
        *                 for the stack of the current thread
        */
        static String::View formatStackTrace(ObjectArray::View vaStack);

        /**
        * Return the stack trace of all known threads as a String.
        *
        * This is a blocking operation, while the calling thread waits for
        * all known threads to report their stack.
        *
        * @param cMillisTimeout  the maximum amount of time to wait for all
        *                        threads to report their stacks, or -1 for
        *                        the system default of twice the interrupt
        *                        resolution
        *
        * @return the stack traces
        */
        static String::View formatStacks(int64_t cMillisTimeout = -1);

        /**
        * Return true iff the caller's thread has been interrupted, or timed out.
        *
        * The thread's interrupt state is reset as part of this operation.
        *
        * @return true iff the caller's thread has been interrupted
        */
        static bool interrupted();

        /**
        * Pause the current thread for the specified duration.
        *
        * @param cMillis  the duration to pause the thread for.
        *
        * @throw InterruptedException if the thread is interrupted while
        *        sleeping.
        */
        static void sleep(int64_t cMillis);

        /**
        * Temporarily pause the current thread.
        */
        static void yield();

         /**
         * Return the number of milliseconds before the calling thread will timeout.
         *
         * Note if the current thread is timed out then invoking this method will
         * also interrupt the thread.
         *
         * @return the number of remaining milliseconds, 0 if timed out, or Integer64::max_value if disabled.
         */
        static int64_t remainingTimeoutMillis();

        /**
         * Return true if the calling thread is timed out.
         *
         * Note if the current thread is timed out then invoking this method will
         * also interrupt the thread.
         *
         * @return true if the calling thread is timed out
         */
        static bool isTimedOut();

    protected:
        /**
        * Instruct all threads to perform maintenance.
        */
        static void doGlobalMaintenance(Maintenance nMaintenance);

    private:
        /**
         * Specify the thread's timeout
         */
        void setTimeout(int64_t timeout);

        /**
         * Return the thread's timeout.
         */
        int64_t getTimeout() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        *
        * If the associated Runnable implements the
        * coherence::util::Describable interface that content will be included
        * as well.
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The thread's group.
        */
        MemberHandle<ThreadGroup> m_hGroup;

        /**
        * The inner runnable associated with this Thread.
        */
        MemberHandle<Runnable> m_hRunnable;

        /**
        * The thread's name.
        */
        MemberView<String> m_vsName;

        /**
        * The thread's description.
        */
        FinalView<String> f_vsDescription;

        /**
        * The configured stack size;
        */
        size32_t m_cBytesStack;

        /**
        * The current thread state.
        */
        Volatile<int32_t> m_nState;

        /**
        * Bit mask identifying required maintenance tasks.
        */
        mutable int m_nMaintenanceMask;

        /**
         * The thread's timeout value. Value which are greater or equal to zero are used to indicate
         * timeout timestamps.  Negative values are relative timeouts which haven't yet been
         * realized into a timestamp.  This allows for an optimization where we can avoid obtaining
         * the current time when "setting" the timeout, and defer it until we are about to block.
         */
        int64_t m_lTimeout;


    // ----- friends --------------------------------------------------------

    friend void    coh_thread_cleanup(Thread*);
    friend Thread* coh_thread_current();
    friend class System;
    friend class TimeoutBlock;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_THREAD_HPP
