/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/ArrayFilter.hpp"

#include "coherence/io/EOFException.hpp"

#include "coherence/util/Arrays.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::EOFException;


namespace
    {
    // ----- constants ------------------------------------------------------

    /**
    * The largest possible value of type size32_t.
    */
    const size32_t npos = size32_t(-1);
    }


// ----- constructors -------------------------------------------------------

ArrayFilter::ArrayFilter()
    : m_vaFilter(self(), NULL, true), m_fPreserveOrder()
    {
    }

ArrayFilter::ArrayFilter(ObjectArray::View vaFilter)
    : m_vaFilter(self(), vaFilter, true), m_fPreserveOrder()
    {
    COH_ENSURE_PARAM(vaFilter);

    size32_t cFilters = vaFilter->length;

    for (size32_t i = 0; i < cFilters; i++)
        {
        COH_ENSURE_PARAM(vaFilter[i]);
        }
    }


// ----- PortableObject interface -------------------------------------------

void ArrayFilter::readExternal(PofReader::Handle hIn)
    {
    m_vaFilter = hIn->readObjectArray(0);

    // if we read an old version of the filter that didn't have this field,
    // it would result in maintaining the old behavior
    m_fPreserveOrder = hIn->readBoolean(1);
    }

void ArrayFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObjectArray(0, getFilters());
    hOut->writeBoolean(1, m_fPreserveOrder);
    }


// ----- Object interface ---------------------------------------------------

bool ArrayFilter::equals(Object::View v) const
    {
    ArrayFilter::View that = cast<ArrayFilter::View>(v, false);
    if (that != NULL)
        {
        return Class::typeEquals(this, that) &&
            this->getFilters()->equals(that->getFilters());
        }
    return false;
    }

size32_t ArrayFilter::hashCode() const
    {
    return getFilters()->hashCode();
    }


TypedHandle<const String> ArrayFilter::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) << ' ' << getFilters());
    }


// ----- data member accessors ----------------------------------------------

ObjectArray::View ArrayFilter::getFilters() const
    {
    return m_vaFilter;
    }

void ArrayFilter::honorOrder()
    {
    m_fPreserveOrder = true;
    }


// ----- internal helpers ---------------------------------------------------

Filter::View ArrayFilter::applyFilter(IndexAwareFilter::View vFilter,
        Map::View vMapIndexes, Set::Handle hSetKeys) const
    {
    return vFilter->applyIndex(vMapIndexes, hSetKeys);
    }

void ArrayFilter::optimizeFilterOrder(Map::View vMapIndexes, Set::View vSetKeys) const
    {
    if (m_fPreserveOrder)
        {
        return;
        }

    ObjectArray::View   vaFilter = m_vaFilter;
    size32_t            cFilters = vaFilter->length;
    ObjectArray::Handle hawf     = ObjectArray::create(cFilters);
    size32_t            nMax     = vSetKeys->size() * ExtractorFilter::eval_cost;
    bool                fSort    = false;
    size32_t            nEffect0 = npos;

    for (size32_t i = 0; i < cFilters; i++)
        {
        Filter::View vFilter = cast<Filter::View>(vaFilter[i]);
        size32_t     nEffect = instanceof<IndexAwareFilter::View>(vFilter)
            ? cast<IndexAwareFilter::View>(vFilter)->calculateEffectiveness(vMapIndexes, vSetKeys)
            : nMax;

        hawf[i] = ArrayFilter::WeightedFilter::create(vFilter, nEffect);

        if (i == 0)
            {
            nEffect0 = nEffect;
            }
        else
            {
            // only need to sort if the weights are different
            fSort |= (nEffect != nEffect0);
            }
        }

    if (fSort)
        {
        Arrays::sort(hawf);
        ObjectArray::Handle haFilter = ObjectArray::create(cFilters);
        for (size32_t i = 0; i < cFilters; i++)
            {
            haFilter[i] = cast<ArrayFilter::WeightedFilter::View>(hawf[i])->getFilter();
            }
        m_vaFilter = haFilter;
        }
    m_fPreserveOrder = true;
    }


// ----- inner class: WeightedFilter ----------------------------------------

// ----- constructors ---------------------------------------------------

ArrayFilter::WeightedFilter::WeightedFilter(Filter::View vFilter, int32_t nEffect) :
        f_vFilter(self(), vFilter),
        m_nEffect(nEffect)
    {
    }

// ----- Comparable interface -------------------------------------------

int32_t ArrayFilter::WeightedFilter::compareTo(Object::View v) const
    {
    int32_t nThis = m_nEffect;
    int32_t nThat = cast<WeightedFilter::View>(v)->m_nEffect;

    return (nThis < nThat ? -1 : (nThis > nThat ? +1 : 0));
    }

// ----- accessors ------------------------------------------------------

Filter::View ArrayFilter::WeightedFilter::getFilter() const
    {
    return f_vFilter;
    }

COH_CLOSE_NAMESPACE3
