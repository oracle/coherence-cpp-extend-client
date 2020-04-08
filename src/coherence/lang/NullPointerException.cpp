/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/NullPointerException.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors -------------------------------------------------------

NullPointerException::NullPointerException(String::View vsMsg,
        Exception::View vCause)
        : super(vsMsg, vCause)
    {
    }

// ----- free functions -----------------------------------------------------

/**
* Throw a new NullPointerException instance.
*/
COH_EXPORT void coh_throw_npe(const std::type_info& info)
    {
    COH_THROW_STREAM (NullPointerException,
            "derefrencing a " << Class::getTypeName(info));
    }

COH_CLOSE_NAMESPACE2
