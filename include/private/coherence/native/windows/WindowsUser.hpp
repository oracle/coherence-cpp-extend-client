/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_USER_HPP
#define COH_WINDOWS_USER_HPP

#include "coherence/lang/compatibility.hpp"

#include "private/coherence/native/NativeUser.hpp"

#include <Lmcons.h>

COH_OPEN_NAMESPACE2(coherence,native)

String::View NativeUser::getUserName()
    {
    DWORD   cb = UNLEN + 1;
    wchar_t ach[UNLEN + 1];

    if (GetUserNameW(ach, &cb))
        {
        return String::create(ach);
        }

    return NULL;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_USER_HPP
