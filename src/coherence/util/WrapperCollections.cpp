/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/WrapperCollections.hpp"
#include "coherence/lang/UnsupportedOperationException.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- WrapperCollections::AbstractWrapperIterator ------------------------

// ----- constructors ---------------------------------------------------

WrapperCollections::AbstractWrapperIterator::AbstractWrapperIterator
    (Iterator::Handle hIterDelegate)
    : f_ohDelegate(self(), hIterDelegate)
    {
    }

WrapperCollections::AbstractWrapperIterator::AbstractWrapperIterator
    (const AbstractWrapperIterator& that)
    : super(that), f_ohDelegate(self(), cast<Iterator::Handle>(that.getDelegate()->clone()))
    {
    }

// ----- Iterator interface ---------------------------------------------

bool WrapperCollections::AbstractWrapperIterator::hasNext() const
    {
    return getDelegate()->hasNext();
    }

Object::Holder WrapperCollections::AbstractWrapperIterator::next()
    {
    return getDelegate()->next();
    }

void WrapperCollections::AbstractWrapperIterator::remove()
    {
    Muterator::Handle hMiter = cast<Muterator::Handle>(getDelegate(), false);
    if (NULL == hMiter)
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    hMiter->remove();
    }


// ----- Object interface -----------------------------------------------

TypedHandle<const String> WrapperCollections::AbstractWrapperIterator::toString() const
    {
    return getDelegate()->toString();
    }

size32_t WrapperCollections::AbstractWrapperIterator::hashCode() const
    {
    return getDelegate()->hashCode();
    }

bool WrapperCollections::AbstractWrapperIterator::equals(Object::View v)
        const
    {
    return getDelegate()->equals(v);
    }


// ----- helper methods -------------------------------------------------

Iterator::Handle WrapperCollections::AbstractWrapperIterator::getDelegate()
    {
    return cast<Iterator::Handle>(f_ohDelegate);
    }

Iterator::View WrapperCollections::AbstractWrapperIterator::getDelegate() const
    {
    return f_ohDelegate;
    }


// ----- WrapperCollections::AbstractWrapperListIterator --------------------

// ----- constructors -------------------------------------------------------

WrapperCollections::AbstractWrapperListIterator::AbstractWrapperListIterator
    (ListIterator::Handle hIterDelegate)
    : super(hIterDelegate),
      f_ohDelegate(self(), hIterDelegate)
    {
    }

WrapperCollections::AbstractWrapperListIterator::AbstractWrapperListIterator
    (const AbstractWrapperListIterator& that)
    : super(cast<Iterator::Handle>(that.getDelegate()->clone())),
      f_ohDelegate(self(), cast<ListIterator::Handle>(that.getListDelegate()->clone()))
    {
    }


// ----- ListIterator interface ---------------------------------------------

bool WrapperCollections::AbstractWrapperListIterator::hasPrevious() const
    {
    return getListDelegate()->hasPrevious();
    }

Object::Holder WrapperCollections::AbstractWrapperListIterator::previous()
    {
    return getListDelegate()->previous();
    }

size32_t WrapperCollections::AbstractWrapperListIterator::nextIndex() const
    {
    return getListDelegate()->nextIndex();
    }

size32_t WrapperCollections::AbstractWrapperListIterator::previousIndex() const
    {
    return getListDelegate()->previousIndex();
    }

// ----- Iterator interface -------------------------------------------------

bool WrapperCollections::AbstractWrapperListIterator::hasNext() const
    {
    return getListDelegate()->hasNext();
    }

Object::Holder WrapperCollections::AbstractWrapperListIterator::next()
    {
    return getListDelegate()->next();
    }

// ----- ListMuterator interface --------------------------------------------

void WrapperCollections::AbstractWrapperListIterator::add(Object::Holder oh)
    {
    ListMuterator::Handle hMiter = cast<ListMuterator::Handle>(getDelegate(), false);
    if (NULL == hMiter)
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    return hMiter->add(oh);
    }

void WrapperCollections::AbstractWrapperListIterator::set(Object::Holder oh)
    {
    ListMuterator::Handle hMiter = cast<ListMuterator::Handle>(getDelegate(), false);
    if (NULL == hMiter)
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    return hMiter->set(oh);
    }

void WrapperCollections::AbstractWrapperListIterator::remove()
    {
    ListMuterator::Handle hMuter = cast<ListMuterator::Handle>(getDelegate(), false);
    if (NULL == hMuter)
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    hMuter->remove();
    }

// ----- helper methods -----------------------------------------------------

