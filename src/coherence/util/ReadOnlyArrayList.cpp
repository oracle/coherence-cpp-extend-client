/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/ReadOnlyArrayList.hpp"


COH_OPEN_NAMESPACE2(coherence,util)

namespace
    {
    ObjectArray::View wrapArray(ObjectArray::View vao, size32_t of = 0,
            size32_t c = List::npos)
        {
        ObjectArray::Handle hao = ObjectArray::create(1);
        hao[0] = of == 0 && (c == List::npos || c == vao->length)
            ? vao
            : vao->subArray(of, of + c);
        return hao;
        }
    }

ReadOnlyArrayList::ReadOnlyArrayList(ObjectArray::View vao, size32_t of,
        size32_t c)
    : super(wrapArray(vao, of, c))
    {
    }


COH_CLOSE_NAMESPACE2
