/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_FLOAT_HPP
#define COH_NATIVE_FLOAT_HPP

#include "coherence/lang/compatibility.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


/**
* NativeFloat provides a generic interface for converting from float32_t to
* uint32_t, from uint32_t to float32_t, from float64_t to uint64_t, and from
* uint64_t to float64_t
*
* @author djl  2008.04.22
*/
class COH_EXPORT NativeFloat
    {
    // ----- NativeFloat interface ------------------------------------------

    public:
        /**
        * Returns a representation of the specified 32-bit floating-point
        * value according to the IEEE 754 floating-point "single format" bit
        * layout.
        *
        * Bit 31 (the bit that is selected by the mask
        * <code>0x80000000</code>) represents the sign of the floating-point
        * number.
        * Bits 30-23 (the bits that are selected by the mask
        * <code>0x7f800000</code>) represent the exponent.
        * Bits 22-0 (the bits that are selected by the mask
        * <code>0x007fffff</code>) represent the significand (sometimes
        * called the mantissa) of the floating-point number.
        *
        * If the argument is positive infinity, the result is
        * <code>0x7f800000</code>.
        *
        * If the argument is negative infinity, the result is
        * <code>0xff800000</code>.
        *
        * If the argument is NaN, the result is <code>0x7fc00000</code>.
        *
        * In all cases, the result is a 32-bit integer that, when given to
        * the {@link #bitsToFloat32} method, will produce a 32-bit
        * floating-point value the same as the argument to
        * <code>float32ToBits</code> (except all NaN values are collapsed to
        * a single &quot;canonical&quot; NaN value).
        *
        * @param fl  a 32-bit floating-point number
        *
        * @return the bits that represent the floating-point number
        */
        static uint32_t float32ToBits(float32_t fl);

        /**
        * Returns the <code>float32_t</code> value corresponding to a given
        * bit represention. The argument is considered to be a representation
        * of a 32-bit floating-point value according to the IEEE 754
        * floating-point "single format" bit layout.
        *
        * If the argument is <code>0x7f800000</code>, the result is positive
        * infinity.
        *
        * If the argument is <code>0xff800000</code>, the result is negative
        * infinity.
        *
        * If the argument is any value in the range
        * <code>0x7f800001</code> through <code>0x7fffffff</code> or in
        * the range <code>0xff800001</code> through
        * <code>0xffffffff</code>, the result is a NaN. No IEEE 754
        * floating-point operation provided can distinguish between two NaN
        * values of the same type with different bit patterns. Distinct
        * values of NaN are only distinguishable by use of the
        * <code>Float32::float32ToBits</code> method.
        *
        * In all other cases, let <i>s</i>, <i>e</i>, and <i>m</i> be three
        * values that can be computed from the argument:
        * <blockquote><pre>
        * int32_t s = ((bits &gt;&gt; 31) == 0) ? 1 : -1;
        * int32_t e = ((bits &gt;&gt; 23) & 0xff);
        * int32_t m = (e == 0) ?
        *                 (bits & 0x7fffff) &lt;&lt; 1 :
        *                 (bits & 0x7fffff) | 0x800000;
        * </pre></blockquote>
        * Then the floating-point result equals the value of the mathematical
        * expression <i>s</i>&middot;<i>m</i>&middot;2<sup><i>e</i>-150</sup>.
        *
        * Note that this method may not be able to return a
        * <code>float32_t</code> NaN with exactly same bit pattern as the
        * <code>int32_t</code> argument. IEEE 754 distinguishes between two
        * kinds of NaNs, quiet NaNs and <i>signaling NaNs</i>.  The
        * differences between the two kinds of NaN are generally not
        * visible. Arithmetic operations on signaling NaNs turn them into
        * quiet NaNs with a different, but often similar, bit pattern.
        * However, on some processors merely copying a signaling NaN also
        * performs that conversion. In particular, copying a signaling NaN to
        * return it to the calling method may perform this conversion. So
        * <code>bitsToFloat32</code> may not be able to return a
        * <code>float32_t</code> with a signaling NaN bit pattern.
        * Consequently, for some <code>int32_t</code> values,
        * <code>float32ToBits(bitsToFloat32(start))</code> may <i>not</i>
        * equal <code>start</code>. Moreover, which particular bit patterns
        * represent signaling NaNs is platform dependent; although all NaN
        * bit patterns, quiet or signaling, must be in the NaN range
        * identified above.
        *
        * @param n  a 32-bit integer
        *
        * @return the <code>float32_t</code> floating-point value with the
        *         same bit pattern.
        */
        static float32_t bitsToFloat32(uint32_t n);

        /**
        * Returns a representation of the specified 64-bit floating-point
        * value according to the IEEE 754 floating-point "double format" bit
        * layout.
        *
        * Bit 63 (the bit that is selected by the mask
        * <code>0x8000000000000000L</code>) represents the sign of the
        * floating-point number. Bits
        * 62-52 (the bits that are selected by the mask
        * <code>0x7ff0000000000000L</code>) represent the exponent. Bits 51-0
        * (the bits that are selected by the mask
        * <code>0x000fffffffffffffL</code>) represent the significand
        * (sometimes called the mantissa) of the floating-point number.
        *
        * If the argument is positive infinity, the result is
        * <code>0x7ff0000000000000L</code>.
        *
        * If the argument is negative infinity, the result is
        * <code>0xfff0000000000000L</code>.
        *
        * If the argument is NaN, the result is
        * <code>0x7ff8000000000000L</code>.
        *
        * In all cases, the result is a 64-bit integer that, when given to
        * the {@link #bitsToFloat64} method, will produce a 64-bit
        * floating-point value the same as the argument to
        * <code>float64ToBits</code> (except all NaN values are collapsed to
        * a single &quot;canonical&quot; NaN value).
        *
        * @param dfl  a 64-bit floating-point number
        *
        * @return the bits that represent the floating-point number
        */
        static uint64_t float64ToBits(float64_t dfl);

        /**
        * Returns the <code>float64_t</code> value corresponding to a given
        * bit representation. The argument is considered to be a
        * representation of a 64-bit floating-point value according to the
        * IEEE 754 floating-point "double format" bit layout.
        *
        * If the argument is <code>0x7ff0000000000000L</code>, the result
        * is positive infinity.
        *
        * If the argument is <code>0xfff0000000000000L</code>, the result
        * is negative infinity.
        *
        * If the argument is any value in the range
        * <code>0x7ff0000000000001L</code> through
        * <code>0x7fffffffffffffffL</code> or in the range
        * <code>0xfff0000000000001L</code> through
        * <code>0xffffffffffffffffL</code>, the result is a NaN. No IEEE
        * 754 floating-point operation provided can distinguish between two
        * NaN values of the same type with different bit patterns. Distinct
        * values of NaN are only distinguishable by use of the
        * <code>Float64::float64ToBits</code> method.
        *
        * In all other cases, let <i>s</i>, <i>e</i>, and <i>m</i> be three
        * values that can be computed from the argument:
        * <blockquote><pre>
        * uint64_t s = ((bits &gt;&gt; 63) == 0) ? 1 : -1;
        * uint64_t e = (int)((bits &gt;&gt; 52) & 0x7ffL);
        * uint64_t m = (e == 0) ?
        *                 (bits & 0xfffffffffffffL) &lt;&lt; 1 :
        *                 (bits & 0xfffffffffffffL) | 0x10000000000000L;
        * </pre></blockquote>
        * Then the floating-point result equals the value of the mathematical
        * expression <i>s</i>&middot;<i>m</i>&middot;2<sup><i>e</i>-1075</sup>.
        *
        * Note that this method may not be able to return a
        * <code>float64_t</code> NaN with exactly same bit pattern as the
        * <code>uint64_t</code> argument. IEEE 754 distinguishes between two
        * kinds of NaNs, quiet NaNs and <i>signaling NaNs</i>.  The
        * differences between the two kinds of NaN are generally not
        * visible. Arithmetic operations on signaling NaNs turn them into
        * quiet NaNs with a different, but often similar, bit pattern.
        * However, on some processors merely copying a signaling NaN also
        * performs that conversion. In particularm copying a signaling NaN to
        * return it to the calling method may perform this conversion. So
        * <code>bitsToFloat64</code> may not be able to return a
        * <code>float64_t</code> with a signaling NaN bit pattern.
        * Consequently, for some <code>uint64_t</code> values,
        * <code>float64ToBits(bitsToFloat64(start))</code> may <i>not</i>
        * equal <code>start</code>. Moreover, which particular bit patterns
        * represent signaling NaNs is platform dependent; although all NaN
        * bit patterns, quiet or signaling, must be in the NaN range
        * identified above.
        *
        * @param l  any <code>uint64_t</code> integer
        *
        * @return  the <code>float64_t</code> floating-point value with the
        *          same bit pattern
        */
        static float64_t bitsToFloat64(uint64_t l);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_FLOAT_HPP
