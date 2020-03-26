/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/ClassNotFoundException.hpp"

#include <sstream>

COH_OPEN_NAMESPACE2(coherence,lang)

// ----- constructors -------------------------------------------------------

ClassNotFoundException::ClassNotFoundException(String::View vsName, Exception::View vCause)
        : super(COH_TO_STRING(vsName << ": class not found"), vCause)
    {
    }

COH_CLOSE_NAMESPACE2
