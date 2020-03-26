/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Objects.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// -------- Objects interface -----------------------------------------------

String::View Objects::toString(Object::View v, String::View vsNullDefault)
    {
    return (v != NULL) ? v->toString() : vsNullDefault;
    }

int32_t Objects::compare(Object::View vA, Object::View vB, Comparator::View vC)
    {
    return (vA == vB) ? 0 :  vC->compare(vA, vB);
    }

Object::Holder Objects::requireNonNull(Object::Holder oh)
    {
    if (oh == NULL)
        {
        COH_THROW(NullPointerException::create());
        }
    return oh;
    }

Object::Holder Objects::requireNonNull(Object::Holder oh, String::View vsMessage)
    {
    if (oh == NULL)
        {
        COH_THROW(NullPointerException::create(vsMessage));
        }
    return oh;
    }

bool Objects::isNull(Object::View v)
    {
    return v == NULL;
    }

bool Objects::nonNull(Object::View v)
    {
    return v != NULL;
    }

COH_CLOSE_NAMESPACE2
