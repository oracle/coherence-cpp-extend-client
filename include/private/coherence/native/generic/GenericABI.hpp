/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GCC_ABI_HPP
#define COH_GCC_ABI_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/String.hpp"

#include "private/coherence/native/NativeABI.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


// ----- NativeABI static interface -----------------------------------------

/**
* {@inheritDoc}
*/
String::View NativeABI::demangle(const char* achMangled)
    {
    if (achMangled)
        {
        return achMangled;
        }
    return String::null_string;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GCC_ABI_HPP
