/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_USER_HPP
#define COH_NATIVE_USER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/String.hpp"


COH_OPEN_NAMESPACE2(coherence,native)


/**
* NativeUser provides a generic interface for obtaining the local process username
*
* @author mf  2015.07.14
*/
class COH_EXPORT NativeUser
    {
    // ----- NativeUser interface -------------------------------------------

    public:
        /**
        * Return the username for the current process.
        *
        * @return the USER for the current process
        */
        static String::View getUserName();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_USER_HPP
