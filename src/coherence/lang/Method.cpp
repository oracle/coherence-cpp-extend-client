/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Method.hpp"

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructors -------------------------------------------------------

Method::Method(String::View vsName)
    : f_vsName(self(), vsName)
    {
    }

// ----- Method interface ---------------------------------------------------

String::View Method::getName() const
    {
    return f_vsName;
    }

Method::Handle Method::annotate(Annotation::View vAnnotation)
    {
    addAnnotation(vAnnotation);
    return this;
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> Method::toString() const
    {
    return COH_TO_STRING(getDeclaringClass()->getName() << "::" << getName() << " " << getParameterTypes());
    }

bool Method::equals(Object::View vThat) const
    {
    if (vThat == this)
        {
        return true;
        }
    else if (!instanceof<Method::View>(vThat))
        {
        return false;
        }
    Method::View that = cast<Method::View>(vThat);

    return Object::equals(getDeclaringClass(), that->getDeclaringClass()) &&
           Object::equals(getName(),           that->getName())           &&
           Object::equals(getParameterTypes(), that->getParameterTypes());
    }

size32_t Method::hashCode() const
    {
    return Object::hashCode(getDeclaringClass()) ^ Object::hashCode(getName());
    }

COH_CLOSE_NAMESPACE2
