/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NUMBER_HPP
#define COH_NUMBER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/interface_spec.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Interface implemented by all managed numeric types.
*
* @author jh  2008.04.21
*/
class COH_EXPORT Number
    : public interface_spec<Number>
    {
    // ----- Number interface -----------------------------------------------

    public:
        /**
        * Return the numeric value of this object as an int16_t. This may
        * involve rounding or truncation.
        *
        * @return the numeric value of this object as an int16_t
        */
        virtual int16_t getInt16Value() const = 0;

        /**
        * Return the numeric value of this object as an int32_t. This may
        * involve rounding or truncation.
        *
        * @return the numeric value of this object as an int32_t
        */
        virtual int32_t getInt32Value() const = 0;

        /**
        * Return the numeric value of this object as an int64_t. This may
        * involve rounding or truncation.
        *
        * @return the numeric value of this object as an int64_t
        */
        virtual int64_t getInt64Value() const = 0;

        /**
        * Return the numeric value of this object as a float32_t. This may
        * involve rounding.
        *
        * @return the numeric value of this object as a float32_t
        */
        virtual float32_t getFloat32Value() const = 0;

        /**
        * Return the numeric value of this object as a float64_t. This may
        * involve rounding.
        *
        * @return the numeric value of this object as a float64_t
        */
        virtual float64_t getFloat64Value() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NUMBER_HPP
