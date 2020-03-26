/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OBJECT_ARRAY_HPP
#define COH_OBJECT_ARRAY_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Array.hpp"
#include "coherence/lang/MemberHolder.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/SubscriptHandle.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An array of Objects, referenced as either Handles or Views.
*
* This is a typedef of Array<MemberHolder> and as such assignments to the
* array elements are thread-safe.
*
* @see Array
*
* @author mf  2008.01.11
*/
typedef Array<MemberHolder<Object> > ObjectArray;

COH_CLOSE_NAMESPACE2

#endif // COH_OBJECT_ARRAY_HPP
