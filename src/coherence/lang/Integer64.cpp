/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Integer64.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Integer64>::create());


// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int64_t Integer64::min_value)
COH_EXPORT_SPEC_MEMBER(const int64_t Integer64::max_value)


// ----- constructors -------------------------------------------------------

Integer64::Integer64(int64_t l)
    : super(l)
    {
    }

Integer64::Integer64(const Integer64& that)
    : super(that)
    {
    }


// ----- Number interface ---------------------------------------------------

int16_t Integer64::getInt16Value() const
    {
    return (int16_t) getValue();
    }

int32_t Integer64::getInt32Value() const
    {
    return (int32_t) getValue();
    }

int64_t Integer64::getInt64Value() const
    {
    return getValue();
    }

float32_t Integer64::getFloat32Value() const
    {
    return (float32_t) getValue();
    }

float64_t Integer64::getFloat64Value() const
    {
    return (float64_t) getValue();
    }


// ----- static methods -----------------------------------------------------

Integer64::Handle Integer64::valueOf(int64_t l)
    {
    static FinalHandle<Integer64>   hMin(System::common(), Integer64::create(min_value));
    static FinalHandle<Integer64>   hMax(System::common(), Integer64::create(max_value));
    static FinalHandle<ObjectArray> ha(System::common(), ObjectArray::create(256));
    static const int64_t                       of    = -1;
    static bool                                fInit = true;
    if (fInit)
        {
        fInit = false;
        for (size32_t i = 0, c = ha->length; i < c; ++i)
            {
            ha->raw[i] = Integer64::create(int64_t(i) + of);
            }
        }
    return l >= of && l < (ha->length + of)
            ? cast<Integer64::Handle>(ha->raw[size32_t(l - of)])
            : l == min_value ? hMin
            : l == max_value ? hMax
            : Integer64::create(l);
    }
COH_STATIC_INIT(Integer64::valueOf(0));

COH_CLOSE_NAMESPACE2
