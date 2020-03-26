/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/IdentityExtractor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(52, IdentityExtractor);


// ----- constructors -------------------------------------------------------

IdentityExtractor::IdentityExtractor()
    {
    }


// ----- ValueExtractor interface -------------------------------------------

Object::Holder IdentityExtractor::extract(Object::Holder ohTarget) const
    {
    return ohTarget;
    }


// ----- PortableObject interface -------------------------------------------

void IdentityExtractor::readExternal(PofReader::Handle /* hIn */)
    {
    }

void IdentityExtractor:: writeExternal(PofWriter::Handle /* hOut */) const
    {
    }


// ----- Object interface ---------------------------------------------------

bool IdentityExtractor::equals(Object::View v) const
    {
    return instanceof<IdentityExtractor::View>(v);
    }

size32_t IdentityExtractor::hashCode() const
    {
    return 7;
    }

TypedHandle<const String> IdentityExtractor::toString() const
    {
    return String::create("IdentityExtractor");
    }


// ---- constants -----------------------------------------------------------

IdentityExtractor::Handle IdentityExtractor::getInstance()
    {
    static FinalHandle<IdentityExtractor> hIE(System::common(),
            IdentityExtractor::create());
    return hIE;
    }
COH_STATIC_INIT(IdentityExtractor::getInstance());

COH_CLOSE_NAMESPACE3
