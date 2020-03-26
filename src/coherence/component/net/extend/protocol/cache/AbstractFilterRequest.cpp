/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/AbstractFilterRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

AbstractFilterRequest::AbstractFilterRequest()
        : f_vFilter(self())
    {
    }


// ----- PortableObject interface -------------------------------------------

void AbstractFilterRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofRequest::readExternal(hIn);

    setFilter(cast<Filter::View>(hIn->readObject(1)));
    }

void AbstractFilterRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofRequest::writeExternal(hOut);

    hOut->writeObject(1, getFilter());
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractFilterRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Filter=" << getFilter());
    }


// ----- accessors ----------------------------------------------------------

Filter::View AbstractFilterRequest::getFilter() const
    {
    return f_vFilter;
    }

void AbstractFilterRequest::setFilter(Filter::View vFilter)
    {
    initialize(f_vFilter, vFilter);
    }

COH_CLOSE_NAMESPACE6
