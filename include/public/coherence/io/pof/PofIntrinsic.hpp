/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_INTRINSIC_HPP
#define COH_POF_INTRINSIC_HPP

#include "coherence/lang.ns"


COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* PofIntrinsic is a marker interface for built-in POF types.
*
* @author mf  2013.03.27
*/
class COH_EXPORT PofIntrinsic
    : public interface_spec<PofIntrinsic>
    {
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_INTRINSIC
