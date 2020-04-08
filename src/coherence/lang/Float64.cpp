/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Float64.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

#include "private/coherence/native/NativeFloat.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Float64>::create());

using coherence::native::NativeFloat;


// ----- constructors -------------------------------------------------------

Float64::Float64(float64_t dfl)
    : super(dfl)
    {
    }

Float64::Float64(const Float64& that)
    : super(that)
    {
    }


// ----- Float64 interface --------------------------------------------------

uint64_t Float64::float64ToBits(float64_t dfl)
    {
    return NativeFloat::float64ToBits(dfl);
    }

float64_t Float64::bitsToFloat64(uint64_t l)
    {
    return NativeFloat::bitsToFloat64(l);
    }


// ----- Number interface ---------------------------------------------------

int16_t Float64::getInt16Value() const
    {
    return (int16_t) getValue();
    }

int32_t Float64::getInt32Value() const
    {
    return (int32_t) getValue();
    }

int64_t Float64::getInt64Value() const
    {
    return (int64_t) getValue();
    }

float32_t Float64::getFloat32Value() const
    {
    return (float32_t) getValue();
    }

float64_t Float64::getFloat64Value() const
    {
    return getValue();
    }


// ----- Comparable interface -----------------------------------------------

int32_t Float64::compareTo(Object::View v) const
    {
    return compare(getValue(), cast<Float64::View>(v)->getValue());
    }


// ----- Object interface ---------------------------------------------------

bool Float64::equals(Object::View v) const
    {
    if (NULL == v)
        {
        return false;
        }

    if (this == v)
        {
        return true;
        }

    View vThat = cast<Float64::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return float64ToBits(getValue()) == float64ToBits(vThat->getValue());
    }

size32_t Float64::hashCode() const
    {
    uint64_t lBits = float64ToBits(getValue());
    return (size32_t) (lBits ^ (lBits >> 32));
    }


// ----- static methods -----------------------------------------------------

Float64::Handle Float64::valueOf(float64_t fl)
    {
    static FinalHandle<Float64> hflZero(System::common(), create(0.0));
    static FinalHandle<Float64> hflOne(System::common(), create(1.0));
    static FinalHandle<Float64> hflPI(System::common(), create(getPositiveInfinity()));
    static FinalHandle<Float64> hflNI(System::common(), create(getNegativeInfinity()));
    static FinalHandle<Float64> hflNaN(System::common(), create(getNaN()));

    if (isZero(fl))
        {
        return hflZero;
        }
    else if (compare(fl, 1.0) == 0)
        {
        return hflOne;
        }
    else if (isInfinite(fl) && compare(fl, getPositiveInfinity()) == 0)
        {
        return hflPI;
        }
    else if (isInfinite(fl) && compare(fl , getNegativeInfinity()) == 0)
        {
        return hflNI;
        }
    else if (isNaN(fl))
        {
        return hflNaN;
        }
    else
        {
        return create(fl);
        }
    }
COH_STATIC_INIT(Float64::valueOf(0.0));

bool Float64::isNaN(float64_t dfl)
    {
    return float64ToBits(dfl) == ((uint64_t) 0x7FF80000) << 32;
    }

bool Float64::isZero(float64_t dfl)
    {
    return (float64ToBits(dfl) &
            (((uint64_t) 0x7FFFFFFF) << 32 | ((uint64_t) 0xFFFFFFFF))) == 0;
    }

bool Float64::isInfinite(float64_t dfl)
    {
    uint64_t l = float64ToBits(dfl);
    return l == (((uint64_t) 0x7FF00000) << 32)
        || l == (((uint64_t) 0xFFF00000) << 32);
    }

int32_t Float64::compare(float64_t dfll, float64_t dflr)
    {
    if (dfll < dflr)
        {
         return -1;     // left number is smaller and neither is NaN
        }
    if (dfll > dflr)
        {
        return 1;      // left value is larger and neiter is NaN
        }

     int64_t lLeftBits  = (int64_t) float64ToBits(dfll);
     int64_t lRightBits = (int64_t) float64ToBits(dflr);

     return (lLeftBits == lRightBits ?  0 : // Values are equal
            (lLeftBits  < lRightBits ? -1 : // -0.0 < 0.0) or !NaN < NaN
             1));                          // 0.0 > -0.0) or NaN > !NaN

    }

// ---- constants -----------------------------------------------------------

float64_t Float64::getNaN()
    {
    static float64_t dfl = bitsToFloat64(((uint64_t) 0x7FF80000) << 32);
    return dfl;
    }
COH_STATIC_INIT(Float64::getNaN());

float64_t Float64::getPositiveInfinity()
     {
     static float64_t dfl = bitsToFloat64(((uint64_t) 0x7FF00000) << 32);
     return dfl;
     }
COH_STATIC_INIT(Float64::getPositiveInfinity());

float64_t Float64::getNegativeInfinity()
    {
    static float64_t dfl = bitsToFloat64(((uint64_t) 0xFFF00000) << 32);
    return dfl;
    }
COH_STATIC_INIT(Float64::getNegativeInfinity());

COH_CLOSE_NAMESPACE2
