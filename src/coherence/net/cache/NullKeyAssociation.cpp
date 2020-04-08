/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/cache/NullKeyAssociation.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)


// ----- KeyAssociation interface -------------------------------------------

Object::View NullKeyAssociation::getAssociatedKey() const
	{
	return NULL;
	}

COH_CLOSE_NAMESPACE3
