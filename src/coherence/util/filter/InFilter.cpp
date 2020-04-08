/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/InFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"


COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(71, InFilter);


// ----- constructors -------------------------------------------------------

InFilter::InFilter()
    : super()
    {
    }

InFilter::InFilter(ValueExtractor::View vExtractor, Set::View vSetValues)
    : super(vExtractor, vSetValues)
    {
    }


// ----- ExtractorFilter interface ------------------------------------------

bool InFilter::evaluateExtracted(Object::View vExtracted) const
    {
    return cast<Collection::View>(getValue())->contains(vExtracted);
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t InFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    return vIndex == NULL ? calculateIteratorEffectiveness(vSetKeys->size())
                         : (cast<Collection::View>(getValue()))->size();
    }

Filter::View InFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (vIndex == NULL)
        {
        // there is no relevant index
        return this;
        }
    else
        {
        Collection::View   vColValues = cast<Collection::View>(getValue());
        Set::Handle        hSetIn     = HashSet::create();
        for (Iterator::Handle hIter = vColValues->iterator(); hIter->hasNext();)
            {
            Object::View ovValue = hIter->next();
            Set::View    vSetEQ  = cast<Set::View>(vIndex->getIndexContents()->get(ovValue));

            if (vSetEQ != NULL)
                {
                hSetIn->addAll(vSetEQ);
                }
            }

        if (hSetIn->isEmpty())
            {
            hSetKeys->clear();
            }
        else
            {
            hSetKeys->retainAll(hSetIn);
            }
        return NULL;
        }
    }

// ----- PortableObject interface -------------------------------------------

void InFilter::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObject(0)));
    initialize(f_vValue,     cast<Collection::View>(hIn->readCollection(1)));
    }

void InFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject    (0, f_vExtractor);
    hOut->writeCollection(1, cast<Collection::View>(f_vValue));
    }

COH_CLOSE_NAMESPACE3
