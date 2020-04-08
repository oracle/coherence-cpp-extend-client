/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/reflect/AbstractPofPath.hpp"

#include "coherence/io/pof/reflect/PofValue.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)

using coherence::io::pof::reflect::PofValue;


// ----- PofNavigator interface ---------------------------------------------

PofValue::Handle AbstractPofPath::navigate(PofValue::Handle /*hValueOrigin*/) const
    {
    COH_THROW (UnsupportedOperationException::create(
            "local POF navigation unsupported"));
    }

COH_CLOSE_NAMESPACE4
