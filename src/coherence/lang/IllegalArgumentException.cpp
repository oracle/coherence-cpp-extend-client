/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/IllegalArgumentException.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors -------------------------------------------------------

IllegalArgumentException::IllegalArgumentException(String::View vsMsg,
        Exception::View vCause)
        : super(vsMsg, vCause)
    {
    }

// ----- free functions -----------------------------------------------------

/**
* Throw a new IllegalArgumentException instance.
*/
COH_EXPORT void coh_throw_illegal_argument(const char* achMsg)
    {
    COH_THROW (IllegalArgumentException::create(achMsg));
    }

COH_CLOSE_NAMESPACE2
