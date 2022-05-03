/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/ConverterCollections.hpp"

#include "coherence/util/Collections.hpp"
#include "coherence/util/MapTriggerListener.hpp"

#include "coherence/util/MapListenerSupport.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- ConverterCollections interface -------------------------------------

ObjectArray::Handle ConverterCollections::convertArray(
        ObjectArray::Handle hArray, Converter::View vConvUp)
    {
    if (NULL == hArray)
        {
        return NULL;
        }

    ObjectArray::Handle hArrayNew = ObjectArray::create(hArray->length);
    for (size32_t i = 0, c = hArray->length; i < c; ++i)
        {
        hArrayNew[i] = Converter::convert(vConvUp, hArray[i]);
        }

    return hArrayNew;
    }

ObjectArray::View ConverterCollections::convertArray(
        ObjectArray::View vArray, Converter::View vConvUp)
    {
    if (NULL == vArray)
        {
        return NULL;
        }

    ObjectArray::Handle hArrayNew = ObjectArray::create(vArray->length);
    for (size32_t i = 0, c = vArray->length; i < c; ++i)
        {
        hArrayNew[i] = Converter::convert(vConvUp, vArray[i]);
        }

    return hArrayNew;
    }


// ----- inner class: ConverterIterator -------------------------------------

ConverterCollections::ConverterIterator::ConverterIterator(
        Iterator::Handle hiterDelegate, Converter::View vConvUp)
    : super(hiterDelegate), f_vConvUp(self(), vConvUp)
    {
    }

ConverterCollections::ConverterIterator::ConverterIterator(
        const ConverterIterator& that)
    : super(that), f_vConvUp(self(), that.f_vConvUp)
    {
    }

Object::Holder ConverterCollections::ConverterIterator::next()
    {
    return Converter::convert(f_vConvUp, AbstractWrapperIterator::next());
    }


// ----- inner class: ConverterCollection -----------------------------------

ConverterCollections::ConverterCollection::ConverterCollection(
        Collection::Handle hcollDelegate,
        Converter::View vConvUp,
        Converter::View vConvDown)
        : super(hcollDelegate)
        , f_vConvUp(self(), vConvUp)
        , f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterCollection::ConverterCollection(
        Collection::View vcollDelegate,
        Converter::View vConvUp,
        Converter::View vConvDown)
    : super(vcollDelegate)
    , f_vConvUp(self(), vConvUp)
    , f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterCollection::ConverterCollection(
        const ConverterCollection& that)
    : super(that),
      f_vConvUp(self(), that.f_vConvUp),
      f_vConvDown(self(), that.f_vConvDown)
    {
    }

bool ConverterCollections::ConverterCollection::contains(Object::View v) const
    {
    return AbstractWrapperCollection::contains
        (Converter::convert(f_vConvDown, v));
    }

Iterator::Handle ConverterCollections::ConverterCollection::iterator() const
    {
    return ConverterIterator::create
        (AbstractWrapperCollection::iterator(), f_vConvUp);
    }

Muterator::Handle ConverterCollections::ConverterCollection::iterator()
    {
    return ConverterIterator::create
        (AbstractWrapperCollection::iterator(), f_vConvUp);
    }

ObjectArray::Handle ConverterCollections::ConverterCollection::toArray(
        ObjectArray::Handle /*hao*/) const
    {
    // Note: hao cannot be used as it could allow the resulting array to
    // contain "garbage" data which is not convertable.
    return convertArray(AbstractWrapperCollection::toArray(), f_vConvUp);
    }

bool ConverterCollections::ConverterCollection::add(Object::Holder oh)
    {
    return AbstractWrapperCollection::add(Converter::convert(f_vConvDown, oh));
    }

bool ConverterCollections::ConverterCollection::remove(Object::View v)
    {
    return AbstractWrapperCollection::remove(
            Converter::convert(f_vConvDown, v));
    }

bool ConverterCollections::ConverterCollection::containsAll(
        Collection::View vc) const
    {
    for (Iterator::Handle hIter = vc->iterator(); hIter->hasNext(); )
        {
        if (!contains(hIter->next()))
            {
            return false;
            }
        }
    return true;
    }

bool ConverterCollections::ConverterCollection::addAll(Collection::View vc)
    {
    return AbstractWrapperCollection::addAll(
            ConverterCollection::create(vc, f_vConvDown, f_vConvUp));
    }

bool ConverterCollections::ConverterCollection::removeAll(Collection::View vc)
    {
    return AbstractWrapperCollection::removeAll(
            ConverterCollection::create(vc, f_vConvDown, f_vConvUp));
    }

bool ConverterCollections::ConverterCollection::retainAll(Collection::View vc)
    {
    return AbstractWrapperCollection::retainAll(
            ConverterCollection::create(vc, f_vConvDown, f_vConvUp));
    }

bool ConverterCollections::ConverterCollection::equals(Object::View that) const
    {
    Collection::View vThat = cast<Collection::View>(that, false);

    if(vThat == this)
        {
        return true;
        }

    if (NULL == vThat )
        {
        return false;
        }

    if(vThat->size() != size())
        {
        return false;
        }

    return containsAll(vThat);
    }

size32_t ConverterCollections::ConverterCollection::hashCode() const
    {
    size32_t nHash = 0;
    for (Iterator::Handle i = iterator(); i->hasNext(); )
        {
        Object::View v = i->next();
        if (NULL != v)
            {
            nHash += v->hashCode();
            }
        }
    return nHash;
    }

TypedHandle<const String> ConverterCollections::ConverterCollection::toString() const
    {
    return Collections::toString(this);
    }


// ----- inner class: ConverterSet ------------------------------------------

ConverterCollections::ConverterSet::ConverterSet(Set::Handle hsetDelegate,
        Converter::View vConvUp,
        Converter::View vConvDown)
        : super(hsetDelegate)
        , f_vConvUp(self(), vConvUp)
        , f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterSet::ConverterSet(Set::View vsetDelegate,
        Converter::View vConvUp,
        Converter::View vConvDown)
    : super(vsetDelegate)
    , f_vConvUp(self(), vConvUp)
    , f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterSet::ConverterSet(const ConverterSet& that)
    : super(that),
      f_vConvUp(self(), that.f_vConvUp),
      f_vConvDown(self(), that.f_vConvDown)
    {
    }

