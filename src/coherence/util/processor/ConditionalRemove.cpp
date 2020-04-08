/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/ConditionalRemove.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(94, ConditionalRemove);

using coherence::util::InvocableMapHelper;


// ----- constructors -------------------------------------------------------

ConditionalRemove::ConditionalRemove()
    : f_vFilter(self()), m_fReturn(false)
    {
    }

ConditionalRemove::ConditionalRemove(Filter::View vFilter,
        bool fReturn)
    : f_vFilter(self(), vFilter), m_fReturn(fReturn)
    {
    COH_ENSURE_PARAM(vFilter);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder ConditionalRemove::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    if (hEntry->isPresent()&&
            InvocableMapHelper::evaluateEntry(f_vFilter, hEntry))
        {
        hEntry->remove(false);
        return NULL;
        }
    return m_fReturn ? hEntry->getValue() : NULL;
    }

Map::View ConditionalRemove::processAll(Set::View vsetEntries) const
    {
    Map::Handle hMapResults = HashMap::create();
    Filter::View vFilter    = f_vFilter;
    bool fReturn            = m_fReturn;

    for (Iterator::Handle hIter = vsetEntries->iterator(); hIter->hasNext(); )
        {
        InvocableMap::Entry::Handle hEntry =
                cast<InvocableMap::Entry::Handle>(hIter->next());

        if (hEntry->isPresent() &&
                InvocableMapHelper::evaluateEntry(vFilter, hEntry))
            {
            hEntry->remove(false);
            }
        else if (fReturn)
            {
            hMapResults->put(hEntry->getKey(), hEntry->getValue());
            }
        }
    return hMapResults;
    }


// ----- PortableObject interface -------------------------------------------

void ConditionalRemove::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    m_fReturn = hIn->readBoolean(2);
    }

void ConditionalRemove::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeBoolean(2,m_fReturn);
    }


// ----- Object interface ---------------------------------------------------

bool ConditionalRemove::equals(Object::View v) const
    {
    if (!instanceof<ConditionalRemove::View>(v))
        {
        return false;
        }
    ConditionalRemove::View vThat = cast<ConditionalRemove::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vFilter, vThat->f_vFilter) &&
            this->m_fReturn == vThat->m_fReturn;
    }

size32_t ConditionalRemove::hashCode() const
    {
    return f_vFilter->hashCode() + (m_fReturn ? -1 : 1);
    }

COH_CLOSE_NAMESPACE3
