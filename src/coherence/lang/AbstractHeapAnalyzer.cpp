/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/AbstractHeapAnalyzer.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include "private/coherence/native/NativeThreadLocal.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::native::NativeThreadLocal;


// ----- file local helpers -------------------------------------------------

namespace
    {
    // ----- nested class: ContextGuard ---------------------------------

    /**
    * ContextGuard marks and unmarks a thread as being inside the analyzers
    * registration methods.  This is used to avoid endlessly recursing as
    * objects are created from within analyzer registration.
    */
    class ContextGuard
        {
        public:
            /**
            * Construct a ContextGuard from a ThreadLocalReference.
            *
            * @param hTl  the ThreadLocalReference to use
            */
            ContextGuard(NativeThreadLocal* pTl)
                : m_pTl(NULL)
                {
                if (NULL == pTl->get())
                    {
                    pTl->set(this); // mark thread as being in the analyzer
                    m_pTl = pTl;    // mark the guard as guarding
                    }
                // else leave set to NULL
                }

            /**
            * Destructor
            */
            ~ContextGuard()
                {
                NativeThreadLocal* pTl = m_pTl;
                if (pTl)
                    {
                    // release the guard
                    pTl->set(NULL);
                    }
                // don't delete pTl, this is not owned by the guard
                }

            /**
            * Return true if this thread was already within the analyzer.
            */
            bool isActive() const
                {
                return m_pTl == NULL;
                }

        // ----- data members -------------------------------------------

        private:
            /**
            * The thread local used to track the context.
            */
            NativeThreadLocal* m_pTl;
        };
    }


// ----- constructor --------------------------------------------------------

AbstractHeapAnalyzer::AbstractHeapAnalyzer()
        : m_pTLContext(NativeThreadLocal::create())
    {
    }

AbstractHeapAnalyzer::~AbstractHeapAnalyzer()
    {
    delete m_pTLContext;
    }


// ----- HeapAnalyzer interface ---------------------------------------------

void AbstractHeapAnalyzer::registerObject(const Object& o)
    {
    ContextGuard guard(m_pTLContext);
    if (!guard.isActive())
        {
        safeRegisterObject(o);
        }
    // else; not the guard, we we're re-entreing registration; do nothing
    }

void AbstractHeapAnalyzer::unregisterObject(const Object& o)
    {
    ContextGuard guard(m_pTLContext);
    if (!guard.isActive())
        {
        safeUnregisterObject(o);
        }
    // else; not the guard, we we're re-entreing registration; do nothing
    }


// ----- Object interface -----------------------------------------------

TypedHandle<const String> AbstractHeapAnalyzer::toString() const
    {
    return COH_TO_STRING(capture() << "; " << getImmortalCount() << " immortals");
    }

COH_CLOSE_NAMESPACE2
