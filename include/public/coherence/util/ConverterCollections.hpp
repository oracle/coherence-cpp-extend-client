/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONVERTER_COLLECTIONS_HPP
#define COH_CONVERTER_COLLECTIONS_HPP

#include "coherence/lang.ns"


#include "coherence/net/cache/CacheEvent.hpp"
#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/ConcurrentMap.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/WrapperCollections.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::net::cache::CacheEvent;
using coherence::net::cache::CacheMap;
using coherence::net::CacheService;
using coherence::net::NamedCache;

/**
* A collection of Collection implementation classes that use the Converter
* interface to convert the items stored in underlying Collection objects.
*
* @author mf  2007.07.05
*/
class COH_EXPORT ConverterCollections
    {
    // ----- ConverterCollections interface ---------------------------------
    public:
        /**
        * Return a new lang#ObjectArray whose contents are the converted
        * contents of the supplied array.
        *
        * @param hArray  the supplied array
        * @param vConvUp the converter to use
        *
        * @return a handle to the converted array
        */
        static ObjectArray::Handle convertArray(ObjectArray::Handle hArray,
                                                Converter::View vConvUp);

        /**
        * Return a new lang#ObjectArray whose contents are the converted
        * contents of the supplied array.
        *
        * @param vArray  the supplied array
        * @param vConvUp the converter to use
        *
        * @return a view of the converted array
        */
        static ObjectArray::View convertArray(ObjectArray::View vArray,
                                              Converter::View vConvUp);


    // ----- inner classes --------------------------------------------------

    public:
        /**
        * A ConverterIterator views an underlying Iterator through a
        * Converter.
        */
        class COH_EXPORT ConverterIterator
            : public cloneable_spec<ConverterIterator,
                extends<WrapperCollections::AbstractWrapperIterator> >
            {
            friend class factory<ConverterIterator>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a WrapperCollections#AbstractWrapperIterator which
                * delegates to the specified Iterator.
                *
                * @param hiterDelegate the specified Iterator
                * @param vConvUp       the Converter to view through
                */
                ConverterIterator(Iterator::Handle hiterDelegate,
                                  Converter::View vConvUp);

                /**
                * Copy constructor.
                */
                ConverterIterator(const ConverterIterator& that);

            // ----- Iterator interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder next();


            // ----- data members ---------------------------------------

            protected:
                /**
                * The Converter from the underlying container
                */
                FinalView<Converter> f_vConvUp;
            };


        /**
        * A ConverterCollection views an underlying Collection through a
        * Converter.
        */
        class COH_EXPORT ConverterCollection
            : public cloneable_spec<ConverterCollection,
                extends<WrapperCollections::AbstractWrapperCollection> >
            {
            friend class factory<ConverterCollection>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a WrapperCollections#AbstractWrapperCollection which
                * delegates to the specified Collection.
                *
                * @param hcollDelegate the specified collection
                * @param vConvUp       the converter from the underlying
                *                      Collection
                * @param vConvDown     the converter to the underlying
                *                      Collection
                */
                ConverterCollection(Collection::Handle hcollDelegate,
                                    Converter::View vConvUp,
                                    Converter::View vConvDown);

                /**
                * Create a WrapperCollections#AbstractWrapperCollection which
                * delegates to the specified Collection.
                *
                * @param vcollDelegate the specified collection
                * @param vConvUp       the converter from the underlying
                *                      Collection
                * @param vConvDown     the converter to the underlying
                *                      Collection
                */
                ConverterCollection(Collection::View vcollDelegate,
                                    Converter::View vConvUp,
                                    Converter::View vConvDown);

                /**
                * Copy constructor.
                */
                ConverterCollection(const ConverterCollection& that);


            // ----- Collection interface -------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool contains(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual Iterator::Handle iterator() const;

                /**
                * {@inheritDoc}
                */
                virtual Muterator::Handle iterator();

                /**
                * {@inheritDoc}
                */
                virtual ObjectArray::Handle toArray(
                        ObjectArray::Handle hao = NULL) const;

                /**
                * {@inheritDoc}
                */
                virtual bool add(Object::Holder oh);

                /**
                * {@inheritDoc}
                */
                virtual bool remove(Object::View v);

                /**
                * {@inheritDoc}
                */
                virtual bool containsAll(Collection::View vc) const;

                /**
                * {@inheritDoc}
                */
                virtual bool addAll(Collection::View vc);

                /**
                * {@inheritDoc}
                */
                virtual bool removeAll(Collection::View vc);

                /**
                * {@inheritDoc}
                */
                virtual bool retainAll(Collection::View vc);

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View that) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The Converter from the underlying container
                */
                FinalView<Converter> f_vConvUp;

                /**
                * The Converter to the underlying container
                */
                FinalView<Converter> f_vConvDown;
            };


        /**
        * A ConverterSet views an underlying Set through a Converter.
        */
        class COH_EXPORT ConverterSet
            : public cloneable_spec<ConverterSet,
                extends<WrapperCollections::AbstractWrapperSet> >
            {
            friend class factory<ConverterSet>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a WrapperCollections#AbstractWrapperSet which
                * delegates to the specified Set.
                *
                * @param hSet      the specified Set
                * @param vConvUp   the Converter from the underlying Set
                * @param vConvDown the Converter to the underlying Set
                */
                ConverterSet(Set::Handle hsetDelegate,
                             Converter::View vConvUp,
                             Converter::View vConvDown);

                /**
                * Create a WrapperCollections#AbstractWrapperSet which
                * delegates to the specified Set.
                *
                * @param vSet      the specified Set
                * @param vConvUp   the Converter from the underlying Set
                * @param vConvDown the Converter to the underlying Set
                */
                ConverterSet(Set::View vsetDelegate,
                             Converter::View vConvUp,
                             Converter::View vConvDown);

                /**
                * Copy constructor.
                */
                ConverterSet(const ConverterSet& that);

            // ----- Set interface --------------------------------------

            public:

                /**
                * {@inheritDoc}
                */
                virtual bool contains(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual Iterator::Handle iterator() const;

                /**
                * {@inheritDoc}
                */
                virtual Muterator::Handle iterator();

                /**
                * {@inheritDoc}
                */
                virtual ObjectArray::Handle toArray(
                        ObjectArray::Handle hoa = NULL) const;

                /**
                * {@inheritDoc}
                */
                virtual bool add(Object::Holder oh);

                /**
                * {@inheritDoc}
                */
                virtual bool remove(Object::View v);

                /**
                * {@inheritDoc}
                */
                virtual bool containsAll(Collection::View vc) const;

                /**
                * {@inheritDoc}
                */
                virtual bool addAll(Collection::View vc);

                /**
                * {@inheritDoc}
                */
                virtual bool removeAll(Collection::View vc);

                /**
                * {@inheritDoc}
                */
                virtual bool retainAll(Collection::View vc);

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View that) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The Converter from the underlying Set
                */
                FinalView<Converter> f_vConvUp;

                /**
                * The Converter to the underlying Set
                */
                FinalView<Converter> f_vConvDown;
            };


        /**
        * Map::Entry Converter, supporting key and value conversion.
        */
        class COH_EXPORT EntryConverter
            : public class_spec<EntryConverter,
                extends<Object>,
                implements<Converter> >
            {
            friend class factory<EntryConverter>;

            // ----- constructor ----------------------------------------

            public:
                /**
                * Create a Map entry Converter.
                *
                * @param vConvKey   the key Converter
                * @param vConvVal   the value Converter
                */
                EntryConverter(Converter::View vConvKey,
                        Converter::View vConvVal);

                /**
                * Copy constructor.
                */
                EntryConverter(const EntryConverter& that);

            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder convert(Object::Holder oh) const;

            // ----- EntryConverter interface ---------------------------

            public:
                /**
                * @return a view of the key Converter
                */
                virtual Converter::View getKeyConverter() const;

                /**
                * @return a view of the value Converter
                */
                virtual Converter::View getValueConverter() const;

            // ----- static helper methods ------------------------------

            public:
                /**
                * @param vConvert the EntryConverter
                *
                * @return a view of the vConvert's key Converter
                */
                static Converter::View getKeyConverter
                    (EntryConverter::View vConvert);

                /**
                * @param vConvert the EntryConverter
                *
                * @return a view of the vConvert's value Converter
                */
                static Converter::View getValueConverter
                    (EntryConverter::View vConvert);

                /**
                * Convert the key of the map entry referenced by the
                * supplied handle using the supplied EntryConverter.
                *
                * @param vConvert a view of the EntryConverter to use
                * @param vKey     a view to the key to convert
                *
                * @return a handle to the key, or the unconverted key
                *         if the vConvert is NULL
                */
                static Object::View convertKey
                    (EntryConverter::View vConvert, Object::View vKey);

                /**
                * Convert the value of the map entry referenced by
                * the supplied view using the supplied
                * EntryConverter.
                *
                * @param vConvert  a view of the EntryConverter to
                *                  use
                * @param ohValue   a holder to the value to convert
                *
                * @return a view of the value, or the unconverted
                *         value if the vConvert is NULL
                */
                static Object::Holder convertValue
                    (EntryConverter::View vConvert, Object::Holder ohValue);

            // ----- data members ---------------------------------------

            protected:
                /**
                * The key Converter.
                */
                FinalView<Converter> f_vConvKey;

                /**
                * The value Converter.
                */
                FinalView<Converter> f_vConvVal;
            };


        /**
        * A ConverterMap views an underlying Map through a Converter.
        */
        class COH_EXPORT ConverterMap
            : public cloneable_spec<ConverterMap,
                extends<WrapperCollections::AbstractWrapperMap> >
            {
            friend class factory<ConverterMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a WrapperCollections#AbstractWrapperMap which
                * delegates to the specified Map.
                *
                * @param hMap      the specified Map
                * @param vConvUp   the EntryConverter from the underlying Map
                * @param vConvDown the EntryConverter to the underlying Map
                */
                ConverterMap(Map::Handle hMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Create a WrapperCollections#AbstractWrapperMap which
                * delegates to the specified Map.
                *
                * @param vMap      the specified Map
                * @param vConvUp   the EntryConverter from the underlying Map
                * @param vConvDown the EntryConverter to the underlying Map
                */
                ConverterMap(Map::View vMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Copy constructor.
                */
                ConverterMap(const ConverterMap& that);

            // ----- Map interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool containsKey(Object::View vKey) const;

                /**
                * {@inheritDoc}
                */
                virtual bool containsValue(Object::View vValue) const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder get(Object::View vKey) const;

                /**
                * {@inheritDoc}
                */
                using Map::get;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder put(Object::View vKey,
                                         Object::Holder ohValue);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder remove(Object::View vKey);
                using Map::remove;

                /**
                * {@inheritDoc}
                */
                virtual void putAll(Map::View vMap);

                /**
                * {@inheritDoc}
                */
                virtual Set::View keySet() const;

                /**
                * {@inheritDoc}
                */
                virtual Set::Handle keySet();

                /**
                * {@inheritDoc}
                */
                virtual Collection::View values() const;

                /**
                * {@inheritDoc}
                */
                virtual Collection::Handle values();

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet() const;

                /**
                * {@inheritDoc}
                */
                virtual Set::Handle entrySet();

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View that) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Get the up converter, which is used to convert from the
                * underlying entries in the Map.
                *
                * @return the up converter
                */
                virtual EntryConverter::View getUpConverter() const;

                /**
                * Get the down converter, which is used to convert to the
                * underlying entries in the Map.
                *
                * @return the down converter
                */
                virtual EntryConverter::View getDownConverter() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The Converter from the underlying entries in the Map
                */
                FinalView<EntryConverter> f_vConvUp;

                /**
                * The Converter to the underlying entries in the Map
                */
                FinalView<EntryConverter> f_vConvDown;
            };


        /**
        * A ConverterEntry views an underlying Map#Entry through a Converter.
        */
        class COH_EXPORT ConverterEntry
            : public cloneable_spec<ConverterEntry,
                extends<WrapperCollections::AbstractWrapperEntry> >
            {
            friend class factory<ConverterEntry>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a WrapperCollections::AbstractWrapperEntry which
                * delegates to the specified Map#Entry.
                *
                * @param hMapEntry  a handle to the specified Map#Entry
                * @param vConvUp    the EntryConverter from the underlying
                *                   Map
                * @param vConvDown  the EntryConverter to the underlying Map
                */
                ConverterEntry(Map::Entry::Handle hMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Create a WrapperCollections::AbstractWrapperEntry which
                * delegates to the specified Map#Entry.
                *
                * @param vMapEntry  a view of the specified Map#Entry
                * @param vConvUp    the EntryConverter from the underlying
                *                   Map
                * @param vConvDown  the EntryConverter to the underlying Map
                */
                ConverterEntry(Map::Entry::View vMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Copy constructor.
                */
                ConverterEntry(const ConverterEntry& that);


            // ----- Map::Entry interface -------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::View getKey() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder getValue() const;

                /**
                * {@inheritDoc}
                */
                using Map::Entry::getValue;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder setValue(Object::Holder ohValue);

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View that) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;


                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The Converter from the underlying Entry.
                */
                FinalView<EntryConverter> f_vConvUp;

                /**
                * The Converter from the underlying Entry.
                */
                FinalView<EntryConverter> f_vConvDown;
            };


        // ----- inner class: ConverterCacheEvent ---------------------------

        /**
        * A Converter CacheEvent views an underlying CacheEvent through a set
        * of key and value Converters.
        */
        class COH_EXPORT ConverterCacheEvent
            : public class_spec<ConverterCacheEvent,
                extends<CacheEvent> >
            {
            friend class factory<ConverterCacheEvent>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a ConverterCacheEvent.
                *
                * @param hMap      the new event's source
                * @param vEvent    the underlying CacheEvent
                * @param vConvKey  the Converter to view the underlying
                *                  CacheEvent's key
                * @param vConvVal  the Converter to view the underlying
                *                  CacheEvent's values
                */
                ConverterCacheEvent(ObservableMap::Handle hMap,
                        CacheEvent::View vEvent, Converter::View vConvKey,
                        Converter::View vConvVal);

            // ----- MapEvent interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::View getKey() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::View getOldValue() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::View getNewValue() const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Return the underlying CacheEvent.
                *
                * @return the underlying CacheEvent
                */
                virtual CacheEvent::View getCacheEvent() const;

                /**
                * Return the Converter used to view the underlying
                * CacheEvent's key through.
                *
                * @return the Converter from the underlying CacheEvent's key
                */
                virtual Converter::View getConverterKeyUp() const;

                /**
                * Return the Converter used to view the underlying
                * CacheEvent's value through.
                *
                * @return the Converter from the underlying CacheEvent's
                *         value
                */
                virtual Converter::View getConverterValueUp() const;

                /**
                * Set the cached converted old value associated with this
                * event.
                *
                * @param voValue the new converted "old" value
                */
                virtual void setOldValue(Object::View voValue);

                /**
                * Set the cached converted new value associated with this
                * event.
                *
                * @param voValue the new converted "new" value
                */
                virtual void setNewValue(Object::View voValue);

            // ----- data members ---------------------------------------

            protected:
                /**
                * The underlying CacheEvent.
                */
                FinalView<CacheEvent> f_vEvent;

                /**
                * The Converter to view the underlying CacheEvent's key.
                */
                FinalView<Converter> f_vConvKey;

                /**
                * The Converter to view the underlying CacheEvent's value.
                */
                FinalView<Converter> f_vConvVal;
            };


        // ----- inner class: ConverterMapEvent -----------------------------

        /**
        * A Converter MapEvent views an underlying MapEvent through a set of
        * key and value Converters.
        */
        class COH_EXPORT ConverterMapEvent
            : public class_spec<ConverterMapEvent,
                extends<MapEvent> >
            {
            friend class factory<ConverterMapEvent>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a ConverterMapEvent.
                *
                * @param hMap      the new event's source
                * @param vEvent    the underlying MapEvent
                * @param vConvKey  the Converter to view the underlying
                *                  MapEvent's key
                * @param vConvVal  the Converter to view the underlying
                *                  MapEvent's values
                */
                ConverterMapEvent(ObservableMap::Handle hMap,
                        MapEvent::View vEvent, Converter::View vConvKey,
                        Converter::View vConvVal);

            // ----- MapEvent interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::View getKey() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::View getOldValue() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::View getNewValue() const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Return the underlying MapEvent.
                *
                * @return the underlying MapEvent
                */
                virtual MapEvent::View getMapEvent() const;

                /**
                * Return the Converter used to view the underlying MapEvent's
                * key through.
                *
                * @return the Converter from the underlying MapEvent's key
                */
                virtual Converter::View getConverterKeyUp() const;

                /**
                * Return the Converter used to view the underlying MapEvent's
                * value through.
                *
                * @return the Converter from the underlying MapEvent's value
                */
                virtual Converter::View getConverterValueUp() const;

                /**
                * Set the cached converted key associated with this event.
                *
                * @param voKey the converted key value
                */
                virtual void setKey(Object::View voKey);

                /**
                * Set the cached converted old value associated with this
                * event.
                *
                * @param voValue the new converted "old" value
                */
                virtual void setOldValue(Object::View voValue);

                /**
                * Set the cached converted new value associated with this
                * event.
                *
                * @param voValue the new converted "new" value
                */
                virtual void setNewValue(Object::View voValue);

                /**
                * Check if the event's key has been converted.
                *
                * @return true iff the key has been converted
                */
                virtual bool isKeyConverted() const;

                /**
                * Check if the event's old value has been converted.
                *
                * @return true iff the old value has been converted
                */
                virtual bool isOldValueConverted() const;

                /**
                * Check if the event's new value has been converted.
                *
                * @return true iff the new value has been converted
                */
                virtual bool isNewValueConverted() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The underlying MapEvent.
                */
                FinalView<MapEvent> f_vEvent;

                /**
                * The Converter to view the underlying MapEvent's key.
                */
                FinalView<Converter> f_vConvKey;

                /**
                * The Converter to view the underlying MapEvent's value.
                */
                FinalView<Converter> f_vConvVal;
            };


        // ----- inner class: ConverterMapListener --------------------------

        /**
        * A converter MapListener that converts events of the underlying
        * MapListener for the underlying map.
        */
        class COH_EXPORT ConverterMapListener
            : public class_spec<ConverterMapListener,
                extends<Object>,
                implements<MapListener> >
            {
            friend class factory<ConverterMapListener>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a ConverterMapListener that converts events of the
                * underlying MapListener for the underlying map.
                */
                ConverterMapListener(ObservableMap::Handle hMap,
                        MapListener::Handle hListener, Converter::View vConvKey,
                        Converter::View vConvVal);

            // ----- MapListener interface ------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void entryInserted(MapEvent::View vEvent);

                /**
                * {@inheritDoc}
                */
                virtual void entryUpdated(MapEvent::View vEvent);

                /**
                * {@inheritDoc}
                */
                virtual void entryDeleted(MapEvent::View vEvent);

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Get the underlying ObservableMap.
                *
                * @return the underlying map
                */
                virtual ObservableMap::Handle getObservableMap();

                /**
                * Get the underlying ObservableMap.
                *
                * @return the underlying map
                */
                virtual ObservableMap::View getObservableMap() const;

                /**
                * Get the underling MapListener.
                *
                * @return the underlying listener
                */
                virtual MapListener::Handle getMapListener();

                /**
                * Get the underling MapListener.
                *
                * @return the underlying listener
                */
                virtual MapListener::View getMapListener() const;

                /**
                * Get the Converter used to view an underlying CacheEvent's
                * key through.
                *
                * @return the Converter used to view an underlying
                *         CacheEvent's key
                */
                virtual Converter::View getConverterKeyUp() const;

                /**
                * Get the Converter used to view an underlying CacheEvent's
                * value through.
                *
                * @return the Converter used to view an underlying
                *         CacheEvent's value
                */
                virtual Converter::View getConverterValueUp() const;

            // ----- data members -------------------------------------------

            protected:
                /**
                * The converting Map the will be the source of converted
                * events.
                */
                FinalHandle<ObservableMap> f_hMap;

                /**
                * The underlying MapListener.
                */
                FinalHandle<MapListener> f_hListener;

                /**
                * The Converter to view an underlying CacheEvent's key.
                */
                FinalView<Converter> f_vConvKey;

                /**
                * The Converter to view an underlying CacheEvent's value.
                */
                FinalView<Converter> f_vConvVal;
            };


        // ----- inner class: ConverterObservableMap ------------------------

        /**
        * A Converter ObservableMap views an underlying ObservableMap through
        * a set of key and value Converters.
        */
        class COH_EXPORT ConverterObservableMap
            : public class_spec<ConverterObservableMap,
                extends<ConverterMap>,
                implements<ObservableMap> >
            {
            friend class factory<ConverterObservableMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a ConverterObservableMap which delegates to the
                * specified ObservableMap.
                *
                * @param hMap      underlying ObservableMap
                * @param vConvUp   EntryConverter from the underlying Map
                * @param vConvDown EntryConverter to the underlying Map
                */
                ConverterObservableMap(ObservableMap::Handle hMap,
                        EntryConverter::View vConvUp,
                        EntryConverter::View vConvDown);

                /**
                * Create a ConverterObservableMap which delegates to the
                * specified ObservableMap.
                *
                * @param vMap      underlying ObservableMap
                * @param vConvUp   EntryConverter from the underlying Map
                * @param vConvDown EntryConverter to the underlying Map
                */
                ConverterObservableMap(ObservableMap::View vMap,
                        EntryConverter::View vConvUp,
                        EntryConverter::View vConvDown);

            // ----- ObservableMap interface ----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void addKeyListener(MapListener::Handle hListener,
                        Object::View vKey, bool fLite);

                /**
                * {@inheritDoc}
                */
                virtual void removeKeyListener(MapListener::Handle hListener,
                        Object::View vKey);

                /**
                * {@inheritDoc}
                */
                virtual void addMapListener(MapListener::Handle hListener);

                /**
                * {@inheritDoc}
                */
                virtual void removeMapListener(MapListener::Handle hListener);
        
                /**
                * {@inheritDoc}
                */
                virtual void addFilterListener(MapListener::Handle hListener,
                        Filter::View vFilter, bool fLite);

                /**
                * {@inheritDoc}
                */
                virtual void removeFilterListener(
                        MapListener::Handle hListener, Filter::View vFilter);

            // ----- helper methods -------------------------------------

            protected:
                /**
                * Create a converter listener for the specified listener.
                *
                * @param hListener  the underlying listener
                *
                * @return the converting listener
                */
                virtual MapListener::Handle getConverterListener(
                        MapListener::Handle hListener);

            // ----- accessors ------------------------------------------

            public:
                /**
                * Return the underlying ObservableMap.
                *
                * @return the underlying ObservableMap
                */
                virtual ObservableMap::Handle getObservableMap();

                /**
                * Return the underlying ObservableMap.
                *
                * @return the underlying ObservableMap
                */
                virtual ObservableMap::View getObservableMap() const;
            };


        // ----- inner class: ConverterCacheMap -----------------------------

        /**
        * A ConverterCacheMap views an underlying CacheMap through a set
        * of key and value Converters.
        */
        class COH_EXPORT ConverterCacheMap
            : public class_spec<ConverterCacheMap,
                extends<ConverterObservableMap>,
                implements<CacheMap> >
            {
            friend class factory<ConverterCacheMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a ConverterCacheMap which delegates to the specified
                * CacheMap.
                *
                * @param hMap      the specified CacheMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterCacheMap(CacheMap::Handle hMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Create a ConverterCacheMap which delegates to the specified
                * CacheMap.
                *
                * @param vMap      the specified CacheMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterCacheMap(CacheMap::View vMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

            // ----- CacheMap interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Map::View getAll(Collection::View vKeys) const;

                /**
                * {@inheritDoc}
                */
                using ConverterMap::put;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder put(Object::View vKey,
                        Object::Holder ohValue, int64_t cMillis);

            // ----- Map interface ----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
               virtual void putAll(Map::View vMap);


            // ----- accessors --------------------------------------

            public:
                /**
                * Return the underlying CacheMap.
                *
                * @return the underlying CacheMap
                */
                virtual CacheMap::View getCacheMap() const;

                /**
                * Return the underlying CacheMap.
                *
                * @return the underlying CacheMap
                */
                virtual CacheMap::Handle getCacheMap();
            };

        /**
        * A Converter ConverterConcurrentMap views an underlying
        * ConcurrentMap through a set of key and value Converters.
        */
        class COH_EXPORT ConverterConcurrentMap
            : public class_spec<ConverterConcurrentMap,
                extends<ConverterMap>,
                implements<ConcurrentMap> >
            {
            friend class factory<ConverterConcurrentMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a ConverterConcurrentMap which delegates to the
                * specified ConcurrentMap.
                *
                * @param hMap      the specified ConcurrentMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterConcurrentMap(ConcurrentMap::Handle hMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Create a ConverterConcurrentMap which delegates to the
                * specified ConcurrentMap.
                *
                * @param vMap      the specified ConcurrentMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterConcurrentMap(ConcurrentMap::View vMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

            // ----- ConcurrentMap interface ----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool lock(Object::View vKey, int64_t cWait) const;

                /**
                * {@inheritDoc}
                */
                virtual bool lock(Object::View vKey) const;

                /**
                * {@inheritDoc}
                */
                virtual bool unlock(Object::View vKey) const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Return the underlying ConcurrentMap.
                *
                * @return the underlying ConcurrentMap
                */
                virtual ConcurrentMap::View getConcurrentMap() const;

                /**
                * Return the underlying ConcurrentMap.
                *
                * @return the underlying ConcurrentMap
                */
                virtual ConcurrentMap::Handle getConcurrentMap();

            };

        /**
        * A ConverterInvocable views an underlying InvocableMap through a set
        * of key and value Converters.
        */
        class COH_EXPORT ConverterInvocableMap
            : public class_spec<ConverterInvocableMap,
                extends<ConverterMap>,
                implements<InvocableMap> >
            {
            friend class factory<ConverterInvocableMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a ConverterInvocableMap which delegates to the s
                * pecified InvocableMap.
                *
                * @param hMap      the specified InvocableMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterInvocableMap(InvocableMap::Handle hMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Create a ConverterInvocableMap which delegates to the s
                * pecified InvocableMap.
                *
                * @param vMap      the specified InvocableMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterInvocableMap(InvocableMap::View vMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

            // ----- InvocableMap interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder invoke(Object::View vKey,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Map::View invokeAll(Collection::View vCollKeys,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Map::View invokeAll(Filter::View vFilter,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder aggregate(Collection::View vCollKeys,
                        InvocableMap::EntryAggregator::Handle hAgent) const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder aggregate(Filter::View vFilter,
                        InvocableMap::EntryAggregator::Handle hAgent) const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Return the underlying InvocableMap.
                *
                * @return the underlying InvocableMap
                */
                virtual InvocableMap::View getInvocableMap() const;

                /**
                * Return the underlying CacheMap.
                *
                * @return the underlying CacheMap
                */
                virtual InvocableMap::Handle getInvocableMap();
            };


        // ----- inner class: ConverterQueryMap -----------------------------

        /**
        * A Converter QueryMap views an underlying QueryMap through a set of
        * key and value Converters.
        */
        class COH_EXPORT ConverterQueryMap
            : public class_spec<ConverterQueryMap,
                extends<ConverterMap>,
                implements<QueryMap> >
            {
            friend class factory<ConverterQueryMap>;

            // ----- constructors -----------------------------------

            protected:
                /**
                * Create a ConverterQueryMap which delegates to the specified
                * QueryMap.
                *
                * @param hMap      the specified InvocableMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterQueryMap(QueryMap::Handle hMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Create a ConverterQueryMap which delegates to the s
                * pecified QueryMap.
                *
                * @param vMap      the specified InvocableMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterQueryMap(QueryMap::View vMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

            // ----- QueryMap interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Set::View keySet(Filter::View vFilter) const;

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet(Filter::View vFilter) const;

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet(Filter::View vFilter,
                        Comparator::View vComparator) const;

                /**
                * {@inheritDoc}
                */
                virtual void addIndex(ValueExtractor::View vExtractor,
                        bool fOrdered, Comparator::View vComparator);

                /**
                * {@inheritDoc}
                */
                virtual void removeIndex(ValueExtractor::View vExtractor);

            // ----- Map interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                using Map::keySet;

                /**
                * {@inheritDoc}
                */
                using Map::entrySet;

            // ----- accessors ----------------------------------------------

            public:
                /**
                * Return the underlying InvocableMap.
                *
                * @return the underlying InvocableMap
                */
                virtual QueryMap::View getQueryMap() const;

                /**
                * Return the underlying CacheMap.
                *
                * @return the underlying CacheMap
                */
                virtual QueryMap::Handle getQueryMap();
            };


        /**
        * A ConverterNamedCache views an underlying NamedCache through a set
        * of key and value Converters.
        */
        class COH_EXPORT ConverterNamedCache
            : public class_spec<ConverterNamedCache,
                extends<ConverterCacheMap>,
                implements<NamedCache> >
            {
            friend class factory<ConverterNamedCache>;

            // ----- constructors -----------------------------------

            protected:
                /**
                * Create a ConverterQueryMap which delegates to the s
                * pecified QueryMap.
                *
                * @param hMap      the specified InvocableMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterNamedCache(NamedCache::Handle hMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

                /**
                * Create a ConverterQueryMap which delegates to the s
                * pecified QueryMap.
                *
                * @param vMap      the specified InvocableMap
                * @param vConvUp   the EntryConverter from the underlying
                *                  Map
                * @param vConvDown the EntryConverter to the underlying
                *                  Map
                */
                ConverterNamedCache(NamedCache::View vMapDelegate,
                             EntryConverter::View vConvUp,
                             EntryConverter::View vConvDown);

            // ----- NamedCache interface -------------------------------

            public:
                /**
                * Return the cache name.
                *
                * @return the cache name
                */
                virtual String::View getCacheName() const;

                /**
                * Return the CacheService that this NamedCache is a part of.
                *
                * @return the CacheService
                */
                virtual CacheService::Handle getCacheService();

                /**
                * Return the CacheService that this NamedCache is a part of.
                *
                * @return the CacheService
                */
                virtual CacheService::View getCacheService() const;

                /**
                * Specifies whether or not the NamedCache is active.
                *
                * @return true if the NamedCache is active; false otherwise
                */
                virtual bool isActive() const;

                /**
                * Release local resources associated with this instance of
                * NamedCache.
                *
                * Releasing a cache makes it no longer usable, but does not
                * affect the cache itself. In other words, all other
                * references to the cache will still be valid, and the cache
                * data is not affected by releasing the reference. Any
                * attempt to use this reference afterword will result in an
                * exception.
                *
                * @see CacheService#releaseCache
                */
                virtual void release();

                /**
                * Release and destroy this instance of NamedCache.
                *
                * <b>Warning:</b> This method is used to completely destroy
                * the specified cache across the cluster. All references in
                * the entire cluster to this cache will be invalidated, the
                * cached data will be cleared, and all resources will be
                * released.
                *
                * @see CacheService#destroyCache
                */
                virtual void destroy();

            // ----- ConcurrentMap interface ----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool lock(Object::View vKey, int64_t cWait) const;

                /**
                * {@inheritDoc}
                */
                virtual bool lock(Object::View vKey) const;

                /**
                * {@inheritDoc}
                */
                virtual bool unlock(Object::View vKey) const;

            // ----- InvocableMap interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder invoke(Object::View vKey,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Map::View invokeAll(Collection::View vCollKeys,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Map::View invokeAll(Filter::View vFilter,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder aggregate(Collection::View vCollKeys,
                        InvocableMap::EntryAggregator::Handle hAgent) const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder aggregate(Filter::View vFilter,
                        InvocableMap::EntryAggregator::Handle hAgent) const;

            // ----- QueryMap interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Set::View keySet(Filter::View vFilter) const;

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet(Filter::View vFilter) const;

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet(Filter::View vFilter,
                        Comparator::View vComparator) const;

                /**
                * {@inheritDoc}
                */
                virtual void addIndex(ValueExtractor::View vExtractor,
                        bool fOrdered, Comparator::View vComparator);

                /**
                * {@inheritDoc}
                */
                virtual void removeIndex(ValueExtractor::View vExtractor);

            // ----- Map interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                using Map::keySet;

                /**
                * {@inheritDoc}
                */
                using Map::entrySet;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Return the underlying InvocableMap.
                *
                * @return the underlying InvocableMap
                */
                virtual NamedCache::View getNamedCache() const;

                /**
                * Return the underlying CacheMap.
                *
                * @return the underlying CacheMap
                */
                virtual NamedCache::Handle getNamedCache();

                /**
                * Return the underlying ConcurrentMap.
                *
                * @return the underlying ConcurrentMap
                */
                virtual ConcurrentMap::View getConcurrentMap() const;

                /**
                * Return the underlying ConcurrentMap.
                *
                * @return the underlying ConcurrentMap
                */
                virtual ConcurrentMap::Handle getConcurrentMap();

                /**
                * Return the underlying InvocableMap.
                *
                * @return the underlying InvocableMap
                */
                virtual InvocableMap::View getInvocableMap() const;

                /**
                * Return the underlying InvocableMap.
                *
                * @return the underlying InvocableMap
                */
                virtual InvocableMap::Handle getInvocableMap();

                /**
                * Return the underlying QueryMap.
                *
                * @return the underlying QueryMap
                */
                virtual QueryMap::View getQueryMap() const;

                /**
                * Return the underlying QueryMap.
                *
                * @return the underlying QueryMap
                */
                virtual QueryMap::Handle getQueryMap();

            // ---- data members ----------------------------------------

            protected:
                /**
                * A Converter ConcurrentMap around the underlying NamedCache.
                */
                FinalHolder<ConverterConcurrentMap> f_ohMapConcurrent;

                /**
                * A Converter InvocableMap around the underlying NamedCache.
                */
                FinalHolder<ConverterInvocableMap> f_ohMapInvocable;

                /**
                * A Converter QueryMap around the underlying NamedCache.
                */
                FinalHolder<ConverterQueryMap> f_ohMapQuery;
            };


        // ----- inner class: UnsupportedConverter --------------------------

        /**
        * Converter which always throws an UnsupportedOperationException.
        */
        class COH_EXPORT UnsupportedConverter
            : public class_spec<UnsupportedConverter,
                extends<Object>,
                implements<Converter> >
            {
            friend class factory<UnsupportedConverter>;

            // ----- constructor ----------------------------------------

            public:
                /**
                * Create an UnsupportedConverter
                *
                * return a handle to the new UnsupportedConverter
                */
                UnsupportedConverter();

            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                *
                * @throws UnsupportedOperationException always
                */
                virtual Object::Holder convert(Object::Holder oh) const;

                /**
                * Return a singleton instance of the UnsupportedConverter.
                */
                static UnsupportedConverter::View getInstance();
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONVERTER_COLLECTIONS_HPP
