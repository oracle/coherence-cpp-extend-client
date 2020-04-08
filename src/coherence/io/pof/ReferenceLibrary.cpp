/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/ReferenceLibrary.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


// ----- constructors --------------------------------------------------------

ReferenceLibrary::ReferenceLibrary()
    : m_cRefs(0),
      f_hMapIdentities(self(), IdentityHashMap::create())
    {
    }


// ----- member methods -------------------------------------------------

int32_t ReferenceLibrary::getIdentity(Object::View v) const
    {
    Integer32::View I = cast<Integer32::View>(f_hMapIdentities->get(v));
    return I == NULL ? -1 : I->getInt32Value();
    }

int32_t ReferenceLibrary::registerReference(Object::View v)
    {
    int32_t iRef = ++m_cRefs;
    Integer32::View IOld = cast<Integer32::View>(f_hMapIdentities->put(v, Integer32::create(iRef)));
    if (NULL != IOld)
        {
        --m_cRefs;
        f_hMapIdentities->put(v, IOld);
        COH_THROW (IllegalStateException::create("object already registered"));
        }

    return iRef;
    }

COH_CLOSE_NAMESPACE3
