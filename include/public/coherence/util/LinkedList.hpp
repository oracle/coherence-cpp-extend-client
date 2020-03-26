/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LINKED_LIST_HPP
#define COH_LINKED_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/ListIterator.hpp"
#include "coherence/util/AbstractList.hpp"
#include "coherence/util/SubList.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A linked list implementation of List.
*
* @author nsa 2008.02.04
*/
class COH_EXPORT LinkedList
    : public cloneable_spec<LinkedList,
        extends<AbstractList> >
    {
    friend class factory<LinkedList>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new LinkedList.
        *
        * @return a new LinkedList
        */
        LinkedList();

        /**
        * Create a new LinkedList that has a reference to every element in
        * the supplied collection.
        *
        * @param vc  The collection to base the LinkedList on
        *
        * @return a new LinkedList
        */
        LinkedList(Collection::View vc);

        /**
        * @internal
        */
        LinkedList(const LinkedList& that);

        /**
        * Destructor.
        */
        virtual ~LinkedList();


    // ----- List interface -------------------------------------------------

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
        using List::get;

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
        virtual ListIterator::Handle listIterator(size32_t index = 0) const;

        /**
        * {@inheritDoc}
        */
        virtual ListMuterator::Handle listIterator(size32_t index = 0);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(size32_t index);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder set(size32_t index, Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual List::View subList(size32_t fromIndex, size32_t toIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual List::Handle subList(size32_t fromIndex, size32_t toIndex);


    // ----- Collection interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

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
        virtual bool addAll(Collection::View vc);

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v);

        /**
        * {@inheritDoc}
        */
        virtual bool removeAll(Collection::View vColl);

        /**
        * {@inheritDoc}
        */
        virtual bool retainAll(Collection::View vCol);

        /**
        * {@inheritDoc}
        */
        virtual void clear();


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onEscape(bool fEscaped) const;


    // ----- nested class: ListElement --------------------------------------

    public:
        /**
        * ListElement is the foundation element for the linked list
        */
        class ListElement
            : public class_spec<ListElement>
            {
            friend class factory<ListElement>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new ListElement object.
                *
                * @param hNext  the element that will be next in the list
                * @param hPrev  the element that will be previous in the
                *               list
                * @param hObj   the object this list element contains
                *
                * @return a newly created ListElement
                */
                ListElement(ListElement::Handle hNext,
                        ListElement::Handle hPrev, Object::Holder hObj);


            // ----- ListElement interface ------------------------------

            public:
                /**
                * Helper method to find a ListElement at the specified
                * position
                *
                * @param thElement  the starting position to find the element
                *                   from
                * @param c          the index of the element to find
                *
                * @return a ListElement at the specified position
                *
                * @throws coherence::lang::IndexOutOfBoundsException if the
                *         index is out of range
                */
                template<class T> static TypedHandle<T>
                        nextElement(const TypedHandle<T>& thElement, size32_t c);

            // ----- Object interface -----------------------------------
            public:
                /**
                * {@inheritDoc}
                */
                virtual size64_t sizeOf(bool fDeep = false) const;

            // ----- data members ---------------------------------------

            public:
                /**
                * Reference to the next element in the list
                */
                mutable MemberHandle<ListElement> next;

                /**
                * Reference to the previous element in the list
                */
                MemberHandle<ListElement> previous;

                /**
                * Reference to the actual object contained by this list
                */
                MemberHolder<Object> value;
            };


    // ----- nested class: SubLinkedList ----------------------------------

    public:
        /**
        * Utility class to implement a SubList of a LinkedList.
        * SubLinkedList delegates through the the LinkedList for all
        * modification operations
        */
        class SubLinkedList
            : public class_spec<SubLinkedList,
                extends<SubList> >
            {
            friend class factory<SubLinkedList>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * create a new SubLinkedList.
                *
                * @param fromIndex  the starting point of the sublist in the
                *                   list provided (inclusive).
                * @param toIndex    the end point of the sublist in the list
                *                   provided (exclusive)
                * @param ohList     the list to create a sublist of
                *
                * @return a new SubLinkedList
                */
                SubLinkedList(size32_t fromIndex, size32_t toIndex,
                        List::Holder ohList);


            // ----- List interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool retainAll(Collection::View vColl);

                /**
                * {@inheritDoc}
                */
                virtual void clear();

                /**
                * {@inheritDoc}
                */
                virtual List::View
                        subList(size32_t fromIndex, size32_t toIndex) const;

                /**
                * {@inheritDoc}
                */
                virtual List::Handle
                        subList(size32_t fromIndex, size32_t toIndex);
            };


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Helper method that will throw a
        * coherence::lang::IndexOutOfBoundsException if the supplied index
        * is out of range
        *
        * @param i  the index to test
        */
        virtual void assertIndex(size32_t i) const;

    private:
        /**
        * Remove a ListElement from the linked list.
        *
        * @param hElement  the element to remove
        */
        Object::Holder removeElement(ListElement::Handle hElement,
                size32_t index);

        /**
        * Add a ListElement to the linked list.
        *
        * @param hNext  the element which will follow the new element
        * @param oh     the value portion of the new element
        */
        void addElement(ListElement::Handle hNext, Object::Holder oh);

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of elements in the list
        */
        size32_t m_cElements;

        /**
        * The head of the list.
        */
        mutable MemberHandle<ListElement> m_hHead;

        /**
        * The tail of the list.
        */
        mutable MemberHandle<ListElement> m_hTail;


    // ----- friends --------------------------------------------------------

    friend class SubLinkedList;
    friend class LinkedListIterator;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LINKED_LIST_HPP
