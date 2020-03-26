/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SORTED_BAG_HPP
#define COH_SORTED_BAG_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractCollection.hpp"
#include "coherence/util/AtomicCounter.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/NavigableMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
 * SortedBag is a <a href="http://en.wikipedia.org/wiki/Multiset">
 * <i>multiset</i> or <i>bag</i></a> implementation that supports sorted traversal
 * of the contained elements and is optimized for insertions and removals.
 *
 * This implementation is not thread-safe and does not support NULL elements.
 *
 * @author rhl 2013.04.24
 */
class COH_EXPORT SortedBag
    : public class_spec<SortedBag,
        extends<AbstractCollection> >
    {
    friend class factory<SortedBag>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        SortedBag();

        /**
        * Construct a SortedBag whose elements are to be ordered by the specified
        * comparator.
        *
        * @param vComparator  the comparator to use to order the elements
        */
        SortedBag(Comparator::View vComparator);


    private:
        /**
        * Blocked copy constructor.
        */
        SortedBag(const SortedBag& that);


    // ----- accessors ------------------------------------------------------

    protected:
        /**
        * Return the internal navigable map holding the bag contents.
        *
        * @return the internal navigable map holding the bag contents
        */
        virtual NavigableMap::Handle getInternalMap() const;

        /**
        * Return the Comparator used to order the bag contents.
        *
        * @return the Comparator used to order the bag contents
        */
        virtual Comparator::View getComparator() const;

        /**
        * Return the nonce counter used to assign unique element ids.
        *
        * @return the nonce counter
        */
        virtual AtomicCounter::Handle getNonceCounter();


    // ----- AbstractCollection methods -------------------------------------

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
        virtual bool contains(Object::View vo) const;

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
        virtual bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v);


    // ----- SortedBag interface --------------------------------------------

    public:
        /**
        * Returns a view of the portion of this bag whose elements range
        * from <tt>vFrom</tt>, inclusive, to <tt>vTo</tt>,
        * exclusive.  (If <tt>vFrom</tt> and <tt>vTo</tt> are
        * equal, the returned bag is empty.)  The returned bag is backed
        * by this bag, so changes in the returned bag are reflected in
        * this bag, and vice-versa.  The returned bag supports all
        * optional bag operations that this bag supports.
        *
        * The returned bag will throw an <tt>IllegalArgumentException</tt>
        * on an attempt to insert an element outside its range.
        *
        * @param vFrom  low endpoint (inclusive) of the returned bag
        * @param vTo    high endpoint (exclusive) of the returned bag
        *
        * @return a view of the portion of this bag whose elements range from
        *         <tt>vFrom</tt>, inclusive, to <tt>vTo</tt>, exclusive
        *
        * @throws ClassCastException if <tt>vFrom</tt> and
        *         <tt>vTo</tt> cannot be compared to one another using this
        *         bag's comparator (or, if the bag has no comparator, using
        *         natural ordering).  Implementations may, but are not required
        *         to, throw this exception if <tt>vFrom</tt> or
        *         <tt>vTo</tt> cannot be compared to elements currently in
        *         the bag.
        * @throws NullPointerException if <tt>vFrom</tt> or
        *         <tt>vTo</tt> is NULL and this bag does not permit NULL
        *         elements
        * @throws IllegalArgumentException if <tt>vFrom</tt> is
        *         greater than <tt>vTo</tt>; or if this bag itself
        *         has a restricted range, and <tt>vFrom</tt> or
        *         <tt>vTo</tt> lies outside the bounds of the range
        */
        virtual SortedBag::Handle subBag(Object::View vFrom, Object::View vTo);
        virtual SortedBag::View   subBag(Object::View vFrom, Object::View vTo) const;

        /**
        * Returns a view of the portion of this bag whose elements are
        * strictly less than <tt>vTo</tt>.  The returned bag is
        * backed by this bag, so changes in the returned bag are
        * reflected in this bag, and vice-versa.  The returned bag
        * supports all optional bag operations that this bag supports.
        *
        * The returned bag will throw an <tt>IllegalArgumentException</tt>
        * on an attempt to insert an element outside its range.
        *
        * @param vTo  high endpoint (exclusive) of the returned bag
        *
        * @return a view of the portion of this bag whose elements are strictly
        *         less than <tt>vTo</tt>
        *
        * @throws ClassCastException if <tt>vTo</tt> is not compatible
        *         with this bag's comparator (or, if the bag has no comparator,
        *         if <tt>vTo</tt> does not implement {@link Comparable}).
        *         Implementations may, but are not required to, throw this
        *         exception if <tt>vTo</tt> cannot be compared to elements
        *         currently in the bag.
        * @throws NullPointerException if <tt>vTo</tt> is NULL and
        *         this bag does not permit NULL elements
        * @throws IllegalArgumentException if this bag itself has a
        *         restricted range, and <tt>vTo</tt> lies outside the
        *         bounds of the range
        */
        virtual SortedBag::Handle headBag(Object::View vTo);
        virtual SortedBag::View   headBag(Object::View vTo) const;

        /**
        * Returns a view of the portion of this bag whose elements are
        * greater than or equal to <tt>vFrom</tt>.  The returned
        * bag is backed by this bag, so changes in the returned bag are
        * reflected in this bag, and vice-versa.  The returned bag
        * supports all optional bag operations that this bag supports.
        *
        * The returned bag will throw an <tt>IllegalArgumentException</tt>
        * on an attempt to insert an element outside its range.
        *
        * @param vFrom  low endpoint (inclusive) of the returned bag
        *
        * @return a view of the portion of this bag whose elements are greater
        *         than or equal to <tt>vFrom</tt>
        *
        * @throws ClassCastException if <tt>vFrom</tt> is not compatible
        *         with this bag's comparator (or, if the bag has no comparator,
        *         if <tt>vFrom</tt> does not implement {@link Comparable}).
        *         Implementations may, but are not required to, throw this
        *         exception if <tt>vFrom</tt> cannot be compared to elements
        *         currently in the bag.
        * @throws NullPointerException if <tt>vFrom</tt> is NULL
        *         and this bag does not permit NULL elements
        * @throws IllegalArgumentException if this bag itself has a
        *         restricted range, and <tt>vFrom</tt> lies outside the
        *         bounds of the range
        */
        virtual SortedBag::Handle tailBag(Object::View vFrom);
        virtual SortedBag::View   tailBag(Object::View vFrom) const;

        /**
        * Returns the first (lowest) element currently in this bag.
        *
        * @return the first (lowest) element currently in this bag
        *
        * @throws NoSuchElementException if this bag is empty
        */
        virtual Object::Holder first() const;

        /**
        * Returns the last (highest) element currently in this bag.
        *
        * @return the last (highest) element currently in this bag
        *
        * @throws NoSuchElementException if this bag is empty
        */
        virtual Object::Holder last() const;

        /**
        * Remove and return the least element in this bag, or NULL if empty.
        *
        * @return the removed first element of this bag, or NULL if empty
        */
        virtual Object::Holder removeFirst();

        /**
        * Remove and return the greatest element in this bag, or NULL if empty.
        *
        * @return the removed last element of this bag, or NULL if empty
        */
        virtual Object::Holder removeLast();


    // ----- inner class: WrapperComparator ---------------------------------

    public:
        /**
        * WrapperComparator is a Comparator implementation that is aware of
        * {@link UniqueElement} wrappers and will delegate the comparison of the
        * elements in both forms to the wrapped comparator.
        */
        class COH_EXPORT WrapperComparator
            : public class_spec<WrapperComparator,
                extends<Object>,
                implements<Comparator> >
        {
        friend class factory<WrapperComparator>;

        // ----- constructors -------------------------------------------

        protected:
            /**
            * @internal
            */
            WrapperComparator(Comparator::View vComparator);

            /**
            * @internal
            */
            WrapperComparator(const WrapperComparator& that);

        // ----- Comparator interface ---------------------------------------

        public:
            /**
            * {@inheritDoc}
            */
            virtual int32_t compare(Object::View v1, Object::View v2) const;


        // ----- data members -----------------------------------------------

        protected:
            /**
            * The underlying "logical" comparator.
            */
            FinalView<Comparator> f_vComparator;
        };


    // ----- inner class: UniqueElement -------------------------------------

    protected:
        /**
        * UniqueElement represents a unique instance of a logical element in the bag.
        */
        class COH_EXPORT UniqueElement
            : public class_spec<UniqueElement,
                extends<Object>,
                implements<Comparable> >
        {
        friend class factory<UniqueElement>;

        // ----- constructors -----------------------------------------------

        protected:
            /**
            * Create a UniqueElement to represent the specified element.
            *
            * @param hBag  reference to the "outer this"
            * @param oh    the element
            */
            UniqueElement(SortedBag::Handle hBag, Object::Holder oh);


        // ----- Comparable interface ---------------------------------------

        public:
            /**
            * {@inheritDoc}
            */
            virtual int32_t compareTo(Object::View vThat) const;


        // ----- data members -----------------------------------------------

        protected:

            /**
            * The unique "id" for this element.
            */
            const int64_t f_nUniqueId;

            /**
            * The "actual" element.
            */
            FinalHolder<Object> f_ohElem;

            /**
            * The handle to the "outer-this" SortedBag.
            */
            WeakHandle<SortedBag> f_hBagOuter;

        // ----- friends ----------------------------------------------------

        friend class SortedBag;
        };


    // ----- inner class: ViewBag -------------------------------------------

        // forward declaration
        class ViewBag;


    // ----- helper methods -------------------------------------------------

    public:

        /**
        * Unwrap the specified element (which could be a {@link UniqueElement
        * wrapped} or an "actual") element.
        *
        * @param o  the element to unwrap
        *
        * @return the unwrapped element
        */
        virtual Object::Holder unwrap(Object::View v) const;

    protected:
        /**
        * Factory for the navigable internal map to use to hold the bag elements.
        *
        * @param comparator  the comparator to use to sort the bag elements
        *
        * @return a navigable map
        */
        virtual NavigableMap::Handle instantiateInternalMap(Comparator::View vComparator);

        /**
        * Wrap the specified element to ensure uniqueness.
        *
        * @param oh  the element to wrap
        *
        * @return a unique element representing the specified element
        */
        virtual UniqueElement::View wrap(Object::Holder oh);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The nonce used to increment the unique element ids.
        */
        mutable MemberHandle<AtomicCounter> m_hAtomicNonce;

        /**
        * The comparator used to compare logical elements.
        */
        MemberView<Comparator> m_vComparator;

        /**
        * The internal navigable map holding the bag contents.
        */
        mutable MemberHandle<NavigableMap> m_hMap;

        friend class ViewBag;
    };


    // ----- inner class: ViewBag -------------------------------------------

    /**
     * A range-limited view of the SortedBag.  This view is backed by the
     * SortedBag, so any modifications made to it are visible to the underlying
     * bag, and vice-versa.
     */
    class COH_EXPORT SortedBag::ViewBag
        : public class_spec<ViewBag,
            extends<SortedBag> >
    {
    friend class factory<SortedBag::ViewBag>;

    // ----- constructors -----------------------------------------------

    protected:
        /**
         * Construct a view of the SortedBag, constrained to the range [ohFrom, ohTo).
         *
         * @param ohBag   reference to the "outer this"
         * @param vFrom   the "from" element (inclusive), or null
         * @param vTo     the "to" element (exclusive), or null
         */
        ViewBag(SortedBag::Holder ohBag, Object::View vFrom, Object::View vTo);


    // ----- SortedBag methods ------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual bool add(Object::Holder oh);

        /**
         * {@inheritDoc}
         */
        virtual SortedBag::Handle subBag(Object::View vFrom, Object::View vTo);

        /**
         * {@inheritDoc}
         */
        virtual SortedBag::View   subBag(Object::View vFrom, Object::View vTo) const;

        /**
         * {@inheritDoc}
         */
        virtual SortedBag::Handle headBag(Object::View vTo);

        /**
         * {@inheritDoc}
         */
        virtual SortedBag::View   headBag(Object::View vTo) const;

        /**
         * {@inheritDoc}
         */
        virtual SortedBag::Handle tailBag(Object::View vFrom);

        /**
         * {@inheritDoc}
         */
        virtual SortedBag::View   tailBag(Object::View vFrom) const;


    // ----- helper methods ---------------------------------------------

    protected:
        /**
         * Check that the specified object is within the range of this view.
         *
         * @param v  the object to check
         */
        void checkRange(Object::View v) const;


    // ----- data members -----------------------------------------------

    protected:
        /**
         * The "outer this".
         */
        FinalHolder<SortedBag> f_ohBag;

        /**
         * The (inclusive) lower bound of this view.
         */
        FinalView<Object> f_vFrom;

        /**
         * The (exclusive) upper bound of this view.
         */
        FinalView<Object> f_vTo;

    // ----- friends ----------------------------------------------------

        friend class SortedBag;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SORTED_BAG_HPP
