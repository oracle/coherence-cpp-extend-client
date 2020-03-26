/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNIT_CALCULATOR_HPP
#define COH_UNIT_CALCULATOR_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,net,cache)

// ---- interface: UnitCalculator ---------------------------------------

/**
* A unit calculator is an object that can calculate the cost of
* caching an object. Used with LocalCache.
*
* @author nsa 2008.06.23
*/
class UnitCalculator
    : public interface_spec<UnitCalculator>
    {
    // ----- constants ------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);

    // ----- UnitCalculator interface ---------------------------

    public:
        /**
        * Calculate a cache cost for the specified cache entry key
        * and value.
        *
        * @param vKey     the cache key to evaluate for unit cost
        * @param vValue  the cache value to evaluate for unit cost
        *
        * @return an integer value 0 or greater, with a larger value
        *         signifying a higher cost
        */
        virtual size32_t calculateUnits(Object::View vKey,
                Object::View vValue) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // #define COH_UNIT_CALCULATOR_HPP
