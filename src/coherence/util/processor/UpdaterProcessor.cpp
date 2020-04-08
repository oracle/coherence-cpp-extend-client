/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/UpdaterProcessor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/extractor/CompositeUpdater.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"

#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(100, UpdaterProcessor);

using coherence::util::StringHelper;
using coherence::util::extractor::CompositeUpdater;
using coherence::util::extractor::ReflectionUpdater;


// ----- constructors -------------------------------------------------------

UpdaterProcessor::UpdaterProcessor()
    : f_vUpdater(self()), f_hValue(self())
    {
    }

UpdaterProcessor::UpdaterProcessor(ValueUpdater::View vUpdater,
    Object::Holder hValue)
    : f_vUpdater(self(), vUpdater), f_hValue(self(), hValue)
    {
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder UpdaterProcessor::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    ValueUpdater::View vUpdater = f_vUpdater;
    if (NULL == vUpdater)
        {
        hEntry->setValue(f_hValue, true);
        }
    else if (hEntry->isPresent())
        {
        Object::Holder hoTarget = hEntry->getValue();
        vUpdater->update(cast<Object::Handle>(hoTarget), f_hValue);
        hEntry->setValue(hoTarget, true);
        }
    return Boolean::valueOf(true);
    }


// ----- PortableObject interface -------------------------------------------

void UpdaterProcessor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vUpdater, cast<ValueUpdater::View>(hIn->readObject(0)));
    initialize(f_hValue, hIn->readObject(1));
    }

void UpdaterProcessor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vUpdater);
    hOut->writeObject(1, f_hValue);
    }


// ----- Object interface ---------------------------------------------------

bool UpdaterProcessor::equals(Object::View v) const
    {
    if (!instanceof<UpdaterProcessor::View>(v))
        {
        return false;
        }
    UpdaterProcessor::View vThat = cast<UpdaterProcessor::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vUpdater, vThat->f_vUpdater) &&
            Object::equals(this->f_hValue, vThat->f_hValue);
    }

size32_t UpdaterProcessor::hashCode() const
    {
    return f_vUpdater->hashCode();
    }

COH_CLOSE_NAMESPACE3
