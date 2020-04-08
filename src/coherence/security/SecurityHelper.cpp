/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,security)


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Set/Test if security has been enabled.  Once enabled it cannot be
    * disabled.
    *
    * @param fEnable  if security it to be enabled
    *
    * @return if security has been enabled
    */
    bool enabled(bool fEnable = false)
        {
        static bool s_fEnabled = fEnable;

        if (fEnable && !s_fEnabled)
            {
            s_fEnabled = true;
            }
        // else don't reset

        return s_fEnabled;
        }
    COH_STATIC_INIT(enabled(false));

    /**
    * Return the ThreadLocalReference used for storage of the thread local
    * Subject.
    *
    * @return the ThreadLocalReference for the subject
    */
    ThreadLocalReference::Handle getThreadLocalSubject()
        {
        static FinalHandle<ThreadLocalReference> hTLS(System::common(),
            ThreadLocalReference::create());
        return hTLS;
        }
    COH_STATIC_INIT(getThreadLocalSubject());
    }


// ----- SecurityHelper interface -------------------------------------------

Subject::View SecurityHelper::updateCurrentSubject(Subject::View vSubject)
    {
    if (vSubject == NULL && !enabled())
        {
        // this is a no-op, see note below
        return NULL;
        }

    ThreadLocalReference::Handle hTLS      = getThreadLocalSubject();
    Subject::View                vSubjCurr = cast<Subject::View>(hTLS->get());

    hTLS->set(vSubject);
    enabled(true); // ensure it is on

    return vSubjCurr;
    }

Subject::View SecurityHelper::getCurrentSubject()
    {
    // Note: there is trickery here, we use a global s_fEnabled without any
    // form for synchronization.  The reason is that since we are working on
    // a thread-local, the only way that this method could ever return a
    // non-NULL result, would be if updateCurrentSubject, had at some point
    // been called on this thread.  Since update always sets s_fEnabled to
    // true, we'd safely see it.  If update was called on any thread then we
    // might see the change, and try to read our subject.  The nice part is
    // that if update has never been called within this process, we avoid
    // the cost of reading the ThreadLocal.  This optimization assumes that
    // most apps won't be setting the Subject.
    if (enabled())
        {
        return cast<Subject::View>(getThreadLocalSubject()->get());
        }
    return NULL;
    }

COH_CLOSE_NAMESPACE2
