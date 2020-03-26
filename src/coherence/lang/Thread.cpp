/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Thread.hpp"

#include "coherence/lang.ns"

#include "coherence/io/InterruptedIOException.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/SafeHashMap.hpp"

#include "private/coherence/native/NativeBacktrace.hpp"
#include "private/coherence/native/NativePID.hpp"
#include "private/coherence/native/NativeThread.hpp"
#include "private/coherence/native/NativeThreadLocal.hpp"
#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

using namespace coherence::native;

using coherence::io::InterruptedIOException;
using coherence::util::Describable;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::SafeHashMap;
using coherence::util::StringHelper;


// ----- static helpers -----------------------------------------------------

namespace
    {
    /**
    * Counter used for making unique thread names.
    *
    * The counter is updated in a non-thread safe manor which could lead to
    * multiple unamed threads being given the same numeric id.  As names are
    * not required to be unique this is allowable.
    */
    static int32_t s_cThreadName = 0;
    }

/**
* Perform thread cleanup tasks related to thread termination.
*/
void coh_thread_cleanup(Thread* pThread)
    {
    pThread->onExit();
    }

/**
* Called when each thread exits, this function initiates cleanup, and performs
* a detach on the Thread Object associated with the thread.
*
* @param pvThread
*/
extern "C" void coh_thread_detach(void* pvThread)
    {
    Thread* pThread = (Thread*) pvThread;
    coh_thread_cleanup(pThread);
    pThread->_detach(/*fEscaped*/ false);
    }

/**
* A no-op runnable used for native threads.
*/
class NullRunnable
    : public class_spec<NullRunnable,
        extends<Describable>,
        implements<Runnable> >
    {
    friend class factory<NullRunnable>;

    public:
        virtual void run() {}

        virtual String::View getDescription() const
            {
            return "<native>";
            }
    };

Thread* coh_thread_current(); //forward decl

/**
* Helper class for initializing and cleaning up the TLS used to store the
* current thread.
*/
class TLSManager
    {
    public:
        TLSManager()
            {
            // all the various NULL checks are protecting against a compiler bug (SunPro)
            // in which static initialization runs multiple times

            if (s_pTLS == NULL)
                {
                // here we create a non-sanka object and don't need to worry
                // that we could recurse
                s_pTLS = NativeThreadLocal::create(&coh_thread_detach);
                }

            // any of the following sanka Object creations could recurse back
            // into this method and thus we ensure we don't overwrite our statics
            Map::Handle hMap = SafeHashMap::create();
            if (s_pMap == NULL)
                {
                hMap->_attach(/*fEscaped*/ true);
                s_pMap = get_pointer(hMap);
                }

            Runnable::Handle hRunnable = NullRunnable::create();
            if (s_pRunnableNative == NULL)
                {
                hRunnable->_attach(/*fEscaped*/ true);
                s_pRunnableNative = get_pointer(hRunnable);
                }

            ThreadGroup::Handle hGroup = ThreadGroup::create("system");
            if (s_pGroupSystem == NULL)
                {
                hGroup->_attach(/*fEscaped*/ true);
                s_pGroupSystem = get_pointer(hGroup);
                }
            }

        ~TLSManager()
            {
            // The Thread object for this thread and any other live threads which
            // have access coherence will be leaked, but this is a small amount
            // of memory, additionally the NullRunnable and root ThreadGroup
            // are intentionally leaked

            if (s_pThreadShutdown == NULL)
                {
                s_pThreadShutdown = coh_thread_current();

                s_pMap->_detach(/*fEscaped*/ true);
                s_pMap = NULL;

                // free up the TLS (a very limited resource)
                delete s_pTLS;
                s_pTLS = NULL;
                }
            // else; static destruction already ran; this is a compiler bug (SunPro)
            }

        /**
        * Return the NativeThreadLocal used to find the Thread associated with
        * the calling thread.
        *
        * @return the NativeThreadLocal
        */
        static NativeThreadLocal* getTLS()
            {
            ensure();
            return s_pTLS;
            }

        /**
        * Return a map of known threads.
        *
        * The keys are Thread::Views, and the values are usually NULL. The value
        * portion is used as part of Thread::dumpStacks, in which case they are
        * ObjectArrays containing the corresponding thread's stack.
        */
        static Map::Handle getThreadMap()
            {
            ensure();
            return s_pMap;
            }

        /**
        * Return the shutdown Thread.
        */
        static Thread* getShutdownThread()
            {
            ensure();
            return s_pThreadShutdown;
            }

        /**
        * Return the Runnable for representing all native threads.
        */
        static Runnable* getNativeRunnable()
            {
            ensure();
            return s_pRunnableNative;
            }

        /**
        * Return the root "system" thread group.
        */
        static ThreadGroup* getSystemGroup()
            {
            ensure();
            return s_pGroupSystem;
            }

        /**
        * Ensure the TLSManager has been initialized.
        */
        static void ensure()
            {
            static TLSManager singleton;
            }

    private:
        static NativeThreadLocal* s_pTLS;
        static Map*               s_pMap;
        static Thread*            s_pThreadShutdown;
        static Runnable*          s_pRunnableNative;
        static ThreadGroup*       s_pGroupSystem;
    };
