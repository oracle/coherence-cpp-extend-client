/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_DYNAMIC_LIBRARY_HPP
#define COH_WINDOWS_DYNAMIC_LIBRARY_HPP

#include "private/coherence/native/NativeDynamicLibrary.hpp"

#include "coherence/lang/IllegalArgumentException.hpp"

#include <windows.h>

COH_OPEN_NAMESPACE2(coherence,native)

// ----- NativeDynamicLibrary static interface ------------------------------

void NativeDynamicLibrary::load(String::View vsLibName)
    {
    if (LoadLibrary(vsLibName->getCString()) == NULL)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "Error opening dynamic library \"" << vsLibName << "\" - "
                << GetLastError());
        }
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_DYNAMIC_LIBRARY_HPP
