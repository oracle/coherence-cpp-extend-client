/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/ConditionalPut.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(92, ConditionalPut);

using coherence::util::InvocableMapHelper;


// ----- constructors -------------------------------------------------------

ConditionalPut::ConditionalPut()
    : f_vFilter(self()), f_hValue(self()), m_fReturn(false)
    {
    }

ConditionalPut::ConditionalPut(Filter::View vFilter, Object::Holder hValue,
        bool fReturn)
    : f_vFilter(self(),vFilter),
      f_hValue(self(),hValue),
      m_fReturn(fReturn)
    {
    COH_ENSURE_PARAM(vFilter);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder ConditionalPut::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    if (InvocableMapHelper::evaluateEntry(f_vFilter, hEntry))
        {
        Object::View vValue = f_hValue;
        hEntry->setValue(vValue, false);
        return NULL;
        }

    return m_fReturn ? hEntry->getValue() : NULL;
    }

Map::View ConditionalPut::processAll(Set::View vsetEntries) const
    {
    Map::Handle  hMapResults = HashMap::create();
    Filter::View vFilter     = f_vFilter;
    bool         fReturn     = m_fReturn;
    Object::View vValue      = f_hValue;

    for (Iterator::Handle hIter = vsetEntries->iterator(); hIter->hasNext(); )
        {
        InvocableMap::Entry::Handle hEntry =
                cast<InvocableMap::Entry::Handle>(hIter->next());

        if (InvocableMapHelper::evaluateEntry(vFilter, hEntry))
            {
            hEntry->setValue(vValue, false);
            }
        else if (fReturn)
            {
            hMapResults->put(hEntry->getKey(), hEntry->getValue());
            }
        }
    return hMapResults;
    }


// ----- PortableObject interface -------------------------------------------

void ConditionalPut::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    initialize(f_hValue, hIn->readObject(1));
    m_fReturn = hIn->readBoolean(2);
    }

void ConditionalPut::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeObject(1, f_hValue);
    hOut->writeBoolean(2,m_fReturn);
    }


// ----- Object interface ---------------------------------------------------

bool ConditionalPut::equals(Object::View v) const
    {
    if (!instanceof<ConditionalPut::View>(v))
        {
        return false;
        }
    ConditionalPut::View vThat = cast<ConditionalPut::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vFilter, vThat->f_vFilter) &&
            Object::equals(this->f_hValue, vThat->f_hValue) &&
            this->m_fReturn == vThat->m_fReturn;
    }

size32_t ConditionalPut::hashCode() const
    {
    size32_t nHash = Object::hashCode(f_hValue);
    return nHash + f_vFilter->hashCode() + (m_fReturn ? -1 : 1);
    }

COH_CLOSE_NAMESPACE3
