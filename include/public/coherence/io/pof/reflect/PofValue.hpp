/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_VALUE_HPP
#define COH_POF_VALUE_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)


/**
* PofValue represents the POF data structure in a POF stream, or any
* sub-structure or value thereof.
*
* In C++ PofValue is simply a place-holder interface, see the corresponding
* Java PofValue interface for details.
*
* @author as/gm  2009.04.01
* @since Coherence 3.5
*/
class COH_EXPORT PofValue
    : public interface_spec<PofValue>
    {
    };

COH_CLOSE_NAMESPACE4

#endif // COH_POF_VALUE_HPP
