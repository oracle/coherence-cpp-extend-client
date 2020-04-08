/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractSet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- Object interface ---------------------------------------------------

bool AbstractSet::equals(Object::View v) const
    {
    Set::View vThat = cast<Set::View>(v, false);

    if(vThat == this)
        {
        return true;
        }

    if (NULL == vThat )
        {
        return false;
        }

    if(vThat->size() != size())
        {
        return false;
        }

    return containsAll(vThat);
    }

size32_t AbstractSet::hashCode() const
    {
    size32_t nHash = 0;
    for (Iterator::Handle i = iterator(); i->hasNext(); )
        {
        Object::View v = i->next();
        if (NULL != v)
            {
            nHash += v->hashCode();
            }
        }
    return nHash;
    }

COH_CLOSE_NAMESPACE2
