/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/DistinctValues.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashSet.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(29, DistinctValues);


// ----- constructors -------------------------------------------------------

DistinctValues::DistinctValues()
    : super(),
      m_hSet(self())
    {
    }

DistinctValues::DistinctValues(ValueExtractor::View vExtractor)
    : super(vExtractor),
      m_hSet(self())
    {
    }


// ----- AbstractAggregator Interface  --------------------------------------

void DistinctValues::init(bool /* fFinal */)
    {
    Set::Handle hSet = m_hSet;
    if (hSet != NULL)
        {
        hSet->clear();
        }
    }

void DistinctValues::process(Object::View v, bool fFinal)
    {
    if (v != NULL)
        {
        if (fFinal)
            {
            // aggregate partial results
            Collection::View vColPartial = cast<Collection::View>(v);
            if (!vColPartial->isEmpty())
                {
                ensureSet()->addAll(vColPartial);
                }
            }
        else
            {
            // collect partial results
            ensureSet()->add(v);
            }
        }
    }

Object::Holder DistinctValues::finalizeResult(bool fFinal)
    {
    Set::View vSet = m_hSet;
    m_hSet = NULL; // COHCPP-341
    if (fFinal)
        {
        // return the final aggregated result
        return vSet == NULL ? cast<Set::View>(HashSet::create()) : vSet;
        }
    else
        {
        // return partial aggregation data
        return vSet;
        }
   }


// ----- internal helpers -----------------------------------------------

Set::Handle DistinctValues::ensureSet()
    {
    Set::Handle hSet = m_hSet;
    if (hSet == NULL)
        {
        m_hSet = HashSet::create();
        return m_hSet;
        }
    return hSet;
    }

COH_CLOSE_NAMESPACE3
