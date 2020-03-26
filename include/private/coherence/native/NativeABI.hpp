/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_ABI_HPP
#define COH_NATIVE_ABI_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

using namespace coherence::lang;


/**
* NativeABI provides access to compiler specific ABI features such as name
* demangling.
*
* @author mf  2008.03.31
*/
class COH_EXPORT NativeABI
    {
    // ----- NativeABI static interface -------------------------------------

    public:
        /**
        * Demangle a mangled name.
        *
        * @param achMangled  the mangled name
        *
        * @return a best approximation at the demangled name
        */
        static String::View demangle(const char* achMangled);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_ABI_HPP
