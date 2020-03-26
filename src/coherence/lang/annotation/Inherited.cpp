/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/annotation/Inherited.hpp"

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,lang,annotation)

COH_REGISTER_CLASS(TypedBarrenClass<Inherited>::create());

Inherited::View Inherited::getInstance()
    {
    static FinalView<Inherited> s_vInherited(System::common(), Inherited::create());
    return s_vInherited;
    }
COH_STATIC_INIT(Inherited::getInstance());

COH_CLOSE_NAMESPACE3
