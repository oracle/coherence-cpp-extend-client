/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/security/GenericPrincipal.hpp"

#include "coherence/io/pof/PrincipalPofSerializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

using coherence::io::pof::PrincipalPofSerializer;
using coherence::io::pof::SystemPofContext;
using coherence::lang::TypedBarrenClass;

COH_REGISTER_POF_SERIALIZER(900, \
        TypedBarrenClass<GenericPrincipal>::create(), \
        PrincipalPofSerializer::create());

// ----- constructors -------------------------------------------------------

GenericPrincipal::GenericPrincipal(String::View vsName)
    : f_vsName(self(), vsName)
    {
    }

GenericPrincipal::GenericPrincipal(const GenericPrincipal& that)
    : super(that), f_vsName(self(), that.f_vsName)
    {
    }


// ----- Principal interface ------------------------------------------------

String::View GenericPrincipal::getName() const
    {
    return f_vsName;
    }


// ----- Object interface ---------------------------------------------------

bool GenericPrincipal::equals(Object::View vThat) const
    {
    return instanceof<GenericPrincipal::View>(vThat) &&
               getName()->equals(cast<GenericPrincipal::View>(vThat)->
                       getName());
    }

size32_t GenericPrincipal::hashCode() const
    {
    return f_vsName->hashCode();
    }

bool GenericPrincipal::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> GenericPrincipal::toString() const
    {
    return COH_TO_STRING("GenericPrincipal: " << getName());
    }

COH_CLOSE_NAMESPACE2
