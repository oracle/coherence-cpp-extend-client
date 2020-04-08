/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/CustomKeyClass.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

// ----- constructors -----------------------------------------------

CustomKeyClass::CustomKeyClass()
        : f_v(self())
    {
    }

CustomKeyClass::CustomKeyClass(Object::View v)
        : f_v(self(), v)
    {
    }


// ----- PortableObject interface -----------------------------------

void CustomKeyClass::readExternal(PofReader::Handle hIn)
    {
    initialize(f_v, hIn->readObject(0));
    }

void CustomKeyClass::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_v);
    }


// ----- Object interface -------------------------------------------

bool CustomKeyClass::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    if (instanceof<CustomKeyClass::View>(v))
        {
        CustomKeyClass::View vThat = cast<CustomKeyClass::View>(v);
        return f_v->equals(vThat->f_v);
        }
    return false;
    }

size32_t CustomKeyClass::hashCode() const
    {
    Object::View v = f_v;
    return v == NULL ? 0: v->hashCode();
    }

TypedHandle<const String> CustomKeyClass::toString() const
    {
    return COH_TO_STRING("CustomKeyClass(" << f_v << ')');
    }

COH_REGISTER_PORTABLE_CLASS(1511, CustomKeyClass);

COH_CLOSE_NAMESPACE2