ListIterator::Handle
    WrapperCollections::AbstractWrapperListIterator::getListDelegate()
    {
    return cast<ListIterator::Handle>(f_ohDelegate);
    }

ListIterator::View
    WrapperCollections::AbstractWrapperListIterator::getListDelegate() const
    {
    return f_ohDelegate;
    }


// -------- WrapperCollections::AbstractWrapperCollection -------------------

// ----- constructors ---------------------------------------------------

WrapperCollections::AbstractWrapperCollection::AbstractWrapperCollection
    (Collection::Holder ohCollDelegate)
    : f_ohDelegate(self(), ohCollDelegate)
    {
    }

WrapperCollections::AbstractWrapperCollection::AbstractWrapperCollection
    (const AbstractWrapperCollection& that)
    : super(that), f_ohDelegate(self(), cast<Collection::Handle>(that.getDelegate()->clone()))
    {
    }

// ----- Collection interface -------------------------------------------

size32_t WrapperCollections::AbstractWrapperCollection::size() const
    {
    return getDelegate()->size();
    }

bool WrapperCollections::AbstractWrapperCollection::isEmpty() const
    {
    return getDelegate()->isEmpty();
    }

bool WrapperCollections::AbstractWrapperCollection::contains(Object::View v)
        const
    {
    return getDelegate()->contains(v);
    }

Iterator::Handle WrapperCollections::AbstractWrapperCollection::iterator()
        const
    {
    return getDelegate()->iterator();
    }

Muterator::Handle WrapperCollections::AbstractWrapperCollection::iterator()
    {
    return getDelegate()->iterator();
    }

ObjectArray::Handle WrapperCollections::AbstractWrapperCollection::toArray(
        ObjectArray::Handle hao) const
    {
    return getDelegate()->toArray(hao);
    }

bool WrapperCollections::AbstractWrapperCollection::add(Object::Holder oh)
    {
    return getDelegate()->add(oh);
    }

bool WrapperCollections::AbstractWrapperCollection::remove(Object::View v)
    {
    return getDelegate()->remove(v);
    }

bool WrapperCollections::AbstractWrapperCollection::containsAll
        (Collection::View vc) const
    {
    return getDelegate()->containsAll(vc);
    }

bool WrapperCollections::AbstractWrapperCollection::addAll
        (Collection::View vc)
    {
    return getDelegate()->addAll(vc);
    }

bool WrapperCollections::AbstractWrapperCollection::removeAll
        (Collection::View vc)
    {
    return getDelegate()->removeAll(vc);
    }

bool WrapperCollections::AbstractWrapperCollection::retainAll
        (Collection::View vc)
    {
    return getDelegate()->retainAll(vc);
    }

void WrapperCollections::AbstractWrapperCollection::clear()
    {
    return getDelegate()->clear();
    }

// ----- Object interface -----------------------------------------------

TypedHandle<const String> WrapperCollections::AbstractWrapperCollection::toString() const
    {
    return getDelegate()->toString();
    }

size32_t WrapperCollections::AbstractWrapperCollection::hashCode() const
    {
    return getDelegate()->hashCode();
    }

bool WrapperCollections::AbstractWrapperCollection::equals(Object::View v)
        const
    {
    return getDelegate()->equals(v);
    }

bool WrapperCollections::AbstractWrapperCollection::isImmutable() const
    {
    return getDelegate()->isImmutable();
    }

// ----- helper methods -------------------------------------------------

Collection::Handle WrapperCollections::AbstractWrapperCollection::getDelegate()
    {
    return cast<Collection::Handle>(f_ohDelegate);
    }

Collection::View WrapperCollections::AbstractWrapperCollection::getDelegate() const
    {
    return f_ohDelegate;
    }


// ----- WrapperCollections::AbstractWrapperSet -----------------------------

// ----- constructors ---------------------------------------------------

WrapperCollections::AbstractWrapperSet::AbstractWrapperSet
    (Set::Holder ohSetDelegate)
    : f_ohDelegate(self(), ohSetDelegate)
    {
    }

WrapperCollections::AbstractWrapperSet::AbstractWrapperSet
    (const AbstractWrapperSet& that)
    : super(that), f_ohDelegate(self(), cast<Set::Handle>(that.getDelegate()->clone()))
    {
    }

// ----- Set interface --------------------------------------------------

size32_t WrapperCollections::AbstractWrapperSet::size() const
    {
    return getDelegate()->size();
    }

bool WrapperCollections::AbstractWrapperSet::isEmpty() const
    {
    return getDelegate()->isEmpty();
    }