NativeThreadLocal* TLSManager::s_pTLS            = NULL;
Map*               TLSManager::s_pMap            = NULL;
Thread*            TLSManager::s_pThreadShutdown = NULL;
Runnable*          TLSManager::s_pRunnableNative = NULL;
ThreadGroup*       TLSManager::s_pGroupSystem    = NULL;
// TLS must be initialized at library load time
static const bool s_tls_init = (TLSManager::ensure(), true);

extern void coh_tls_compact(); // see ThreadLocalReference.cpp

/**
* Return the Thread object representing the current thread of execution.
*/
Thread* coh_thread_current()
    {
    NativeThreadLocal* pTLS  = TLSManager::getTLS();
    Thread*            pSelf = pTLS == NULL
            ? TLSManager::getShutdownThread()
            : (Thread*) pTLS->get();

    if (NULL == pSelf)
        {
        // This indicates that we are running on a non-Coherence thread;
        // create a Thread to represent it.
        Thread::Handle hThread = Thread::create(
                TLSManager::getNativeRunnable(),
                s_cThreadName == 0 ? "main" : (char*) NULL,
                TLSManager::getSystemGroup());

        // it is possible that the creation of the Thread object actually
        // re-entered this call, and in that case a Thread was created and
        // associated with this thread already.  This can happen
        // occasionally as Object's constructor does a periodic interrupt
        // check which involves looking up the thread.
        pSelf = (Thread*) pTLS->get();
        if (NULL == pSelf)
            {
            pSelf = (Thread*) get_pointer(hThread);
            hThread->m_nState = Thread::state_runnable;
            hThread->run(); // for <native> performs registration
            ++s_cThreadName; // ensures new native threads won't be named "main"
            }
        }
    else
        {
        // any time the running thread asks for it's Thread we can cheaply
        // check if maintenance is required since we already have the Thread
        int nMaint;

        // synchronized member read
            {
            SynchronizedMemberReadBlock lock(*pSelf);
            nMaint = pSelf->m_nMaintenanceMask;
            }

        if (nMaint != Thread::none)
            {
            if (nMaint & Thread::collect_stack)
                {
                Map::Handle hMap = TLSManager::getThreadMap();
                hMap->put(Thread::View(pSelf), Thread::getStackTrace(/*cTrim*/ 2));
                if (!hMap->containsValue(NULL))
                    {
                    COH_SYNCHRONIZED (hMap)
                        {
                        hMap->notify();
                        }
                    }
                }
            if (nMaint & Thread::compact_tls)
                {
                coh_tls_compact();
                }

            // synchronized member write
                {
                SynchronizedMemberWriteBlock lock(*pSelf);
                pSelf->m_nMaintenanceMask &= ~nMaint;
                }
            }
        }

    return pSelf;
    }
