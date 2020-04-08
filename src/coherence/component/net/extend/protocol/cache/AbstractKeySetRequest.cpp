/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/AbstractKeySetRequest.hpp"

#include "coherence/util/ArrayList.hpp" // REVIEW

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::util::ArrayList;


// ----- constructors -------------------------------------------------------

AbstractKeySetRequest::AbstractKeySetRequest()
        : f_vCol(self())
    {
    }


// ----- PortableObject interface -------------------------------------------

void AbstractKeySetRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofRequest::readExternal(hIn);

    setKeySet(hIn->readCollection(1, ArrayList::create()));
    }

void AbstractKeySetRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofRequest::writeExternal(hOut);

    hOut->writeCollection(1, getKeySet());

    //release state
    // m_vCol = NULL; // c++ optimization uses FinalView and thus can't be released
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractKeySetRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", KeySet=" << getKeySet());
    }


// ----- accessors ----------------------------------------------------------

Collection::View AbstractKeySetRequest::getKeySet() const
    {
    return f_vCol;
    }

void AbstractKeySetRequest::setKeySet(Collection::View vCol)
    {
    initialize(f_vCol, vCol);
    }

COH_CLOSE_NAMESPACE6