bool WrapperCollections::AbstractWrapperSet::contains(Object::View v) const
    {
    return getDelegate()->contains(v);
    }

Iterator::Handle WrapperCollections::AbstractWrapperSet::iterator() const
    {
    return getDelegate()->iterator();
    }

Muterator::Handle WrapperCollections::AbstractWrapperSet::iterator()
    {
    return getDelegate()->iterator();
    }

ObjectArray::Handle WrapperCollections::AbstractWrapperSet::toArray(
        ObjectArray::Handle hao) const
    {
    return getDelegate()->toArray(hao);
    }

bool WrapperCollections::AbstractWrapperSet::add(Object::Holder oh)
    {
    return getDelegate()->add(oh);
    }

bool WrapperCollections::AbstractWrapperSet::remove(Object::View v)
    {
    return getDelegate()->remove(v);
    }

bool WrapperCollections::AbstractWrapperSet::containsAll(Collection::View vc)
        const
    {
    return getDelegate()->containsAll(vc);
    }

bool WrapperCollections::AbstractWrapperSet::addAll(Collection::View vc)
    {
    return getDelegate()->addAll(vc);
    }

bool WrapperCollections::AbstractWrapperSet::removeAll(Collection::View vc)
    {
    return getDelegate()->removeAll(vc);
    }

bool WrapperCollections::AbstractWrapperSet::retainAll(Collection::View vc)
    {
    return getDelegate()->retainAll(vc);
    }

void WrapperCollections::AbstractWrapperSet::clear()
    {
    return getDelegate()->clear();
    }

// ----- Object interface -----------------------------------------------

TypedHandle<const String> WrapperCollections::AbstractWrapperSet::toString() const
    {
    return getDelegate()->toString();
    }

size32_t WrapperCollections::AbstractWrapperSet::hashCode() const
    {
    return getDelegate()->hashCode();
    }

bool WrapperCollections::AbstractWrapperSet::equals(Object::View v) const
    {
    return getDelegate()->equals(v);
    }

bool WrapperCollections::AbstractWrapperSet::isImmutable() const
    {
    return getDelegate()->isImmutable();
    }

// ----- helper methods -------------------------------------------------

Set::Handle WrapperCollections::AbstractWrapperSet::getDelegate()
    {
    return cast<Set::Handle>(f_ohDelegate);
    }

Set::View WrapperCollections::AbstractWrapperSet::getDelegate() const
    {
    return f_ohDelegate;
    }


// ----- WrapperCollections::AbstractWrapperList ----------------------------

// ----- constructors -------------------------------------------------------

WrapperCollections::AbstractWrapperList::AbstractWrapperList
    (List::Holder ohListDelegate)
    : f_ohDelegate(self(), ohListDelegate)
    {
    }

WrapperCollections::AbstractWrapperList::AbstractWrapperList
    (const AbstractWrapperList& that)
    : super(that), f_ohDelegate(self(), cast<List::Handle>(that.getDelegate()->clone()))
    {
    }


// ----- List interface -----------------------------------------------------

bool WrapperCollections::AbstractWrapperList::add(
        size32_t i, Object::Holder oh)
    {
    return getDelegate()->add(i, oh);
    }

bool WrapperCollections::AbstractWrapperList::addAll(
        size32_t i, Collection::View vc)
    {
    return getDelegate()->addAll(i, vc);
    }

Object::Holder WrapperCollections::AbstractWrapperList::get(size32_t i) const
    {
    return getDelegate()->get(i);
    }

Object::Holder WrapperCollections::AbstractWrapperList::get(size32_t i)
    {
    return getDelegate()->get(i);
    }

size32_t WrapperCollections::AbstractWrapperList::indexOf(
        Object::View v) const
    {
    return getDelegate()->indexOf(v);
    }

size32_t WrapperCollections::AbstractWrapperList::lastIndexOf(
        Object::View v) const
    {
    return getDelegate()->lastIndexOf(v);
    }

ListIterator::Handle WrapperCollections::AbstractWrapperList::listIterator() const
    {
    return getDelegate()->listIterator();
    }

ListIterator::Handle WrapperCollections::AbstractWrapperList::listIterator(
        size32_t index) const
    {
    return getDelegate()->listIterator(index);
    }

ListMuterator::Handle WrapperCollections::AbstractWrapperList::listIterator()
    {
    return getDelegate()->listIterator();
    }

ListMuterator::Handle WrapperCollections::AbstractWrapperList::listIterator(
        size32_t index)
    {
    return getDelegate()->listIterator(index);
    }

Object::Holder WrapperCollections::AbstractWrapperList::remove(size32_t idx)
    {
    return getDelegate()->remove(idx);
    }

