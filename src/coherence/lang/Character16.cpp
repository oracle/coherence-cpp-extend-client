/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Character16.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Character16>::create());

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const wchar16_t Character16::min_value)
COH_EXPORT_SPEC_MEMBER(const wchar16_t Character16::max_value)


// ----- constructors -------------------------------------------------------

Character16::Character16(wchar16_t ch)
    : super(ch)
    {
    }

Character16::Character16(const Character16& that)
    : super(that)
    {
    }

// ----- Character16 interface ----------------------------------------------

bool Character16::isASCII() const
    {
    wchar16_t ch = getValue();
    return ch > 0 && ch < 128;
    }


// ----- Comparable interface -----------------------------------------------

int32_t Character16::compareTo(Object::View v) const
    {
    Character16::View vThat = cast<Character16::View>(v);
    return ((int16_t) getValue()) - ((int16_t) vThat->getValue());
    }


// ----- static methods -----------------------------------------------------

Character16::Handle Character16::valueOf(wchar16_t ch)
    {
    static FinalHandle<Character16> hMax(System::common(),
            Character16::create(max_value));
    static FinalHandle<ObjectArray> ha(System::common(),
            ObjectArray::create(128));
    static bool                      fInit = true;
    if (fInit)
        {
        fInit = false;
        for (size32_t i = 0, c = ha->length; i < c; ++i)
            {
            ha->raw[i] = Character16::create(wchar16_t(i));
            }
        }
    return ch < wchar16_t(ha->length)
            ? cast<Character16::Handle>(ha->raw[ch])
            : ch == max_value ? hMax
            : Character16::create(ch);
    }
COH_STATIC_INIT(Character16::valueOf(0));

COH_CLOSE_NAMESPACE2