COH_STATIC_INIT(coh_thread_current()); // triggers labeling of "main" thread


/**
* Return the default stack size for threads.  A value of zero indicates to
* use the OSs default.
*/
size32_t coh_get_thread_default_stack_size()
    {
    static size32_t cBytesDefault = (size32_t) StringHelper::parseMemorySize(
            System::getProperty("coherence.thread.defaultstacksize",
                    "0"));
    return cBytesDefault;
    }
COH_STATIC_INIT(coh_get_thread_default_stack_size());

/**
* Called during object creation and destruction to periodically test if
* a thread dump has been requested. This is used to identify "stuck"
* threads which don't ever call wait(). It is important that this method
* only be called from other relatively expensive methods.
*/
void coh_checkThreadDumpRequest()
    {
    // periodically test for thread dump request; frequency is relative to
    // interrupt resolution
    static uint64_t cIters = 5000; // not synchronized, fuzzy value
    if ((++cIters % (System::getInterruptResolution() * 100) == 0))
        {
        // perform rarely as getting the currentThread is a bit expensive
        // Note: simply obtaining the current thread will make the check, so
        // we don't actually need to make the call
        Thread::currentThread();
        }
    }

// ----- constructors -------------------------------------------------------

Thread::Thread(Runnable::Handle hRunnable, String::View vsName,
    ThreadGroup::Handle hGroup, size32_t cBytesStack)
    : m_hGroup(self(), NULL == hGroup
            ? currentThread()->getThreadGroup()
            : hGroup),
      m_hRunnable(self(), hRunnable),
      m_vsName(self(), NULL == vsName
              ? (String::View) COH_TO_STRING("Thread-" << s_cThreadName++)
              : vsName),
      f_vsDescription(self(), instanceof<Describable::View>(hRunnable)
              ? (String::View) String::null_string
              : (String::View) COH_TO_STRING(Class::getClassName(hRunnable)
                  << "@" << (void*) (get_pointer(hRunnable)))),
      m_cBytesStack(cBytesStack),
      m_nState(self(), state_initial),
      m_nMaintenanceMask(Thread::none),
      m_lTimeout(Integer64::max_value)
    {
    COH_ENSURE_PARAM(hRunnable);
    }


// ----- Runnable interface -------------------------------------------------

void Thread::run()
    {
    // validate that we haven't been called on another thread
    NativeThreadLocal* pTLS = TLSManager::getTLS();
    if (pTLS != NULL && pTLS->get() == NULL)
        {
        try
            {
            Runnable::Handle hRunnable = m_hRunnable;

            if (!instanceof<Describable::View>(hRunnable))
                {
                // once run we no longer need to retain a reference to the
                // Runnable, detact our escaped reference to it allowing it
                // to potentially be captured
                m_hRunnable = NULL;

                if (instanceof<NullRunnable::View>(hRunnable))
                    {
                    // <native> threads don't pass through start(), so we
                    // register with the group here
                    getThreadGroup()->add(this);
                    }
                hRunnable->_isEscaped(); // attempt capture
                }

            pTLS->set(this);
            _attach(/*fEscaped*/ false); // see coh_thread_detach
            TLSManager::getThreadMap()->put(this, NULL);

            hRunnable->run();
            }
        catch (InterruptedException::View e)
            {
            // this is not an error, allow thread to silently exit, don't rethrow
            COH_LOG (*this << "\n\tExiting thread interrupted by: " << e, 6);
            }
        catch (InterruptedIOException::View e)
            {
            // this is not an error, allow thread to silently exit, don't rethrow
            COH_LOG (*this << "\n\tExiting thread interrupted by: " << e, 6);
            }
        catch (Exception::View e)
            {
            COH_LOG (*this << "\n\tTerminating with unhandled Exception: " << e, -2);
            COH_THROW (e);
            }
        catch (const std::exception& e)
            {
            COH_LOG (*this << "\n\tTerminating with unhandled std::exception: "
                << e.what(), -2);
            throw;
            }
        catch (...)
            {
            COH_LOG (*this << "\n\tTerminating with unknown exception.", -1);
            throw;
            }
        }
    else
        {
        // Thread is simply being run as a Runnable, rather then being started
        m_hRunnable->run();
        }
    }


