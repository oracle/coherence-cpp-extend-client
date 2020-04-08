/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/dev/compiler/ErrorList.hpp"

#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)

using coherence::util::Iterator;


// ----- constructors -------------------------------------------------------

ErrorList::ErrorList()
    {
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ErrorList::toString() const
    {
    if (isEmpty())
        {
        return String::create("ErrorList is empty.");
        }
    else
        {
        String::View vs = COH_TO_STRING("ErrorList contains " << size() << " items:");

        size32_t c = 0;
        for (Iterator::Handle i = iterator(); i->hasNext(); ++c)
            {
            vs = COH_TO_STRING(vs << "\n[" << c << "]=" << i->next());
            }

        return vs;
        }
    }

COH_CLOSE_NAMESPACE3
