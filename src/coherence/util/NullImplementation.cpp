/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/NullImplementation.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_NAMED_CLASS(NullCollection, TypedClass<NullImplementation::NullCollection>::create());
COH_REGISTER_NAMED_CLASS(NullObservableMap, TypedClass<NullImplementation::NullObservableMap>::create());
COH_REGISTER_NAMED_CLASS(NullValueExtractor, TypedClass<NullImplementation::NullValueExtractor>::create());

COH_REGISTER_ONLY_PORTABLE_CLASS(11, NullCollection);
COH_REGISTER_ONLY_PORTABLE_CLASS(12, NullObservableMap);
COH_REGISTER_ONLY_PORTABLE_CLASS(13, NullValueExtractor);

ObjectArray::Handle NullImplementation::getArray()
    {
    static FinalHandle<ObjectArray> hResult(System::common(),
            ObjectArray::create(0));
    return hResult;
    }
COH_STATIC_INIT(NullImplementation::getArray());

Converter::Handle NullImplementation::getConverter()
    {
    return NullImplementation::NullConverter::getInstance();
    }

Iterator::Handle NullImplementation::getIterator()
    {
    return NullImplementation::NullMuterator::getInstance();
    }

Muterator::Handle NullImplementation::getMuterator()
    {
    return NullImplementation::NullMuterator::getInstance();
    }

Set::Handle NullImplementation::getSet()
    {
    return NullImplementation::NullSet::getInstance();
    }

Collection::Handle NullImplementation::getCollection()
    {
    return NullImplementation::NullCollection::getInstance();
    }

Map::Handle NullImplementation::getMap()
    {
    return NullImplementation::NullMap::getInstance();
    }

ObservableMap::Handle NullImplementation::getObservableMap()
    {
    return NullImplementation::NullObservableMap::getInstance();
    }

ValueExtractor::Handle NullImplementation::getValueExtractor()
    {
    return NullImplementation::NullValueExtractor::getInstance();
    }


// ----- Inner class: NullConverter -----------------------------------------

// ----- Converter interface --------------------------------------------

NullImplementation::NullConverter::NullConverter()
    {
    }

Object::Holder NullImplementation::NullConverter::convert(Object::Holder oh) const
    {
    return oh;
    }

NullImplementation::NullConverter::Handle NullImplementation::NullConverter::getInstance()
  {
  return NullImplementation::NullConverter::create();
  }
COH_STATIC_INIT(NullImplementation::NullConverter::getInstance());


// ----- Inner class: NullMuterator -----------------------------------------

// ----- Muterator interface -------------------------------------------

NullImplementation::NullMuterator::NullMuterator()
    {
    }

bool NullImplementation::NullMuterator::hasNext() const
  {
  return false;
  }

Object::Holder NullImplementation::NullMuterator::next()
  {
  COH_THROW (UnsupportedOperationException::create(""));
  }

void NullImplementation::NullMuterator::remove()
  {
  }

NullImplementation::NullMuterator::Handle NullImplementation::NullMuterator::getInstance()
  {
  return NullImplementation::NullMuterator::create();
  }
COH_STATIC_INIT(NullImplementation::NullMuterator::getInstance());


// ----- Inner class: NullSet -----------------------------------------------

// ----- Set interface -------------------------------------------

NullImplementation::NullSet::NullSet()
    {
    }

NullImplementation::NullSet::NullSet(const NullSet& that)
    : super(that)
    {
    }

bool NullImplementation::NullSet::contains(Object::View /*v*/) const
  {
  return false;
  }

Iterator::Handle NullImplementation::NullSet::iterator() const
  {
  return NullImplementation::getIterator();
  }

Muterator::Handle NullImplementation::NullSet::iterator()
  {
  return NullImplementation::getMuterator();
  }

ObjectArray::Handle NullImplementation::NullSet::toArray(
        ObjectArray::Handle /*hao*/ ) const
  {
  return NullImplementation::getArray();
  }

bool NullImplementation::NullSet::add(Object::Holder /*oh*/)
  {
  return false;
  }

bool NullImplementation::NullSet::remove(Object::View /*v*/)
  {
  return false;
  }

bool NullImplementation::NullSet::containsAll(Collection::View /*vc*/) const
  {
  return false;
  }

bool NullImplementation::NullSet::addAll(Collection::View /*vc*/)
  {
  return false;
  }

