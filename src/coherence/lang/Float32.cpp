/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Float32.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

#include "private/coherence/native/NativeFloat.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Float32>::create());

using coherence::native::NativeFloat;


// ----- constructors -------------------------------------------------------

Float32::Float32(float32_t fl)
    : super(fl)
    {
    }

Float32::Float32(const Float32& that)
    : super(that)
    {
    }


// ----- Float32 interface --------------------------------------------------

uint32_t Float32::float32ToBits(float32_t fl)
    {
    return NativeFloat::float32ToBits(fl);
    }

float32_t Float32::bitsToFloat32(uint32_t n)
    {
    return NativeFloat::bitsToFloat32(n);
    }


// ----- Number interface ---------------------------------------------------

int16_t Float32::getInt16Value() const
    {
    return (int16_t) getValue();
    }

int32_t Float32::getInt32Value() const
    {
    return (int32_t) getValue();
    }

int64_t Float32::getInt64Value() const
    {
    return (int64_t) getValue();
    }

float32_t Float32::getFloat32Value() const
    {
    return getValue();
    }

float64_t Float32::getFloat64Value() const
    {
    return (float64_t) getValue();
    }


// ----- Comparable interface -----------------------------------------------

int32_t Float32::compareTo(Object::View v) const
    {
    return compare(getValue(), cast<Float32::View>(v)->getValue());
    }


// ----- Object interface ---------------------------------------------------

bool Float32::equals(Object::View v) const
    {
    if (NULL == v)
        {
        return false;
        }

    if (this == v)
        {
        return true;
        }

    View vThat = cast<Float32::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return float32ToBits(getValue()) == float32ToBits(vThat->getValue());
    }

size32_t Float32::hashCode() const
    {
    return (size32_t) float32ToBits(getValue());
    }


// ----- static methods -----------------------------------------------------

Float32::Handle Float32::valueOf(float32_t fl)
    {
    static FinalHandle<Float32> hflZero(System::common(), create(0.0F));
    static FinalHandle<Float32> hflOne(System::common(), create(1.0F));
    static FinalHandle<Float32> hflPI(System::common(), create(getPositiveInfinity()));
    static FinalHandle<Float32> hflNI(System::common(), create(getNegativeInfinity()));
    static FinalHandle<Float32> hflNaN(System::common(), create(getNaN()));

    if (isZero(fl))
        {
        return hflZero;
        }
    else if (compare(fl, 1.0f) == 0)
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
COH_STATIC_INIT(Float32::valueOf(0.0F));

bool Float32::isNaN(float32_t fl)
    {
    return float32ToBits(fl) == 0x7FC00000;
    }

bool Float32::isZero(float32_t fl)
    {
    return (float32ToBits(fl) & 0x7FFFFFFF) == 0;
    }

bool Float32::isInfinite(float32_t fl)
    {
    uint32_t n = float32ToBits(fl);
    return n == 0x7F800000 || n == 0xFF800000;
    }

int32_t Float32::compare(float32_t fll, float32_t flr)
    {
    if (fll < flr)
        {
         return -1;     // left number is smaller and neither is NaN
        }
    if (fll > flr)
        {
        return 1;      // left value is larger and neiter is NaN
        }

     int32_t nLeftBits  = (int32_t) float32ToBits(fll);
     int32_t nRightBits = (int32_t) float32ToBits(flr);

     //The IEEE754 format allows us to now compare
     return (nLeftBits == nRightBits ?  0 : // Values are equal
            (nLeftBits  < nRightBits ? -1 : // -0.0 < 0.0) or !NaN < NaN
             1));                           // 0.0 > -0.0) or NaN > !NaN
    }


// ---- constants -----------------------------------------------------------

float32_t Float32::getNaN()
    {
    static float32_t fl = bitsToFloat32(0x7FC00000);
    return fl;
    }
COH_STATIC_INIT(Float32::getNaN());

float32_t Float32::getPositiveInfinity()
     {
     static float32_t fl = bitsToFloat32(0x7F800000);
     return fl;
     }
COH_STATIC_INIT(Float32::getPositiveInfinity());

float32_t Float32::getNegativeInfinity()
    {
    static float32_t fl = bitsToFloat32(0xFF800000);
    return fl;
    }
COH_STATIC_INIT(Float32::getPositiveInfinity());

COH_CLOSE_NAMESPACE2
