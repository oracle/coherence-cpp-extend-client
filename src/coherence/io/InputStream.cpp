/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/InputStream.hpp"
#include "coherence/io/EOFException.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

size32_t InputStream::read(Array<octet_t>::Handle hab, size32_t i, size32_t cb)
    {
    if (cb == npos)
        {
        cb = hab->length - i;
        }

    size32_t cbRead = 0;
    try
        {
        while (cbRead < cb)
            {
            hab[i + cbRead] = read();
            ++cbRead;
            }
        }
    catch (EOFException::View e)
        {
        if (cbRead == 0)
            {
            throw;
            }
        }
    return cbRead;
    }

void InputStream::readFully(Array<octet_t>::Handle hab, size32_t i, size32_t cb)
    {
    if (cb == npos)
        {
        cb = hab->length - i;
        }

    size32_t cbRead = 0;
    while (cbRead < cb)
        {
        cbRead += read(hab, i + cbRead, cb - cbRead);
        }
    }

void InputStream::close()
    {
    }

size32_t InputStream::available()
    {
    return 0;
    }

COH_CLOSE_NAMESPACE2
