/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/QueueProcessor.hpp"

#include "coherence/util/DualQueue.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::util::DualQueue;


// ----- constructor --------------------------------------------------------

QueueProcessor::QueueProcessor()
    : f_hQueue(self())
    {
    }


// ----- QueueProcessor interface -------------------------------------------

Queue::Handle QueueProcessor::getQueue()
    {
    return f_hQueue;
    }

Queue::Handle QueueProcessor::instantiateQueue() const
    {
    return DualQueue::create();
    }

void QueueProcessor::setQueue(Queue::Handle hQueue)
    {
    initialize(f_hQueue, hQueue);
    }


// ----- Daemon interface ---------------------------------------------------

bool QueueProcessor::isNotification() const
    {
    return !f_hQueue->isEmpty() || super::isNotification();
    }


// ----- Object interface ---------------------------------------------------

void QueueProcessor::onInit()
    {
    Queue::Handle hQueue = instantiateQueue();

    setQueue(hQueue);
    setLock(hQueue);

    super::onInit();
    }

COH_CLOSE_NAMESPACE3
