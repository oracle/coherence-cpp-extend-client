/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_READ_ONLY_MULTI_LIST_HPP
#define COH_READ_ONLY_MULTI_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractList.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/WrapperCollections.hpp"


COH_OPEN_NAMESPACE2(coherence,util)

/**
* Implementation of the List interface in a read-only fashion based on a
* collection of arrays.
*
* This class also implements the Set interface, although the contents are
* not checked to determine whether each element is unique. It is the
* responsibility of the user to ensure that the elements are unique if the
* object is used as a Set.
*
* Note: while preserved for backward compatibility, as of Coherence 3.6, use of
*       this class specifically as a List or a Set is deprecated.  Instead, the
*       getList(), getSet() methods should be used.
*
* @author gg, mf  2009.1.20
*
* @since Coherence 3.6
* @see ReadOnlyArrayList
*/
class COH_EXPORT ReadOnlyMultiList
    : public class_spec<ReadOnlyMultiList,
        extends<AbstractList>,
        implements<Set> >
    {
    friend class factory<ReadOnlyMultiList>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a List containing the elements of the specified array of
        * Object arrays.
        *
        * @param vaao  the array of arrays backing the MultiList
        */
        ReadOnlyMultiList(ObjectArray::View vaao);

    private:
        /**
        * Blocked copy constructor.
        */
        ReadOnlyMultiList(const ReadOnlyMultiList&);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return a List view of this ReadOnlyMultiList.
        *
        * @return a List view of this ReadOnlyMultiList
        */
        virtual List::Handle getList();

        /**
        * Return a List view of this ReadOnlyMultiList.
        *
        * @return a List view of this ReadOnlyMultiList
        */
        virtual List::View getList() const;

        /**
        * Return a Set view of this ReadOnlyMultiList.
        *
        * @return a Set view of this ReadOnlyMultiList
        */
        virtual Set::Handle getSet();

        /**
        * Return a Set view of this ReadOnlyMultiList.
        *
        * @return a Set view of this ReadOnlyMultiList
        */
        virtual Set::View getSet() const;


    // ----- List interface -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(size32_t i) const;
        using List::get;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder set(size32_t i, Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool add(size32_t i, Object::Holder oh);
        using Collection::add;

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(size32_t i, Collection::View vColl);
        using Collection::addAll;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(size32_t i);
        using Collection::remove;

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
        virtual bool contains(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle toArray(ObjectArray::Handle ha = NULL) const;

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const;
        using Collection::iterator;

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator();
        using List::iterator;

        /**
        * {@inheritDoc}
        */
        virtual ListIterator::Handle listIterator(size32_t i = 0) const;

        /**
        * {@inheritDoc}
        */
        virtual ListMuterator::Handle listIterator(size32_t i = 0);

        /**
        * {@inheritDoc}
        */
        virtual List::View subList(size32_t iFrom, size32_t iTo) const;

        /**
        * {@inheritDoc}
        */
        virtual List::Handle subList(size32_t iFrom, size32_t iTo);


    // ----- Object methods -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;


    // ----- helpers --------------------------------------------------------

    public:
        /**
        * Calculate the total number of element in the array of arrays.
        *
        * @param vaao  an array of arrays
        *
        * @return the total number of elements
        */
        static size32_t calculateTotalLength(ObjectArray::View vaao);

        /**
        * Create a single dimensional array containing all elements of the
        * specified array of arrays.
        *
        * @param vaaoFrom  an array of arrays to copy from
        * @param cTotal    the total length of the flattened array; pass npos
        *                  for it to be calculated
        * @param vaoTo     an array to copy the elements into (optional)
        *
        * @return an array containing all the elements of the array of arrays
        *
        * @throws ArrayIndexOutOfBoundsException if the total length parameter
        *         was not sufficient to hold the flattened array
        */
        static ObjectArray::Handle flatten(ObjectArray::View vaaoFrom,
                size32_t cTotal, ObjectArray::Handle haoTo = NULL);

    protected:
        /**
        * Common subList method implementation.
        */
        template<class T> static List::Holder subList(T, size32_t iFrom,
                size32_t iTo);


    // ----- inner class: ListView ------------------------------------------

    protected:
        /**
        * ListView exposes the underlying ReadOnlyMultiList through the List
        * interface, maintaining correct equals() and hashCode() semantics.
        */
        class COH_EXPORT ListView
            : public class_spec<ListView,
                extends<WrapperCollections::AbstractWrapperList> >
            {
            friend class factory<ListView>;

            // ----- constructors -------------------------------------------

            protected:    
                /**
                * Construct a ListView over the specified ReadOnlyMultiList.
                */
                ListView(ReadOnlyMultiList::Holder ohList);


            // ----- Object methods -----------------------------------------

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;
            };


    // ----- inner class: SetView -------------------------------------------

    protected:
        /**
        * SetView exposes the underlying ReadOnlyMultiList through the Set
        * interface, maintaining correct equals() and hashCode() semantics.
        */
        class COH_EXPORT SetView
            : public class_spec<SetView,
                extends<WrapperCollections::AbstractWrapperSet> >
            {
            friend class factory<SetView>;

            // ----- constructors -------------------------------------------

            protected:    
                /**
                * Construct a SetView over the specified ReadOnlyMultiList.
                */
                SetView(ReadOnlyMultiList::Holder ohSet);


            // ----- Object methods -----------------------------------------

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;
            };


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The array of Object arrays.
        */
        FinalView<ObjectArray> f_vaao;

        /**
        * A fully realized HashSet of this collections contents. This is
        * inflated and used for doing Set based operations if it is detected
        * that this collection is large and being accessed as a Set.
        */
        mutable FinalView<Set> f_vSet;

        /**
        * The total number of items.
        */
        const size32_t m_cTotal;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_READ_ONLY_MULTI_LIST_HPP
