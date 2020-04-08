/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_DYNAMIC_LIBRARY_HPP
#define COH_NATIVE_DYNAMIC_LIBRARY_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

using namespace coherence::lang;

/**
* NativeDynamicLibrary provides platform specific implementation for loading
* dynamic libraries.
*
* @author mf  2008.10.02
*/
class COH_EXPORT NativeDynamicLibrary
    {
    // ----- NativeDynamicLibrary static interface --------------------------

    public:
        /**
        * Load the dynamic library
        *
        * @param vsLibName  the name of the library to load
        */
        static void load(String::View vsLibName);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_DYNAMIC_LIBRARY_HPP
