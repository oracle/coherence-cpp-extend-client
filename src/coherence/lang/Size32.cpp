/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Size32.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Size32>::create());

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const size32_t Size32::min_value)
COH_EXPORT_SPEC_MEMBER(const size32_t Size32::max_value)


// ----- constructors -------------------------------------------------------

Size32::Size32(size32_t b)
    : super(b)
    {
    }

Size32::Size32(const Size32& that)
    : super(that)
    {
    }


// ----- Comparable interface -----------------------------------------------

int32_t Size32::compareTo(Object::View v) const
    {
    Size32::View vThat = cast<Size32::View>(v);
    size32_t nThis = getValue();
    size32_t nThat = vThat->getValue();
    return nThis < nThat ? -1 : nThis > nThat ? 1 : 0;
    }

// ----- factory methods ----------------------------------------------------

Size32::Handle Size32::valueOf(size32_t n)
    {
    static FinalHandle<ObjectArray> ha(System::common(), ObjectArray::create(256));
    static bool fInit = true;
    if (fInit)
        {
        fInit = false;
        for (size32_t i = 0, c = ha->length; i < c; ++i)
            {
            ha->raw[i] = Size32::create(i);
            }
        }
    return cast<Size32::Handle>(ha->raw[n]);
    }
COH_STATIC_INIT(Size32::valueOf(0));

COH_CLOSE_NAMESPACE2
