/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/ConditionalPutAll.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/NullImplementation.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(93, ConditionalPutAll);

using coherence::util::InvocableMapHelper;


// ----- constructors -------------------------------------------------------

ConditionalPutAll::ConditionalPutAll()
    : f_vFilter(self()), f_vMap(self())
    {
    }

ConditionalPutAll::ConditionalPutAll(Filter::View vFilter, Map::View vMap)
    : f_vFilter(self(),vFilter), f_vMap(self(),vMap)
    {
    COH_ENSURE_PARAM(vFilter);
    COH_ENSURE_PARAM(vMap);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder ConditionalPutAll::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    Map::View      vMap  = f_vMap;
    Object::Holder ohKey = hEntry->getKey();

    if (vMap->containsKey(ohKey) &&
            InvocableMapHelper::evaluateEntry(f_vFilter, hEntry))
        {
        hEntry->setValue(vMap->get(ohKey), false);
        }
    return NULL;
    }

Map::View ConditionalPutAll::processAll(Set::View vsetEntries) const
    {
    Filter::View vFilter     = f_vFilter;
    Map::View    vMap        = f_vMap;

    for (Iterator::Handle hIter = vsetEntries->iterator(); hIter->hasNext(); )
        {
        InvocableMap::Entry::Handle hEntry = cast<InvocableMap::Entry::Handle>
                (hIter->next());
        Object::Holder              ohKey  = hEntry->getKey();

        if (vMap->containsKey(ohKey) &&
                InvocableMapHelper::evaluateEntry(vFilter, hEntry))
            {
            hEntry->setValue(vMap->get(ohKey), false);
            }
        }
    return NullImplementation::getMap();
    }


// ----- PortableObject interface -------------------------------------------

void ConditionalPutAll::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    initialize(f_vMap, cast<Map::View>(hIn->readObject(1)));
    }

void ConditionalPutAll::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeObject(1, f_vMap);
    }


// ----- Object interface ---------------------------------------------------

bool ConditionalPutAll::equals(Object::View v) const
    {
    if (!instanceof<ConditionalPutAll::View>(v))
        {
        return false;
        }
    ConditionalPutAll::View vThat = cast<ConditionalPutAll::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vFilter, vThat->f_vFilter) &&
            Object::equals(this->f_vMap, vThat->f_vMap);
    }

size32_t ConditionalPutAll::hashCode() const
    {
    return f_vFilter->hashCode() + f_vMap->hashCode();
    }

COH_CLOSE_NAMESPACE3
