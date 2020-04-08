/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/POFObjectInvocable.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

COH_REGISTER_PORTABLE_CLASS(1503, POFObjectInvocable);


// ----- constructors -------------------------------------------------------

POFObjectInvocable::POFObjectInvocable()
    : f_vPOFObject(self())
    {
    }

POFObjectInvocable::POFObjectInvocable(Object::View v)
    : f_vPOFObject(self(), v)
    {
    }

// ----- PortableObject interface -------------------------------------------

void POFObjectInvocable::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vPOFObject, hIn->readObject(0));
    }

void POFObjectInvocable::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vPOFObject);
    }

COH_CLOSE_NAMESPACE2
