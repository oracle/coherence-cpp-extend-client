/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/ComparisonFilter.hpp"

#include "coherence/util/MapIndex.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


// ----- constructors -------------------------------------------------------

ComparisonFilter::ComparisonFilter()
   : super(), f_vValue(self())
    {
    }

ComparisonFilter::ComparisonFilter(ValueExtractor::View vValueExtractor,
            Object::View vObject)
   : super(vValueExtractor), f_vValue(self(), vObject)
    {
    }


// ----- PortableObject interface -------------------------------------------

void ComparisonFilter::readExternal(PofReader::Handle hIn)
    {
    ExtractorFilter::readExternal(hIn);
    initialize(f_vValue, cast<Object::View>(hIn->readObject(1)));
   }

void ComparisonFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    ExtractorFilter::writeExternal(hOut);
    hOut->writeObject(1, f_vValue);
    }

// ----- helpers ------------------------------------------------------------

int32_t ComparisonFilter::calculateMatchEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    return vIndex == NULL ? calculateIteratorEffectiveness(vSetKeys->size()) : 1;
    }

int32_t ComparisonFilter::calculateRangeEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (vIndex == NULL)
        {
        return calculateIteratorEffectiveness(vSetKeys->size());
        }
    else if (vIndex->isOrdered())
        {
        // TODO we could be more precise if the position of the value
        // in the SortedMap could be quickly calculated
        int32_t nEffectiveness = vIndex->getIndexContents()->size() / 4;
        return nEffectiveness > 1 ? nEffectiveness : 1;
        }
    else
        {
        return vIndex->getIndexContents()->size();
        }
    }

int32_t ComparisonFilter::calculateIteratorEffectiveness(int32_t cKeys)
    {
    // convert int32_t to int64_t to prevent overflow
    int64_t lCost = ((int64_t) eval_cost) * cKeys;
    return lCost <= Integer32::max_value ? (int32_t) lCost : Integer32::max_value;
    }


// ----- Object interface ---------------------------------------------------

bool ComparisonFilter::equals(Object::View v) const
    {
    ComparisonFilter::View that = cast<ComparisonFilter::View>(v, false);
    if (NULL == that)
        {
        return false;
        }
    return Class::typeEquals(this, that)
          && Object::equals(f_vExtractor, that->f_vExtractor)
          && Object::equals(f_vValue, that->f_vValue);
     }

size32_t ComparisonFilter::hashCode() const
    {
    return Object::hashCode(f_vExtractor) + Object::hashCode(f_vValue);
    }

TypedHandle<const String> ComparisonFilter::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << ": ("
        << getValueExtractor()
        << ", "
        << getValue()
        << ")");
    }


// ----- data members accessor-----------------------------------------------

Object::View ComparisonFilter::getValue() const
    {
    return f_vValue;
    }

COH_CLOSE_NAMESPACE3
