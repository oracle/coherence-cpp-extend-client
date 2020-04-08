/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/PriorityProcessor.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(103, PriorityProcessor);

using coherence::util::InvocableMapHelper;


// ----- constructors -------------------------------------------------------

PriorityProcessor::PriorityProcessor()
    : super(), f_vProcessor(self())
    {
    }

PriorityProcessor::PriorityProcessor(
        InvocableMap::EntryProcessor::View vProcessor)
    : super(),
      f_vProcessor(self(), vProcessor)
    {
    COH_ENSURE_PARAM(vProcessor);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder PriorityProcessor::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    return f_vProcessor->process(hEntry);
    }

Map::View PriorityProcessor::processAll(Set::View vSetEntries) const
    {
    return f_vProcessor->processAll(vSetEntries);
    }


// ----- PortableObject interface -------------------------------------------

void PriorityProcessor::readExternal(PofReader::Handle hIn)
    {
    AbstractPriorityTask::readExternal(hIn);
    initialize(f_vProcessor, cast<InvocableMap::EntryProcessor::View>
            (hIn->readObject(10)));
    }

void PriorityProcessor::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPriorityTask::writeExternal(hOut);
    hOut->writeObject(10, f_vProcessor);
    }


// ----- accessors ----------------------------------------------------------

InvocableMap::EntryProcessor::View PriorityProcessor::getProcessor() const
    {
    return f_vProcessor;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> PriorityProcessor::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << ": ("
        << getProcessor()
        << ")");
    }

COH_CLOSE_NAMESPACE3
