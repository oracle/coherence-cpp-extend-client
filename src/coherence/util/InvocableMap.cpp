/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/InvocableMap.hpp"

#include "coherence/util/Optional.hpp"

#include "private/coherence/internal/util/processor/CacheProcessors.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::internal::util::processor::CacheProcessors;
using coherence::util::Optional;

Object::Holder InvocableMap::getOrDefault(Object::View vKey,
        Object::Holder ohDefaultValue) const
    {
    InvocableMap::Handle hThis = const_cast<InvocableMap*>(this);
    return cast<Optional::View>(hThis->invoke(vKey, CacheProcessors::getOrDefault()))->orElse(ohDefaultValue);
    }

Object::Holder InvocableMap::putIfAbsent(Object::View vKey, Object::Holder ohValue)
    {
    return invoke(vKey, CacheProcessors::putIfAbsent(ohValue));
    }

bool InvocableMap::removeValue(Object::View vKey, Object::View vValue)
    {
    return Object::equals(invoke(vKey, CacheProcessors::remove(vValue)), Boolean::valueOf(true)) ? true : false;
    }

Object::Holder InvocableMap::replace(Object::View vKey, Object::Holder ohValue)
    {
    return invoke(vKey, CacheProcessors::replace(ohValue));
    }

bool InvocableMap::replace(Object::View vKey, Object::View vOldValue, Object::Holder ohNewValue)
    {
    return Object::equals(invoke(vKey, CacheProcessors::replace(vOldValue, ohNewValue)), Boolean::valueOf(true)) ? true : false;
    }

COH_CLOSE_NAMESPACE2
