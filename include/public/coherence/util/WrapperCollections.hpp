/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WRAPPER_COLLECTIONS_HPP
#define COH_WRAPPER_COLLECTIONS_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/ListIterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/Set.hpp"

#include <cstddef>


COH_OPEN_NAMESPACE2(coherence,util)


/**
* A collection of abstract Collection implementation classes for wrapping
* Collection types.
*
* @author mf  2007.07.05
*/
class COH_EXPORT WrapperCollections
    {
    // ----- inner classes --------------------------------------------------

    public:
        /**
        * Iterator implementation which delegates all calls to another
        * Iterator.
        */
        class COH_EXPORT AbstractWrapperIterator
            : public abstract_spec<AbstractWrapperIterator,
                extends<Object>,
                implements<Muterator> >
            {
            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create an AbstractWrapperIterator which delegates to the
                * specified Iterator.
                *
                * @param hiterDelegate a handle to the specified delegate
                *        Iterator
                */
                AbstractWrapperIterator(Iterator::Handle hiterDelegate);

                /**
                * Copy constructor.
                */
                AbstractWrapperIterator(const AbstractWrapperIterator& that);


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


            // ----- Muterator interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void remove();

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;


            // ----- accessors ------------------------------------------

            protected:
                /**
                * Return a handle to the delegate.
                *
                * @return the delegate handle
                *
                * @throw UnsupportedOperationException if the delegate is a
                *        view
                */
                virtual Iterator::Handle getDelegate();

                /**
                * Return a view to the delegate.
                *
                * @return the delegate view
                */
                virtual Iterator::View getDelegate() const;

            // ----- data members ---------------------------------------

            private:
                /**
                * The Iterator to which this wrapper delegates.
                */
                FinalHolder<Iterator> f_ohDelegate;
            };


        /**
        * ListIterator implementation which delegates all calls to another
        * ListIterator.
        */
        class COH_EXPORT AbstractWrapperListIterator
            : public abstract_spec<AbstractWrapperListIterator,
                extends<AbstractWrapperIterator>,
                implements<ListMuterator> >
            {
            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create an AbstractWrapperListIterator which delegates to
                * the specified ListIterator.
                *
                * @param hIterDelegate  the Iterator to delegate to
                */
                AbstractWrapperListIterator(
                        ListIterator::Handle hIterDelegate);

                /**
                * Copy constructor.
                */
                AbstractWrapperListIterator(
                        const AbstractWrapperListIterator& that);

            // ----- ListIterator interface ----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool hasPrevious() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder previous();

                /**
                * {@inheritDoc}
                */
                virtual size32_t nextIndex() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t previousIndex() const;

            // ----- ListMuterator interface ----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void remove();
                /**
                * {@inheritDoc}
                */
                virtual void add(Object::Holder oh);

                /**
                * {@inheritDoc}
                */
                virtual void set(Object::Holder oh);

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


            // ----- helper methods -------------------------------------

            protected:
                /**
                * Return a handle to the delegate.
                *
                * @return the delegate handle
                *
                * @throw UnsupportedOperationException if the delegate is a
                *        view
                */
                virtual ListIterator::Handle getListDelegate();

                /**
                * Return a view to the delegate.
                *
                * @return the delegate view
                */
                virtual ListIterator::View getListDelegate() const;


            // ----- data members -------------------------------------------

            private:
                /**
                * The ListIterator to which this wrapper delegates.
                */
                FinalHolder<ListIterator> f_ohDelegate;
            };


        /**
        * Collection implementation which delegates all calls to another
        * Collection.
        */
        class COH_EXPORT AbstractWrapperCollection
            : public abstract_spec<AbstractWrapperCollection,
                extends<Object>,
                implements<Collection> >
            {
            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create an AbstractWrapperCollection which delegates to the
                * specified Collection.
                *
                * @param ohCollDelegate  the Collection to delegate to
                */
                AbstractWrapperCollection(Collection::Holder ohCollDelegate);

                /**
                * Copy constructor.
                */
                AbstractWrapperCollection(const AbstractWrapperCollection& that);

            // ----- Collection interface -------------------------------

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
                virtual void clear();

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual bool isImmutable() const;

            // ----- accessors ------------------------------------------

            protected:
                /**
                * Return a handle to the delegate.
                *
                * @return the delegate handle
                *
                * @throw UnsupportedOperationException if the delegate is a
                *        view
                */
                virtual Collection::Handle getDelegate();

                /**
                * Return a view to the delegate.
                *
                * @return the delegate view
                */
                virtual Collection::View getDelegate() const;

            // ----- data members ---------------------------------------

            private:
                /**
                * The Collection to which this wrapper delegates.
                */
                FinalHolder<Collection> f_ohDelegate;
            };

        /**
        * Set implementation which delegates all calls to another Set.
        */
        class COH_EXPORT AbstractWrapperSet
            : public abstract_spec<AbstractWrapperSet,
                extends<Object>,
                implements<Set> >
            {
            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create an AbstractWrapperSet which delegates to the
                * specified Set.
                *
                * @param ohSetDelegate  the Set to delegate to
                */
                AbstractWrapperSet(Set::Holder ohSetDelegate);

                /**
                * Copy constructor.
                */
                AbstractWrapperSet(const AbstractWrapperSet& that);

            // ----- Set interface --------------------------------------

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
                virtual void clear();

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual bool isImmutable() const;

            // ----- accessors ------------------------------------------

            protected:
                /**
                * Return a handle to the delegate.
                *
                * @return the delegate handle
                *
                * @throw UnsupportedOperationException if the delegate is a
                *        view
                */
                virtual Set::Handle getDelegate();

                /**
                * Return a view to the delegate.
                *
                * @return the delegate view
                */
                virtual Set::View getDelegate() const;

            // ----- data members ---------------------------------------

            private:
                /**
                * The Set to which this wrapper delegates.
                */
                FinalHolder<Set> f_ohDelegate;
            };


        /**
        * List implementation which delegates all calls to another List.
        */
        class COH_EXPORT AbstractWrapperList
            : public abstract_spec<AbstractWrapperList,
                extends<Object>,
                implements<List> >
            {
            // ----- constructors -------------------------------------------

            protected:
                /**
                * Create an AbstractWrapperList which delegates to the
                * specified List.
                *
                * @param ohListDelegate  the List to delegate to
                */
                AbstractWrapperList(List::Holder ohListDelegate);

                /**
                * Copy constructor.
                */
                AbstractWrapperList(const AbstractWrapperList& that);


            // ----- List interface ------------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool add(size32_t i, Object::Holder oh);

                /**
                * {@inheritDoc}
                */
                virtual bool addAll(size32_t i, Collection::View vc);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder get(size32_t i) const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder get(size32_t i);

                /**
                * {@inheritDoc}
                */
                virtual size32_t indexOf(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t lastIndexOf(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual ListIterator::Handle listIterator() const;

                /**
                * {@inheritDoc}
                */
                virtual ListIterator::Handle listIterator(size32_t index) const;

                /**
                * {@inheritDoc}
                */
                virtual ListMuterator::Handle listIterator();

                /**
                * {@inheritDoc}
                */
                virtual ListMuterator::Handle listIterator(size32_t index);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder remove(size32_t idx);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder set(size32_t idx, Object::Holder oh);

                /**
                * {@inheritDoc}
                */
                virtual List::Handle subList(size32_t fromIndex,
                        size32_t toIndex);

                /**
                * {@inheritDoc}
                */
                virtual List::View subList(size32_t fromIndex,
                        size32_t toIndex) const;


            // ----- Collection interface -----------------------------------

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
                virtual bool contains(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual bool containsAll(Collection::View vCol) const;

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
                virtual bool addAll(Collection::View vCol);

                /**
                * {@inheritDoc}
                */
                virtual bool remove(Object::View v);

                /**
                * {@inheritDoc}
                */
                virtual bool removeAll(Collection::View vCol);

                /**
                * {@inheritDoc}
                */
                virtual bool retainAll(Collection::View vCol);

                /**
                * {@inheritDoc}
                */
                virtual void clear();


            // ----- Object interface ---------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual bool isImmutable() const;


            // ----- helper methods -----------------------------------------

            protected:
                /**
                * Return a handle to the delegate.
                *
                * @return the delegate handle
                *
                * @throw UnsupportedOperationException if the delegate is a
                *        view
                */
                virtual List::Handle getDelegate();

                /**
                * Return a view to the delegate.
                *
                * @return the delegate view
                */
                virtual List::View getDelegate() const;


            // ----- data members -------------------------------------------

            private:
                /**
                * The List to which this wrapper delegates.
                */
                FinalHolder<List> f_ohDelegate;
            };

        /**
        * Map implementation which delegates all calls to another Map.
        */
        class COH_EXPORT AbstractWrapperMap
            : public abstract_spec<AbstractWrapperMap,
                extends<Object>,
                implements<Map> >
            {
            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create an AbstractWrapperMap which delegates to the
                * specified Map.
                *
                * @param ohMapDelegate  the Map to delegate to
                */
                AbstractWrapperMap(Map::Holder ohMapDelegate);

                /**
                * Copy constructor.
                */
                AbstractWrapperMap(const AbstractWrapperMap& that);

            // ----- Map interface --------------------------------------

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
                virtual void clear();

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
                virtual TypedHandle<const String> toString() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual bool isImmutable() const;

            // ----- accessors ------------------------------------------

            protected:
                /**
                * Return a handle to the delegate.
                *
                * @return the delegate handle
                *
                * @throw UnsupportedOperationException if the delegate is a
                *        view
                */
                virtual Map::Handle getDelegate();

                /**
                * Return a view to the delegate.
                *
                * @return the delegate view
                */
                virtual Map::View getDelegate() const;

            // ----- data members ---------------------------------------

            private:
                /**
                * The Map to which this wrapper delegates.
                */
                FinalHolder<Map> f_ohDelegate;
            };


    // ----- inner class: AbstractWrapperEntry ------------------------------

    public:
        /**
        * Map#Entry implementation which delegates all calls to another
        * Map#Entry.
        */
        class COH_EXPORT AbstractWrapperEntry
            : public abstract_spec<AbstractWrapperEntry,
                extends<Object>,
                implements<Map::Entry> >
            {
            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create an AbstractWrapperEntry which delegates to the
                * specified Map#Entry.
                *
                * @param ohEntryDelegate  the Entry to delegate to
                */
                AbstractWrapperEntry(Map::Entry::Holder ohEntryDelegate);

                /**
                * Copy constructor.
                */
                AbstractWrapperEntry(const AbstractWrapperEntry& that);

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
                virtual Object::Holder getValue();

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder setValue(Object::Holder ohValue);

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual bool isImmutable() const;

            // ----- accessors ------------------------------------------

            protected:
                /**
                * Return a handle to the delegate.
                *
                * @return the delegate handle
                *
                * @throw UnsupportedOperationException if the delegate is a
                *        view
                */
                virtual Map::Entry::Handle getDelegate();

                /**
                * Return a view to the delegate.
                *
                * @return the delegate view
                */
                virtual Map::Entry::View getDelegate() const;

            // ----- data members ---------------------------------------

            private:
                /**
                * The Map::Entry to which this wrapper delegates.
                */
                FinalHolder<Map::Entry> f_ohDelegate;
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_WRAPPER_COLLECTIONS_HPP