Object::Holder WrapperCollections::AbstractWrapperList::set(
        size32_t idx, Object::Holder oh)
    {
    return getDelegate()->set(idx, oh);
    }

List::Handle WrapperCollections::AbstractWrapperList::subList(
        size32_t fromIndex, size32_t toIndex)
    {
    return getDelegate()->subList(fromIndex, toIndex);
    }

List::View WrapperCollections::AbstractWrapperList::subList(
        size32_t fromIndex, size32_t toIndex) const
    {
    return getDelegate()->subList(fromIndex, toIndex);
    }


// ----- Collection interface -----------------------------------------------

size32_t WrapperCollections::AbstractWrapperList::size() const
    {
    return getDelegate()->size();
    }

bool WrapperCollections::AbstractWrapperList::isEmpty() const
    {
    return getDelegate()->isEmpty();
    }

bool WrapperCollections::AbstractWrapperList::contains(
        Object::View v) const
    {
    return getDelegate()->contains(v);
    }

bool WrapperCollections::AbstractWrapperList::containsAll(
        Collection::View vCol) const
    {
    return getDelegate()->containsAll(vCol);
    }

Iterator::Handle WrapperCollections::AbstractWrapperList::iterator() const
    {
    return getDelegate()->iterator();
    }

Muterator::Handle WrapperCollections::AbstractWrapperList::iterator()
    {
    return getDelegate()->iterator();
    }

ObjectArray::Handle WrapperCollections::AbstractWrapperList::toArray(
        ObjectArray::Handle hao) const
    {
    return getDelegate()->toArray(hao);
    }

bool WrapperCollections::AbstractWrapperList::add(Object::Holder oh)
    {
    return getDelegate()->add(oh);
    }

bool WrapperCollections::AbstractWrapperList::addAll(Collection::View vCol)
    {
    return getDelegate()->addAll(vCol);
    }

bool WrapperCollections::AbstractWrapperList::remove(
        Object::View v)
    {
    return getDelegate()->remove(v);
    }

bool WrapperCollections::AbstractWrapperList::removeAll(
        Collection::View vCol)
    {
    return getDelegate()->removeAll(vCol);
    }

bool WrapperCollections::AbstractWrapperList::retainAll(
        Collection::View vCol)
    {
    return getDelegate()->retainAll(vCol);
    }

