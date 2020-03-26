/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INTERRUPTED_EXCEPTION_HPP
#define COH_INTERRUPTED_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Exception.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Signals that the current thread has been interrupted.
*/
class COH_EXPORT InterruptedException
    : public throwable_spec<InterruptedException,
        extends<Exception> >
    {
    friend class factory<InterruptedException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an InterruptedException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new InterruptedException
        */
        InterruptedException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INTERRUPTED_EXCEPTION_HPP
