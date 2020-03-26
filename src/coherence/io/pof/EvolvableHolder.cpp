/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/EvolvableHolder.hpp"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/SimpleEvolvable.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;
using coherence::io::SimpleEvolvable;
using coherence::util::HashMap;
using coherence::util::Map;
using coherence::util::Set;

// ---- constructors --------------------------------------------------------

EvolvableHolder::EvolvableHolder()
    : f_hEvolvableMap(self(), HashMap::create(), true)
    {
    }

// ---- EvolvableHolder methods----------------------------------------------

Evolvable::Handle EvolvableHolder::get(int32_t nTypeId) const
    {
    Map::Handle       hMap       = f_hEvolvableMap;
    Integer32::View   vTypeId    = Integer32::create(nTypeId);
    Evolvable::Handle hEvolvable = cast<Evolvable::Handle>(hMap->get(vTypeId));

    if (NULL == hEvolvable)
        {
        hEvolvable = SimpleEvolvable::create(0);
        hMap->put(vTypeId, hEvolvable);
        }

    return hEvolvable;
    }

Set::View EvolvableHolder::getTypeIds() const
    {
    return f_hEvolvableMap->keySet();
    }

bool EvolvableHolder::isEmpty() const
    {
    return f_hEvolvableMap->isEmpty();
    }

COH_CLOSE_NAMESPACE3
