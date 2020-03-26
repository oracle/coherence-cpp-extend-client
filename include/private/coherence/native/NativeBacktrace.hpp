/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_BACKTRACE_HPP
#define COH_NATIVE_BACKTRACE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/ObjectArray.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

using coherence::lang::ObjectArray;


/**
* NativeBacktrace provides an interface for obtaining a stack trace for the
* current thread.
*
* @author mf  2008.03.31
*/
class COH_EXPORT NativeBacktrace
    {
    // ----- NativeBacktrace static interface -------------------------------

    public:
        /**
        * Return a stack trace for the calling thread.
        *
        * @param cTrim  the number of frames to trim from the top of the stack
        *               relative to this method
        *
        * @return an array of StackTraceElements describing the stack
        */
        static ObjectArray::Handle getStackTrace(size32_t cTrim = 0);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_BACKTRACE_HPP
