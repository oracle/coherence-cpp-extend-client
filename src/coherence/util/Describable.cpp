/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Describable.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// ----- Describable interface ----------------------------------------------

String::View Describable::getDescription() const
    {
    return "";
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> Describable::toString() const
    {
    return COH_TO_STRING(Class::getSimpleClassName(this) << '(' << getDescription() << ')');
    }

COH_CLOSE_NAMESPACE2
