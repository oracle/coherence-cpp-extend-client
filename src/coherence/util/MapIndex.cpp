/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MapIndex.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ---- constants -------------------------------------------------------

Object::View MapIndex::getNoValue()
    {
    static FinalView<Object> vNOVALUE(System::common(), Object::create());
    return vNOVALUE;
    }
COH_STATIC_INIT(MapIndex::getNoValue());

COH_CLOSE_NAMESPACE2
