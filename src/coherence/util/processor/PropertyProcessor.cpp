/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/PropertyProcessor.hpp"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "coherence/util/ValueManipulator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using coherence::util::ValueManipulator;


// ----- constructors -------------------------------------------------------

PropertyProcessor::PropertyProcessor()
    : f_vManipulator(self())
    {
    }

PropertyProcessor::PropertyProcessor(ValueManipulator::View vManipulator)
    : f_vManipulator(self(), vManipulator)
    {
    }


// ----- PortableObject interface -------------------------------------------

void PropertyProcessor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vManipulator, cast<ValueManipulator::View>(hIn->readObject(0)));
    }

void PropertyProcessor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vManipulator);
    }


// ----- Object interface ---------------------------------------------------

bool PropertyProcessor::equals(Object::View v) const
    {
    if (!instanceof<PropertyProcessor::View>(v))
        {
        return false;
        }
    PropertyProcessor::View vThat = cast<PropertyProcessor::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vManipulator, vThat->f_vManipulator);
    }

size32_t PropertyProcessor::hashCode() const
    {
    return f_vManipulator->hashCode();
    }

// ----- helpers ------------------------------------------------------------

Object::View PropertyProcessor::get(InvocableMap::Entry::View vEntry) const
    {
    ValueManipulator::View vManipulator = f_vManipulator;
    if (vManipulator != NULL)
        {
        ValueExtractor::View vExtractor = vManipulator->getExtractor();
        if (NULL == vExtractor)
            {
            COH_THROW_STREAM(IllegalStateException,
                    "The ValueManipulator (" << vManipulator <<
                    ") failed to provide a ValueExtractor");
                }
            else
                {
                return vEntry->extract(vExtractor);
                }
            }
        return vEntry->getValue();
        }

void PropertyProcessor::set(InvocableMap::Entry::Handle hEntry,
        Object::View vValue) const
    {
    ValueManipulator::View vManipulator = f_vManipulator;
    if (vManipulator != NULL)
        {
        ValueUpdater::View vUpdater = vManipulator->getUpdater();
        if (vUpdater != NULL)
            {
            hEntry->update(vUpdater, vValue);
            return;
            }
        }
    hEntry->setValue(vValue, false);
    }

COH_CLOSE_NAMESPACE3
