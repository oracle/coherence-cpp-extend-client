/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/IdentityHolder.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::HashMap;
using coherence::util::Map;


// ----- file local helpers -------------------------------------------------

namespace
    {
    ThreadLocalReference::Handle getThreadLocalMap()
        {
        static FinalHandle<ThreadLocalReference> hTLR(System::common(),
                ThreadLocalReference::create());
        return hTLR;
        }
    COH_STATIC_INIT(getThreadLocalMap());
    }


// ----- member methods -----------------------------------------------------

void IdentityHolder::set(PofBufferReader::Handle hReader, int32_t nId)
    {
    Map::Handle hMapId = cast<Map::Handle>(getThreadLocalMap()->get());
    if (NULL == hMapId)
        {
        getThreadLocalMap()->set(hMapId = HashMap::create());
        }
    hMapId->put(hReader, Integer32::create(nId));
    }

void IdentityHolder::reset(PofBufferReader::Handle hReader, int32_t nId, Object::Holder oh)
    {
    Map::Handle hMapId = cast<Map::Handle>(getThreadLocalMap()->get());
    if (NULL != hMapId && !hMapId->isEmpty())
        {
        while (NULL != hReader)
            {
            Object::View v = hMapId->get(hReader);
            if (NULL != v)
                {
                int32_t nValue = cast<Integer32::View>(v)->getInt32Value();
                if (nId == -1 || nValue == nId)
                    {
                    hReader->registerIdentity(nValue, oh);
                    hMapId->remove(hReader);
                    }
                break;
                }
            hReader = hReader->getParentParser();
            }
        }
    }

COH_CLOSE_NAMESPACE3
