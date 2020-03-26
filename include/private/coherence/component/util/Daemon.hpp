/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DAEMON_HPP
#define COH_DAEMON_HPP

#include "coherence/lang.ns"

#include "coherence/util/Describable.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::util::Describable;


/**
* This class is used to create and manage a daemon thread.
*
* A caller may use the following methods to control the Daemon component:
* <ol>
* <li><tt>start</tt> - creates and starts the daemon thread</li>
* <li><tt>isStarted</tt> - determines whether the daemon is running</li>
* <li><tt>stop</tt> - stops the daemon thread and releases the related
* resources</li>
* </ol>
*
* Advanced options available to a designer or caller include:
* <ol>
* <li><tt>ThreadName</tt> - before starting the daemon, a Thread name can be
* provided</li>
* <li><tt>Thread</tt> - the actual Thread object can be accessed via this
* property</li>
* <li><tt>StartException</tt> - if the start method fails to start the
* daemon, the StartException property provides the failure information</li>
* </ol><br>
* The daemon thread itself executes the following events while it is running:
* <br><ol>
* <li><tt>onEnter</tt> - invoked when the daemon starts</li>
* <li><tt>onWait</tt> - invoked to wait for notification</li>
* <li><tt>onNotify</tt> - invoked when a notification occurs</li>
* <li><tt>onInterrupt</tt> - invoked when the thread is interrupted when
* waiting for a notification</li>
* <li><tt>onException</tt> - invoked when an exception occurs while invoking
* one of the daemon events</li>
* <li><tt>onExit</tt> - invoked before the daemon exits</li>
* </ol>
*
* @author nsa 2008.01.18
*/
class COH_EXPORT Daemon
    : public class_spec<Daemon,
        extends<Describable>,
        implements<Runnable> >
    {
    friend class factory<Daemon>;

    // ----- enums ----------------------------------------------------------

    public:
        /**
        * Enumeration comprised of the different daemon states
        */
        typedef enum
            {
            daemon_initial,
            daemon_starting,
            daemon_running,
            daemon_exiting,
            daemon_exited
            } DaemonStatus;


    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new Daemon.
        */
        Daemon();


    // ----- Daemon interface -----------------------------------------------

    public:
        /**
        * Return the current state of the daemon.
        *
        * @return the current state of the daemon
        */
        virtual DaemonStatus getDaemonState() const;

        /**
        * Return whether the daemon is exiting .
        *
        * @return true iff the daemon is in the exiting state
        */
        virtual bool isExiting() const;

        /**
        * Return the lock associated with this daemon.
        *
        * @return the lock associated with this daemon
        */
        virtual Object::Handle getLock();

        /**
        * Set the lock associated with this daemon
        *
        * @param hLock  The lock associated with this daemon
        */
        virtual void setLock(Object::Handle hLock);

        /**
        * Returns the number of milliseconds that the daemon will wait for
        * notification. Zero means to wait indefinitely. Negative value
        * means to skip waiting altogether.
        *
        * @return the number of milliseconds that the daemon will wait for
        *         notification
        */
        virtual int64_t getWaitMillis() const;

        /**
        * Setter for property WaitMillis.
        *
        * The number of milliseconds that the daemon will wait for notification.
        * Zero means to wait indefinitely. Negative value means to skip waiting
        * altogether.
        */
        virtual void setWaitMillis(int64_t cMillis);

        /**
        * Return whether the daemon has a notification.
        *
        * @return true iff the daemon has a notification
        */
        virtual bool isNotification() const;

        /**
        * Set the daemon to be notified.
        *
        * @param fNotify  whether the daemon should be notified or not
        */
        virtual void setNotification(bool fNotify = true);

        /**
        * Return whether the daemon has been started or not.
        *
        * @return whether the daemon is started or not
        */
        virtual bool isStarted() const;

        /**
        * Return the start exception if there was one.
        *
        * @return the start exception or null if there was not one
        */
        virtual Exception::Holder getStartException() const;

        /**
        * Set the start exception for this daemon.
        *
        * @param ohe  the start exception for this daemon
        */
        virtual void setStartException(Exception::Holder ohe);

        /**
        * Return the start time for the daemon.
        *
        * @return int64_t the start time for the daemon
        */
        virtual int64_t getStartTimeStamp() const;

        /**
        * Return the worker thread for this daemon.
        *
        * @return the worker thread for this daemon
        */
        virtual Thread::Handle getThread();

        /**
        * Return the worker thread for this daemon.
        *
        * @return the worker thread for this daemon
        */
        virtual Thread::View getThread() const;

        /**
        * Specifies the ThreadGroup within which the daemon thread will be
        * created. If not specified, the current Thread's ThreadGroup will be
        * used.
        *
        * This property can only be set at runtime, and must be configured
        * before start() is invoked to cause the daemon thread to be created
        * within the specified ThreadGroup.
        *
        * @param hGroup  the group to use
        */
        virtual void setThreadGroup(ThreadGroup::Handle hGroup);

        /**
        * Return the ThreadGroup for the Daemon.
        *
        * @return the ThreadGroup for the Daemon.
        */
        virtual ThreadGroup::Handle getThreadGroup();

        /**
        * Return the ThreadGroup for the Daemon.
        *
        * @return the ThreadGroup for the Daemon.
        */
        virtual ThreadGroup::View getThreadGroup() const;

        /**
        * Return the name of the thread that the daemon will create.
        *
        * @return the name of the thread that the daemon will create
        */
        virtual String::View getThreadName() const;

        /**
        * Set the name of the thread that the daemon will create.
        *
        * @param vsThread  the name of the thread to create
        */
        virtual void setThreadName(String::View vsThread);

        /**
        * Wait for the daemon's thread to stop.
        *
        * @param  cMillis the number of milliseconds to wait for, or zero for infinite
        *
        * @return true iff the thread is no longer running
        *
        * @since Coherence 3.7.1.13
        */
        virtual bool join(int64_t cMillis);

        /**
        * Starts the daemon thread associated with this component. If the
        * thread is already starting or has started, invoking this method
        * has no effect.
        *
        * Synchronization is used here to verify that the start of the
        * thread occurs; the lock is obtained before the thread is started,
        * and the daemon thread notifies back that it has started from the
        * run() method.
        *
        * Note: This method is synchronized.
        *
        * @throws coherence::lang::IllegalStateException
        */
        virtual void start();

        /**
        * Stops the daemon thread associated with this component.
        */
        virtual void stop();

        /**
        * Causes the current thread to sleep for the specified interval.  If
        * interrupted while sleeping the interrupt flag will be set and sleep
        * will return false.
        *
        * @return true if the thread slept, or false if its sleep was
        * interrupted.
        */
        virtual bool sleep(int64_t lMillis);

    protected:
        /**
        * Event notification called once the daemon's thread starts and
        * before the daemon thread goes into the "wait - perform" loop.
        * This method executes on the daemon's thread.
        *
        * Note1: this method is called while the caller's thread is still
        * waiting for a notification to  "unblock" itself.
        *
        * Note2: any exception thrown by this method will terminate the
        * thread immediately
        */
        virtual void onEnter();

        /**
        * This event occurs when an exception is thrown from onEnter,
        * onWait, onNotify and onExit. If the exception should terminate the
        * daemon, call stop(). The default implementation prints debugging
        * information and terminates the daemon.
        *
        * @param ohe  the Exception that was thrown
        */
        virtual void onException(Exception::Holder ohe);

        /**
        * This event occurs when an exception is thrown from onEnter,
        * onWait, onNotify and onExit.
        *
        * If the exception should terminate the daemon, call stop(). The
        * default implementation prints debugging information and terminates
        * the daemon.
        *
        * @param e  the Exception that was thrown
        */
        virtual void onExit();

        /**
        * Event notification called if the daemon's thread get interrupted.
        *
        * @param ohe  the exception that interrupted the daemon
        *
        * @see #stop
        */
        virtual void onInterrupt(InterruptedException::Holder ohe);

        /**
        * Event notification to perform a regular daemon activity. To get it
        * called, another thread has to set Notification to true:
        * <code>
        * daemon.setNotification(true);
        * </code>
        *
        * @see #onWait
        */
        virtual void onNotify();

        /**
        * This event occurs when the daemon's run() method is called on an
        * unexpected thread.
        *
        * @throws coherence::lang::IllegalStateException
        */
        virtual void onUnexpectedThread();

        /**
        * Event notification called when  the daemon's Thread is waiting for
        * work.
        *
        * @see run
        */
        virtual void onWait();

        /**
        * Change the daemon's state to the specified state iff the new state
        * is greater than the current state.
        *
        * Despite this property being Volatile, this setter is synchronized
        * to ensure forward only state transitions.  Additionally this
        * allows for queries of the state to be held stable by synchronizing
        * before the get and the corresponding usage.
        *
        * @param nState  the state to set the daemon to
        */
        virtual void setDaemonState(DaemonStatus nState);

        /**
        * Set the daemon into the exiting state.
        */
        virtual void setExiting();

        /**
        * Set the start time stamp for the daemon.
        *
        * @param lMillis  The start time for the daemon
        */
        virtual void setStartTimeStamp(int64_t lMillis);

        /**
        * Set the worker thread for the daemon.
        *
        * @param hThread  The worker thread for the daemon
        */
        virtual void setThread(Thread::Handle hThread);


    // ----- Runnable interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- Describable interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * @internal
        */
        virtual void runFinally();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Specifies the state of the daemon (daemon_initial, daemon_starting,
        * daemon_running, daemon_exiting, daemon_exited). The state is not
        * allowed to regress, thus the ordering of the daemon state values
        * is meaningful.
        *
        * @see #setDaemonState
        * volatile - @see #stop
        */
        Volatile<int32_t> m_nDaemonState;

        /**
         * Monitor object to coordinate clearing the thread interrupt set by stop prior to running onExit().
         *
         * @since 12.2.1.0.7
         */
        FinalView<Object> f_vExitMonitor;

        /**
        * Specifies whether there is work for the daemon to do; if there is
        * work, m_fNotification must evaluate to true, and if there is no work
        * (implying that the daemon should wait for work) then m_fNotification
        * must evaluate to false. To verify that a wait is necessary, the
        * monitor on the Lock property is first obtained and then
        * m_fNotification is evaluated; only if m_fNotification evaluates to
        * false will the daemon go into a wait state on the Lock property. To
        * unblock (notify) the daemon, another thread should set
        * Notification to true.
        */
        bool m_fNotification;

        /**
        * The exception (if any) that prevented the daemon from starting
        * successfully.
        */
        MemberHolder<Exception> m_ohExceptionStart;

        /**
        * Date/time value that this Daemon's thread has started.
        */
        int64_t m_lStartTimeStamp;

        /**
        * The daemon thread if it is running, or null before the daemon
        * starts and after the daemon stops.
        */
        MemberHandle<Thread> m_hThread;

        /**
        * The ThreadGroup for the Daemon's thread.
        */
        FinalHandle<ThreadGroup> f_hThreadGroup;

        /**
        * The number of milliseconds that the daemon will wait for
        * notification. Zero means wait indefinitely. Negative value means
        * to skip waiting altogether.
        */
        int64_t m_cWaitMillis;

        /**
        * An object that serves as a mutex for this Daemon synchronization.
        * When idle, the Daemon is waiting for a notification on the Lock
        * object.
        */
        FinalHandle<Object> f_hLock;

        /**
        * The name of the thread that daemon will create.
        */
        MemberView<String> m_vsThreadName;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_DAEMON_HPP
