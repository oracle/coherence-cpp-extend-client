/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_COLLECTIONS_HPP
#define COH_TYPED_COLLECTIONS_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/ConverterCollections.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/WrapperCollections.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A collection of wrapper classes which expose an underlying collection's
* contents as explicit types rather then just as Objects.  Essentially these
* are auto-casting collections.
*
* Each typed collection is templated based on one or more handle types
* to a const coherence::lang::Object Object. For instance:
* <tt>TypedCollection<String::View></tt> or
* <tt>TypedMap<String::View, Integer32::View></tt>
*
* Alternate handle types may also be used for added functionality,
* for example: <tt>TypedMap<String::View, BoxHandle<const Integer32> ></tt>
* produces a TypedMap which will auto-box the supplied values to Integer32,
* making statements such as <tt>hMap->put("abc", 456)</tt> possible.
*
* These typed wrappers only provide a convenience over the underlying
* collection. Thus they cannot guarantee that external usage of the
* the underlying collection will not violate the restrictions imposed by the
* wrapper. For instance Objects of a type other then those specified by
* this template may exist within the collection.  Any retrieval of those
* elements from the wrapper will result in ClassCastException.
* External usage is possible anytime the typed wrapper is passed by its
* inherited collection interface, i.e. passing a TypedMap as a plain Map.
*
* Member functions which take no input parameters cannot have their return
* type changed. Instead there will be an auxiliary function with the same name
* plus a "Typed" suffix, e.g. TypedMap::keySetTyped().
*
* Example usage:
* @code
* typedef TypedCollections::TypedMap<String::View, Integer32::Handle> StringIntMap;
* StringIntMap::Handle hMap = StringIntMap::create(HashMap::create());
*
* hMap->put("abc", Integer32::create(123));  // String's auto-boxing is utilized
* Integer32::Handle hInt = hMap->get("abc"); // no cast of return type required
* @endcode
*
* @author mf  2007.07.05
*/
class COH_EXPORT TypedCollections
    {
    // ----- inner class: TypedIterator -------------------------------------

    public:
        /**
        * Typed wrapper over Iterator interface.
        */
        template<class T>
        class TypedIterator
            : public cloneable_spec<TypedIterator<T>,
                extends<WrapperCollections::AbstractWrapperIterator> >
            {
            friend class factory<TypedIterator<T> >;
            typedef typename TypedIterator::super super;

            // ----- typedefs -------------------------------------------

            public:
                /**
                * The type of the handles returned by this iterator,
                * e.g. String::View.
                */
                typedef T ValueType;


            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new TypedIterator over the given Iterator.
                *
                * @param hIterDelegate a general "Object" Iterator to wrap
                *        around
                */
                TypedIterator(Iterator::Handle hIterDelegate)
                    : super(hIterDelegate)
                    {
                    }

                /**
                * Copy constructor
                */
                TypedIterator(const TypedIterator& that)
                    : super(that)
                    {
                    }

            // ----- TypedIterator interface ----------------------------

            public:
                /**
                * The typed version of the method Iterator::next. Return the
                * next element in the iteration.
                *
                * @return the next element in the iteration.
                *
                * @throws coherence::lang::NoSuchElementException
                *         if iteration has no more elements.
                * @throws ClassCastException
                *         if the typed collection is broken by external usage
                */
                virtual ValueType nextTyped()
                    {
                    return cast<ValueType>(super::next());
                    }
            };


    // ----- inner class: TypedListIterator ---------------------------------

    public:
        /**
        * Typed wrapper over ListIterator interface.
        */
        template<class T>
        class TypedListIterator
            : public cloneable_spec<TypedListIterator<T>,
                extends<WrapperCollections::AbstractWrapperListIterator> >
            {
            friend class factory<TypedListIterator<T> >;
            typedef typename TypedListIterator::super super;

            // ----- typedefs -------------------------------------------

            public:
                /**
                * The type of the handles returned by this iterator,
                * e.g. String::View.
                */
                typedef T ValueType;


            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new TypedListIterator over the given ListIterator.
                *
                * @param hIterDelegate a general "Object" Iterator to wrap
                *        around
                */
                TypedListIterator(ListIterator::Handle hIterDelegate)
                    : super(hIterDelegate)
                    {
                    }

                /**
                * Copy constructor.
                */
                TypedListIterator(const TypedListIterator& that)
                    : super(that)
                    {
                    }


            // ----- TypedIterator interface ----------------------------

            public:
                /**
                * The typed version of the method Iterator::next. Return the
                * next element in the iteration.
                *
                * @return the next element in the iteration.
                *
                * @throws coherence::lang::NoSuchElementException
                *         if iteration has no more elements.
                * @throws ClassCastException
                *         if the typed collection is broken by external usage
                */
                virtual ValueType nextTyped()
                    {
                    return cast<ValueType>(super::next());
                    }


            // ----- ListIterator interface -----------------------------

            public:
                /**
                * The typed version of the method ListIterator::previous.
                * <p> Return the previous element in the iteration.
                *
                * @return the previous element in the iteration.
                *
                * @throws coherence::lang::NoSuchElementException
                *         if iteration has no previous elements.
                * @throws ClassCastException
                *         if the typed collection is broken by external usage
                */
                virtual ValueType previousTyped()
                    {
                    return cast<ValueType>(super::previous());
                    }


            // ----- ListMuterator interface ----------------------------

            public:
                /**
                * The typed version of the method ListIterator::add.
                * <p>
                * Insert the specified element immediately before the element
                * to be returned from the next call to the next() method. A
                * subsequent call to next() will not return the added
                * element, while a call to previous() would return the added
                * element.
                *
                * @param v  the element to add
                *
                * @throws UnsupportedOperationException if addition is not
                *         supported
                */
                virtual void add(ValueType v)
                    {
                    super::add(v);
                    }

                using super::add;

                /**
                * The typed version of the method ListIterator::set.
                * <p>
                * Replace the last element returned by next() or previous() with
                * the specified element.
                *
                * @param oh  the element to store
                *
                * @throws UnsupportedOperationException if set is not supported
                */
                virtual void set(ValueType v)
                    {
                    super::set(v);
                    }

                using super::set;
            };


        // ---- inner class: TypedCollection --------------------------------

        /**
        * Typed wrapper over Collection interface.
        */
        template<class T>
        class TypedCollection
            : public cloneable_spec<TypedCollection<T>,
                extends<WrapperCollections::AbstractWrapperCollection> >
            {
            friend class factory<TypedCollection<T> >;
            typedef typename TypedCollection::this_spec super;

            // ----- typedefs -------------------------------------------

            public:
                /**
                * The type of the objects stored in this collection, that
                * e.g. String::View.
                */
                typedef T ValueType;

                /**
                * The type of the iterator over this collection.
                */
                typedef TypedIterator<T> Iterator;


            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new TypedCollection over the given Collection.
                *
                * @param ohCollDelegate  the Collection to delegate to
                */
                TypedCollection(Collection::Holder ohCollDelegate)
                    : super(ohCollDelegate)
                    {
                    }

                /**
                * Copy constructor.
                */
                TypedCollection(const TypedCollection& that)
                    : super(that)
                    {
                    }


            // ----- TypedCollection interface --------------------------

            public:
                /**
                * The typed version of the method Collection::iterator.
                * Return a handle to an TypedIterator over the elements in
                * this collection.
                *
                * @return a TypedIterator over the elements in this
                *         collection
                */
                virtual typename TypedCollection::Iterator::Handle iteratorTyped() const
                    {
                    return TypedCollection::Iterator::create(super::iterator());
                    }

                /**
                * The typed version of the method Collection::iterator.
                * Return a handle to an TypedIterator over the elements in
                * this collection.
                *
                * @return a TypedIterator over the elements in this
                *         collection
                */
                virtual typename TypedCollection::Iterator::Handle iteratorTyped()
                    {
                    return TypedCollection::Iterator::create(super::iterator());
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::Iterator::Handle iterator() const
                    {
                    return iteratorTyped();
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::Muterator::Handle iterator()
                    {
                    return iteratorTyped();
                    }

                /**
                * {@inheritDoc}
                */
                using super::contains;

                /**
                * The typed version of the method Collection::contains.
                *
                * Return true if this collection contains the specified
                * element.
                *
                * @param v element whose presence in this collection is to
                *          be tested
                *
                * @return true if this collection contains the specified
                *         element
                */
                virtual bool contains(ValueType v) const
                    {
                    return super::contains(v);
                    }

                /**
                * {@inheritDoc}
                */
                using super::add;

                /**
                * The typed version of the method Collection::add. Ensure
                * that this collection contains the specified element.
                *
                * @param v element whose presence in this collection is to be
                *          ensured
                *
                * @return true if this collection changed as a result of the
                *         call
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         collection
                */
                virtual bool add(ValueType v)
                    {
                    return super::add(v);
                    }

                /**
                * {@inheritDoc}
                */
                using super::remove;

                /**
                * The typed version of the method Collection::remove. Remove
                * a single instance of the specified element from this
                * collection, if it is present.
                *
                * @param v element to be removed from this collection, if
                *          present
                *
                * @return true if this collection changed as a result of
                *         the call
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         collection
                */
                virtual bool remove(ValueType v)
                    {
                    return super::remove(v);
                    }

                /**
                * {@inheritDoc}
                */
                using super::addAll;

                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified collection to this
                * collection.
                *
                * @param vtc TypedCollection of elements to be inserted into
                *            this collection
                *
                * @return true if this collection changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         collection
                */
                virtual bool addAll(typename TypedCollection::View vtc)
                    {
                    return super::addAll(vtc);
                    }

                /**
                * {@inheritDoc}
                */
                using super::removeAll;

                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this collection's elements that are also
                * contained in the specified typed collection.
                *
                * @param vtc elements to be removed from this collection
                *
                * @return true if this collection changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         collection
                */
                virtual bool removeAll(typename TypedCollection::View vtc)
                    {
                    return super::removeAll(vtc);
                    }

                /**
                * {@inheritDoc}
                */
                using super::retainAll;

                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this collection that are
                * contained in the specified typed collection.
                *
                * @param vtc elements to be retained in this collection
                *
                * @return true if this collection changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         collection
                */
                virtual bool retainAll(typename TypedCollection::View vtc)
                    {
                    return super::retainAll(vtc);
                    }
            };


        // ----- inner class: TypedSet --------------------------------------

        /**
        * Typed wrapper over Set interface.
        */
        template<class T>
        class TypedSet
            : public cloneable_spec<TypedSet<T>,
                extends<WrapperCollections::AbstractWrapperSet> >
            {
            friend class factory<TypedSet<T> >;
            typedef typename TypedSet::super super;

            // ----- typedefs -------------------------------------------

            public:
                /**
                * The type of the objects stored in this set, that
                * e.g. String::View.
                */
                typedef T ValueType;

                /**
                * The type of the iterator over this set.
                */
                typedef TypedIterator<T> Iterator;


            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new TypedSet over the given Set.
                *
                * @param ohSetDelegate  the Set to delegate to
                */
                TypedSet(Set::Holder ohSetDelegate)
                    : super(ohSetDelegate)
                    {
                    }

                /**
                * Copy constructor.
                */
                TypedSet(const TypedSet& that)
                    : super(that)
                    {
                    }


            // ----- TypedSet interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                using super::contains;

                /**
                * The typed version of the method Collection::contains.
                * Return true if this set contains the specified element.
                *
                * @param v element whose presence in this set is to be tested
                *
                * @return true if this collection contains the specified
                *         element
                */
                virtual bool contains(ValueType v) const
                    {
                    return super::contains(v);
                    }

                /**
                * The typed version of the method Collection::iterator.
                * Return a handle to an TypedIterator over the elements in
                * this set.
                *
                * @return a TypedIterator over the elements in this
                *         set
                */
                virtual typename TypedSet::Iterator::Handle iteratorTyped() const
                    {
                    return TypedSet::Iterator::create(super::iterator());
                    }

                /**
                * The typed version of the method Collection::iterator.
                * Return a handle to an TypedIterator over the elements in
                * this set.
                *
                * @return a TypedIterator over the elements in this
                *         set
                */
                virtual typename TypedSet::Iterator::Handle iteratorTyped()
                    {
                    return TypedSet::Iterator::create(super::iterator());
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::Iterator::Handle iterator() const
                    {
                    return iteratorTyped();
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::Muterator::Handle iterator()
                    {
                    return iteratorTyped();
                    }

                /**
                * {@inheritDoc}
                */
                using super::add;

                /**
                * The typed version of the method Set::add. Add the specified
                * element to this set if it is not already present.
                *
                * @param v element to be added to this set.
                *
                * @return true if this set changed as a result of the
                *         call
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         set
                */
                virtual bool add(ValueType v)
                    {
                    return super::add(v);
                    }

                /**
                * {@inheritDoc}
                */
                using super::remove;

                /**
                * The typed version of the method Collection::remove. Remove
                * the specified element from this set, if it is present.
                *
                * @param v element to be removed from this set, if present
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool remove(ValueType v)
                    {
                    return super::remove(v);
                    }

                /**
                * {@inheritDoc}
                */
                using super::addAll;

#if (!defined(_MSC_VER) || _MSC_VER==1400) // COH-3357 - works on all compilers but VS2008+
                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedCollection to
                * this set.
                *
                * @param vtc TypedCollection of elements to be inserted into
                *            this set
                *
                * @return true if this set changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this set
                */
                virtual bool addAll(typename
                                TypedCollection<ValueType>::View vtc)
                    {
                    return super::addAll(vtc);
                    }

                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedCollection to
                * this set.
                *
                * @param htc TypedCollection of elements to be inserted into
                *            this set
                *
                * @return true if this set changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this set
                */
                virtual bool addAll(typename
                                TypedCollection<ValueType>::Handle htc)
                    {
                    return super::addAll(htc);
                    }
#endif

                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedSet to this
                * set.
                *
                * @param vts TypedSet of elements to be inserted into
                *            this set
                *
                * @return true if this set changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this set
                */
                virtual bool addAll(typename TypedSet<ValueType>::View vts)
                    {
                    return super::addAll(vts);
                    }

                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedSet to this
                * set.
                *
                * @param hts TypedSet of elements to be inserted into
                *            this set
                *
                * @return true if this set changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this set
                */
                virtual bool addAll(typename TypedSet<ValueType>::Handle hts)
                    {
                    return super::addAll(hts);
                    }

                /**
                * {@inheritDoc}
                */
                using super::removeAll;

#if (!defined(_MSC_VER) || _MSC_VER==1400) // COH-3357 - works on all compilers but VS2008+
                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this set's elements that are also contained in
                * the specified typed collection.
                *
                * @param vtc elements to be removed from this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool removeAll(typename
                                TypedCollection<ValueType>::View vtc)
                    {
                    return super::removeAll(vtc);
                    }

                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this set's elements that are also contained in
                * the specified typed collection.
                *
                * @param htc elements to be removed from this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool removeAll(typename
                                TypedCollection<ValueType>::Handle htc)
                    {
                    return super::removeAll(htc);
                    }
#endif

                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this set's elements that are also contained in
                * the specified typed set.
                *
                * @param vts elements to be removed from this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool removeAll(typename
                                TypedSet<ValueType>::View vts)
                    {
                    return super::removeAll(vts);
                    }

                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this set's elements that are also contained in
                * the specified typed set.
                *
                * @param hts elements to be removed from this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool removeAll(typename
                                TypedSet<ValueType>::Handle hts)
                    {
                    return super::removeAll(hts);
                    }

                /**
                * {@inheritDoc}
                */
                using super::retainAll;

#if (!defined(_MSC_VER) || _MSC_VER==1400) // COH-3357 - works on all compilers but VS2008+
                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this set that are
                * contained in the specified typed collection.
                *
                * @param vtc elements to be retained in this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool retainAll(typename
                                TypedCollection<ValueType>::View vtc)
                    {
                    return super::retainAll(vtc);
                    }

                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this set that are
                * contained in the specified typed collection.
                *
                * @param htc elements to be retained in this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool retainAll(typename
                                TypedCollection<ValueType>::Handle htc)
                    {
                    return super::retainAll(htc);
                    }
#endif

                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this set that are
                * contained in the specified typed set.
                *
                * @param vts elements to be retained in this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool retainAll(typename
                                TypedSet<ValueType>::View vts)
                    {
                    return super::retainAll(vts);
                    }

                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this set that are
                * contained in the specified typed set.
                *
                * @param hts elements to be retained in this set
                *
                * @return true if this set changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         set
                */
                virtual bool retainAll(typename
                                TypedSet<ValueType>::Handle hts)
                    {
                    return super::retainAll(hts);
                    }

                /**
                * Convert a TypedSet to a TypedCollection, wrapping the same
                * underlying Collection::Handle.
                *
                * @return a handle to the new TypedCollection
                */
                virtual typename TypedCollection<ValueType>::Handle
                    asTypedCollection()
                    {
                    return TypedCollection<ValueType>::create(
                        cast<Collection::Handle>(super::getDelegate()));
                    }

                /**
                * Convert a TypedSet to a TypedCollection, wrapping the same
                * underlying Collection::View.
                *
                * @return a view to the new TypedCollection
                */
                virtual typename TypedCollection<ValueType>::View
                    asTypedCollection() const
                    {
                    return TypedCollection<ValueType>::create(
                        cast<Collection::View>(super::getDelegate()));
                    }
            };


        // ----- inner class: TypedEntry ------------------------------------

        /**
        * Typed wrapper over Map::Entry interface. Both key and value objects
        * are typed. The types of key and value may differ.
        */
        template<class K, class V>
        class TypedEntry
            : public cloneable_spec<TypedEntry<K, V>,
                extends<WrapperCollections::AbstractWrapperEntry> >
            {
            friend class factory<TypedEntry<K, V> >;
            typedef typename TypedEntry::this_spec super;

            // ----- typedefs -------------------------------------------

            public:
                /**
                * The type of the keys, e.g. String::View.
                */
                typedef K KeyType;

                /**
                * The type the values, e.g. String::View.
                */
                typedef V ValueType;


            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new TypedEntry over the given Map::Entry.
                *
                *
                * @param ohEntryDelegate  the Entry to delegate to
                */
                TypedEntry(Map::Entry::Holder ohEntryDelegate)
                    : super(ohEntryDelegate)
                    {
                    }

                /**
                * Copy constructor.
                */
                TypedEntry(const TypedEntry& that)
                    : super(that)
                    {
                    }


            // ----- TypedEntry interface -------------------------------

            public:
                /**
                * The typed version of the method Map::Entry::getKey.
                *
                * Return the key corresponding to this entry.
                *
                * @return the key corresponding to this entry.
                *
                * @throws ClassCastException
                *         if the typed entry is broken by external usage
                */
                virtual KeyType getKeyTyped() const
                    {
                    return cast<KeyType>(super::getKey());
                    }

                /**
                * The typed version of the method Map::Entry::getKey.
                *
                * Return the value corresponding to this entry.
                *
                * @return the value corresponding to this entry.
                *
                * @throws ClassCastException
                *         if the typed entry is broken by external usage
                */
                virtual ValueType getValueTyped() const
                    {
                    return cast<ValueType>(super::getValue());
                    }

                /**
                * The typed version of the method Map::Entry::getKey.
                *
                * Return the value corresponding to this entry.
                *
                * @return the value corresponding to this entry.
                *
                * @throws ClassCastException
                *         if the typed entry is broken by external usage
                */
                virtual ValueType getValueTyped()
                    {
                    return cast<ValueType>(super::getValue());
                    }

                /**
                * {@inheritDoc}
                */
                using super::setValue;

                /**
                * Update the value of the map entry.
                *
                * @param ohValue  the new value for the entry
                *
                * @return the prior value
                */
                virtual ValueType setValue(ValueType ohValue)
                    {
                    return cast<ValueType>(super::setValue(ohValue));
                    }
            };


        // ----- inner class: TypedMap --------------------------------------

        /**
        * Typed wrapper over Map interface.
        */
        template<class K, class V>
        class TypedMap
            : public cloneable_spec<TypedMap<K, V>,
                extends<WrapperCollections::AbstractWrapperMap> >
            {
            friend class factory<TypedMap<K, V> >;
            typedef typename TypedMap::this_spec super;

            // ----- typedefs -------------------------------------------

            public:
                /**
                * The type of the map's keys, e.g. String::View.
                */
                typedef K KeyType;

                /**
                * The type of the map's values, e.g. String::View.
                */
                typedef V ValueType;

                /**
                * The type of the entries stored in this map.
                */
                typedef TypedEntry<KeyType, ValueType> Entry;

                /**
                * The type of the key set associated with this map.
                */
                typedef TypedSet<KeyType> KeySet;

                /**
                * The type of the values collection associated with this map.
                */
                typedef TypedCollection<ValueType> ValueCollection;

                /**
                * The type of the entry set associated with this map.
                */
                typedef TypedSet<typename Entry::View> EntrySet;


            // ----- inner class: EntryConverter ------------------------

            protected:
                /**
                * A converter from untyped to typed entries:
                * Map::Entry -> TypedEntry
                */
                class EntryConverter
                    : public class_spec<EntryConverter,
                        extends<Object>,
                        implements<Converter> >
                    {
                    friend class factory<EntryConverter>;

                    // ----- constructors ---------------------------

                    protected:
                        /**
                        * Create new EntryConverter instance.
                        */
                        EntryConverter()
                            {
                            }

                        /**
                        * Copy constructor.
                        */
                        EntryConverter(const EntryConverter&)
                            {
                            }


                    // ----- Converter interface --------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual Object::Holder convert(Object::Holder oh) const
                            {
                            typename Entry::View vEntry =
                                    cast<typename Entry::View>(oh, false);
                            if (vEntry == NULL)
                                {
                                // conversion from Map::Entry to
                                // TypedMap::Entry required
                                // maintain Object::Holder view/handle
                                Map::Entry::Handle h =
                                        cast<Map::Entry::Handle>(oh, false);
                                if (h == NULL)
                                    {
                                    return Entry::create(
                                            cast<Map::Entry::View>(oh));
                                    }
                                return Entry::create(h);
                                }
                            else
                                {
                                // no conversion necessary
                                return oh;
                                }
                            }

                    // ----- Object interface -----------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual bool isImmutable() const
                            {
                            return true;
                            }
                    };

            // ----- constructors -----------------------------------

            protected:
                /**
                * Create a new TypedMap over the given Map.
                *
                * @param ohMapDelegate  the Map to delegate to
                */
                TypedMap(Map::Holder ohMapDelegate)
                    : super(ohMapDelegate),
                      f_vConverter(Object::self(), EntryConverter::create())
                    {
                    }

                /**
                * Copy constructor.
                */
                TypedMap(const TypedMap& that)
                    : super(that),
                      f_vConverter(Object::self(), EntryConverter::create())
                    {
                    }


            // ----- TypedMap interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                using super::containsKey;

                /**
                * The typed version of the method Map::containsKey.
                *
                * Return <tt>true</tt> if this map contains a mapping for the
                * specified key.
                *
                * @param vKey key whose presence in this map is to be tested.
                *
                * @return <tt>true</tt> if this map contains a mapping for
                *         the specified key.
                */
                virtual bool containsKey(KeyType vKey) const
                    {
                    return super::containsKey(vKey);
                    }

                /**
                * {@inheritDoc}
                */
                using super::containsValue;

                /**
                * The typed version of the method Map::containsValue.
                *
                * Return <tt>true</tt> if this map maps one or more keys to
                * the specified value.
                *
                * @param ohValue value whose presence in this map is to be
                *               tested.
                *
                * @return <tt>true</tt> if this map maps one or more keys to
                *          the specified value.
                */
                virtual bool containsValue(ValueType ohValue) const
                    {
                    return super::containsValue(ohValue);
                    }

                /**
                * {@inheritDoc}
                */
                using super::get;

                /**
                * The typed version of the method Map::get.
                *
                * Return the value to which this map maps the specified key,
                * if any.
                *
                * @param vKey key whose associated value is to be returned.
                *
                * @return the value to which this map maps the specified key,
                *          or <tt>NULL</tt> if the map contains no mapping
                *          for this key.
                *
                * @throws ClassCastException
                *         if the typed map is broken by external usage
                */
                virtual ValueType get(KeyType vKey) const
                    {
                    return cast<ValueType>(super::get(vKey));
                    }

                /**
                * The typed version of the method Map::get.
                *
                * Return the value to which this map maps the specified key,
                * if any.
                *
                * @param vKey key whose associated value is to be returned.
                *
                * @return the value to which this map maps the specified key,
                *          or <tt>NULL</tt> if the map contains no mapping
                *          for this key.
                *
                * @throws ClassCastException
                *         if the typed map is broken by external usage
                */
                virtual ValueType get(KeyType vKey)
                    {
                    return cast<ValueType>(super::get(vKey));
                    }

                /**
                * {@inheritDoc}
                */
                using super::put;

                /**
                * The typed version of the method Map::put.
                *
                * Associate the specified value with the specified key in
                * this map.
                *
                * @param vKey   key with which the specified value is to be
                *               associated.
                * @param ohValue value to be associated with the specified
                *               key.
                *
                * @return previous value associated with specified key, or
                *         <tt>NULL</tt> if there was no mapping for key.  A
                *         <tt>NULL</tt> return can also indicate that the map
                *         previously associated <tt>NULL</tt> with the
                *         specified key.
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #put() operation is not supported by this map
                * @throws ClassCastException
                *         if the typed map is broken by external usage
                */
                virtual ValueType put(KeyType vKey, ValueType ohValue)
                    {
                    return cast<ValueType>(super::put(vKey, ohValue));
                    }

                /**
                * {@inheritDoc}
                */
                using super::remove;

                /**
                * The typed version of the method Map::put.
                *
                * Remove the mapping for this key from this map if it
                * is present.
                *
                * @param vKey key whose mapping is to be removed from
                *             the map.
                *
                * @return previous value associated with specified key, or
                *         <tt>NULL</tt> if there was no mapping for key.
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         map
                * @throws ClassCastException
                *         if the typed map is broken by external usage
                */
                virtual ValueType remove(KeyType vKey)
                    {
                    return cast<ValueType>(super::remove(vKey));
                    }

                /**
                * {@inheritDoc}
                */
                using super::putAll;

                /**
                * The typed version of the method Map::putAll.
                *
                * Copy all of the mappings from the specified typed map
                * to this map.
                *
                * @param vMap mappings to be stored in this map.
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #put() operation is not supported by this
                *         map
                */
                virtual void putAll(typename TypedMap::View vMap)
                    {
                    super::putAll(vMap);
                    }

                /**
                * The typed version of the method Map::keySet.
                *
                * Return a typed set view of the keys contained in this map.
                *
                * @return a typed set view of the keys contained in this map.
                */
                virtual typename KeySet::View keySetTyped() const
                    {
                    return KeySet::create(super::keySet());
                    }

                /**
                * The typed version of the method Map::keySet.
                *
                * Return a typed set view of the keys contained in this map.
                *
                * @return a typed set view of the keys contained in this map.
                */
                virtual typename KeySet::Handle keySetTyped()
                    {
                    return KeySet::create(super::keySet());
                    }

                /**
                * {@inheritDoc}
                */
                virtual Set::View keySet() const
                    {
                    return keySetTyped();
                    }

                /**
                * {@inheritDoc}
                */
                virtual Set::Handle keySet()
                    {
                    return keySetTyped();
                    }

                /**
                * The typed version of the method Map::values.
                *
                * Return a typed collection view of the values contained in
                * this map.
                *
                * @return a typed collection view of the values contained in
                *         this map.
                */
                virtual typename ValueCollection::View valuesTyped() const
                    {
                    return ValueCollection::create(super::values());
                    }

                /**
                * The typed version of the method Map::values.
                *
                * Return a typed collection view of the values contained in
                * this map.
                *
                * @return a typed collection view of the values contained in
                *         this map.
                */
                virtual typename ValueCollection::Handle valuesTyped()
                    {
                    return ValueCollection::create(super::values());
                    }

                /**
                * {@inheritDoc}
                */
                virtual Collection::View values() const
                    {
                    return valuesTyped();
                    }

                /**
                * {@inheritDoc}
                */
                virtual Collection::Handle values()
                    {
                    return valuesTyped();
                    }

                /**
                * The typed version of the method Map::entrySet.
                *
                * Return a typed set view of the mappings contained in
                * this map.
                *
                * @return a typed set view of the mappings contained in
                *         this map.
                */
                virtual typename EntrySet::View entrySetTyped() const
                    {
                    return EntrySet::create(cast<Set::View>(
                        ConverterCollections::ConverterSet::create(
                            super::entrySet(), f_vConverter,
                            (Converter::View) NULL)));
                    }

                /**
                * The typed version of the method Map::entrySet.
                *
                * Return a typed set view of the mappings contained in
                * this map.
                *
                * @return a typed set view of the mappings contained in
                *         this map.
                */
                virtual typename EntrySet::Handle entrySetTyped()
                    {
                    return EntrySet::create(cast<Set::Handle>(
                        ConverterCollections::ConverterSet::create(
                            super::entrySet(), f_vConverter,
                            (Converter::View) NULL)));
                    }

                /**
                * {@inheritDoc}
                */
                virtual Set::View entrySet() const
                    {
                    return entrySetTyped();
                    }

                /**
                * {@inheritDoc}
                */
                virtual Set::Handle entrySet()
                    {
                    return entrySetTyped();
                    }


            // ----- data members ---------------------------------------

            protected:
                /**
                * The converter used to convert untyped entries to typed ones
                */
                FinalView<EntryConverter> f_vConverter;
            };


        // ----- inner class: TypedList -------------------------------------

        /**
        * Typed wrapper over List interface.
        */
        template<class T>
        class TypedList
            : public cloneable_spec<TypedList<T>,
                extends<WrapperCollections::AbstractWrapperList> >
            {
            friend class factory<TypedList<T> >;
            typedef typename TypedList::super super;

            // ----- typedefs -------------------------------------------

            public:
                /**
                * The type of the objects stored in this list,
                * e.g. String::View.
                */
                typedef T ValueType;

                /**
                * The type of the iterator over this list.
                */
                typedef TypedIterator<ValueType> Iterator;

                /**
                * The type of the list iterator over this list.
                */
                typedef TypedListIterator<ValueType> ListIterator;


            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new TypedList over the given List.
                *
                * @param ohListDelegate  the list to delegate to
                */
                TypedList(List::Holder ohListDelegate)
                    : super(ohListDelegate)
                    {
                    }

                /**
                * Copy constructor.
                */
                TypedList(const TypedList& that)
                    : super(that)
                    {
                    }


            // ----- TypedList interface --------------------------------

            public:
                /**
                * The typed version of the method List::get.
                * <p>
                * Return the element from the specified position in the list.
                *
                * @param i  the position of the item to return
                *
                * @return the element from the specified position in the list
                *
                * @throws coherence::lang::IndexOutOfBoundsException if the
                *         index is out of range
                *
                * @throws ClassCastException
                *         if the typed map is broken by external usage
                */
                virtual ValueType getTyped(size32_t i) const
                    {
                    return cast<ValueType>(super::get(i));
                    }

                /**
                * {@inheritDoc}
                */
                using super::contains;

                /**
                * The typed version of the method Collection::contains.
                * Return true if this list contains the specified element.
                *
                * @param v element whose presence in this list is to be
                *          tested
                *
                * @return true if this collection contains the specified
                *         element
                */
                virtual bool contains(ValueType v) const
                    {
                    return super::contains(v);
                    }

                /**
                * The typed version of the method Collection::iterator.
                * Return a handle to an TypedIterator over the elements in
                * this list.
                *
                * @return a TypedIterator over the elements in this list
                */
                virtual typename TypedList::Iterator::Handle
                        iteratorTyped() const
                    {
                    return TypedList::Iterator::create(
                            Iterator::create(super::iterator()));
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::Iterator::Handle iterator() const
                    {
                    return iteratorTyped();
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::Muterator::Handle iterator()
                    {
                    return iteratorTyped();
                    }

                /**
                * The typed version of the method List::listIterator.
                * <p>
                * Return a handle to an TypedListIterator over the elements
                * in this list.
                *
                * @return a TypedListIterator over the elements in this list
                */
                virtual typename TypedList::ListIterator::Handle
                        listIteratorTyped() const
                    {
                    return TypedList::ListIterator::create(
                            super::listIterator());
                    }

                /**
                * The typed version of the method List::listIterator.
                * <p>
                * Return a handle to an TypedListIterator over the elements
                * in this list.
                *
                * @return a TypedListIterator over the elements in this list
                */
                virtual typename TypedList::ListIterator::Handle
                        listIteratorTyped()
                    {
                    return TypedList::ListIterator::create(
                        super::listIterator());
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::ListIterator::Handle listIterator() const
                    {
                    return listIteratorTyped();
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::ListMuterator::Handle listIterator()
                    {
                    return listIteratorTyped();
                    }

                /**
                * The typed version of the method Collection::iterator.
                * <p> Return a handle to an TypedList Iterator over the
                * elements in this list starting at the position specified.
                *
                * @param i  the position in the list that the iterator will
                *           start at
                *
                * @return a TypedListIterator over the elements in this list
                *         starting at the specified position in the list
                */
                virtual typename TypedList::ListIterator::Handle
                        listIteratorTyped(size32_t i) const
                    {
                    return TypedList::ListIterator::create(
                        super::listIterator(i));
                    }

                /**
                * The typed version of the method Collection::iterator.
                * <p> Return a handle to an TypedList Iterator over the
                * elements in this list starting at the position specified.
                *
                * @param i  the position in the list that the iterator will
                *           start at
                *
                * @return a TypedListIterator over the elements in this list
                *         starting at the specified position in the list
                */
                virtual typename TypedList::ListIterator::Handle
                        listIteratorTyped(size32_t i)
                    {
                    return TypedList::ListIterator::create(
                        super::listIterator(i));
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::ListIterator::Handle
                        listIterator(size32_t i) const
                    {
                    return listIteratorTyped(i);
                    }

                /**
                * {@inheritDoc}
                */
                virtual coherence::util::ListMuterator::Handle
                        listIterator(size32_t i)
                    {
                    return listIteratorTyped(i);
                    }

                /**
                * {@inheritDoc}
                */
                using super::add;

                /**
                * The typed version of the method List::add. Add the
                * specified element to this list if it is not already
                * present.
                *
                * @param v element to be added to this list.
                *
                * @return true if this list changed as a result of the
                *         call
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         list
                */
                virtual bool add(ValueType v)
                    {
                    return super::add(v);
                    }

                /**
                * {@inheritDoc}
                */
                using super::set;

                /**
                * The typed version of the method List::set.
                * <p>
                * Replace the element at the specified position in this list
                * with the specified element.
                *
                * @param idx  the position in the list to replace
                * @param v    the object to replace at the specified position
                *
                * @return the element at the specified position prior to this
                *         operation
                *
                * @throw coherence::lang::IndexOutOfBoundsException if the
                *        index is out of range
                */
                virtual ValueType set(size32_t idx, ValueType v)
                    {
                    return cast<ValueType>(super::set(idx, v));
                    }

                /**
                * {@inheritDoc}
                */
                using super::remove;

                /**
                * The typed version of the method Collection::remove. Remove
                * the specified element from this list, if it is present.
                *
                * @param v element to be removed from this list, if present
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool remove(ValueType v)
                    {
                    return super::remove(v);
                    }

                /**
                * The typed version of the method Collection::remove.
                * <p> Remove the element at the specified position in the
                * list.
                *
                * @param idx  the index from which to remove an element
                *
                * @return the element at the specified position prior to this
                *         operation
                *
                * @throw coherence::lang::IndexOutOfBoundsException if the
                *        index is out of range
                */
                virtual ValueType removeTyped(size32_t idx)
                    {
                    return cast<ValueType>(super::remove(idx));
                    }

                /**
                * {@inheritDoc}
                */
                using super::addAll;

#if (!defined(_MSC_VER) || _MSC_VER==1400) // COH-3357 - works on all compilers but VS2008+
                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedCollection to
                * this list.
                *
                * @param vtc TypedCollection of elements to be inserted into
                *            this list
                *
                * @return true if this list changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         list
                */
                virtual bool addAll(typename
                                TypedCollection<ValueType>::View vtc)
                    {
                    return super::addAll(vtc);
                    }

                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedCollection to
                * this list.
                *
                * @param htc TypedCollection of elements to be inserted into
                *            this list
                *
                * @return true if this list changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         list
                */
                virtual bool addAll(typename
                                TypedCollection<ValueType>::Handle htc)
                    {
                    return super::addAll(htc);
                    }
#endif

                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedList to this
                * list.
                *
                * @param vtl TypedList of elements to be inserted into
                *            this list
                *
                * @return true if this list changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         list
                */
                virtual bool addAll(
                        typename TypedList<ValueType>::View vtl)
                    {
                    return super::addAll(vtl);
                    }

                /**
                * The typed version of the method Collection::addAll.
                *
                * Add all of the elements in the specified TypedList to this
                * list.
                *
                * @param htl TypedList of elements to be inserted into
                *            this list
                *
                * @return true if this list changed as a result of the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #add() operation is not supported by this
                *         list
                */
                virtual bool addAll(
                        typename TypedList<ValueType>::Handle htl)
                    {
                    return super::addAll(htl);
                    }

                /**
                * {@inheritDoc}
                */
                using super::removeAll;

#if (!defined(_MSC_VER) || _MSC_VER==1400) // COH-3357 - works on all compilers but VS2008+
                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this list's elements that are also contained in
                * the specified typed collection.
                *
                * @param vtc elements to be removed from this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool removeAll(typename
                                TypedCollection<ValueType>::View vtc)
                    {
                    return super::removeAll(vtc);
                    }

                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this list's elements that are also contained in
                * the specified typed collection.
                *
                * @param htc elements to be removed from this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool removeAll(typename
                                TypedCollection<ValueType>::Handle htc)
                    {
                    return super::removeAll(htc);
                    }
#endif

                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this list's elements that are also contained in
                * the specified typed list.
                *
                * @param vtl elements to be removed from this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool removeAll(typename
                                TypedList<ValueType>::View vtl)
                    {
                    return super::removeAll(vtl);
                    }

                /**
                * The typed version of the method Collection::removeAll.
                *
                * Remove all this list's elements that are also contained in
                * the specified typed list.
                *
                * @param htl elements to be removed from this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool removeAll(typename
                                TypedList<ValueType>::Handle htl)
                    {
                    return super::removeAll(htl);
                    }

                /**
                * {@inheritDoc}
                */
                using super::retainAll;

#if (!defined(_MSC_VER) || _MSC_VER==1400) // COH-3357 - works on all compilers but VS2008+
                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this list that are
                * contained in the specified typed collection.
                *
                * @param vtc elements to be retained in this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool retainAll(typename
                                TypedCollection<ValueType>::View vtc)
                    {
                    return super::retainAll(vtc);
                    }

                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this list that are
                * contained in the specified typed collection.
                *
                * @param htc elements to be retained in this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool retainAll(typename
                                TypedCollection<ValueType>::Handle htc)
                    {
                    return super::retainAll(htc);
                    }
#endif

                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this list that are
                * contained in the specified typed list.
                *
                * @param vtl elements to be retained in this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool retainAll(typename
                                TypedList<ValueType>::View vtl)
                    {
                    return super::retainAll(vtl);
                    }

                /**
                * The typed version of the method Collection::retainAll.
                *
                * Retain only the elements in this list that are
                * contained in the specified typed list.
                *
                * @param htl elements to be retained in this list
                *
                * @return true if this list changed as a result of
                *         the call
                *
                * @throws coherence::lang::NullPointerException
                *         if the specified collection is NULL
                * @throws coherence::lang::UnsupportedOperationException
                *         if the #remove() operation is not supported by this
                *         list
                */
                virtual bool retainAll(typename
                                TypedList<ValueType>::Handle htl)
                    {
                    return super::retainAll(htl);
                    }

                /**
                * Convert a TypedList to a TypedCollection, wrapping the same
                * underlying Collection::Handle.
                *
                * @return a handle to the new TypedCollection
                */
                virtual typename TypedCollection<ValueType>::Handle
                        asTypedCollection()
                    {
                    return TypedCollection<ValueType>::create(
                        cast<Collection::Handle>(super::getDelegate()));
                    }

                /**
                * Convert a TypedList to a TypedCollection, wrapping the same
                * underlying Collection::View.
                *
                * @return a view to the new TypedCollection
                */
                virtual typename TypedCollection<ValueType>::View
                        asTypedCollection() const
                    {
                    return TypedCollection<ValueType>::create(
                        cast<Collection::View>(super::getDelegate()));
                    }

                /**
                * The typed version of the method Collection::subList.
                * <p> Return a new typed list containing the contents of the
                * list between the specified fromIndex (inclusive) and
                * toIndex (exclusive).
                *
                * @param fromIndex  the start position in this list to create
                *                   the sublist from (inclusive).
                * @param toIndex    the end position in this list to end the
                *                   sublist at (exclusive).
                *
                * @return the new typed sublist of this typed list
                */
                virtual typename TypedList::Handle subListTyped(
                        size32_t fromIndex, size32_t toIndex)
                    {
                    return TypedList::create(super::subList(fromIndex, toIndex));
                    }

                /**
                * The typed version of the method Collection::subList.
                * <p> Return a new typed list containing the contents of the
                * list between the specified fromIndex (inclusive) and
                * toIndex (exclusive).
                *
                * @param fromIndex  the start position in this list to create
                *                   the sublist from (inclusive).
                * @param toIndex    the end position in this list to end the
                *                   sublist at (exclusive).
                *
                * @return the new typed sublist of this typed list
                */
                virtual typename TypedList::Handle subListTyped(
                        size32_t fromIndex, size32_t toIndex) const
                    {
                    return TypedList::create(super::subList(fromIndex, toIndex));
                    }
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TYPED_COLLECTIONS_HPP
