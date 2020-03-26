/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_ABI_HPP
#define COH_WINDOWS_ABI_HPP

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
        // TODO: use http://msdn2.microsoft.com/en-us/library/ms681400.aspx
        // or similar.

        if (strncmp("class ", achMangled, 6) == 0)
            {
            // assuming type_info.name was supplied, MS does a curious thing for
            // class names, and return "class name", rather then "name"
            return achMangled + 6;
            }
        else if (strncmp("__thiscall ", achMangled, 11) == 0)
            {
            // pretty function names are prefixed by __thiscall, strip it
            return achMangled + 11;
            }
        return achMangled;
        }
    return String::null_string;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_ABI_HPP
