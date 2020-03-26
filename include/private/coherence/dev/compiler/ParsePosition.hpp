/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PARSE_POSITION_HPP
#define COH_PARSE_POSITION_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)


/**
* This interface tags an object as being a storable/restorable location
* in a parsing/tokenizing process.
*
* @author tb  2007.12.13
*/
class COH_EXPORT ParsePosition
    : public interface_spec<ParsePosition>
    {
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PARSE_POSITION_HPP
