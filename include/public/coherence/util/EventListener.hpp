/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVENT_LISTENER_HPP
#define COH_EVENT_LISTENER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A tag interface that all event listener interfaces must extend.
*
* @author jh  2007.12.12
*/
class COH_EXPORT EventListener
    : public interface_spec<EventListener>
    {
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EVENT_LISTENER_HPP
