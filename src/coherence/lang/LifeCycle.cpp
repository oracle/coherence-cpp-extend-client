/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/LifeCycle.hpp"

#include <sstream>

COH_OPEN_NAMESPACE2(coherence,lang)

// ensure that the bitfield fits within an int32_t
COH_STATIC_ASSERT(sizeof(LifeCycle::State) <= sizeof(int32_t));
COH_STATIC_ASSERT(sizeof(LifeCycle::Refs) <= sizeof(int32_t));

COH_CLOSE_NAMESPACE2
