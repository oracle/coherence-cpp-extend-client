/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Size64.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Size64>::create());

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const size64_t Size64::min_value)
COH_EXPORT_SPEC_MEMBER(const size64_t Size64::max_value)


// ----- constructors -------------------------------------------------------

Size64::Size64(size64_t b)
    : super(b)
    {
    }

Size64::Size64(const Size64& that)
    : super(that)
    {
    }


// ----- Comparable interface -----------------------------------------------

int32_t Size64::compareTo(Object::View v) const
    {
    Size64::View vThat = cast<Size64::View>(v);
    size64_t nThis = getValue();
    size64_t nThat = vThat->getValue();
    return nThis < nThat ? -1 : nThis > nThat ? 1 : 0;
    }

// ----- factory methods ----------------------------------------------------

Size64::Handle Size64::valueOf(size64_t n)
    {
    static FinalHandle<ObjectArray> ha(System::common(), ObjectArray::create(256));
    static bool fInit = true;
    if (fInit)
        {
        fInit = false;
        for (size32_t i = 0, c = ha->length; i < c; ++i)
            {
            ha->raw[i] = Size64::create(size64_t(i));
            }
        }
    return cast<Size64::Handle>(ha->raw[n]);
    }
COH_STATIC_INIT(Size64::valueOf(0));

COH_CLOSE_NAMESPACE2
