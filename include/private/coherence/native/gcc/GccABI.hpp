/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GCC_ABI_HPP
#define COH_GCC_ABI_HPP

#include "private/coherence/native/NativeABI.hpp"

#include <cxxabi.h>
#include <stdlib.h>

COH_OPEN_NAMESPACE2(coherence,native)


// ----- NativeABI static interface -----------------------------------------

/**
* {@inheritDoc}
*/
String::View NativeABI::demangle(const char* achMangled)
    {
    char* achClear = NULL;
    try
        {
        int nStatus = 0;
        achClear = abi::__cxa_demangle(achMangled, 0, 0, &nStatus);

        if (nStatus == 0)
            {
            String::View vsClear;

            if (achClear)
                {
                vsClear = String::create(achClear);
                free(achClear);
                }

            return vsClear;
            }
        else if (achMangled)
            {
            return achMangled;
            }
        return NULL;
        }
    catch (...)
        {
        if (achClear)
            {
            free(achClear);
            }
        throw;
        }
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GCC_ABI_HPP
