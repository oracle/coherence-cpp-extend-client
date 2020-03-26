/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_PID_HPP
#define COH_POSIX_PID_HPP

#include "coherence/lang/compatibility.hpp"

#include "private/coherence/native/NativePID.hpp"

#include <unistd.h>

COH_OPEN_NAMESPACE2(coherence,native)

int32_t NativePID::getPID()
    {
    return getpid();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_PID_HPP
