/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/ReducerAggregator.hpp"

#include "coherence/lang.ns"

#include "coherence/util/NullImplementation.hpp"
#include "coherence/util/Hashtable.hpp"

using coherence::util::NullImplementation;
using coherence::util::Hashtable;
using coherence::util::ValueExtractor;

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(250, ReducerAggregator);


// ----- constructors ---------------------------------------------------

ReducerAggregator::ReducerAggregator()
        : super(), m_hMap(self())
    {
    }

ReducerAggregator::ReducerAggregator(ValueExtractor::View vExtractor)
        : super(vExtractor), m_hMap(self())
    {
    }

ReducerAggregator::ReducerAggregator(String::View vsMethod)
        : super(vsMethod), m_hMap(self())
    {
    }


// ----- AbstractAggregator methods -------------------------------------

void ReducerAggregator::init(bool /*fFinal*/)
    {
    Map::Handle hMap = m_hMap;
    if (hMap != NULL)
        {
        hMap->clear();
        }
    }

void ReducerAggregator::process(Object::View vO, bool fFinal)
    {
    if (vO != NULL)
        {
        if (fFinal)
            {
            // aggregate partial results
            Map::View vMap = cast<Map::View>(vO);

            if (!vMap->isEmpty())
                {
                ensureMap()->putAll(vMap);
                }
            }
        else
            {
                // should not be called with fFinal == false
                throw IllegalStateException::create();
            }
        }
    }

Object::Holder ReducerAggregator::finalizeResult(bool fFinal)
    {
    Map::Handle hMap = m_hMap;
    m_hMap           = NULL;  // COH-1487

    if (fFinal)
        {
        // return the final aggregated result
        return hMap == NULL ? NullImplementation::getMap() : hMap;
        }
    else
        {
        // return partial aggregation data
        return hMap;
        }
    }


// ----- internal helpers -----------------------------------------------

Map::Handle ReducerAggregator::ensureMap()
    {
    if (m_hMap == NULL)
        {
        m_hMap = Hashtable::create();
        }
    return m_hMap;
    }

COH_CLOSE_NAMESPACE3
