/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_HASH_MAP_HPP
#define COH_HASH_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Hashtable.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
* A hash based implementation of coherence::util::Map.
*
* The returned implementation is not guaranteed to be thread-safe.
*
* @see SafeHashMap
* @see Hashtable
*
* @author mf  2008.02.25
*/
typedef Hashtable HashMap;

COH_CLOSE_NAMESPACE2

#endif // COH_HASH_MAP_HPP
