/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/annotation/Portable.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,annotation)

COH_REGISTER_TYPED_CLASS(Portable);

// ----- Annotation members -------------------------------------------------

Class::View Portable::annotationType() const
    {
    return SystemClassLoader::getInstance()->loadByType(typeid(Portable));
    }

// ----- static members -----------------------------------------------------

Class::View Portable::getStaticClass()
    {
    static FinalView<Class> vClzPortable(System::common(),
            SystemClassLoader::getInstance()->loadByType(typeid(Portable)));
    return vClzPortable;
    }
COH_STATIC_INIT(Portable::getStaticClass());

COH_CLOSE_NAMESPACE4
