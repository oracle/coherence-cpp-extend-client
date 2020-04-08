/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FLOAT32_HPP
#define COH_FLOAT32_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Number.hpp"
#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 32-bit floating-point value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Float32
    : public cloneable_spec<Float32,
        extends<Primitive<float32_t> >,
        implements<Number, Comparable> >
    {
    friend class factory<Float32>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Float32 instance.
        *
        * @param fl the value of the new Float32
        *
        * @return the new Float32
        */
        Float32(float32_t fl);

        /**
        * Copy constructor.
        */
        Float32(const Float32& that);


    // ----- Float32 interface ----------------------------------------------

    public:
        /**
        * Return a <code>uint32_t</code> representation of the specified
        * 32-bit floating point value according to the IEEE 754
        * floating-point "single format" bit layout.
        *
        * @param fl  a 32-bit floating-point number
        *
        * @return the <code>uint32_t</code> representation of the
        *         floating-point number
        */
        static uint32_t float32ToBits(float32_t fl);

        /**
        * Return the <code>float32_t</code> value corresponding to a given
        * bit representation. The argument is considered to be a representation
        * of a 32-bit floating-point value according to the IEEE 754
        * floating-point "single format" bit layout.
        *
        * @param n  a 32-bit unsigned integer containing the bit
        *           representation of the floating-point number
        *
        * @return the <code>float32_t</code> floating-point value with the
        *         same bit pattern.
        */
        static float32_t bitsToFloat32(uint32_t n);


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


    // ----- static methods ------------------------------------------------

    public:
        /**
        * Factory method to produce Float32 objects with an optimization
        * that uses cached objects for all common numbers.
        *
        * @param fl  a float32_t
        *
        * @return a Float32 whose value is the passed float32_t
        */
        static Handle valueOf(float32_t fl);

        /**
        * Test whether the given float32_t is a NaN.
        *
        * @param fl  a float32_t to test
        *
        * @return true if the given float32_t is a NaN, else return false
        */
        static bool isNaN(float32_t fl);

        /**
        * Test whether the given float32_t is -0.0 or 0.0.
        *
        * @param fl  a float32_t to test
        *
        * @return true if the given float32_t is a zero, else return false
        */
        static bool isZero(float32_t fl);

        /**
        * Test whether the given float32_t is -infinity or +infinity.
        *
        * @param fl  a float32_t to test
        *
        * @return true if the given float32_t is infinite else return false
        */
        static bool isInfinite(float32_t fl);

        /**
        * Compare the given float32_t values for order. Return a negative
        * integer, zero, or a positive integer if the left value is
        * less than, equal to, or greater than the right. The comparison
        * is based on the following order:
        * -infinity < -0.0 < +0.0 < +infinity < NaN
        *
        * @param fll  the left hand value to be compared
        * @param flr  the right hand value to be compared
        *
        * @return a negative integer, zero, or a positive integer if the
        *         left value is less than, equal to, or greater than the
        *         right value
        */
        static int32_t compare(float32_t fll, float32_t flr);

    // ---- constants -------------------------------------------------------

    public:
       /**
        * Return a constant holding the Not-a-Number (NaN) value of type
        * <code>float32_t</code>. It is equivalent to the value returned by
        * <code>Float32::bitsToFloat32(0x7FC00000)</code>.
        *
        * @return the <code>float32_t</code> NaN value
        */
        static float32_t getNaN();

        /**
        * Return a constant holding the positive infinity value of type
        * <code>float32_t</code>. It is equal to the value returned by
        * <code>Float32::bitsToFloat32(0x7F800000)</code>.
        *
        * @return the <code>float32_t</code> positive infinity value
        */
        static float32_t getPositiveInfinity();

        /**
        * Returns a constant holding the negative infinity value of type
        * <code>float32_t</code>. It is equal to the value returned by
        * <code>Float32::bitsToFloat(0xFF800000)</code>.
        *
        * @return the <code>float32_t</code> negative infinity  value
        */
        static float32_t getNegativeInfinity();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_FLOAT32_HPP