// ----- Thread interface ---------------------------------------------------

int64_t Thread::getId() const
    {
    // Note if this assertion fails, the getId functionality will need to
    // migrate to a counter based approach.
    COH_STATIC_ASSERT(sizeof(void *) <= sizeof(int64_t));
    return reinterpret_cast<int64_t>(this);
    }

Thread::State Thread::getState() const
    {
    return State((int32_t) m_nState);
    }

String::View Thread::getName() const
    {
    return m_vsName;
    }

ThreadGroup::Handle Thread::getThreadGroup()
    {
    return m_hGroup;
    }

ThreadGroup::View Thread::getThreadGroup() const
    {
    return m_hGroup;
    }

bool Thread::isAlive() const
    {
    Thread::State nState = getState();
    return nState > state_initial && nState < state_terminated;
    }

void Thread::start()
    {
    // before going multi-threaded ensure that we've run all our static
    // initializers
    System::loadLibrary(NULL);

    Thread::Handle hThis = this;
    COH_SYNCHRONIZED (hThis)
        {
        COH_ENSURE_EQUALITY(m_nState, state_initial);
        m_nState = state_runnable;

        // the thread must be added to its group before having been started
        // this accomplishes two things:
        // - allows a thread->start(); thread->getGroup()->interrupt(); to
        //   function as expected without a race condition of thread startup
        // - escapes the Thread object prior to the new thread accessing it;
        //   the escape is guaranteed as any any active group must descend
        //   from the "system" group, which is a global and therefore escaped
        getThreadGroup()->add(this);

        try
            {
            size32_t cBytesStack = m_cBytesStack;
            NativeThread::create(hThis, cBytesStack == 0
                    ? coh_get_thread_default_stack_size() : cBytesStack);
            }
        catch (...)
            {
            onExit();
            throw;
            }
        }
    }

void Thread::interrupt()
    {
    COH_SYNCHRONIZED (this)
        {
        if (m_nState < state_interrupted)
            {
            m_nState = state_interrupted;
            }
        }
    }

bool Thread::isInterrupted() const
    {
    return getState() == state_interrupted;
    }

void Thread::join() const
    {
    join(0);
    }

void Thread::join(int64_t cMillis) const
    {
    if (m_nState != state_terminated)
        {
        COH_SYNCHRONIZED (this)
            {
            do
                {
                if (m_nState == state_terminated)
                    {
                    return;
                    }
                wait(cMillis);
                }
            while (cMillis == 0);
            // only in the case of cMillis == 0 do we loop
            }
        }
    }

void Thread::setName(String::View vsName)
    {
    m_vsName = vsName;
    }

void Thread::onExit()
    {
    Thread::Handle hThis = this;
    TLSManager::getThreadMap()->remove(hThis);
    getThreadGroup()->remove(hThis);
    COH_SYNCHRONIZED (hThis)
        {
        m_nState = state_terminated;
        m_hGroup = NULL;
        notifyAll();
        }
    }

void Thread::scheduleMaintenance(Maintenance nMaintenance) const
    {
    COH_SYNCHRONIZED_MEMBER_WRITE
        {
        m_nMaintenanceMask |= nMaintenance;
        }
    }

void Thread::doGlobalMaintenance(Maintenance nMaintenance)
    {
    Map::Handle hMapThread = TLSManager::getThreadMap();
    COH_SYNCHRONIZED (hMapThread)
        {
        for (Iterator::Handle hIter = hMapThread->entrySet()->iterator();
                hIter->hasNext(); )
            {
            Map::Entry::Handle hEntry = cast<Map::Entry::Handle> (hIter->next());
            if (NULL != hEntry)
                {
                cast<Thread::View>(hEntry->getKey())->scheduleMaintenance(nMaintenance);
                }
            }
        }

    // sleep for long enough for all threads to complete this task
    try
        {
        Thread::sleep((System::getInterruptResolution() * 3) / 2);
        }
    catch (InterruptedException::View e) {}
    }

