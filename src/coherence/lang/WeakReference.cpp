/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/WeakReference.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/MemberHandle.hpp"
#include "coherence/lang/System.hpp"

#include "private/coherence/lang/WeakReferenceImpl.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- file local helpers -------------------------------------------------

namespace
    {
    WeakReference::Handle getNULLWeakReference()
        {
        static FinalHandle<WeakReference> h(System::common(),
                WeakReferenceImpl::create((Object::Holder) NULL));
        return h;
        }
    COH_STATIC_INIT(getNULLWeakReference());
    }

WeakReference::Holder WeakReference::valueOf(Object::Holder oh)
    {
    if (NULL == oh)
        {
        return getNULLWeakReference();
        }
    else if (instanceof<Object::Handle>(oh))
        {
        return cast<Object::Handle>(oh)->_attachWeak();
        }
    return oh->_attachWeak();
    }

COH_CLOSE_NAMESPACE2
