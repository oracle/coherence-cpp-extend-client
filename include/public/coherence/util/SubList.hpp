/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUB_LIST_HPP
#define COH_SUB_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractList.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* SubList delegates all operations to it's full list while providing a
* partial view of the list.
*
* @author nsa 2008.02.08
*/
class COH_EXPORT SubList
    : public abstract_spec<SubList,
        extends<AbstractList> >
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * @internal
        */
        SubList(size32_t fromIndex, size32_t toIndex, List::Holder ohList);

    private:
        /**
        * Blocked copy constructor
        */
        SubList(const SubList&);


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
        virtual Object::Holder remove(size32_t idx);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder set(size32_t idx, Object::Holder oh);


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


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Validate that the supplied index is in the appropriate range
        * for this SubList.
        *
        * @param i  the index to test against the SubList's boundaries
        *
        * @throws coherence::lang::IndexOutOfBoundsException iff the
        * supplied index is outside the range of this SubList.
        */
        void assertIndex(size32_t i) const;

        /**
        * Return the to delegate to.
        *
        * @return the list to delegate to
        */
        virtual List::Handle getDelegate();

        /**
        * Return the to delegate to.
        *
        * @return the list to delegate to
        */
        virtual List::View getDelegate() const;

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying list this is a sublist of
        */
        FinalHolder<List> f_ohList;

        /**
        * The offset this list is based on from the underlying list.
        */
        size32_t m_iOffset;

        /**
        * The size of this sublist.
        */
        size32_t m_iSize;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SUB_LIST_HPP
