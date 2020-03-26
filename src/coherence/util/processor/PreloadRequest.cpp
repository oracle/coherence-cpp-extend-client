/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/PreloadRequest.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/NullImplementation.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(98, PreloadRequest);


// ----- constructors -------------------------------------------------------

PreloadRequest::PreloadRequest()
    {
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder PreloadRequest::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    if (!hEntry->isPresent())
        {
        hEntry->getValue();
        }
    return NULL;
    }

Map::View PreloadRequest::processAll(Set::View vsetEntries) const
    {
    super::processAll(vsetEntries);
    return NullImplementation::getMap();
    }


// ----- PortableObject interface -------------------------------------------

void PreloadRequest::readExternal(PofReader::Handle /*hIn*/)
    {
    }

void PreloadRequest::writeExternal(PofWriter::Handle /*hOut*/) const
    {
    }


// ----- Object interface ---------------------------------------------------

bool PreloadRequest::equals(Object::View v) const
    {
    if (!instanceof<PreloadRequest::View>(v))
        {
        return false;
        }
    return true;
    }

size32_t PreloadRequest::hashCode() const
    {
    return 3;
    }

COH_CLOSE_NAMESPACE3
