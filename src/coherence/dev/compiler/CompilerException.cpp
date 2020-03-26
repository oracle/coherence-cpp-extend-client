/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/dev/compiler/CompilerException.hpp"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)


// ----- constructors -------------------------------------------------------

CompilerException::CompilerException(String::View vsMsg, Exception::View vCause)
        : super(vsMsg, vCause)
    {
    }

COH_CLOSE_NAMESPACE3
