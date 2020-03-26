/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/InetAddressHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,net)


InetAddress::View InetAddressHelper::getLocalHost()
    {
    // REVIEW NSA - review the decision not to port this method.
    return InetAddress::getLocalHost();
    }

String::View InetAddressHelper::toString(InetAddress::View vAddr)
    {
    return COH_TO_STRING(vAddr);
    }

COH_CLOSE_NAMESPACE2
