/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYNCHRONOUS_LISTENER_HPP
#define COH_SYNCHRONOUS_LISTENER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)

/**
* A tag interface indicating that a listener implementation has to receive
* the event notifications synchronously on the corresponding service's thread.
* 
* This interface should be considered as a very advanced feature, as
* a listener implementation that is marked as a SynchronousListener
* must exercise extreme caution during event processing since any delay
* with return or unhandled exception will cause a delay or complete
* shutdown of the corresponding service.
*/
class COH_EXPORT SynchronousListener
    : public interface_spec<SynchronousListener>
    {
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SYNCHRONOUS_LISTENER_HPP