bool NullImplementation::NullSet::removeAll(Collection::View /*vc*/)
  {
  return false;
  }

bool NullImplementation::NullSet::retainAll(Collection::View /*vc*/)
  {
  return false;
  }

size32_t NullImplementation::NullSet::size() const
  {
  return 0;
  }

bool NullImplementation::NullSet::isEmpty() const
  {
  return true;
  }

void NullImplementation::NullSet::clear()
  {
  }


// ----- PortableObject interface -------------------------------------------

void NullImplementation::NullSet::readExternal(PofReader::Handle /* hIn */ )
  {
  }

void NullImplementation::NullSet::writeExternal(PofWriter::Handle /* hOut */) const
  {
  }


// ----- Object interface ---------------------------------------------------

bool NullImplementation::NullSet::equals(Object::View v) const
  {
  return (instanceof<NullSet::View>(v) && (cast<NullSet::View>(v))->isEmpty());
  }

NullImplementation::NullSet::Handle NullImplementation::NullSet::getInstance()
  {
  return NullImplementation::NullSet::create();
  }
COH_STATIC_INIT(NullImplementation::NullSet::getInstance());


// ----- Inner class: NullCollection ----------------------------------------

// ----- Collection interface -------------------------------------------

NullImplementation::NullCollection::NullCollection()
    {
    }

NullImplementation::NullCollection::NullCollection(const NullCollection& that)
    : super(that)
    {
    }

bool NullImplementation::NullCollection::contains(Object::View /*v*/) const
  {
  return false;
  }

Iterator::Handle NullImplementation::NullCollection::iterator() const
  {
  return NullImplementation::getIterator();
  }

Muterator::Handle NullImplementation::NullCollection::iterator()
  {
  return NullImplementation::getMuterator();
  }

ObjectArray::Handle NullImplementation::NullCollection::toArray(
        ObjectArray::Handle /*hao*/ ) const
  {
  return NullImplementation::getArray();
  }

bool NullImplementation::NullCollection::add(Object::Holder /*oh*/)
  {
  return false;
  }

bool NullImplementation::NullCollection::remove(Object::View /*v*/)
  {
  return false;
  }

bool NullImplementation::NullCollection::containsAll(Collection::View /*vc*/) const
  {
  return false;
  }

bool NullImplementation::NullCollection::addAll(Collection::View /*vc*/)
  {
  return false;
  }

bool NullImplementation::NullCollection::removeAll(Collection::View /*vc*/)
  {
  return false;
  }

bool NullImplementation::NullCollection::retainAll(Collection::View /*vc*/)
  {
  return false;
  }

size32_t NullImplementation::NullCollection::size() const
  {
  return 0;
  }

bool NullImplementation::NullCollection::isEmpty() const
  {
  return true;
  }

void NullImplementation::NullCollection::clear()
  {
  }


// ----- PortableObject interface -------------------------------------------

void NullImplementation::NullCollection::readExternal(PofReader::Handle /* hIn */ )
  {
  }

void NullImplementation::NullCollection::writeExternal(PofWriter::Handle /* hOut */) const
  {
  }


// ----- Object interface -------------------------------------------

bool NullImplementation::NullCollection::equals(Object::View v) const
  {
  return (instanceof<Collection::View>(v) && (cast<Collection::View>(v))->isEmpty());
  }

NullImplementation::NullCollection::Handle NullImplementation::NullCollection::getInstance()
  {
  return NullImplementation::NullCollection::create();
  }
COH_STATIC_INIT(NullImplementation::NullCollection::getInstance());


// ----- Inner class: NullMap -----------------------------------------------

// ----- Map interface -------------------------------------------

NullImplementation::NullMap::NullMap()
    {
    }

NullImplementation::NullMap::NullMap(const NullMap& that)
    : super(that)
    {
    }

size32_t NullImplementation::NullMap::size() const
  {
  return 0;
  }

bool NullImplementation::NullMap::isEmpty() const
  {
  return true;
  }

void NullImplementation::NullMap::clear()
  {
  }

bool NullImplementation::NullMap::containsKey(Object::View /*vKey*/) const
  {
  return false;
  }

bool NullImplementation::NullMap::containsValue(Object::View /*vValue*/) const
  {
  return false;
  }

Object::Holder NullImplementation::NullMap::get(Object::View /*vKey*/) const
  {
  COH_THROW (UnsupportedOperationException::create(""));
  }

