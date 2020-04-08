/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_PID_HPP
#define COH_NATIVE_PID_HPP

#include "coherence/lang/compatibility.hpp"



COH_OPEN_NAMESPACE2(coherence,native)


/**
* NativePID provides a generic interface for obtaining the local process PID
*
* @author mf  2008.04.15
*/
class COH_EXPORT NativePID
    {
    // ----- NativePID interface --------------------------------------------

    public:
        /**
        * Return the PID for the current process.
        *
        * @return the PID for the current process
        */
        static int32_t getPID();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_PID_HPP
