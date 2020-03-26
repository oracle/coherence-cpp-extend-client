/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Integer16.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Integer16>::create());

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int16_t Integer16::min_value)
COH_EXPORT_SPEC_MEMBER(const int16_t Integer16::max_value)


// ----- constructors -------------------------------------------------------

Integer16::Integer16(int16_t n)
    : super(n)
    {
    }

Integer16::Integer16(const Integer16& that)
    : super(that)
    {
    }

// ----- Number interface ---------------------------------------------------

int16_t Integer16::getInt16Value() const
    {
    return getValue();
    }

int32_t Integer16::getInt32Value() const
    {
    return (int32_t) getValue();
    }

int64_t Integer16::getInt64Value() const
    {
    return (int64_t) getValue();
    }

float32_t Integer16::getFloat32Value() const
    {
    return (float32_t) getValue();
    }

float64_t Integer16::getFloat64Value() const
    {
    return (float64_t) getValue();
    }


// ----- Comparable interface -----------------------------------------------

int32_t Integer16::compareTo(Object::View v) const
    {
    Integer16::View vThat = cast<Integer16::View>(v);
    return getValue() - vThat->getValue();
    }


// ----- static methods -----------------------------------------------------

Integer16::Handle Integer16::valueOf(int16_t n)
    {
    static FinalHandle<Integer16>   hMin(System::common(), Integer16::create(min_value));
    static FinalHandle<Integer16>   hMax(System::common(), Integer16::create(max_value));
    static FinalHandle<ObjectArray> ha(System::common(), ObjectArray::create(256));
    static const int16_t            of    = -1;
    static bool                     fInit = true;
    if (fInit)
        {
        fInit = false;
        for (size32_t i = 0, c = ha->length; i < c; ++i)
            {
            ha->raw[i] = Integer16::create(int16_t(i) + of);
            }
        }
    return n >= of && n < (int16_t(ha->length) + of)
            ? cast<Integer16::Handle>(ha->raw[size32_t(n - of)])
            : n == min_value ? hMin
            : n == max_value ? hMax
            : Integer16::create(n);
    }
COH_STATIC_INIT(Integer16::valueOf(0));

COH_CLOSE_NAMESPACE2
