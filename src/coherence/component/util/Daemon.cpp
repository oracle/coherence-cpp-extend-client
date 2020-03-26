/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/Daemon.hpp"

#include "private/coherence/component/run/EventDeathException.hpp"
#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::run::EventDeathException;
using coherence::util::logging::Logger;


// ----- constructor --------------------------------------------------------

Daemon::Daemon()
    : m_nDaemonState(self(), daemon_initial),
      f_vExitMonitor(self(), Object::create()),
      m_fNotification(false),
      m_ohExceptionStart(self()),
      m_lStartTimeStamp(0),
      m_hThread(self()),
      f_hThreadGroup(self()),
      m_cWaitMillis(0),
      f_hLock(self()),
      m_vsThreadName(self())
    {
    }


// ----- Daemon interface ---------------------------------------------------

Daemon::DaemonStatus Daemon::getDaemonState() const
    {
    return static_cast<Daemon::DaemonStatus>((int32_t) m_nDaemonState);
    }

bool Daemon::isExiting() const
    {
    return getDaemonState() == daemon_exiting;
    }

Object::Handle Daemon::getLock()
    {
    Object::Handle hLock = f_hLock;
    return NULL == hLock ? this : hLock;
    }

void Daemon::setLock(Object::Handle hLock)
    {
    initialize(f_hLock, hLock);
    }

int64_t Daemon::getWaitMillis() const
    {
    return m_cWaitMillis;
    }

void Daemon::setWaitMillis(int64_t cMillis)
    {
    m_cWaitMillis = cMillis;
    }

bool Daemon::isNotification() const
    {
    return m_fNotification;
    }

void Daemon::setNotification(bool fNotify)
    {
    Object::Handle hLock = getLock();
    COH_SYNCHRONIZED(hLock)
        {
        m_fNotification = fNotify;

        if (fNotify)
            {
            Thread::Handle hThread = getThread();
            if (NULL != hThread && hThread != Thread::currentThread())
                {
                hLock->notifyAll();
                }
            }
        }
    }

bool Daemon::isStarted() const
    {
    DaemonStatus nState = getDaemonState();
    return (nState > daemon_initial) && ( nState < daemon_exited);
    }

Exception::Holder Daemon::getStartException() const
    {
    return m_ohExceptionStart;
    }

void Daemon::setStartException(Exception::Holder ohe)
    {
    m_ohExceptionStart = ohe;
    }

int64_t Daemon::getStartTimeStamp() const
    {
    return m_lStartTimeStamp;
    }

Thread::Handle Daemon::getThread()
    {
    return m_hThread;
    }

Thread::View Daemon::getThread() const
    {
    return m_hThread;
    }

void Daemon::setThreadGroup(ThreadGroup::Handle hGroup)
    {
    initialize(f_hThreadGroup, hGroup);
    }

ThreadGroup::Handle Daemon::getThreadGroup()
    {
    return f_hThreadGroup;
    }

ThreadGroup::View Daemon::getThreadGroup() const
    {
    return f_hThreadGroup;
    }

String::View Daemon::getThreadName() const
    {
    String::View vsThreadName = m_vsThreadName;
    return NULL == vsThreadName ? Class::getSimpleClassName(this) : vsThreadName;
    }

void Daemon::setThreadName(String::View vsThread)
    {
    m_vsThreadName = vsThread;
    }

bool Daemon::join(int64_t cMillis)
    {
    try
        {
        Thread::Handle hThread = getThread();
        if (hThread != NULL)
            {
            hThread->join(cMillis);
            return !hThread->isAlive();
            }
        return true;
        }
    catch (InterruptedException::View /* ve */)
        {
        Thread::currentThread()->interrupt();
        return false;
        }
    }

void Daemon::start()
    {
    COH_SYNCHRONIZED(this)
        {
        if (isStarted())
            {
            return;
            }

        if (getDaemonState() == daemon_exited)
            {
            COH_THROW (IllegalStateException::create("Daemon is not restartable"));
            }

        Thread::Handle hThread = Thread::create(this, getThreadName(),
                getThreadGroup());
        m_hThread = hThread;

        setStartException(NULL);
        setDaemonState(daemon_starting);
        hThread->start();

        do
            {
            wait(1000);
            }
        while (getDaemonState() < daemon_running && hThread->isAlive());

        Exception::Holder e = getStartException();
        if (e != NULL)
            {
            setStartException(NULL);
            COH_THROW (Exception::create(String::null_string, e));
            }

        if (getDaemonState() == daemon_starting)
            {
            // daemon thread failed to start or capture exception
            // the most likely cause of this condition is a reference equality failure
            // see run()
            COH_THROW_STREAM(IllegalStateException, "daemon thread start failed: " << hThread);
            }
        }
    }

void Daemon::stop()
    {
    // Once setExiting(true) is invoked the daemon’s thread will attempt to clear any interrupts and then proceed to onExit.
    // In order to ensure that this doesn’t occur before we actually get to interrupt the thread we synchronize this method
    // as well as run’s call to clear the interrupt.
    COH_SYNCHRONIZED(f_vExitMonitor)
        {
        // only go through stop() once to prevent spurious interrupts during onExit()
        if (!isExiting())
            {
            setExiting();

            Thread::Handle hThread = getThread();
            if (NULL != hThread && hThread != Thread::currentThread())
                {
                try
                    {
                    hThread->interrupt();
                    }
                catch (Exception::View)
                    {
                    }
                }
            }
        }
    }

