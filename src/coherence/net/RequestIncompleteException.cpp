/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/RequestIncompleteException.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

COH_REGISTER_PORTABLE_CLASS(4, RequestIncompleteException);

// ----- constructors ----------------------------------------------------

RequestIncompleteException::RequestIncompleteException(String::View vsMsg,
        Exception::View vCause)
        : super(vsMsg, vCause), f_ohPartialResult(self())
    {
    }

RequestIncompleteException::RequestIncompleteException(const RequestIncompleteException& that)
    : super(that),
      f_ohPartialResult(self(), that.f_ohPartialResult)
    {
    }

	
// ----- RequestIncompleteException interface ----------------------------------

Object::Holder RequestIncompleteException::getPartialResult() const
    {
    return f_ohPartialResult;
    }

void RequestIncompleteException::setPartialResult(Object::Holder ohPartialResult)
    {
    initialize(f_ohPartialResult, ohPartialResult);
    }


// ----- PortableObject interface -------------------------------------------

void RequestIncompleteException::readExternal(PofReader::Handle hIn)
    {
    super::readExternal(hIn);

    setPartialResult(hIn->readObject(4));
    }

void RequestIncompleteException::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);

    hOut->writeObject(4, getPartialResult());
    }

COH_CLOSE_NAMESPACE2
