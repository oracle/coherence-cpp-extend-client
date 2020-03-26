/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/EventObject.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

EventObject::EventObject(Object::Handle hSource)
        : f_hoSource(self(), hSource, /*fMutable*/ true)
    {
    }


// ----- EventObject interface ----------------------------------------------

Object::Handle EventObject::getSource() const
    {
    return f_hoSource;
    }


// ----- Describable interface ----------------------------------------------

String::View EventObject::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << "[source=" << f_hoSource << "]");
    }

COH_CLOSE_NAMESPACE2