bool Daemon::sleep(int64_t lMillis)
    {
    try
        {
        if (lMillis == 0)
            {
            Thread::currentThread()->yield();
            }
        else
            {
            Thread::currentThread()->sleep(lMillis);
            }
        return true;
        }
    catch (InterruptedException::View)
        {
        return false;
        }
    }

void Daemon::onEnter()
    {
    setStartTimeStamp(System::currentTimeMillis());
    }

void Daemon::onException(Exception::Holder ohe)
    {
    if (isExiting())
        {
        COH_LOG("Daemon: caught an unhandled exception  (" << Class::getClassName(ohe) <<
                ": " << ohe->getMessage() << ") while exiting.", 4);
        }
    else
        {
        COH_LOG("Daemon terminating due to unhandled exception: (" <<
                Class::getClassName(ohe) << ": " << ohe->getMessage() << ").", 1);
        stop();
        }
    }

void Daemon::onExit()
    {
    }

void Daemon::onInterrupt(InterruptedException::Holder ohe)
    {
    if (!isExiting())
        {
        COH_LOG("Daemon Interrupted, " << Thread::currentThread() << ": " <<
                ohe->getMessage(), 1);
        }
    }

void Daemon::onNotify()
    {
    }

void Daemon::onUnexpectedThread()
    {
    COH_THROW (IllegalStateException::create(
            "Daemon is running on an unexpected thread"));
    }

void Daemon::onWait()
    {
    if (!isNotification())
        {
        Object::Handle hLock = getLock();
        COH_SYNCHRONIZED(hLock)
            {
            if (!isNotification())
                {
                int64_t cWait = getWaitMillis();
                if (cWait >= 0)
                    {
                    hLock->wait(cWait);
                    }
                }
            setNotification(false);
            }
        }
    }

void Daemon::setDaemonState(DaemonStatus nState)
    {
    COH_SYNCHRONIZED(this)
        {
        if (nState > getDaemonState())
            {
            m_nDaemonState = nState;
            }
        }
    }

void Daemon::setExiting()
    {
    setDaemonState(daemon_exiting);
    }

void Daemon::setStartTimeStamp(int64_t lMillis)
    {
    m_lStartTimeStamp = lMillis;
    }

void Daemon::setThread(Thread::Handle hThread)
    {
    m_hThread = hThread;
    }


// ----- Runnable interface -------------------------------------------------

void Daemon::run()
    {
    if (Thread::currentThread() != getThread())
        {
        onUnexpectedThread();
        }

    struct RunFinally
        {
        RunFinally(Daemon::Handle hDaemon)
            : hThis(hDaemon)
            {
            }
        ~RunFinally()
            {
            try
                {
                hThis->runFinally();
                }
            catch (Exception::View e)
                {
                COH_LOG("Error exiting run: " << e, -1);
                // eat it
                }
            }
        Daemon::Handle hThis;
        } finally(this);

    try
        {
        onEnter();
        }
    catch (Exception::View ve)
        {
        setStartException(ve);
        onException(ve);
        }

    // the starting thread is waiting for this thread (see start())
    COH_SYNCHRONIZED(this)
        {
        setDaemonState(daemon_running);
        notifyAll();
        }

    while (!isExiting())
        {
        try
            {
            onWait();
            if (!isExiting())
                {
                onNotify();
                }
            }
        catch (EventDeathException::View)
            {
            // a "normal" exception to "get out of" an event
            }
        catch (InterruptedException::View e)
            {
            onInterrupt(e);
            }
        catch (Exception::View e)
            {
            onException(e);
            }
        }
    }

void Daemon::runFinally()
    {
    COH_SYNCHRONIZED(this)
        {
        struct Finally
            {
            Finally(Daemon::Handle hDaemon)
                : hThis(hDaemon)
                {
                }

            ~Finally()
                {
                try
                    {
                    hThis->setThread(NULL);
                    hThis->setDaemonState(daemon_exited);

                    // just in case anyone is still waiting on this (in the case that onEnter
                    // threw an exception and then onException threw an exception, the thread
                    // that called start() will still be waiting)
                    hThis->notifyAll();
                    }
                catch (Exception::View e)
                    {
                    COH_LOG("Error terminating thread: " << e, -1);
                    // eat it
                    }
                }

            Daemon::Handle hThis;
            }finnally(this);

        try
            {
            // see comment in stop()
            COH_SYNCHRONIZED(f_vExitMonitor)
                {
                Thread::interrupted(); // clear interrupt flag
                }

            onExit();
            }
        catch (Exception::View e)
            {
            onException(e);
            }
        }
    // the thread terminates right here
    }


// ----- Describable interface ------------------------------------------

String::View Daemon::getDescription() const
    {
    String::View vsState;
    switch(getDaemonState())
        {
        case daemon_initial:
            vsState = "daemon_initial";
            break;
        case daemon_starting:
            vsState = "daemon_starting";
            break;
        case daemon_running:
            vsState = "daemon_running";
            break;
        case daemon_exiting:
            vsState = "daemon_exiting";
            break;
        case daemon_exited:
            vsState = "daemon_exited";
            break;
        }

    return COH_TO_STRING(super::getDescription() << "Daemon{State=" << vsState <<
           ", Notification=" << Boolean::toString(isNotification()) <<
           ", StartTimeStamp=" << getStartTimeStamp() <<
           ", WaitTime=" << getWaitMillis() <<
           ", ThreadName=" << getThreadName() <<
           "}");
    }

COH_CLOSE_NAMESPACE3
