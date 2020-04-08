/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/annotation/AbstractAnnotation.hpp"

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,lang,annotation)

COH_REGISTER_CLASS(TypedBarrenClass<AbstractAnnotation>::create());

Class::View AbstractAnnotation::annotationType() const
    {
    return SystemClassLoader::getInstance()->loadByType(typeid(*this));
    }

COH_CLOSE_NAMESPACE3
