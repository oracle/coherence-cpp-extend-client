/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FILTER_HPP
#define COH_FILTER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Provide for "pluggable" conditional behavior.
*
* @author jh  2008.02.26
*/
class COH_EXPORT Filter
    : public interface_spec<Filter>
    {
    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * Apply the test to the object.
        *
        * @param v  the object to test
        *
        * @return true if the test passes, false otherwise
        */
        virtual bool evaluate(Object::View v) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_FILTER_HPP
