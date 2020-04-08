/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Octet.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Octet>::create());

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const octet_t Octet::min_value)
COH_EXPORT_SPEC_MEMBER(const octet_t Octet::max_value)


// ----- constructors -------------------------------------------------------

Octet::Octet(octet_t b)
    : super(b)
    {
    }

Octet::Octet(const Octet& that)
    : super(that)
    {
    }


// ----- Comparable interface -----------------------------------------------

int32_t Octet::compareTo(Object::View v) const
    {
    Octet::View vThat = cast<Octet::View>(v);
    return ((int8_t) getValue()) - ((int8_t) vThat->getValue());
    }

// ----- factory methods ----------------------------------------------------

Octet::Handle Octet::valueOf(octet_t b)
    {
    static FinalHandle<ObjectArray> ha(System::common(), ObjectArray::create(256));
    static bool fInit = true;
    if (fInit)
        {
        fInit = false;
        for (size32_t i = 0, c = ha->length; i < c; ++i)
            {
            ha->raw[i] = Octet::create(octet_t(i));
            }
        }
    return cast<Octet::Handle>(ha->raw[b]);
    }
COH_STATIC_INIT(Octet::valueOf(0));

COH_CLOSE_NAMESPACE2
