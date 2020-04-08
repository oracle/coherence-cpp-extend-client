/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/security/auth/GenericSubject.hpp"

#include "coherence/io/pof/SubjectPofSerializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/security/Principal.hpp"
#include "coherence/security/GenericPrincipal.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE3(coherence,security,auth)

using coherence::io::pof::SubjectPofSerializer;
using coherence::io::pof::SystemPofContext;
using coherence::lang::TypedBarrenClass;
using coherence::security::Principal;
using coherence::util::Collections;
using coherence::util::HashSet;
using coherence::util::Iterator;

COH_REGISTER_POF_SERIALIZER(950, TypedBarrenClass<GenericSubject>::create(), \
        SubjectPofSerializer::create());

namespace
    {
    /**
    * Ensure the stored set contains only immutable Principals.
    *
    * @param vSet  the set to ensure
    *
    * @return an "ensured" copy of the origial set
    */
    Set::View ensurePrincipals(Set::View vSet)
        {
        if (vSet == NULL)
            {
            return NULL;
            }

        Set::Handle hSetCopy = HashSet::create(vSet->size());
        for (Iterator::Handle hIter = vSet->iterator(); hIter->hasNext(); )
            {
            hSetCopy->add(Immutable<Principal>(cast<Principal::View>(hIter->next())));
            }

        return hSetCopy;
        }

    /**
    * Return a Set containing a GenericPrincipal.
    */
    Set::View ensurePrincipal(String::View vsName)
        {
        return Collections::singleton(GenericPrincipal::create(vsName));
        }
    }


// ----- constructors -------------------------------------------------------

GenericSubject::GenericSubject(Set::View vsSetPrincipals)
    : f_vSetPrincipals(self(), ensurePrincipals(vsSetPrincipals))
    {
    }

GenericSubject::GenericSubject(String::View vsName)
    : f_vSetPrincipals(self(), ensurePrincipal(vsName))
    {
    }

GenericSubject::GenericSubject(const GenericSubject& that)
    : f_vSetPrincipals(self(), that.f_vSetPrincipals)
    {
    }


// ----- Subject interface --------------------------------------------------

Set::View GenericSubject::getPrincipals() const
    {
    return f_vSetPrincipals;
    }


// ----- Object interface ---------------------------------------------------

bool GenericSubject::equals(Object::View vThat) const
    {
    return instanceof<GenericSubject::View>(vThat) &&
               Object::equals(getPrincipals(), cast<GenericSubject::View>(vThat)->
                       getPrincipals());
    }

size32_t GenericSubject::hashCode() const
    {
    return Object::hashCode(getPrincipals());
    }

bool GenericSubject::isImmutable() const
    {
    return true;
    }

TypedHandle<const String> GenericSubject::toString() const
    {
    return COH_TO_STRING("GenericSubject: " << getPrincipals());
    }


COH_CLOSE_NAMESPACE3
