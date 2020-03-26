/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NULL_IMPLEMENTATION_HPP
#define COH_NULL_IMPLEMENTATION_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/AbstractSet.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::AbstractSet;
using coherence::util::MapListener;
using coherence::util::ObservableMap;
using coherence::util::ValueExtractor;

/**
* A collection of classes that do nothing.
* For each class implemented, a factory method will exist following the
* naming convention "get" plus the class or interface name.
*
* @author gm  2008.07.28
*/
class COH_EXPORT NullImplementation
    {
    // ----- NullImplementation interface -----------------------------------

    public:
        /**
        * Returns an instance of the empty ObjectArray.
        */
        static ObjectArray::Handle getArray();

        /**
        * Factory method:  Obtain a null implementation of a Converter.
        *
        * @return a conforming implementation of Converter that does as little
        *         as possible
        *
        * @since Coherence 3.7.1
        */
        static Converter::Handle getConverter();

        /**
        * Returns an instance of the NullIterator.
        */
        static Iterator::Handle getIterator();

        /**
        * Returns an instance of the NullMuterator.
        */
        static Muterator::Handle getMuterator();

        /**
        * Returns an instance of the NullSet.
        */
        static Set::Handle getSet();

        /**
        * Returns an instance of the NullCollection.
        */
        static Collection::Handle getCollection();

        /**
        * Returns an instance of the NullMap.
        */
        static Map::Handle getMap();

        /**
        * Returns an instance of the NullObservableMap.
        */
        static ObservableMap::Handle getObservableMap();

        /**
        * Returns an instance of the NullValueExtractor.
        */
        static ValueExtractor::Handle getValueExtractor();


    // ----- Inner class: NullConverter -------------------------------------

    public:
        class COH_EXPORT NullConverter
            : public class_spec<NullConverter,
                     extends<Object>,
                     implements<Converter> >
            {
            friend class factory<NullImplementation::NullConverter>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Default constructor.
                */
                NullConverter();


            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder convert(Object::Holder oh) const;

            public:
                static Handle getInstance();
            };


    // ----- Inner class: NullMuterator -------------------------------------

    public:
        class COH_EXPORT NullMuterator
            : public class_spec<NullMuterator,
                     extends<Object>,
                     implements<Muterator> >
            {
            friend class factory<NullImplementation::NullMuterator>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Default constructor.
                */
                NullMuterator();


            // ----- Iterator interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool hasNext() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder next();

                /**
                * {@inheritDoc}
                */
                virtual void remove();

            public:
                static Handle getInstance();
            };


    // ----- Inner class: NullSet -------------------------------------------
    public:
        class COH_EXPORT NullSet
            : public cloneable_spec<NullSet,
                     extends<AbstractSet>,
                     implements<PortableObject> >
            {
            friend class factory<NullImplementation::NullSet>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Default constructor (for PortableObject).
                */
                NullSet();

                /**
                * Copy constructor.
                */
                NullSet(const NullSet& that);


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

                /**
                * {@inheritDoc}
                */
                virtual size32_t size() const;

                /**
                * {@inheritDoc}
                */
                virtual bool isEmpty() const;

                /**
                * {@inheritDoc}
                */
                virtual void clear();


            // ----- PortableObject interface ---------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void readExternal(PofReader::Handle hIn);

                /**
                * {@inheritDoc}
                */
                virtual void writeExternal(PofWriter::Handle hOut) const;


            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
          virtual bool equals(Object::View v) const;

            public:
                static Handle getInstance();
            };

    // ----- Inner class: NullCollection ------------------------------------
    public:
        class COH_EXPORT NullCollection
            : public cloneable_spec<NullCollection,
                     extends<Object>,
                     implements<Collection, PortableObject> >
            {
            friend class factory<NullImplementation::NullCollection>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Default constructor (for PortableObject).
                */
                NullCollection();

                /**
                * Copy constructor.
                */
                NullCollection(const NullCollection& that);


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

                /**
                * {@inheritDoc}
                */
                virtual size32_t size() const;

                /**
                * {@inheritDoc}
                */
                virtual bool isEmpty() const;

                /**
                * {@inheritDoc}
                */
                virtual void clear();


            // ----- PortableObject interface ---------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void readExternal(PofReader::Handle hIn);

                /**
                * {@inheritDoc}
                */
                virtual void writeExternal(PofWriter::Handle hOut) const;


            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
          virtual bool equals(Object::View v) const;

            public:
                static Handle getInstance();
            };

    // ----- Inner class: NullMap --------------------------------------------

    public:
        class COH_EXPORT NullMap
            : public cloneable_spec<NullMap,
                     extends<Object>,
                     implements<Map, PortableObject> >
            {
            friend class factory<NullImplementation::NullMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Default constructor (for PortableObject).
                */
                NullMap();

                /**
                * Copy constructor.
                */
                NullMap(const NullMap& that);


            // ----- Map interface ---------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual size32_t size() const;

                /**
                * {@inheritDoc}
                */
                virtual bool isEmpty() const;

                /**
                * {@inheritDoc}
                */
                virtual void clear();

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
                virtual Object::Holder get(Object::View vKey);

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


            // ----- PortableObject interface ---------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void readExternal(PofReader::Handle hIn);

                /**
                * {@inheritDoc}
                */
                virtual void writeExternal(PofWriter::Handle hOut) const;


            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
          virtual bool equals(Object::View v) const;

            public:
                static Handle getInstance();
            };


    // ----- Inner class: NullObservableMap ---------------------------------

    public:
        class COH_EXPORT NullObservableMap
            : public cloneable_spec<NullObservableMap,
                     extends<NullMap>,
                     implements<ObservableMap> >
            {
            friend class factory<NullImplementation::NullObservableMap>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Default constructor (for PortableObject).
                */
                NullObservableMap();

                /**
                * Copy constructor.
                */
                NullObservableMap(const NullObservableMap& that);


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
                virtual void removeFilterListener(MapListener::Handle hListener,
                        Filter::View vFilter);

            public:
                static Handle getInstance();
            };


    // ----- Inner class: NullValueExtractor --------------------------------

    public:
        class COH_EXPORT NullValueExtractor
            : public class_spec<NullValueExtractor,
                     extends<Object>,
                     implements<ValueExtractor> >
            {
            friend class factory<NullImplementation::NullValueExtractor>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Default constructor (for PortableObject).
                */
                NullValueExtractor();


            // ----- ValueExtractor interface ---------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder extract(Object::Holder ohTarget) const;


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


            // ----- PortableObject interface ---------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void readExternal(PofReader::Handle hIn);

                /**
                * {@inheritDoc}
                */
                virtual void writeExternal(PofWriter::Handle hOut) const;

            public:
                static Handle getInstance();
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NULL_IMPLEMENTATION_HPP
