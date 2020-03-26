/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/PartialResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

PartialResponse::PartialResponse()
	: f_vFilter(self()), f_vFilterCookie(self())
    {
    }


// ----- Message interface --------------------------------------------------

int32_t PartialResponse::getTypeId() const
    {
    return type_id;
    }

void PartialResponse::run()
    {
    // no-op
    }


// ----- PortableObject interface -------------------------------------------

void PartialResponse::readExternal(PofReader::Handle hIn)
    {
    AbstractPartialResponse::readExternal(hIn);

    // COH-6337
    if (getImplVersion() > 2)
        {
        setFilter(cast<Filter::View>(hIn->readObject(7)));
        setFilterCookie(hIn->readObject(8));
        }
    }

void PartialResponse::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPartialResponse::writeExternal(hOut);
    
    // COH-6337
    if (getImplVersion() > 2)
        {
        hOut->writeObject(7, getFilter());
        hOut->writeObject(8, getFilterCookie());
        }
    }


// ----- accessors ------------------------------------------------------

Filter::View PartialResponse::getFilter() const
    {
    return f_vFilter;
    }

void PartialResponse::setFilter(Filter::View vFilter)
    {
    initialize(f_vFilter, vFilter);
    }

Object::View PartialResponse::getFilterCookie() const
    {
    return f_vFilterCookie;
    }

void PartialResponse::setFilterCookie(Object::View vCookie)
    {
    initialize(f_vFilterCookie, vCookie);
    }

COH_CLOSE_NAMESPACE6