// ----- helper methods -----------------------------------------------------

Thread::Handle Thread::currentThread()
    {
    return coh_thread_current();
    }

ObjectArray::Handle Thread::getStackTrace(size32_t cTrim)
    {
    return NativeBacktrace::getStackTrace(cTrim + 1);
    }

String::View Thread::formatStackTrace(ObjectArray::View vaStack)
    {
    if (NULL == vaStack)
        {
        vaStack = getStackTrace(1);
        }

    String::View vs = String::create();
    size32_t     ce = vaStack->length;
    for (size32_t i = 0; i < ce; ++i)
        {
        vs = COH_TO_STRING(vs << "    " << cast<StackTraceElement::View>(vaStack[i]) << std::endl);
        }
    if (ce <= 1)
        {
        // one or less frames is a indication of failure to obtain a full stack
        vs = COH_TO_STRING(vs << "    ... debugger may be required to obtain stack frames"
            << std::endl);
        }

    return vs;
    }

String::View Thread::formatStacks(int64_t cMillisTimeout)
    {
    Map::Handle hMapThread = TLSManager::getThreadMap();
    Map::Handle hMapResult = HashMap::create();
    int64_t     ldtStart   = System::currentTimeMillis();
    COH_SYNCHRONIZED (hMapThread)
        {
        bool fWait = false;
        for (Iterator::Handle hIter = hMapThread->entrySet()->iterator();
                hIter->hasNext(); )
            {
            Map::Entry::Handle hEntry = cast<Map::Entry::Handle> (hIter->next());
            if (NULL != hEntry)
                {
                fWait = true;
                hEntry->setValue(NULL); // remove any old value
                cast<Thread::View>(hEntry->getKey())->scheduleMaintenance(collect_stack);
                }
            }

        if (fWait)
            {
            hMapThread->wait(cMillisTimeout < 0
                    ? (System::getInterruptResolution() * 3) / 2
                    : cMillisTimeout);
            }

        // record results
        hMapResult->putAll(hMapThread);

        // clear values to reduce memory consumption
        for (Iterator::Handle hIter = hMapThread->entrySet()->iterator();
                hIter->hasNext(); )
            {
            Map::Entry::Handle hEntry = cast<Map::Entry::Handle>(hIter->next());
            if (NULL != hEntry)
                {
                hEntry->setValue(NULL);
                }
            }
        }

    // output collected results (reuse some of Loggers helper macros)
    String::View vs = COH_TO_STRING("Thread dump " << COH_SYMB_TO_STRING(COH_PRODUCT) << ' '
        << COH_SYMB_TO_STRING(COH_VERSION) << " Build "
        << COH_SYMB_TO_STRING(COH_BUILD_INFO) << " ("
        << COH_SYMB_TO_STRING(COH_PLATFORM) << ' '
        << COH_SYMB_TO_STRING(COH_BUILD_TYPE) << ' '
        << COH_SYMB_TO_STRING(COH_CC)
        << ") pid=" << NativePID::getPID()
        << "; spanning " << (System::currentTimeMillis() - ldtStart) << "ms"
        << std::endl);

    for (Iterator::Handle hIter = hMapResult->entrySet()->iterator();
            hIter->hasNext(); )
        {
        Map::Entry::Handle hEntry = cast<Map::Entry::Handle>(hIter->next());
        if (NULL != hEntry)
            {
            Thread::View      vThread        = cast<Thread::View>(hEntry->getKey());
            ObjectArray::View vaElementStack = cast<ObjectArray::View>(hEntry->getValue());

            vs = COH_TO_STRING(vs << std::endl << vThread << std::endl);
            if (NULL == vaElementStack)
                {
                // thread did not report, it could be in one of the following
                // state:
                // - blocked (mutex, io, unknown)
                // - spinning
                // - outside of the coherence API
                vs = COH_TO_STRING(vs << "  ... stack not reported within timeout; blocked or "
                    << "outside of coherence library" << std::endl);
                }
            else
                {
                vs = COH_TO_STRING(vs << formatStackTrace(vaElementStack));
                }
            }
        }

    return vs;
    }