void WrapperCollections::AbstractWrapperList::clear()
    {
    return getDelegate()->clear();
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> WrapperCollections::AbstractWrapperList::toString() const
    {
    return getDelegate()->toString();
    }

size32_t WrapperCollections::AbstractWrapperList::hashCode() const
    {
    return getDelegate()->hashCode();
    }

bool WrapperCollections::AbstractWrapperList::equals(Object::View v) const
    {
    return getDelegate()->equals(v);
    }

bool WrapperCollections::AbstractWrapperList::isImmutable() const
    {
    return getDelegate()->isImmutable();
    }


// ----- helper methods ----------------------------------------------------

List::Handle WrapperCollections::AbstractWrapperList::getDelegate()
    {
    return cast<List::Handle>(f_ohDelegate);
    }

List::View WrapperCollections::AbstractWrapperList::getDelegate() const
    {
    return f_ohDelegate;
    }


// ----- WrapperCollections::AbstractWrapperMap -----------------------------

// ----- constructors ---------------------------------------------------

WrapperCollections::AbstractWrapperMap::AbstractWrapperMap
    (Map::Holder ohMapDelegate)
    : f_ohDelegate(self(), ohMapDelegate)
    {
    }

WrapperCollections::AbstractWrapperMap::AbstractWrapperMap
        (const AbstractWrapperMap& that)
    : super(that), f_ohDelegate(self(), cast<Map::Handle>(that.getDelegate()->clone()))
    {
    }

// ----- Map interface --------------------------------------------------

size32_t WrapperCollections::AbstractWrapperMap::size() const
    {
    return getDelegate()->size();
    }

bool WrapperCollections::AbstractWrapperMap::isEmpty() const
    {
    return getDelegate()->isEmpty();
    }

bool WrapperCollections::AbstractWrapperMap::containsKey(Object::View vKey)
        const
    {
    return getDelegate()->containsKey(vKey);
    }

bool WrapperCollections::AbstractWrapperMap::containsValue
        (Object::View vValue) const
    {
    return getDelegate()->containsValue(vValue);
    }

Object::Holder WrapperCollections::AbstractWrapperMap::get(Object::View vKey)
        const
    {
    return getDelegate()->get(vKey);
    }

Object::Holder WrapperCollections::AbstractWrapperMap::get(Object::View vKey)
    {
    return getDelegate()->get(vKey);
    }

Object::Holder WrapperCollections::AbstractWrapperMap::put
        (Object::View vKey, Object::Holder ohValue)
    {
    return getDelegate()->put(vKey, ohValue);
    }

Object::Holder WrapperCollections::AbstractWrapperMap::remove
        (Object::View vKey)
    {
    return getDelegate()->remove(vKey);
    }

void WrapperCollections::AbstractWrapperMap::putAll(Map::View vmap)
    {
    getDelegate()->putAll(vmap);
    }

void WrapperCollections::AbstractWrapperMap::clear()
    {
    getDelegate()->clear();
    }

Set::View WrapperCollections::AbstractWrapperMap::keySet() const
    {
    return getDelegate()->keySet();
    }

Set::Handle WrapperCollections::AbstractWrapperMap::keySet()
    {
    return getDelegate()->keySet();
    }

Collection::View WrapperCollections::AbstractWrapperMap::values() const
    {
    return getDelegate()->values();
    }

Collection::Handle WrapperCollections::AbstractWrapperMap::values()
    {
    return getDelegate()->values();
    }

Set::View WrapperCollections::AbstractWrapperMap::entrySet() const
    {
    return getDelegate()->entrySet();
    }

Set::Handle WrapperCollections::AbstractWrapperMap::entrySet()
    {
    return getDelegate()->entrySet();
    }

TypedHandle<const String> WrapperCollections::AbstractWrapperMap::toString() const
    {
    return getDelegate()->toString();
    }

// ----- Object interface -----------------------------------------------

size32_t WrapperCollections::AbstractWrapperMap::hashCode() const
    {
    return getDelegate()->hashCode();
    }

bool WrapperCollections::AbstractWrapperMap::equals(Object::View v) const
    {
    return getDelegate()->equals(v);
    }

bool WrapperCollections::AbstractWrapperMap::isImmutable() const
    {
    return getDelegate()->isImmutable();
    }

// ----- helper methods -------------------------------------------------

Map::Handle WrapperCollections::AbstractWrapperMap::getDelegate()
    {
    return cast<Map::Handle>(f_ohDelegate);
    }

Map::View WrapperCollections::AbstractWrapperMap::getDelegate() const
    {
    return f_ohDelegate;
    }


// ----- inner class: AbstractWrapperEntry ----------------------------------

// ----- constructors ---------------------------------------------------

WrapperCollections::AbstractWrapperEntry::AbstractWrapperEntry
    (Map::Entry::Holder ohEntryDelegate)
    : f_ohDelegate(self(), ohEntryDelegate)
    {
    }

WrapperCollections::AbstractWrapperEntry::AbstractWrapperEntry
        (const AbstractWrapperEntry& that)
    : super(that), f_ohDelegate(self(), cast<Map::Entry::Handle>(that.getDelegate()->clone()))
    {
    }

// ----- Map::Entry interface -------------------------------------------

Object::View WrapperCollections::AbstractWrapperEntry::getKey() const
    {
    return getDelegate()->getKey();
    }

Object::Holder WrapperCollections::AbstractWrapperEntry::getValue() const
    {
    return getDelegate()->getValue();
    }

Object::Holder WrapperCollections::AbstractWrapperEntry::getValue()
    {
    return getDelegate()->getValue();
    }

Object::Holder WrapperCollections::AbstractWrapperEntry::setValue(Object::Holder ohValue)
    {
    return getDelegate()->setValue(ohValue);
    }

// ----- Object interface -----------------------------------------------

TypedHandle<const String> WrapperCollections::AbstractWrapperEntry::toString() const
    {
    return getDelegate()->toString();
    }

size32_t WrapperCollections::AbstractWrapperEntry::hashCode() const
    {
    return getDelegate()->hashCode();
    }

bool WrapperCollections::AbstractWrapperEntry::equals(Object::View v) const
    {
    return getDelegate()->equals(v);
    }

bool WrapperCollections::AbstractWrapperEntry::isImmutable() const
    {
    return getDelegate()->isImmutable();
    }

// ----- helper methods -------------------------------------------------

Map::Entry::Handle WrapperCollections::AbstractWrapperEntry::getDelegate()
    {
    return cast<Map::Entry::Handle>(f_ohDelegate);
    }

Map::Entry::View WrapperCollections::AbstractWrapperEntry::getDelegate() const
    {
    return f_ohDelegate;
    }

COH_CLOSE_NAMESPACE2
