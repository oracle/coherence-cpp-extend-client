/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GENERIC_BACKTRACE_HPP
#define COH_GENERIC_BACKTRACE_HPP

#include "private/coherence/native/NativeBacktrace.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


// ----- NativeBacktrace static interface -----------------------------------

ObjectArray::Handle NativeBacktrace::getStackTrace(size32_t /*cTrim*/)
    {
    return ObjectArray::create(0); // NULL has a different meaning in Thread::dumpStack
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GENERIC_BACKTRACE_HPP
