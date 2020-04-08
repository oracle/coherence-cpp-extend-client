/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/ConditionalProcessor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(91, ConditionalProcessor);

using coherence::util::InvocableMapHelper;


// ----- constructors -------------------------------------------------------

ConditionalProcessor::ConditionalProcessor()
    : f_vFilter(self()), f_vProcessor(self())
    {
    }

ConditionalProcessor::ConditionalProcessor(Filter::View vFilter,
    InvocableMap::EntryProcessor::View vProcessor)
    : f_vFilter(self(), vFilter), f_vProcessor(self(), vProcessor)
    {
    COH_ENSURE_PARAM(vFilter);
    COH_ENSURE_PARAM(vProcessor);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder ConditionalProcessor::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    if (InvocableMapHelper::evaluateEntry(f_vFilter, hEntry))
        {
        return f_vProcessor->process(hEntry);
        }

    return NULL;
    }

Map::View ConditionalProcessor::processAll(Set::View vsetEntries) const
    {
    Map::Handle                        hMapResults = HashMap::create();
    Filter::View                       vFilter     = f_vFilter;
    InvocableMap::EntryProcessor::View vProcessor  = f_vProcessor;

    for (Iterator::Handle hIter = vsetEntries->iterator(); hIter->hasNext(); )
        {
        InvocableMap::Entry::Handle hEntry =
                cast<InvocableMap::Entry::Handle>(hIter->next());

        if (InvocableMapHelper::evaluateEntry(vFilter, hEntry))
            {
            hMapResults->put(hEntry->getKey(), vProcessor->process(hEntry));
            }
        }
    return hMapResults;
    }


// ----- PortableObject interface -------------------------------------------

void ConditionalProcessor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    initialize(f_vProcessor, cast<InvocableMap::EntryProcessor::View>
            (hIn->readObject(1)));
    }

void ConditionalProcessor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeObject(1, f_vProcessor);
    }


// ----- Object interface ---------------------------------------------------

bool ConditionalProcessor::equals(Object::View v) const
    {
    if (!instanceof<ConditionalProcessor::View>(v))
        {
        return false;
        }
    ConditionalProcessor::View vThat = cast<ConditionalProcessor::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vFilter, vThat->f_vFilter) &&
            Object::equals(this->f_vProcessor, vThat->f_vProcessor);
    }

size32_t ConditionalProcessor::hashCode() const
    {
    return f_vFilter->hashCode() + f_vProcessor->hashCode();
    }

COH_CLOSE_NAMESPACE3