Object::Holder NullImplementation::NullMap::get(Object::View /*vKey*/)
  {
  COH_THROW (UnsupportedOperationException::create(""));
  }

Object::Holder NullImplementation::NullMap::put(Object::View /*vKey*/, Object::Holder /*ohValue*/)
  {
  return NULL;
  }

Object::Holder NullImplementation::NullMap::remove(Object::View /*vKey*/)
  {
  return NULL;
  }

void NullImplementation::NullMap::putAll(Map::View /*vMap*/)
  {
  }

Set::View NullImplementation::NullMap::keySet() const
  {
  return NullImplementation::getSet();
  }

Set::Handle NullImplementation::NullMap::keySet()
  {
  return NullImplementation::getSet();
  }

Collection::View NullImplementation::NullMap::values() const
  {
  return NullImplementation::getCollection();
  }

Collection::Handle NullImplementation::NullMap::values()
  {
  return NullImplementation::getCollection();
  }

Set::View NullImplementation::NullMap::entrySet() const
  {
  return NullImplementation::getSet();
  }

Set::Handle NullImplementation::NullMap::entrySet()
  {
  return NullImplementation::getSet();
  }


// ----- PortableObject interface -------------------------------------------

void NullImplementation::NullMap::readExternal(PofReader::Handle /* hIn */ )
  {
  }

void NullImplementation::NullMap::writeExternal(PofWriter::Handle /* hOut */) const
  {
  }


// ----- Object interface -------------------------------------------

bool NullImplementation::NullMap::equals(Object::View v) const
  {
  return (instanceof<Map::View>(v) && (cast<Map::View>(v))->isEmpty());
  }

NullImplementation::NullMap::Handle NullImplementation::NullMap::getInstance()
  {
  return NullImplementation::NullMap::create();
  }
COH_STATIC_INIT(NullImplementation::NullMap::getInstance());


// ----- Inner class: NullObservableMap -------------------------------------


// ----- ObservableMap interface ----------------------------------------

NullImplementation::NullObservableMap::NullObservableMap()
    {
    }

NullImplementation::NullObservableMap::NullObservableMap(const NullObservableMap& that)
    : super(that)
    {
    }

void NullImplementation::NullObservableMap::addKeyListener(MapListener::Handle /*hListener*/,
      Object::View /*vKey*/, bool /*fLite*/)
  {
  }

void NullImplementation::NullObservableMap::removeKeyListener(MapListener::Handle /*hListener*/,
      Object::View /*vKey*/)
  {
  }

void NullImplementation::NullObservableMap::addMapListener(MapListener::Handle /*hListener*/)
  {
  }

void NullImplementation::NullObservableMap::removeMapListener(MapListener::Handle /*hListener*/)
  {
  }

void NullImplementation::NullObservableMap::addFilterListener(MapListener::Handle /*hListener*/,
      Filter::View /*vFilter*/, bool /*fLite*/)
  {
  }

void NullImplementation::NullObservableMap::removeFilterListener(MapListener::Handle /*hListener*/,
      Filter::View /*vFilter*/)
  {
  }

NullImplementation::NullObservableMap::Handle NullImplementation::NullObservableMap::getInstance()
  {
  return NullImplementation::NullObservableMap::create();
  }
COH_STATIC_INIT(NullImplementation::NullObservableMap::getInstance());


// ----- Inner class: NullValueExtractor -------------------------------------------

// ----- ValueExtractor interface -------------------------------------------

NullImplementation::NullValueExtractor::NullValueExtractor()
    {
    }

Object::Holder NullImplementation::NullValueExtractor::extract(Object::Holder ohTarget) const
  {
  return ohTarget;
  }

bool NullImplementation::NullValueExtractor::equals(Object::View /*v*/) const
  {
  return false;
  }

size32_t NullImplementation::NullValueExtractor::hashCode() const
  {
  return 0;
  }

// ----- PortableObject interface -------------------------------------------

void NullImplementation::NullValueExtractor::readExternal(PofReader::Handle /* hIn */ )
  {
  }

void NullImplementation::NullValueExtractor::writeExternal(PofWriter::Handle /* hOut */) const
  {
  }

NullImplementation::NullValueExtractor::Handle NullImplementation::NullValueExtractor::getInstance()
  {
  return NullImplementation::NullValueExtractor::create();
  }
COH_STATIC_INIT(NullImplementation::NullValueExtractor::getInstance());

COH_CLOSE_NAMESPACE2
