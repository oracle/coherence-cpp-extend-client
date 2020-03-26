/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/CloneNotSupportedException.hpp"

#include "coherence/lang/Class.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors -------------------------------------------------------

CloneNotSupportedException::CloneNotSupportedException(String::View vsMsg, Exception::View vCause)
        : super(vsMsg, vCause)
    {
    }

// ----- free functions -----------------------------------------------------

/**
* Throw a new CloneNotSupportedException instance.
*/
COH_EXPORT void coh_throw_clone_not_supported(const std::type_info& info)
    {
    COH_THROW (CloneNotSupportedException::create(Class::getTypeName(info)));
    }

COH_CLOSE_NAMESPACE2
