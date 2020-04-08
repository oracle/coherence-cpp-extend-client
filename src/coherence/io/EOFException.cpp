/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/EOFException.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


// ----- constructors -------------------------------------------------------

EOFException::EOFException(String::View vsMsg, Exception::View vCause)
        : super(vsMsg, vCause)
    {
    }

COH_CLOSE_NAMESPACE2
