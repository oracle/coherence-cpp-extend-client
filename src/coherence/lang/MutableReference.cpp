/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/lang/MutableReference.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

// ----- constructors -------------------------------------------------------

MutableReference::MutableReference(Object::Holder ohReferant)
    : m_ohReferant(super::self(), ohReferant)
    {
    }


// ----- Reference interface ------------------------------------------------

void MutableReference::set(Object::Holder ohReferant)
    {
    m_ohReferant = ohReferant;
    }

Object::Holder MutableReference::get() const
    {
    return m_ohReferant;
    }

Object::Holder MutableReference::get()
    {
    return m_ohReferant;
    }

COH_CLOSE_NAMESPACE2
