/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Event.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

Event::Event(Object::View vSource)
        : f_vSource(self(), vSource)
    {
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> Event::toString() const
    {
    return COH_TO_STRING("Event{" << getSource() << '}');
    }


// ----- accessors ----------------------------------------------------------

Object::View Event::getSource() const
    {
    return f_vSource;
    }

COH_CLOSE_NAMESPACE2
