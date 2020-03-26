/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/messaging/ConnectionException.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

COH_REGISTER_PORTABLE_CLASS(3, ConnectionException);

// ----- constructors -------------------------------------------------------

ConnectionException::ConnectionException(String::View vsMsg, Exception::View vCause,
        Object::View vConnection)
        : super((vConnection == NULL
                ? vsMsg
                : vsMsg == NULL
                    ? (String::View) COH_TO_STRING(vConnection)
                    : (String::View) COH_TO_STRING(vConnection << ": " << vsMsg))
                      , vCause)
    {
    }

COH_CLOSE_NAMESPACE3
