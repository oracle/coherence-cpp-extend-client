/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LIST_HPP
#define COH_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/ListIterator.hpp"
#include "coherence/util/ListMuterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An ordered collection (also known as a sequence).
*
* The user of this interface has precise control over where in the list each
* element is inserted. The user can access elements by their integer index
* (position in the list), and search for elements in the list. Unlike sets,
* lists typically allow duplicate elements.
*
* The List interface places additional stipulations, beyond those specified
* in the Collection interface, on the contracts of the iterator, add, remove,
* equals, and hashCode methods.
*
* @see Collection
*
* @author js/nsa  2008.01.03
*/
class COH_EXPORT List
    : public interface_spec<List,
        implements<Collection> >
    {
    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);


    // ----- List interface -------------------------------------------------

    public:
        /**
        * Add the given element to this collection at the position specified.
        * If an element is already at that position it will be shifted to the
        * right by 1.
        *
        * @param i   the position to insert the element at
        * @param oh  the element to add
        *
        * @return true iff this list was modified as a result of this
        *         operation
        *
        * @throw coherence::lang::IndexOutOfBoundsException if the index is
        *        out of range
        */
        virtual bool add(size32_t i, Object::Holder oh) = 0;

        /**
        * Add all the elements from the supplied collection to this
        * collection at the position specified.
        *
        * @param i   the index of the list to add the elements to
        * @param vc  the collection to add to this list
        *
        * @return true iff this list was modified as a result of this
        *         operation
        *
        * @throw coherence::lang::IndexOutOfBoundsException if the inedex
        *        is out of range
        */
        virtual bool addAll(size32_t i, Collection::View vc) = 0;

        /**
        * Return the element from the specified position in the list.
        *
        * @param i  the position of the item to return
        *
        * @return the element from the specified position in the list
        *
        * @throw coherence::lang::IndexOutOfBoundsException if the index is
        *        out of range
        */
        virtual Object::Holder get(size32_t i) const = 0;

        /**
        * Return the element from the specified position in the list.
        *
        * @param i  the position of the item to return
        *
        * @return the element from the specified position in the list
        *
        * @throw coherence::lang::IndexOutOfBoundsException if the index is
        *        out of range
        */
        virtual Object::Holder get(size32_t i) = 0;

        /**
        * Return the position in the list of the first instance of the
        * specified element.
        *
        * @param v  The object in the list to return the index of
        *
        * @return the position of the object if found, or List::npos
        */
        virtual size32_t indexOf(Object::View v) const = 0;

        /**
        * Return the position in this list of the last instance of the
        * specified element.
        *
        * @param v  The element to search for in the list
        *
        * @return the position of the element if found, or List::npos
        */
        virtual size32_t lastIndexOf(Object::View v) const = 0;

        /**
        * Return a ListIterator for this list starting at index
        *
        * @param i  the index to start the ListIterator at
        *
        * @return a ListIterator for this list start at index
        */
        virtual ListIterator::Handle listIterator(size32_t i = 0) const = 0;

        /**
        * Return a ListIterator for this list starting at index
        *
        * @param i  the index to start the ListIterator at
        *
        * @return a ListIterator for this list start at index
        */
        virtual ListMuterator::Handle listIterator(size32_t i = 0) = 0;

        /**
        * Remove the element at the specified position in the list.
        *
        * @param i  the index from which to remove an element
        *
        * @return the element at the specified position prior to this
        *         operation
        *
        * @throw coherence::lang::IndexOutOfBoundsException if the index is
        *        out of range
        */
        virtual Object::Holder remove(size32_t i) = 0;

        /**
        * Replace the element at the specified position in this list with the
        * specified element.
        *
        * @param i   the position in the list to replace
        * @param oh  the object to replace at the specified position
        *
        * @return the element at the specified position prior to this
        *         operation
        *
        * @throw coherence::lang::IndexOutOfBoundsException if the index is
        *        out of range
        */
        virtual Object::Holder set(size32_t i, Object::Holder oh) = 0;

        /**
        * Return a new list containing the contents of the list between the
        * specified fromIndex (inclusive) and toIndex (exclusive).
        *
        * @param iFrom  the start position in this list to create the
        *               sublist from (inclusive).
        * @param iTo    the end position in this list to end the sublist
        *               at (exclusive).
        *
        * @return the new sublist of this list
        */
        virtual List::View subList(size32_t iFrom, size32_t iTo) const = 0;

        /**
        * Return a new list containing the contents of the list between the
        * specified fromIndex (inclusive) and toIndex (exclusive).
        *
        * @param iFrom  the start position in this list to create the
        *               sublist from (inclusive).
        * @param iTo    the end position in this list to end the sublist
        *               at (exclusive).
        *
        * @return the new sublist of this list
        */
        virtual List::Handle subList(size32_t iFrom, size32_t iTo) = 0;


    // ----- Collection interface -------------------------------------------

    public:
        /**
        * Add the given element to the end of this list.
        *
        * @param oh  element to be added to this list
        *
        * @return true
        */
        using Collection::add;

        /**
        * {@inheritDoc}
        */
        using Collection::addAll;

        /**
        * {@inheritDoc}
        */
        using Collection::remove;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Compares the specified object with this list for equality.
        *
        * Two lists are defined to be equal iff they contain the same
        * elements in the same order. This definition ensures that the equals
        * method works properly across different implementations of the List
        * interface.
        *
        * @param vObj  the Object to be compared for equality with this list
        *
        * @return true if the specified Object is equal to this list.
        */
        using Object::equals;

        /**
        * The hash code of a List is defined as follows:
        * <code>
        * size32_t nHash = 1;
        * for (Iterator::Handle hIter  = list.iterator(); hIter->hasNext(); )
        *     {
        *     Object::View v = hIter->next();
        *     nHash = 31 * nHash + Object::hashCode(v);
        *     }
        * </code> This ensures that <code>l1->equals(l2)</code> implies that
        * <code>l1->hashCode() == l2->hashCode()</code> for any two lists
        * <tt>l1</tt> and <tt>l2</tt>, as required by the general contract of
        * Object::hashCode().
        *
        * @return the hash code value for this list
        */
        using Object::hashCode;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LIST_HPP