bool Thread::interrupted()
    {
    // we use coh_thread_current, to force this frame to be trimmed of backtraces
    Thread::Handle hSelf    = coh_thread_current();
    int64_t        lTimeout = hSelf->m_lTimeout;

    if (lTimeout > 0 && lTimeout != Integer64::max_value)
        {
        Thread::isTimedOut(); // has a side effect of interrupting the thread if timed out
        }

    if (hSelf->m_nState == state_interrupted)
        {
        COH_SYNCHRONIZED (hSelf)
            {
            if (hSelf->m_nState == state_interrupted)
                {
                hSelf->m_nState = state_runnable;
                return true;
                }
            }
        }

    return false;
    }

void Thread::sleep(int64_t cMillis)
    {
    // reuse Thread's monitor to sleep
    // TODO: considering replacing with NativeThread::sleep (once added),
    //       though that will require us to add periodic thread dump and
    //       interrupt detection checks here similar to those in Object::wait.
    Thread::Handle hThread = currentThread();
    COH_SYNCHRONIZED (hThread)
        {
        hThread->wait(cMillis == 0 ? 1 : cMillis);
        }
    }

void Thread::yield()
    {
    NativeThread::yield();
    coh_checkThreadDumpRequest();
    }

int64_t Thread::remainingTimeoutMillis()
    {
    // we use coh_thread_current, to force this frame to be trimmed of backtraces
    Thread::Handle hThread = coh_thread_current();

    // from Java com.oracle.common.base.Timeout; see corresponding logic in TimeoutBlock.cpp
    int64_t lTimeout = hThread->getTimeout();
    if (lTimeout == Integer64::max_value) // common case
        {
        // no timeout configured; avoid pulling local time
        return Integer64::max_value;
        }

    int64_t ldtNow = System::safeTimeMillis();
    if (lTimeout < 0)
        {
        // timeout is still relative; actualize and store it
        hThread->setTimeout(ldtNow - lTimeout); // sets timeout as now + -lTimeout
        return -lTimeout; // no need to compute relative
        }
    // else; timeout was already realized, compute remainder

    int64_t cMillis = lTimeout - ldtNow;
    if (cMillis <= 0)
        {
        hThread->interrupt();
        return 0;
        }
    return cMillis;
    }

bool Thread::isTimedOut()
    {
    return Thread::remainingTimeoutMillis() == 0;
    }

void Thread::setTimeout(int64_t timeout)
    {
    m_lTimeout = timeout;
    }

int64_t Thread::getTimeout() const
    {
    return m_lTimeout;
    }

// ----- Object interface ---------------------------------------------------

static const int   COH_STATE_TEXT_LENGTH = 4;
static const char* COH_STATE_TEXT[COH_STATE_TEXT_LENGTH] =
    {
    "state_initial",
    "state_runnable",
    "state_interrupted",
    "state_terminated"
    };

TypedHandle<const String> Thread::toString() const
    {
    int         nState  = (int) getState();
    const char* acState = (nState < 0 || nState >= COH_STATE_TEXT_LENGTH)
        ? "unknown" : COH_STATE_TEXT[nState];
    String::View vs = COH_TO_STRING('"' << getName() << "\" tid=0x" << std::hex << getId()
        << ' ' << acState << ": ");

    String::View vsDescription = f_vsDescription;
    if (NULL == vsDescription)
        {
        // no stored description, Runnable must be Describable
        vs = COH_TO_STRING(vs << cast<Describable::View>(m_hRunnable)->getDescription());
        }
    else
        {
        vs = COH_TO_STRING(vs << vsDescription);
        }

    return vs;
    }

COH_CLOSE_NAMESPACE2
