/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FLOAT64_HPP
#define COH_FLOAT64_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Number.hpp"
#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 64-bit floating-point value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Float64
    : public cloneable_spec<Float64,
        extends<Primitive<float64_t> >,
        implements<Number, Comparable> >
    {
    friend class factory<Float64>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Float64 instance.
        *
        * @param dfl the value of the new Float64
        */
        Float64(float64_t dfl);

        /**
        * Copy constructor.
        */
        Float64(const Float64& that);


    // ----- Float64 interface ----------------------------------------------

    public:
        /**
        * Return a <code>uint64_t</code> representation of the specified
        * 64-bit floating point value according to the IEEE 754
        * floating-point "double format" bit layout.
        *
        * @param dfl  a 64-bit floating-point number
        *
        * @return the <code>uint64_t</code> representation of the
        *         floating-point number
        */
        static uint64_t float64ToBits(float64_t dfl);

        /**
        * Return the <code>float64_t</code> value corresponding to a given
        * bit representation. The argument is considered to be a representation
        * of a 64-bit floating-point value according to the IEEE 754
        * floating-point "double format" bit layout.
        *
        * @param l  a 64-bit unsigned integer containing the bit
        *           representation of the floating-point number
        *
        * @return the <code>float64_t</code> floating-point value with the
        *         same bit pattern.
        */
        static float64_t bitsToFloat64(uint64_t l);


    // ----- Number interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int16_t getInt16Value() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getInt32Value() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getInt64Value() const;

        /**
        * {@inheritDoc}
        */
        virtual float32_t getFloat32Value() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getFloat64Value() const;


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Factory method to produce Float64 objects with an optimization
        * that uses cached objects for all common numbers.
        *
        * @param dfl  a float64_t
        *
        * @return a Float64 whose value is the passed float64_t
        */
        static Handle valueOf(float64_t fl);

        /**
        * Test whether the given float64_t is a NaN.
        *
        * @param dfl  a float64_t to test
        *
        * @return true if the given float64_t is a Nan, else return false
        */
        static bool isNaN(float64_t dfl);

        /**
        * Test whether the given float64_t is -0.0 or 0.0.
        *
        * @param dfl  a float64_t to test
        *
        * @return true if the given float64_t is a zero, else return false
        */
        static bool isZero(float64_t dfl);

        /**
        * Test whether the given float64_t is -infinity or +infinity
        *
        * @param dfl  a float64_t to test
        *
        * @return true if the given float64_t is infinity else return false
        */
        static bool isInfinite(float64_t dfl);

        /**
        * Compare the given float32_t values for order. Return a negative
        * integer, zero, or a positive integer if the left value is
        * less than, equal to, or greater than the right. The comparison
        * is based on the following order:
        * -infinity < -0.0 < +0.0 < +infinity < NaN
        *
        * @param dfll  the left hand value to be compared
        * @param dflr  the right hand value to be compared
        *
        *
        * @return a negative integer, zero, or a positive integer if the
        *         left value is less than, equal to, or greater than the
        *         right value
        */
        static int32_t compare(float64_t dfll, float64_t dflr);

    // ---- constants -------------------------------------------------------

    public:
        /**
        * Return a constant holding the Not-a-Number (NaN) value of type
        * <code>float64_t</code>. It is equivalent to the value returned by
        * <code>Float64::bitsToFloat64(((uint64_t) 0x7FF80000) << 32)</code>.
        *
        * @return the <code>float64_t</code> NaN value
        */
        static float64_t getNaN();

        /**
        * Return a constant holding the positive infinity value of type
        * <code>float64_t</code>. It is equal to the value returned by
        * <code>Float64::bitsToFloat64(((uint64_t) 0x7FF00000) << 32)</code>.
        *
        * @return the <code>float64_t</code> positive infinity value
        */
        static float64_t getPositiveInfinity();

        /**
        * Returns a constant holding the negative infinity value of type
        * <code>float64_t</code>. It is equal to the value returned by
        * <code>Float64::bitsToFloat64(((uint64_t) 0xFFF00000) << 32)</code>.
        *
        * @return the <code>float64_t</code> negative infinity  value
        */
        static float64_t getNegativeInfinity();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_FLOAT64_HPP
