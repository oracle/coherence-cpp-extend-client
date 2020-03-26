/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RUNNABLE_HPP
#define COH_RUNNABLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/interface_spec.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Interface implemented by any class whose instances are intended to be
* executed by a thread.
*
* @author mf  2007.12.10
*/
class COH_EXPORT Runnable
    : public interface_spec<Runnable>
    {
    // ----- Runnable interface ---------------------------------------------

    public:
        /**
        * Invoke the Runnable.
        */
        virtual void run() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_RUNNABLE_HPP
