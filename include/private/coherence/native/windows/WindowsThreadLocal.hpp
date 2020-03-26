/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_THREAD_LOCAL_HPP
#define COH_WINDOWS_THREAD_LOCAL_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeThreadLocal.hpp"
#include "private/coherence/native/windows/WindowsMutex.hpp"

#include <iostream>
#include <sstream>
#include <windows.h>
#include <vector>

COH_OPEN_NAMESPACE3(coherence,native,windows)

class WindowsThreadLocal;


// ----- local file helpers -------------------------------------------------

namespace
    {
    // ----- typedefs -------------------------------------------------------

    typedef std::vector<WindowsThreadLocal*> ListTLS;

    /**
    * Vector of all WindowsThreadLocal objects.
    */
    static ListTLS& getTLSList()
        {
        static ListTLS s_listTLS;
        return s_listTLS;
        }
    COH_STATIC_INIT(getTLSList());

    /**
    * Lock protecting the TLS list.
    */
    static WindowsMutex& getTLSLock()
         {
         static WindowsMutex s_lockTLS;
         return s_lockTLS;
         }
     COH_STATIC_INIT(getTLSLock());
    }

/**
* Windows implementation of NativeThreadLocal
*
* @author mf 2007.12.12
*/
class COH_EXPORT WindowsThreadLocal
    : public NativeThreadLocal
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Create a posix thread local variable.
        */
        WindowsThreadLocal(CleanupFunction func)
            : m_pFuncCleanup(func)
            {
            DWORD nKey = TlsAlloc();
            COH_ENSURE(nKey != TLS_OUT_OF_INDEXES);
            m_nKey = nKey;

            WindowsMutex& lock = getTLSLock();
            lock.lock();
            try
                {
                getTLSList().push_back(this);
                lock.unlock();
                }
            catch (...)
                {
                lock.unlock();
                throw;
                }
            }

        virtual ~WindowsThreadLocal()
            {
            // it is assumed that there are no live threads still storing data
            // in this TLS, so there is no need to perform cleanup. Performing
            // cleanup wouldn't even be safe since the cleanup needs to occur
            // on each thread
            COH_ENSURE(TlsFree(m_nKey) != FALSE); // unregister the key
            }


    // ----- NativeThreadLocal interface ------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void set(void* pvValue)
            {
            COH_ENSURE(TlsSetValue(m_nKey, pvValue) != FALSE);
            }

        /**
        * @inheritDoc
        */
        virtual void* get() const
            {
            return TlsGetValue(m_nKey);
            }

        /**
        * Perform cleanup of the value on a thread which is about to exit.
        *
        * @return true if a cleanup routine was found (and invoked).
        */
        boolean cleanup()
            {
            CleanupFunction pFunc = m_pFuncCleanup;
            if (NULL != pFunc)
                {
                void* pValue = get();
                if (NULL != pValue)
                    {
                    // Set to NULL prior to calling cleanup routine - imitating the Posix spec
                    set(NULL);
                    (*pFunc)(pValue);
                    return true;
                    }
                }
            return false;
            }

        /**
        * Called automatically as part of thread exit.
        */
        static void onThreadExit()
            {
            WindowsMutex& lock = getTLSLock();
            lock.lock();
            try
                {
                ListTLS& listTLS = getTLSList();
#define COH_WINDOWS_DESTRUCTOR_ITERATIONS 4
                // Loop through until no more work or max iterations is reached
                // See posix spec on pthread_key_create()
                boolean fRepeat = true;
                for (int j = 0;
                    j < COH_WINDOWS_DESTRUCTOR_ITERATIONS && fRepeat; ++j)
                    {
                    fRepeat = false;
                    for (ListTLS::iterator i = listTLS.begin(), iEnd = listTLS.end();
                        i != iEnd; ++i)
                        {
                        fRepeat |= (*i)->cleanup();
                        }
                    }
                lock.unlock();
                }
            catch (...)
                {
                lock.unlock();
                throw;
                }
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Windows key associated with the ThreadLocal.
        */
        DWORD m_nKey;

        /**
        * The cleanup function associated with ThreadLocal or NULL if no
        * cleanup is required.
        */
        CleanupFunction m_pFuncCleanup;
    };

COH_CLOSE_NAMESPACE3


// ----- NativeThreadLocal factory methods ----------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeThreadLocal* NativeThreadLocal::create(CleanupFunction func)
    {
    return new coherence::native::windows::WindowsThreadLocal(func);
    }

COH_CLOSE_NAMESPACE2


// ---- DllMain implementation ----------------------------------------------

/**
* DllMain is the entry/exit point for Windows DLLs. It is used here as a
* means of cleaning up TLS upon thread-exit.
*
* @param hDll         HANDLE to the Coherence DLL
* @param nReason      the reason the function is being called
* @param lpvReserved  Microsoft's future proofing
*
* @return true upon success
*
* @see http://msdn2.microsoft.com/en-us/library/ms686997.aspx
*/
BOOL WINAPI DllMain(HINSTANCE hDll, DWORD nReason, LPVOID lpvReserved)
    {
    switch (nReason)
        {
        case DLL_PROCESS_ATTACH:
            // no break (main thread attaching)

        case DLL_THREAD_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_DETACH:
            coherence::native::windows::WindowsThreadLocal::onThreadExit();
            break;

        default:
            break;
        }

    return true;
    }

#endif // COH_WINDOWS_THREAD_LOCAL_HPP
