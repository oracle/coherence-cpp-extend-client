/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Integer32.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Integer32>::create());

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int32_t Integer32::min_value)
COH_EXPORT_SPEC_MEMBER(const int32_t Integer32::max_value)


// ----- constructors -------------------------------------------------------

Integer32::Integer32(int32_t n)
    : super(n)
    {
    }

Integer32::Integer32(const Integer32& that)
    : super(that)
    {
    }


// ----- Number interface ---------------------------------------------------

int16_t Integer32::getInt16Value() const
    {
    return (int16_t) getValue();
    }

int32_t Integer32::getInt32Value() const
    {
    return getValue();
    }

int64_t Integer32::getInt64Value() const
    {
    return (int64_t) getValue();
    }

float32_t Integer32::getFloat32Value() const
    {
    return (float32_t) getValue();
    }

float64_t Integer32::getFloat64Value() const
    {
    return (float64_t) getValue();
    }

// ----- static methods -----------------------------------------------------

Integer32::Handle Integer32::valueOf(int32_t n)
    {
    static FinalHandle<Integer32>   hMin(System::common(), Integer32::create(min_value));
    static FinalHandle<Integer32>   hMax(System::common(), Integer32::create(max_value));
    static FinalHandle<ObjectArray> ha(System::common(), ObjectArray::create(256));
    static const int32_t            of    = -1;
    static bool                     fInit = true;
    if (fInit)
        {
        fInit = false;
        for (size32_t i = 0, c = ha->length; i < c; ++i)
            {
            ha->raw[i] = Integer32::create(int32_t(i) + of);
            }
        }
    return n >= of && n < (int32_t(ha->length) + of)
            ? cast<Integer32::Handle>(ha->raw[size32_t(n - of)])
            : n == min_value ? hMin
            : n == max_value ? hMax
            : Integer32::create(n);
    }
COH_STATIC_INIT(Integer32::valueOf(0));

COH_CLOSE_NAMESPACE2