bool ConverterCollections::ConverterSet::contains(Object::View v) const
    {
    return AbstractWrapperSet::contains(Converter::convert(f_vConvDown, v));
    }

Iterator::Handle ConverterCollections::ConverterSet::iterator() const
    {
    return ConverterIterator::create(AbstractWrapperSet::iterator(), f_vConvUp);
    }

Muterator::Handle ConverterCollections::ConverterSet::iterator()
    {
    return ConverterIterator::create(AbstractWrapperSet::iterator(), f_vConvUp);
    }

ObjectArray::Handle ConverterCollections::ConverterSet::toArray(
        ObjectArray::Handle /*hao*/) const
    {
    // Note: hao cannot be used as it could allow the resulting array to
    // contain "garbage" data which is not convertable.
    return convertArray(AbstractWrapperSet::toArray(), f_vConvUp);
    }

bool ConverterCollections::ConverterSet::add(Object::Holder oh)
    {
    return AbstractWrapperSet::add(Converter::convert(f_vConvDown, oh));
    }

bool ConverterCollections::ConverterSet::remove(Object::View v)
    {
    return AbstractWrapperSet::remove(Converter::convert(f_vConvDown, v));
    }

bool ConverterCollections::ConverterSet::containsAll(Collection::View vc)
        const
    {
    for (Iterator::Handle hIter = vc->iterator(); hIter->hasNext(); )
        {
        if (!contains(hIter->next()))
            {
            return false;
            }
        }
    return true;
    }

bool ConverterCollections::ConverterSet::addAll(Collection::View vc)
    {
    return AbstractWrapperSet::addAll(
            ConverterCollection::create(vc, f_vConvDown, f_vConvUp));
    }

bool ConverterCollections::ConverterSet::removeAll(Collection::View vc)
    {
    return AbstractWrapperSet::removeAll(
            ConverterCollection::create(vc, f_vConvDown, f_vConvUp));
    }

bool ConverterCollections::ConverterSet::retainAll(Collection::View vc)
    {
    return AbstractWrapperSet::retainAll(
            ConverterCollection::create(vc, f_vConvDown, f_vConvUp));
    }

bool ConverterCollections::ConverterSet::equals(Object::View that) const
    {
    Set::View vThat = cast<Set::View>(that, false);

    if(vThat == this)
        {
        return true;
        }

    if (NULL == vThat )
        {
        return false;
        }

    if(vThat->size() != size())
        {
        return false;
        }

    return containsAll(vThat);
    }

size32_t ConverterCollections::ConverterSet::hashCode() const
    {
    size32_t nHash = 0;
    for (Iterator::Handle i = iterator(); i->hasNext(); )
        {
        Object::View v = i->next();
        if (NULL != v)
            {
            nHash += v->hashCode();
            }
        }
    return nHash;
    }


TypedHandle<const String> ConverterCollections::ConverterSet::toString() const
    {
    return Collections::toString(this);
    }


// ----- inner class: ConverterMap ------------------------------------------

