/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/invocation/InvocationRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)


// ----- constructors -------------------------------------------------------

InvocationRequest::InvocationRequest()
    : f_vTask(self())
    {
    }


// ----- Message interface --------------------------------------------------

int32_t InvocationRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void InvocationRequest::readExternal(PofReader::Handle hIn)
    {
    InvocationServiceRequest::readExternal(hIn);

    setTask(cast<Invocable::View>(hIn->readObject(1)));
    }

void InvocationRequest::writeExternal(PofWriter::Handle hOut) const
    {
    InvocationServiceRequest::writeExternal(hOut);

    hOut->writeObject(1, getTask());
    }


// ----- Describable interface ----------------------------------------------

String::View InvocationRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Task=" << getTask());
    }


// ----- internal methods ---------------------------------------------------

void InvocationRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

Invocable::View InvocationRequest::getTask() const
    {
    return f_vTask;
    }

void InvocationRequest::setTask(Invocable::View vTask)
    {
    initialize(f_vTask, vTask);
    }

COH_CLOSE_NAMESPACE6
