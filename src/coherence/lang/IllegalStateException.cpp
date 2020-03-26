/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/IllegalStateException.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors -------------------------------------------------------

IllegalStateException::IllegalStateException(String::View vsMsg,
        Exception::View vCause)
        : super(vsMsg, vCause)
    {
    }

// ----- free functions -----------------------------------------------------

/**
* Throw a new IllegalStateException instance.
*/
COH_EXPORT void coh_throw_illegal_state(const char* achMsg)
    {
    COH_THROW (IllegalStateException::create(achMsg));
    }

COH_CLOSE_NAMESPACE2