ConverterCollections::ConverterMap::ConverterMap(Map::Handle hMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
        : super(hMapDelegate), f_vConvUp(self(), vConvUp),
          f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterMap::ConverterMap(Map::View vMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(vMapDelegate), f_vConvUp(self(), vConvUp),
      f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterMap::ConverterMap(const ConverterMap& that)
    : super(that),
      f_vConvUp(self(), that.f_vConvUp),
      f_vConvDown(self(), that.f_vConvDown)
    {
    }

bool ConverterCollections::ConverterMap::containsKey(Object::View vKey)
        const
    {
    return AbstractWrapperMap::containsKey(
            EntryConverter::convertKey(f_vConvDown, vKey));
    }

bool ConverterCollections::ConverterMap::containsValue(Object::View vValue)
        const
    {
    return AbstractWrapperMap::containsValue(
            EntryConverter::convertValue(f_vConvDown, vValue));
    }

Object::Holder ConverterCollections::ConverterMap::get(Object::View vKey)
        const
    {
    return EntryConverter::convertValue(f_vConvUp, AbstractWrapperMap::get(
            EntryConverter::convertKey(f_vConvDown, vKey)));
    }

Object::Holder ConverterCollections::ConverterMap::put
        (Object::View vKey, Object::Holder ohValue)
    {
    return EntryConverter::convertValue(f_vConvUp, AbstractWrapperMap::put(
            EntryConverter::convertKey(f_vConvDown, vKey),
            EntryConverter::convertValue(f_vConvDown, ohValue)));
    }

Object::Holder ConverterCollections::ConverterMap::remove(Object::View vKey)
    {
    return EntryConverter::convertValue(f_vConvUp,
            AbstractWrapperMap::remove(
                    EntryConverter::convertKey(f_vConvDown, vKey)));
    }

void ConverterCollections::ConverterMap::putAll(Map::View vMap)
    {
    return AbstractWrapperMap::putAll(
            ConverterMap::create(vMap, f_vConvDown, f_vConvUp));
    }

Set::View ConverterCollections::ConverterMap::keySet() const
    {
    return ConverterSet::create(AbstractWrapperMap::keySet(),
            EntryConverter::getKeyConverter(f_vConvUp),
            EntryConverter::getKeyConverter(f_vConvDown));
    }

Set::Handle ConverterCollections::ConverterMap::keySet()
    {
    return ConverterSet::create(AbstractWrapperMap::keySet(),
            EntryConverter::getKeyConverter(f_vConvUp),
            EntryConverter::getKeyConverter(f_vConvDown));
    }

Collection::View ConverterCollections::ConverterMap::values() const
    {
    return ConverterCollection::create(AbstractWrapperMap::values(),
            EntryConverter::getValueConverter(f_vConvUp),
            EntryConverter::getValueConverter(f_vConvDown));
    }

Collection::Handle ConverterCollections::ConverterMap::values()
    {
    return ConverterCollection::create(AbstractWrapperMap::values(),
            EntryConverter::getValueConverter(f_vConvUp),
            EntryConverter::getValueConverter(f_vConvDown));
    }

Set::View ConverterCollections::ConverterMap::entrySet() const
    {
    return ConverterSet::create(
            AbstractWrapperMap::entrySet(), f_vConvUp, f_vConvDown);
    }

Set::Handle ConverterCollections::ConverterMap::entrySet()
    {
    return ConverterSet::create(
            AbstractWrapperMap::entrySet(), f_vConvUp, f_vConvDown);
    }

bool ConverterCollections::ConverterMap::equals(Object::View that) const
    {
    return AbstractWrapperMap::equals(ConverterMap::create(
            cast<Map::View>(that), f_vConvDown, f_vConvUp));
    }


size32_t ConverterCollections::ConverterMap::hashCode() const
    {
    size32_t nHash = 0;
    for (Iterator::Handle i = entrySet()->iterator(); i->hasNext(); )
        {
        nHash += i->next()->hashCode();
        }
    return nHash;
    }

TypedHandle<const String> ConverterCollections::ConverterMap::toString() const
    {
    return Collections::toString((Collection::View) entrySet());
    }

// ----- accessors ------------------------------------------------------

ConverterCollections::EntryConverter::View
        ConverterCollections::ConverterMap::getUpConverter() const
    {
    return f_vConvUp;
    }

ConverterCollections::EntryConverter::View
        ConverterCollections::ConverterMap::getDownConverter() const
    {
    return f_vConvDown;
    }


// ----- inner class: EntryConverter ----------------------------------------

ConverterCollections::EntryConverter::EntryConverter(
        Converter::View vConvKey, Converter::View vConvVal)
    : f_vConvKey(self(), vConvKey), f_vConvVal(self(), vConvVal)
    {
    }

ConverterCollections::EntryConverter::EntryConverter(const EntryConverter& that)
    : super(that), f_vConvKey(self(), that.f_vConvKey),
      f_vConvVal(self(), that.f_vConvVal)
    {
    }

Object::Holder ConverterCollections::EntryConverter::convert(Object::Holder oh) const
    {
    Converter::View vUnsupported = UnsupportedConverter::getInstance();
    return ConverterEntry::create(cast<Map::Entry::View>(oh), this,
            EntryConverter::create(vUnsupported, vUnsupported));
    }

Converter::View ConverterCollections::EntryConverter::getKeyConverter() const
    {
    return f_vConvKey;
    }

Converter::View ConverterCollections::EntryConverter::getValueConverter() const
    {
    return f_vConvVal;
    }

Converter::View ConverterCollections::EntryConverter::getKeyConverter(
        EntryConverter::View vConvert)
    {
    return NULL == vConvert ? Converter::View() : vConvert->getKeyConverter();
    }

Converter::View ConverterCollections::EntryConverter::getValueConverter(
        EntryConverter::View vConvert)
    {
    return NULL == vConvert ? Converter::View() : vConvert->getValueConverter();
    }

Object::View ConverterCollections::EntryConverter::convertKey(
        EntryConverter::View vConvert, Object::View vKey)
    {
    return Converter::convert(getKeyConverter(vConvert), vKey);
    }

Object::Holder ConverterCollections::EntryConverter::convertValue(
        EntryConverter::View vConvert, Object::Holder ohValue)
    {
    return Converter::convert(getValueConverter(vConvert), ohValue);
    }

// ----- inner class: ConverterEntry ----------------------------------------

ConverterCollections::ConverterEntry::ConverterEntry(
        Map::Entry::Handle hMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(hMapDelegate), f_vConvUp(self(), vConvUp),
      f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterEntry::ConverterEntry(
        Map::Entry::View vMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(vMapDelegate), f_vConvUp(self(), vConvUp),
      f_vConvDown(self(), vConvDown)
    {
    }

ConverterCollections::ConverterEntry::ConverterEntry(const ConverterEntry& that)
    : super(that),
      f_vConvUp(self(), that.f_vConvUp),
      f_vConvDown(self(), that.f_vConvDown)
    {
    }

Object::View ConverterCollections::ConverterEntry::getKey() const
    {
    return Converter::convert(f_vConvUp->getKeyConverter(),
            AbstractWrapperEntry::getKey());
    }

Object::Holder ConverterCollections::ConverterEntry::getValue() const
    {
    return Converter::convert(f_vConvUp->getValueConverter(),
            AbstractWrapperEntry::getValue());
    }

Object::Holder ConverterCollections::ConverterEntry::setValue(Object::Holder ohValue)
    {
    return Converter::convert(f_vConvUp->getValueConverter(),
            AbstractWrapperEntry::setValue(Converter::convert(
                    f_vConvDown->getValueConverter(), ohValue)));
    }

bool ConverterCollections::ConverterEntry::equals(Object::View that) const
    {
    Entry::View vThat = cast<Entry::View>(that, false);
    return vThat != NULL                              &&
            Object::equals(vThat->getKey(), getKey()) &&
            Object::equals(vThat->getValue(), getValue());
    }

size32_t ConverterCollections::ConverterEntry::hashCode() const
    {
    return Object::hashCode(getKey());
    }

TypedHandle<const String> ConverterCollections::ConverterEntry::toString() const
    {
    return Collections::toString(this);
    }


// ----- inner class: ConverterCacheEvent -----------------------------------

// ----- constructors ---------------------------------------------------

ConverterCollections::ConverterCacheEvent::ConverterCacheEvent(
        ObservableMap::Handle hMap, CacheEvent::View vEvent,
        Converter::View vConvKey, Converter::View vConvVal)
    : super(hMap, vEvent->getId(), (Object::View) NULL, (Object::View) NULL,
            (Object::View) NULL, vEvent->isSynthetic(), 
            vEvent->getTransformState(), vEvent->isPriming(), vEvent->isExpired()),
      f_vEvent(self(), vEvent),
      f_vConvKey(self(), vConvKey),
      f_vConvVal(self(), vConvVal)
    {
    }

// ----- MapEvent interface ---------------------------------------------

Object::View ConverterCollections::ConverterCacheEvent::getKey() const
    {
    Object::View voKey = m_voKey;
    if (NULL == voKey)
        {
        voKey = Converter::convert(getConverterKeyUp(),
                getCacheEvent()->getKey());
        m_voKey = voKey;
        }
    return voKey;
    }

Object::View ConverterCollections::ConverterCacheEvent::getOldValue() const
    {
    Object::View voValueOld = m_voValueOld;
    if (NULL == voValueOld)
        {
        voValueOld = Converter::convert(getConverterValueUp(),
                getCacheEvent()->getOldValue());
        m_voValueOld = voValueOld;
        }
    return voValueOld;
    }

Object::View ConverterCollections::ConverterCacheEvent::getNewValue() const
    {
    Object::View voValueNew = m_voValueNew;
    if (NULL == voValueNew)
        {
        voValueNew = Converter::convert(getConverterValueUp(),
                getCacheEvent()->getNewValue());
        m_voValueNew = voValueNew;
        }
    return voValueNew;
    }

// ----- accessors ------------------------------------------------------

CacheEvent::View ConverterCollections::ConverterCacheEvent::getCacheEvent() const
    {
    return f_vEvent;
    }

Converter::View ConverterCollections::ConverterCacheEvent::getConverterKeyUp() const
    {
    return f_vConvKey;
    }

Converter::View ConverterCollections::ConverterCacheEvent::getConverterValueUp() const
    {
    return f_vConvVal;
    }

void ConverterCollections::ConverterCacheEvent::setOldValue(Object::View voValue)
    {
    m_voValueOld = voValue;
    }

void ConverterCollections::ConverterCacheEvent::setNewValue(Object::View voValue)
    {
    m_voValueNew = voValue;
    }


// ----- inner class: ConverterMapEvent -------------------------------------

// ----- static initialization ------------------------------------------

namespace
    {
    Object::View getNullObj()
        {
        static FinalView<Object> vo(System::common(), Object::create());
        return vo;
        }
    COH_STATIC_INIT(getNullObj());
    }


// ----- constructors ---------------------------------------------------

ConverterCollections::ConverterMapEvent::ConverterMapEvent(
        ObservableMap::Handle hMap, MapEvent::View vEvent,
        Converter::View vConvKey, Converter::View vConvVal)
    : super(hMap, vEvent->getId(), getNullObj(), getNullObj(),
            getNullObj()),
      f_vEvent(self(), vEvent),
      f_vConvKey(self(), vConvKey),
      f_vConvVal(self(), vConvVal)
    {
    }

// ----- MapEvent interface ---------------------------------------------

Object::View ConverterCollections::ConverterMapEvent::getKey() const
    {
    Object::View voKey = m_voKey;
    if (getNullObj() == voKey)
        {
        voKey = Converter::convert(getConverterKeyUp(),
                getMapEvent()->getKey());
        m_voKey = voKey;
        }
    return voKey;
    }

Object::View ConverterCollections::ConverterMapEvent::getOldValue() const
    {
    Object::View voValueOld = m_voValueOld;
    if (getNullObj() == voValueOld)
        {
        voValueOld = Converter::convert(getConverterValueUp(),
                getMapEvent()->getOldValue());
        m_voValueOld = voValueOld;
        }
    return voValueOld;
    }

Object::View ConverterCollections::ConverterMapEvent::getNewValue() const
    {
    Object::View voValueNew = m_voValueNew;
    if (getNullObj() == voValueNew)
        {
        voValueNew = Converter::convert(getConverterValueUp(),
                getMapEvent()->getNewValue());
        m_voValueNew = voValueNew;
        }
    return voValueNew;
    }

// ----- accessors ------------------------------------------------------

MapEvent::View ConverterCollections::ConverterMapEvent::getMapEvent() const
    {
    return f_vEvent;
    }

Converter::View ConverterCollections::ConverterMapEvent::getConverterKeyUp() const
    {
    return f_vConvKey;
    }

Converter::View ConverterCollections::ConverterMapEvent::getConverterValueUp() const
    {
    return f_vConvVal;
    }

void ConverterCollections::ConverterMapEvent::setKey(Object::View voKey)
    {
    m_voKey = voKey;
    }

void ConverterCollections::ConverterMapEvent::setOldValue(Object::View voValue)
    {
    m_voValueOld = voValue;
    }

void ConverterCollections::ConverterMapEvent::setNewValue(Object::View voValue)
    {
    m_voValueNew = voValue;
    }

bool ConverterCollections::ConverterMapEvent::isKeyConverted() const
    {
    return m_voKey != getNullObj();
    }

bool ConverterCollections::ConverterMapEvent::isOldValueConverted() const
    {
    return m_voValueOld != getNullObj();
    }

bool ConverterCollections::ConverterMapEvent::isNewValueConverted() const
    {
    return m_voValueNew != getNullObj();
    }


// ----- inner class: ConverterMapListener ----------------------------------

// ----- constructors ---------------------------------------------------

ConverterCollections::ConverterMapListener::ConverterMapListener(
        ObservableMap::Handle hMap, MapListener::Handle hListener,
        Converter::View vConvKey, Converter::View vConvVal)
        : f_hMap(self(), hMap), f_hListener(self(), hListener),
          f_vConvKey(self(), vConvKey), f_vConvVal(self(), vConvVal)
    {
    }

// ----- MapListener interface ------------------------------------------

void ConverterCollections::ConverterMapListener::entryInserted(
        MapEvent::View vEvent)
    {
    MapEvent::View vConvEvent = instanceof<CacheEvent::View>(vEvent) ?
            (MapEvent::View) ConverterCacheEvent::create(getObservableMap(),
                    cast<CacheEvent::View>(vEvent), getConverterKeyUp(),
                    getConverterValueUp()) :
            (MapEvent::View) ConverterMapEvent::create(getObservableMap(),
                    vEvent, getConverterKeyUp(), getConverterValueUp());

    getMapListener()->entryInserted(vConvEvent);
    }

void ConverterCollections::ConverterMapListener::entryUpdated(
        MapEvent::View vEvent)
    {
    MapEvent::View vConvEvent = instanceof<CacheEvent::View>(vEvent) ?
            (MapEvent::View) ConverterCacheEvent::create(getObservableMap(),
                    cast<CacheEvent::View>(vEvent), getConverterKeyUp(),
                    getConverterValueUp()) :
            (MapEvent::View) ConverterMapEvent::create(getObservableMap(),
                    vEvent, getConverterKeyUp(), getConverterValueUp());

    getMapListener()->entryUpdated(vConvEvent);
    }

void ConverterCollections::ConverterMapListener::entryDeleted(
        MapEvent::View vEvent)
    {
    MapEvent::View vConvEvent = instanceof<CacheEvent::View>(vEvent) ?
            (MapEvent::View) ConverterCacheEvent::create(getObservableMap(),
                    cast<CacheEvent::View>(vEvent), getConverterKeyUp(),
                    getConverterValueUp()) :
            (MapEvent::View) ConverterMapEvent::create(getObservableMap(),
                    vEvent, getConverterKeyUp(), getConverterValueUp());

    getMapListener()->entryDeleted(vConvEvent);
    }

// ----- Object interface -----------------------------------------------

bool ConverterCollections::ConverterMapListener::equals(Object::View v) const
    {
    ConverterMapListener::View vThat = cast<ConverterMapListener::View>
            (v, false);

    if (vThat != NULL)
        {
        return getMapListener()->equals(vThat->getMapListener()) &&
                getConverterKeyUp()->equals(vThat->getConverterKeyUp()) &&
                getConverterValueUp()->equals(vThat->getConverterValueUp());
        }
    else
        {
        return false;
        }
    }

size32_t ConverterCollections::ConverterMapListener::hashCode() const
    {
    return getMapListener()->hashCode();
    }

// ----- accessors ------------------------------------------------------

ObservableMap::Handle
        ConverterCollections::ConverterMapListener::getObservableMap()
    {
    return f_hMap;
    }

ObservableMap::View
        ConverterCollections::ConverterMapListener::getObservableMap() const
    {
    return f_hMap;
    }

MapListener::Handle
        ConverterCollections::ConverterMapListener::getMapListener()
    {
    return f_hListener;
    }

MapListener::View
        ConverterCollections::ConverterMapListener::getMapListener() const
    {
    return f_hListener;
    }

Converter::View
        ConverterCollections::ConverterMapListener::getConverterKeyUp() const
    {
    return f_vConvKey;
    }

Converter::View
        ConverterCollections::ConverterMapListener::getConverterValueUp() const
    {
    return f_vConvVal;
    }


// ----- inner class: ConverterCacheMap ------------------------------------

// ----- constructor/destrcutor -----------------------------------------

ConverterCollections::ConverterCacheMap::ConverterCacheMap(
        CacheMap::Handle hMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<ObservableMap::Handle>(hMapDelegate),
            vConvUp, vConvDown)
    {
    }

ConverterCollections::ConverterCacheMap::ConverterCacheMap(
        CacheMap::View vMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<ObservableMap::View>(vMapDelegate),
            vConvUp, vConvDown)
    {
    }


// ----- CacheMap interface ------------------------------------

Map::View ConverterCollections::ConverterCacheMap::getAll(
        Collection::View vColKeys) const
    {
    Collection::View     vKeysConv = NULL;
    EntryConverter::View vConvDown = f_vConvDown;
    EntryConverter::View vConvUp   = f_vConvUp;

    Set::View vKeySet = cast<Set::View>(vColKeys, false);
    if (NULL == vKeySet)
        {
        vKeysConv = ConverterCollection::create(vColKeys,
                vConvDown->getKeyConverter(),
                vConvUp->getKeyConverter());
        }
    else
        {
        vKeysConv = ConverterSet::create(vKeySet,
                vConvDown->getKeyConverter(),
                vConvUp->getKeyConverter());
        }

    return ConverterMap::create(getCacheMap()->getAll(vKeysConv), vConvUp,
            vConvDown);
    }

Object::Holder ConverterCollections::ConverterCacheMap::put(
        Object::View vKey, Object::Holder ohValue, int64_t cMillis)
    {
    EntryConverter::View vConvDown = f_vConvDown;

    return EntryConverter::convertValue(f_vConvUp, getCacheMap()->put(
                EntryConverter::convertKey(vConvDown, vKey),
                EntryConverter::convertValue(vConvDown, ohValue),
                cMillis));
    }

// ----- Map interface --------------------------------------------------

void ConverterCollections::ConverterCacheMap::putAll(Map::View vMap)
   {
   // rather then use ConverterMap's lazy putAll, for NamedCaches we do early
   // serialization, so that the serialized object can avoid being escaped
   // when the cache request is registered with the service during a send

   EntryConverter::View vConvDown = f_vConvDown;
   if (vMap == NULL)
       {
       return;
       }

   size32_t c = vMap->size();
   if (c == 1)
       {
       Map::Entry::View vEntry = cast<Map::Entry::View>(
               vMap->entrySet()->iterator()->next());
       getDelegate()->putAll(Collections::singletonMap(
               EntryConverter::convertKey(vConvDown, vEntry->getKey()),
               EntryConverter::convertValue(vConvDown, vEntry->getValue())));
       }
   else
       {
       Map::Handle hMap = HashMap::create(c);
       for (Iterator::Handle hIter = vMap->entrySet()->iterator(); hIter->hasNext(); )
           {
           Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
           hMap->put(EntryConverter::convertKey(vConvDown, vEntry->getKey()),
                     EntryConverter::convertValue(vConvDown, vEntry->getValue()));
           }
       getDelegate()->putAll(hMap);
       }
   }

// ----- accessors ------------------------------------------------------

CacheMap::View ConverterCollections::ConverterCacheMap::getCacheMap() const
    {
    return cast<CacheMap::View>(getDelegate());
    }

CacheMap::Handle ConverterCollections::ConverterCacheMap::getCacheMap()
    {
    return cast<CacheMap::Handle>(getDelegate());
    }


// ----- inner class: ConverterConcurrentMap --------------------------------

// ----- constructor/destrcutor -----------------------------------------

ConverterCollections::ConverterConcurrentMap::ConverterConcurrentMap(
        ConcurrentMap::Handle hMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<Map::Handle>(hMapDelegate), vConvUp, vConvDown)
    {
    }

ConverterCollections::ConverterConcurrentMap::ConverterConcurrentMap(
        ConcurrentMap::View vMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<Map::View>(vMapDelegate), vConvUp, vConvDown)
    {
    }

// ----- ConcurrentMap interface ------------------------------------

bool ConverterCollections::ConverterConcurrentMap::lock(
        Object::View vKey, int64_t cWait) const
    {
    return getConcurrentMap()->lock(
            EntryConverter::convertKey(f_vConvDown, vKey), cWait);
    }

bool ConverterCollections::ConverterConcurrentMap::lock(
        Object::View vKey) const
    {
    return getConcurrentMap()->lock(
            EntryConverter::convertKey(f_vConvDown, vKey));
    }

bool ConverterCollections::ConverterConcurrentMap::unlock(
        Object::View vKey) const
    {
    return getConcurrentMap()->unlock(
            EntryConverter::convertKey(f_vConvDown, vKey));
    }

// ----- accessors ------------------------------------------------------

ConcurrentMap::View ConverterCollections::ConverterConcurrentMap::getConcurrentMap() const
    {
    return cast<ConcurrentMap::View>(getDelegate());
    }

ConcurrentMap::Handle ConverterCollections::ConverterConcurrentMap::getConcurrentMap()
    {
    return cast<ConcurrentMap::Handle>(getDelegate());
    }


// ----- nested class: ConverterInvocableMap-----------------------------

// ----- constructor/destrcutor -----------------------------------------

ConverterCollections::ConverterInvocableMap::ConverterInvocableMap(
        InvocableMap::Handle hMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super((Map::Handle) hMapDelegate, vConvUp, vConvDown)
    {
    }

ConverterCollections::ConverterInvocableMap::ConverterInvocableMap(
        InvocableMap::View vMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super((Map::View) vMapDelegate, vConvUp, vConvDown)
    {
    }

// ----- InvocableMap interface -----------------------------------------

Object::Holder ConverterCollections::ConverterInvocableMap::invoke(
        Object::View vKey, EntryProcessor::Handle hAgent)
    {
    return EntryConverter::convertValue(f_vConvUp, getInvocableMap()->invoke(
            EntryConverter::convertKey(f_vConvDown, vKey), hAgent));
    }

Map::View ConverterCollections::ConverterInvocableMap::invokeAll(
        Collection::View vCollKeys, EntryProcessor::Handle hAgent)
    {
    Collection::View      vColKeysConv;
    EntryConverter::View  vConvUp   = f_vConvUp;
    EntryConverter::View  vConvDown = f_vConvDown;

    Set::View vKeySet = cast<Set::View>(vCollKeys, false);

    if (NULL == vKeySet)
           {
           vColKeysConv = ConverterCollection::create(vCollKeys,
                   vConvDown->getKeyConverter(),
                   vConvUp->getKeyConverter());
           }
       else
           {
           vColKeysConv = ConverterSet::create(vKeySet,
                   vConvDown->getKeyConverter(),
                   vConvUp->getKeyConverter());
           }


    return ConverterMap::create(getInvocableMap()->invokeAll(vColKeysConv,
            hAgent), vConvUp, vConvDown);
    }

Map::View ConverterCollections::ConverterInvocableMap::invokeAll(
        Filter::View vFilter, EntryProcessor::Handle hAgent)
    {
    return ConverterMap::create(getInvocableMap()->invokeAll(vFilter,
            hAgent), f_vConvUp, f_vConvDown);
    }

Object::Holder ConverterCollections::ConverterInvocableMap::aggregate(
        Collection::View vCollKeys, EntryAggregator::Handle hAgent) const
    {
    Collection::View vColKeysConv;
    EntryConverter::View  vConvUp   = f_vConvUp;
    EntryConverter::View  vConvDown = f_vConvDown;

    Set::View vKeySet = cast<Set::View>(vCollKeys, false);
    if (NULL != vKeySet)
        {
        vColKeysConv = ConverterSet::create(vKeySet,
                vConvDown->getKeyConverter(),
                vConvUp->getKeyConverter());
        }
    else
        {
        vColKeysConv = ConverterCollection::create(vCollKeys,
                vConvDown->getKeyConverter(),
                vConvUp->getKeyConverter());
        }

    return EntryConverter::convertValue(vConvUp, getInvocableMap()->aggregate(
            vColKeysConv, hAgent));
    }

Object::Holder ConverterCollections::ConverterInvocableMap::aggregate(
        Filter::View vFilter, EntryAggregator::Handle hAgent) const
    {
    return EntryConverter::convertValue(f_vConvUp, getInvocableMap()->aggregate(
            vFilter, hAgent));
    }

// ----- accessors ------------------------------------------------------

InvocableMap::View ConverterCollections::ConverterInvocableMap::getInvocableMap() const
    {
    return cast<InvocableMap::View>(getDelegate());
    }

InvocableMap::Handle ConverterCollections::ConverterInvocableMap::getInvocableMap()
    {
    return cast<InvocableMap::Handle>(getDelegate());
    }


// ----- inner class: ConverterObservableMap --------------------------------

// ----- constructors ---------------------------------------------------

ConverterCollections::ConverterObservableMap::ConverterObservableMap(
        ObservableMap::Handle hMap, EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<Map::Handle>(hMap), vConvUp, vConvDown)
    {
    }

ConverterCollections::ConverterObservableMap::ConverterObservableMap(
        ObservableMap::View vMap, EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<Map::View>(vMap), vConvUp, vConvDown)
    {
    }

// ----- ObservableMap interface ----------------------------------------

void ConverterCollections::ConverterObservableMap::addKeyListener(
        MapListener::Handle hListener, Object::View vKey, bool fLite)
    {
    getObservableMap()->addKeyListener(getConverterListener(hListener),
            Converter::convert(getDownConverter()->getKeyConverter(), vKey),
            fLite);
    }

void ConverterCollections::ConverterObservableMap::removeKeyListener(
        MapListener::Handle hListener, Object::View vKey)
    {
    getObservableMap()->removeKeyListener(getConverterListener(hListener),
            Converter::convert(getDownConverter()->getKeyConverter(), vKey));
    }

void ConverterCollections::ConverterObservableMap::addMapListener(
        MapListener::Handle hListener)
    {
    getObservableMap()->addMapListener(getConverterListener(hListener));
    }

void ConverterCollections::ConverterObservableMap::removeMapListener(
        MapListener::Handle hListener)
    {
    getObservableMap()->removeMapListener(getConverterListener(hListener));
    }

void ConverterCollections::ConverterObservableMap::addFilterListener(
        MapListener::Handle hListener, Filter::View vFilter, bool fLite)
    {
    getObservableMap()->addFilterListener(getConverterListener(hListener),
            vFilter, fLite);
    }

void ConverterCollections::ConverterObservableMap::removeFilterListener(
        MapListener::Handle hListener, Filter::View vFilter)
    {
    getObservableMap()->removeFilterListener(
            getConverterListener(hListener), vFilter);
    }

// ----- helper methods -------------------------------------------------

MapListener::Handle
        ConverterCollections::ConverterObservableMap::getConverterListener(
        MapListener::Handle hListener)
    {
    // special case MapTriggerListener, as it's not a "real" listener
    if (instanceof<MapTriggerListener::Handle>(hListener))
        {
        return hListener;
        }

    MapListener::Handle hConvListener = ConverterMapListener::create(this,
        hListener, getUpConverter()->getKeyConverter(),
        getUpConverter()->getValueConverter());

    if (instanceof<MapListenerSupport::SynchronousListener::Handle>(hListener))
        {
        hConvListener = MapListenerSupport::WrapperSynchronousListener::create(
                hConvListener);
        }
    return hConvListener;
    }

// ----- accessors ------------------------------------------------------

ObservableMap::Handle
        ConverterCollections::ConverterObservableMap::getObservableMap()
    {
    //TODO: casting the map every time is not efficient
    return cast<ObservableMap::Handle>(getDelegate());
    }

ObservableMap::View
        ConverterCollections::ConverterObservableMap::getObservableMap() const
    {
    //TODO: casting the map every time is not efficient
    return cast<ObservableMap::View>(getDelegate());
    }


// ----- inner class: ConverterQueryMap -------------------------------------

// ----- constructor/destrcutor -----------------------------------------

ConverterCollections::ConverterQueryMap::ConverterQueryMap(
        QueryMap::Handle hMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<Map::Handle>(hMapDelegate), vConvUp, vConvDown)
    {
    }

ConverterCollections::ConverterQueryMap::ConverterQueryMap(
        QueryMap::View vMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<Map::View>(vMapDelegate), vConvUp, vConvDown)
    {
    }


// ----- QueryMap interface -----------------------------------------

Set::View ConverterCollections::ConverterQueryMap::keySet(Filter::View vFilter) const
    {
    return ConverterSet::create(getQueryMap()->keySet(vFilter),
            EntryConverter::getKeyConverter(f_vConvUp),
            EntryConverter::getKeyConverter(f_vConvDown));
    }

Set::View ConverterCollections::ConverterQueryMap::entrySet(Filter::View vFilter) const
    {
    return ConverterSet::create(getQueryMap()->entrySet(vFilter), f_vConvUp,
            f_vConvDown);
    }

Set::View ConverterCollections::ConverterQueryMap::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    return ConverterSet::create(getQueryMap()->entrySet(vFilter, vComparator),
            f_vConvUp, f_vConvDown);
    }

void ConverterCollections::ConverterQueryMap::addIndex(ValueExtractor::View vExtractor,
        bool fOrdered, Comparator::View vComparator)
    {
    getQueryMap()->addIndex(vExtractor, fOrdered, vComparator);
    }

void ConverterCollections::ConverterQueryMap::removeIndex(ValueExtractor::View vExtractor)
    {
    getQueryMap()->removeIndex(vExtractor);
    }

// ----- accessors ------------------------------------------------------

QueryMap::View ConverterCollections::ConverterQueryMap::getQueryMap() const
    {
    return cast<QueryMap::View>(getDelegate());
    }

QueryMap::Handle ConverterCollections::ConverterQueryMap::getQueryMap()
    {
    return cast<QueryMap::Handle>(getDelegate());
    }

// ----- nested class: ConverterNamedCache ----------------------------------

// ----- constructor/destrcutor -----------------------------------------

ConverterCollections::ConverterNamedCache::ConverterNamedCache(
        NamedCache::Handle hMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<CacheMap::Handle>(hMapDelegate), vConvUp, vConvDown),
      f_ohMapConcurrent(self(), ConverterConcurrentMap::create(
          cast<ConcurrentMap::Handle>(hMapDelegate), vConvUp, vConvDown)),
      f_ohMapInvocable(self(), ConverterInvocableMap::create(
          cast<InvocableMap::Handle>(hMapDelegate), vConvUp, vConvDown)),
      f_ohMapQuery(self(), ConverterQueryMap::create(
          cast<QueryMap::Handle>(hMapDelegate), vConvUp, vConvDown))
    {
    }

ConverterCollections::ConverterNamedCache::ConverterNamedCache(
        NamedCache::View vMapDelegate,
        EntryConverter::View vConvUp,
        EntryConverter::View vConvDown)
    : super(cast<CacheMap::View>(vMapDelegate), vConvUp, vConvDown),
      f_ohMapConcurrent(self(), ConverterConcurrentMap::create(
          cast<ConcurrentMap::View>(vMapDelegate), vConvUp, vConvDown)),
      f_ohMapInvocable(self(), ConverterInvocableMap::create(
          cast<InvocableMap::View>(vMapDelegate), vConvUp, vConvDown)),
      f_ohMapQuery(self(), ConverterQueryMap::create(
          cast<QueryMap::View>(vMapDelegate), vConvUp, vConvDown))
    {
    }

// ----- Named cache interface ------------------------------------------

String::View ConverterCollections::ConverterNamedCache::getCacheName() const
    {
    return getNamedCache()->getCacheName();
    }

CacheService::Handle ConverterCollections::ConverterNamedCache::getCacheService()
    {
    return getNamedCache()->getCacheService();
    }

CacheService::View ConverterCollections::ConverterNamedCache::getCacheService() const
    {
    return getNamedCache()->getCacheService();
    }

bool ConverterCollections::ConverterNamedCache::isActive() const
    {
    return getNamedCache()->isActive();
    }

void ConverterCollections::ConverterNamedCache::release()
    {
    getNamedCache()->release();
    }

void ConverterCollections::ConverterNamedCache::destroy()
    {
    getNamedCache()->destroy();
    }

// ----- ConcurrentMap interface ------------------------------------

bool ConverterCollections::ConverterNamedCache::lock(
        Object::View vKey, int64_t cWait) const
    {
    return getConcurrentMap()->lock(vKey, cWait);
    }

bool ConverterCollections::ConverterNamedCache::lock(
        Object::View vKey) const
    {
    return getConcurrentMap()->lock(vKey);
    }

bool ConverterCollections::ConverterNamedCache::unlock(
        Object::View vKey) const
    {
    return getConcurrentMap()->unlock(vKey);
    }

// ----- InvocableMap interface -----------------------------------------

Object::Holder ConverterCollections::ConverterNamedCache::invoke(
        Object::View vKey, EntryProcessor::Handle hAgent)
    {
    return getInvocableMap()->invoke(vKey, hAgent);
    }

Map::View ConverterCollections::ConverterNamedCache::invokeAll(
        Collection::View vCollKeys, EntryProcessor::Handle hAgent)
    {
    return getInvocableMap()->invokeAll(vCollKeys, hAgent);
    }

Map::View ConverterCollections::ConverterNamedCache::invokeAll(
        Filter::View vFilter, EntryProcessor::Handle hAgent)
    {
    return getInvocableMap()->invokeAll(vFilter, hAgent);
    }

Object::Holder ConverterCollections::ConverterNamedCache::aggregate(
        Collection::View vCollKeys, EntryAggregator::Handle hAgent) const
    {
    return getInvocableMap()->aggregate(vCollKeys, hAgent);
    }

Object::Holder ConverterCollections::ConverterNamedCache::aggregate(
        Filter::View vFilter, EntryAggregator::Handle hAgent) const
    {
    return getInvocableMap()->aggregate(vFilter, hAgent);
    }

// ----- QueryMap interface ---------------------------------------------

Set::View ConverterCollections::ConverterNamedCache::keySet(Filter::View vFilter) const
    {
    return getQueryMap()->keySet(vFilter);
    }

Set::View ConverterCollections::ConverterNamedCache::entrySet(Filter::View vFilter) const
    {
    return getQueryMap()->entrySet(vFilter);
    }

Set::View ConverterCollections::ConverterNamedCache::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    return getQueryMap()->entrySet(vFilter, vComparator);
    }

void ConverterCollections::ConverterNamedCache::addIndex(ValueExtractor::View vExtractor,
        bool fOrdered, Comparator::View vComparator)
    {
    getQueryMap()->addIndex(vExtractor, fOrdered, vComparator);
    }

void ConverterCollections::ConverterNamedCache::removeIndex(ValueExtractor::View vExtractor)
    {
    getQueryMap()->removeIndex(vExtractor);
    }

// ----- accessors ------------------------------------------------------

NamedCache::View ConverterCollections::ConverterNamedCache::getNamedCache() const
    {
    return cast<NamedCache::View>(getDelegate());
    }

NamedCache::Handle ConverterCollections::ConverterNamedCache::getNamedCache()
    {
    return cast<NamedCache::Handle>(getDelegate());
    }

ConcurrentMap::View ConverterCollections::ConverterNamedCache::getConcurrentMap() const
    {
    return f_ohMapConcurrent;
    }

ConcurrentMap::Handle ConverterCollections::ConverterNamedCache::getConcurrentMap()
    {
    // cast to ensure that we're holding a handle
    return cast<ConcurrentMap::Handle>(f_ohMapConcurrent);
    }

InvocableMap::View ConverterCollections::ConverterNamedCache::getInvocableMap() const
    {
    return f_ohMapInvocable;
    }

InvocableMap::Handle ConverterCollections::ConverterNamedCache::getInvocableMap()
    {
    // cast to ensure that we're holding a handle
    return cast<InvocableMap::Handle>(f_ohMapInvocable);
    }

QueryMap::View ConverterCollections::ConverterNamedCache::getQueryMap() const
    {
    return f_ohMapQuery;
    }

QueryMap::Handle ConverterCollections::ConverterNamedCache::getQueryMap()
    {
    // cast to ensure that we're holding a handle
    return cast<QueryMap::Handle>(f_ohMapQuery);
    }

// ---- nested class: UnsupportedConverter ----------------------------------

// ----- constructor ----------------------------------------------------

ConverterCollections::UnsupportedConverter::UnsupportedConverter()
    {
    }

// ----- Converter interface --------------------------------------------

Object::Holder ConverterCollections::UnsupportedConverter::convert(
        Object::Holder /*oh*/) const
    {
    COH_THROW (UnsupportedOperationException::create());
    }

ConverterCollections::UnsupportedConverter::View
ConverterCollections::UnsupportedConverter::getInstance()
    {
    static FinalView<UnsupportedConverter> v(System::common(), create());
    UnsupportedConverter::View vLocal = v;
    return vLocal == NULL
            ? (UnsupportedConverter::View) UnsupportedConverter::create() // avoid NULL during shutdown
            : vLocal;
    }
COH_STATIC_INIT(ConverterCollections::UnsupportedConverter::getInstance());

COH_CLOSE_NAMESPACE2
