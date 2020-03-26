/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/InterruptedIOException.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

// ----- constructors -------------------------------------------------------

InterruptedIOException::InterruptedIOException(String::View vsMsg,
        size32_t cBytesTransfered, Exception::View vCause)
        : super(vsMsg, vCause), m_cBytesTransfered(cBytesTransfered)
    {
    }

// ----- InterruptedIOException ---------------------------------------------

size32_t InterruptedIOException::getBytesTransfered() const
    {
    return m_cBytesTransfered;
    }

COH_CLOSE_NAMESPACE2
