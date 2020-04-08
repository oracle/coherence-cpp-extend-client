/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/ExtractorProcessor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/extractor/ChainedExtractor.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"

#include "private/coherence/util/StringHelper.hpp"


COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(95, ExtractorProcessor);

using coherence::util::StringHelper;
using coherence::util::extractor::ChainedExtractor;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::ReflectionExtractor;


// ----- constructors -------------------------------------------------------

ExtractorProcessor::ExtractorProcessor()
    : f_vExtractor(self())
    {
    }

ExtractorProcessor::ExtractorProcessor(ValueExtractor::View vExtractor)
    : f_vExtractor(self(), NULL == vExtractor ?
            cast<ValueExtractor::View>(IdentityExtractor::getInstance()) :
            vExtractor)
    {
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder ExtractorProcessor::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    return hEntry->extract(f_vExtractor);
    }


// ----- PortableObject interface -------------------------------------------

void ExtractorProcessor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObject(0)));
    }

void ExtractorProcessor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vExtractor);
    }


// ----- Object interface ---------------------------------------------------

bool ExtractorProcessor::equals(Object::View v) const
    {
    if (!instanceof<ExtractorProcessor::View>(v))
        {
        return false;
        }
    ExtractorProcessor::View vThat = cast<ExtractorProcessor::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vExtractor, vThat->f_vExtractor);
    }

size32_t ExtractorProcessor::hashCode() const
    {
    return f_vExtractor->hashCode();
    }

COH_CLOSE_NAMESPACE3
