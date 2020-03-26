/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/IndexOutOfBoundsException.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors -------------------------------------------------------

IndexOutOfBoundsException::IndexOutOfBoundsException(String::View vsMsg,
        Exception::View vCause)
        : super(vsMsg, vCause)
    {
    }


// ----- free functions -----------------------------------------------------

/**
* Throw a new IndexOutOfBoundsException instance.
*/
COH_EXPORT void coh_throw_ioob(size32_t i)
    {
    COH_THROW_STREAM (IndexOutOfBoundsException, i);
    }

COH_CLOSE_NAMESPACE2
