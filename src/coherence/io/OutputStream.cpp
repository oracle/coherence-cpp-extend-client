/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include  "private/coherence/io/OutputStream.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

void OutputStream::close()
    {
    }

void OutputStream::flush()
    {
    }

void OutputStream::write(Array<octet_t>::View vab, size32_t i, size32_t cb)
    {
    if (cb == npos)
        {
        cb = vab->length - i;
        }

    for (size32_t cbWritten = 0; cbWritten < cb; ++cbWritten)
        {
        write(vab[i + cbWritten]);
        }
    }
COH_CLOSE_NAMESPACE2
