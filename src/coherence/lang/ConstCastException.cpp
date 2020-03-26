/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/ConstCastException.hpp"

#include "coherence/lang/Class.hpp"

#include <sstream>

COH_OPEN_NAMESPACE2(coherence,lang)

// ----- constructors -------------------------------------------------------

ConstCastException::ConstCastException(String::View vsMessage, Exception::View vCause)
    : super(vsMessage, vCause)
    {
    }

// ----- free functions -----------------------------------------------------

/**
* Throw a new ConstCastException instance.
*/
COH_EXPORT void coh_throw_const_cast(const std::type_info& infoD,
        const std::type_info& infoT)
    {
    COH_THROW_STREAM (ConstCastException, "attempt to cast from a \"const "
            << Class::getTypeName(infoT)
            << "\" to a \""
            << Class::getTypeName(infoD) << '"');
    }

COH_CLOSE_NAMESPACE2
