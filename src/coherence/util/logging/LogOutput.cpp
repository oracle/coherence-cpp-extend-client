/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/logging/LogOutput.hpp"

COH_OPEN_NAMESPACE3(coherence,util,logging)


// ----- LogOutput interface ------------------------------------------------

void LogOutput::log(int32_t nLevel, Exception::View vEx,
                    String::View vsMessage)
    {
    write(translateLevel(nLevel), vEx, vsMessage);
    }

void LogOutput::log(int32_t nLevel, String::View vsMessage)
    {
    write(translateLevel(nLevel), vsMessage);
    }

void LogOutput::log(int32_t nLevel, Exception::View vEx)
    {
    write(translateLevel(nLevel), vEx);
    }

void LogOutput::close()
    {
    }

COH_CLOSE_NAMESPACE3
