/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/InKeySetFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"
#include "coherence/util/HashSet.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(72, InKeySetFilter);


// ----- constructors -------------------------------------------------------

InKeySetFilter::InKeySetFilter()
    : f_vFilter(self()), m_vSetKeys(self(), NULL, /* mutable */ true), m_fConverted(false)
    {
    }

InKeySetFilter::InKeySetFilter(Filter::View vFilter, Set::View vSetKeys)
    : f_vFilter(self(), vFilter), m_vSetKeys(self(), vSetKeys, /* mutable */ true),
      m_fConverted(false)
    {
    }


// ----- EntryFilter interface ----------------------------------------------

bool InKeySetFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    if (!m_vSetKeys->contains(vEntry->getKey()))
        {
        return false;
        }
    return InvocableMapHelper::evaluateEntry(getFilter(), vEntry);
    }


// ----- Filter interface ---------------------------------------------------

bool InKeySetFilter::evaluate(Object::View /* v */) const
    {
    COH_THROW (UnsupportedOperationException::create(
            "InKeySetFilter::evaluate"));
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t InKeySetFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    Filter::View vFilter  = f_vFilter;

    if (m_vSetKeys->size() < vSetKeys->size())
        {
        vSetKeys = m_vSetKeys;
        }
    return instanceof<IndexAwareFilter::View>(vFilter)
            ? (cast<IndexAwareFilter::View>(vFilter))->calculateEffectiveness(vMapIndexes, vSetKeys)
            : vSetKeys->size()*ExtractorFilter::eval_cost;
    }

Filter::View InKeySetFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    hSetKeys->retainAll(m_vSetKeys);

    Filter::View vFilter = f_vFilter;
    return instanceof<IndexAwareFilter::View>(vFilter)
            ? (cast<IndexAwareFilter::View>(vFilter))->applyIndex(vMapIndexes, hSetKeys)
            : NULL;
    }

// ----- PortableObject interface -------------------------------------------

void InKeySetFilter::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    m_vSetKeys = cast<Set::View>(hIn->readObject(1));
    }

void InKeySetFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, getFilter());
    hOut->writeObject(1, m_vSetKeys);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> InKeySetFilter::toString() const
    {
    return COH_TO_STRING("InKeySetFilter(" << getFilter() << ", keys=" << m_vSetKeys << ')');
    }


// ----- data member accessors ----------------------------------------------

Filter::View InKeySetFilter::getFilter() const
    {
    return f_vFilter;
    }

Set::View InKeySetFilter::getKeys() const
    {
    return m_vSetKeys;
    }


// ----- helpers ------------------------------------------------------------

void InKeySetFilter::ensureConverted(Converter::View vConverter) const
    {
    COH_SYNCHRONIZED (this)
        {
        if (!m_fConverted)
            {
            HashSet::Handle hSetConv = HashSet::create();
            for (Iterator::Handle iter = m_vSetKeys->iterator(); iter->hasNext();)
                {
                hSetConv->add(vConverter->convert(iter->next()));
                }
            m_vSetKeys = hSetConv;
            m_fConverted = true;
            }
        }
    }

COH_CLOSE_NAMESPACE3
