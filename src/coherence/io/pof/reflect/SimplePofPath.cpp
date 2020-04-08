/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/reflect/SimplePofPath.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)

COH_REGISTER_PORTABLE_CLASS(140, SimplePofPath);


// ----- constructors -------------------------------------------------------

SimplePofPath::SimplePofPath()
    : f_vaiElements(self())
    {
    }

SimplePofPath::SimplePofPath(int32_t nIndex)
    : f_vaiElements(self())
    {
    Array<int32_t>::Handle han = Array<int32_t>::create(1);
    han[0] = nIndex;
    initialize(f_vaiElements, han);
    }

SimplePofPath::SimplePofPath(Array<int32_t>::View vanIndices)
    : f_vaiElements(self(), vanIndices)
    {
    }


// ----- internal -----------------------------------------------------------

Array<int32_t>::View SimplePofPath::getPathElements() const
    {
    return f_vaiElements;
    }


// ----- PortableObject interface -------------------------------------------

void SimplePofPath::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vaiElements, hIn->readInt32Array(0));
    }

void SimplePofPath::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32Array(0, f_vaiElements);
    }


// ----- Object interface ---------------------------------------------------

bool SimplePofPath::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }
    SimplePofPath::View that = cast<SimplePofPath::View>(v, false);
    if (that != NULL)
        {
        return Object::equals(this->f_vaiElements, that->f_vaiElements);
        }
    return false;
    }

size32_t SimplePofPath::hashCode() const
    {
    return f_vaiElements->hashCode();
    }

TypedHandle<const String> SimplePofPath::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) << "(indices=" << f_vaiElements << ")");
    }

COH_CLOSE_NAMESPACE4
