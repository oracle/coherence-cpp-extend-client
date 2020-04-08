/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/SimpleMapEntry.hpp"

#include "coherence/util/Collections.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constants ----------------------------------------------------------

namespace
    {
    Object::View getNoValue()
        {
        static FinalView<Object> vo(System::common(), Object::create());
        return vo;
        }
    COH_STATIC_INIT(getNoValue());
    }


// ----- constructors -------------------------------------------------------

SimpleMapEntry::SimpleMapEntry(Object::View vKey, Object::Holder ohValue, Object::View vOrigValue)
    : f_vKey(self(), vKey),
      m_ohValue(self(), ohValue),
      f_vOrigValue(self(), vOrigValue == NULL ? getNoValue() : vOrigValue)
    {
    }

SimpleMapEntry::SimpleMapEntry(const SimpleMapEntry& that)
    : super(that), f_vKey(self(), that.f_vKey->clone()),
      m_ohValue(self(), that.m_ohValue->clone()),
      f_vOrigValue(self(), that.f_vOrigValue->clone())
    {
    }


// ----- Map::Entry interface -----------------------------------------------

Object::View SimpleMapEntry::getKey() const
    {
    return f_vKey;
    }

Object::Holder SimpleMapEntry::getValue() const
    {
    return m_ohValue;
    }

Object::Holder SimpleMapEntry::getValue()
    {
    return m_ohValue;
    }

Object::Holder SimpleMapEntry::setValue(Object::Holder ohValue)
    {
    Object::Holder ohPrev = m_ohValue;
    m_ohValue = ohValue;
    return ohPrev;
    }


// ----- MapTrigger::Entry interface -------------------------------------

Object::View SimpleMapEntry::getOriginalValue() const
    {
    Object::View vOrig = f_vOrigValue;
    return vOrig == getNoValue() ? NULL : vOrig;
    }

bool SimpleMapEntry::isOriginalPresent() const
    {
    return f_vOrigValue != getNoValue();
    }

Object::Holder SimpleMapEntry::setValue(Object::Holder ohValue, bool /*fSynthetic*/)
    {
    return setValue(ohValue);
    }

void SimpleMapEntry::update(ValueUpdater::View vUpdater, Object::Holder ohValue)
    {
    InvocableMapHelper::updateEntry(vUpdater, this, ohValue);
    }

bool SimpleMapEntry::isPresent() const
    {
    return true;
    }

void SimpleMapEntry::remove(bool /*fSynthetic*/)
    {
    COH_THROW(UnsupportedOperationException::create());
    }

Object::Holder SimpleMapEntry::extract(ValueExtractor::View vExtractor) const
    {
    return InvocableMapHelper::extractFromEntry(vExtractor, this);
    }


// ----- Object interface ---------------------------------------------------

bool SimpleMapEntry::equals(Object::View v) const
    {
    Map::Entry::View that = cast<Map::Entry::View>(v, false);
    if (this == that || NULL == that)
        {
        return this == that;
        }

    return Object::equals(getKey()  , that->getKey()  )
        && Object::equals(getValue(), that->getValue());
    }

size32_t SimpleMapEntry::hashCode() const
    {
    return Object::hashCode(getKey());
    }

TypedHandle<const String> SimpleMapEntry::toString() const
    {
    return Collections::toString(this);
    }

COH_CLOSE_NAMESPACE2
