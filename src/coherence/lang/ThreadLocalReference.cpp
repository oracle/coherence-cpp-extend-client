/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/ThreadLocalReference.hpp"

#include "coherence/lang.ns"

#include "coherence/util/WeakHashMap.hpp"

#include "private/coherence/native/NativeThreadLocal.hpp"

#include <iostream>
#include <map>

COH_OPEN_NAMESPACE2(coherence,lang)

using namespace coherence::native;

using coherence::util::WeakHashMap;

// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * This function is called upon thread exit, and deletes the thread's
    * Map.
    *
    * @param pvStdMap  pointer to the coh_handle_map
    */
    extern "C" void coh_delete_thread_local_map(void* pvStdMap)
        {
        Map* pMap = (Map*) pvStdMap;
        if (pMap)
            {
            // this will also cause all values to destruct/detach
            pMap->_detach(/*fEscaped*/ false);
            }
        }

    Map::Handle coh_get_thread_local_map(); // forward decl

    /**
    * Helper class for initializing and cleaning up the TLS used to store the
    * current thread.
    */
    class TLSManager
        {
        public:
            TLSManager()
                {
                s_pTLS = NativeThreadLocal::create(&coh_delete_thread_local_map);
                s_pMap = NULL;
                }

            ~TLSManager()
                {
                // The Map object for this thread and any other live threads which
                // have access coherence will be leaked, but this is a small amount
                // of memory

                Map::Handle hMap = coh_get_thread_local_map();
                s_pMap = get_pointer(hMap); // note map is already kept alive

                // free up the TLS (a very limited resource)
                delete s_pTLS;
                s_pTLS = NULL;
                }

            /**
            * Return the NativeThreadLocal used to find the ThreadLocal Map
            * associated with the calling thread.
            *
            * @return the NativeThreadLocal
            */
            static NativeThreadLocal* getTLS()
                {
                static TLSManager singleton;
                return s_pTLS;
                }

            /**
            * Return the shutdown Thread's TLS Map.
            */
            static Map* getShutdownMap()
                {
                return s_pMap;
                }

        private:
            static NativeThreadLocal* s_pTLS;
            static Map*               s_pMap;
        };
    NativeThreadLocal* TLSManager::s_pTLS = NULL;
    Map*               TLSManager::s_pMap = NULL;
    COH_STATIC_INIT(TLSManager::getTLS());


    /**
    * Return the thread-local map associated with the calling thread.
    *
    * @return the thread local coh_handle_map
    */
    Map::Handle coh_get_thread_local_map()
        {
        // Note: in order to avoid the static destruction problem we intentionally
        // "leak" the NativeThreadLocal. Note, this really isn't a leak since only
        // one instance exists for the life of the process, we just don't delete
        // it during shutdown.

        NativeThreadLocal* pTLS = TLSManager::getTLS();

        Map::Handle hMap = pTLS == NULL
                ? TLSManager::getShutdownMap()
                : (Map*) pTLS->get();
        if (NULL == hMap)
            {
            // this is the first time the calling thread has used any TLR
            hMap = WeakHashMap::create();
            hMap->_attach(/*fEscaped*/ false);
            pTLS->set(get_pointer(hMap));
            }
        return hMap;
        }
    COH_STATIC_INIT(coh_get_thread_local_map());
    }

void coh_tls_compact()
    {
    cast<WeakHashMap::Handle>(coh_get_thread_local_map())->compact();
    }

// ----- constructors -------------------------------------------------------

ThreadLocalReference::ThreadLocalReference()
    {
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ThreadLocalReference::toString() const
    {
    return COH_TO_STRING("ThreadLocalReference{" << get() << '}');
    }


// ----- Reference interface ------------------------------------------------

void ThreadLocalReference::set(Object::Holder ohObject)
    {
    Map::Handle hMap = coh_get_thread_local_map();

    if (NULL == ohObject)
        {
        hMap->remove(this);
        }
    else
        {
        hMap->put(this, ohObject);
        }
    }

Object::Holder ThreadLocalReference::get() const
    {
    return coh_get_thread_local_map()->get(this);
    }

Object::Holder ThreadLocalReference::get()
    {
    return coh_get_thread_local_map()->get(this);
    }

COH_CLOSE_NAMESPACE2
