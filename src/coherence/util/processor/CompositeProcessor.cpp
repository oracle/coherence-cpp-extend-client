/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/CompositeProcessor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(90, CompositeProcessor);


// ----- constructors -------------------------------------------------------

CompositeProcessor::CompositeProcessor()
    : f_vaProcessor(self())
    {
    }

CompositeProcessor::CompositeProcessor(ObjectArray::View vaProcessor)
    : f_vaProcessor(self(), vaProcessor)
    {
    COH_ENSURE_PARAM(vaProcessor);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder CompositeProcessor::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    ObjectArray::View    vaProcessor = f_vaProcessor;
    size32_t             cProcessors = vaProcessor->length;
    ObjectArray::Handle  haResult    = ObjectArray::create(cProcessors);

    for (size32_t i = 0; i < cProcessors; i++)
        {
        haResult[i] =
                cast<InvocableMap::EntryProcessor::Handle>
                (vaProcessor[i])->process(hEntry);
        }
    return haResult;
    }


// ----- PortableObject interface -------------------------------------------

void CompositeProcessor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vaProcessor, cast<ObjectArray::View>(hIn->readObjectArray(0)));
    }

void CompositeProcessor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObjectArray(0, f_vaProcessor);
    }


// ----- Object interface ---------------------------------------------------

bool CompositeProcessor::equals(Object::View v) const
    {
    if (!instanceof<CompositeProcessor::View>(v))
        {
        return false;
        }
    CompositeProcessor::View vThat = cast<CompositeProcessor::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(f_vaProcessor, vThat->f_vaProcessor);
    }

size32_t CompositeProcessor::hashCode() const
    {
    ObjectArray::View vaProcessor = f_vaProcessor;
    size32_t          cProcessors = vaProcessor->length;
    size32_t          iHash       = 0;

    for (size32_t i = 0; i < cProcessors; i++)
        {
        iHash += vaProcessor[i]->hashCode();
        }
    return iHash;
    }

COH_CLOSE_NAMESPACE3
