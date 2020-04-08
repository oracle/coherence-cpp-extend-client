/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SubList.hpp"

#include "coherence/util/ListMuterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(SubList)

// ----- helper class SubListIterator ---------------------------------------

class SubListIterator
    : public class_spec<SubListIterator,
        extends<Object>,
        implements<ListMuterator> >
    {
    friend class factory<SubListIterator>;

    // ----- constructor --------------------------------------------

    protected:
        SubListIterator(SubList::View vList,
                ListIterator::Handle hIterator, size32_t iCurrentIndex)
            :  f_hIterator(self(), hIterator), f_vList(self(), vList),
               m_iCurrentIndex(iCurrentIndex)
            {
            }

    // ----- ListIterator interface ---------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t nextIndex() const
            {
            return m_iCurrentIndex;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t previousIndex() const
            {
            size32_t iNext = m_iCurrentIndex;
            return iNext == 0 ? List::npos : iNext - 1;
            }

        /**
        * @internal
        */
        virtual bool hasPrevious() const
            {
            return (m_iCurrentIndex != 0) && f_hIterator->hasPrevious();
            }

        /**
        * @internal
        */
        virtual Object::Holder previous()
            {
            if (m_iCurrentIndex == 0)
                {
                COH_THROW (IndexOutOfBoundsException::create());
                }

            --m_iCurrentIndex;
            return f_hIterator->previous();
            }

        /**
        *  @internal
        */
        virtual void add(Object::Holder oh)
            {
            return getMuterator()->add(oh);
            }

        /**
        *  @internal
        */
        virtual void set(Object::Holder oh)
            {
            return getMuterator()->set(oh);
            }

    // ----- Iterator interface -----------------------------------------

    public:
        /**
        * @internal
        */
        virtual bool hasNext() const
            {
            return ((m_iCurrentIndex + 1 < f_vList->size()) &&
                f_hIterator->hasNext());
            }

        /**
        * @internal
        */
        virtual Object::Holder next()
            {
            if (m_iCurrentIndex + 1 > f_vList->size())
                {
                COH_THROW (NoSuchElementException::create());
                }

            return f_hIterator->next();
            }

        /**
        *  @internal
        */
        virtual void remove()
            {
            getMuterator()->remove();
            }

    // ------ helper methods --------------------------------------------

    protected:
        /**
        * Test if the wrapped iterator isa  muterator. If it is return
        * it as such, otherwise throw an unsupported operation exception.
        *
        * @return the muterator
        *
        * @throws UnsupportedOperationException
        */
        ListMuterator::Handle getMuterator()
            {
            ListMuterator::Handle hMiter = cast<ListMuterator::Handle>(f_hIterator, false);
            if (NULL == hMiter)
                {
                COH_THROW (UnsupportedOperationException::create());
                }
            return hMiter;
            }


    // ------ data members ----------------------------------------------

    protected:
        /**
        * The iterator being delegated to.
        */
        FinalHandle<ListIterator> f_hIterator;

        /**
        * Reference to the list backing this iterator.
        */
        FinalView<SubList> f_vList;

        /**
        * The current index of the iterator.
        */
        size32_t m_iCurrentIndex;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

SubList::SubList(size32_t fromIndex, size32_t toIndex,
        List::Holder ohList)
    : f_ohList(self(), ohList), m_iOffset(fromIndex)
    {
    size32_t iParentSize = ohList->size();

    if (fromIndex > iParentSize)
        {
        COH_THROW_STREAM(IllegalArgumentException, "fromIndex (" << fromIndex
            << ") not within the range of the parent list(" <<
            iParentSize << ")");
        }
    else if (toIndex < fromIndex)
        {
        COH_THROW_STREAM(IllegalArgumentException, "toIndex (" << toIndex <<
                ") small than fromIndex (" << fromIndex << ").");
        }
    else if (toIndex > iParentSize)
        {
        COH_THROW_STREAM(IllegalArgumentException, "toIndex (" << toIndex
            << ") not within the range of the parent list(" <<
            iParentSize << ")");
        }

    m_iSize = toIndex - fromIndex;
    }


// ----- List interface -----------------------------------------------------

bool SubList::add(size32_t i, Object::Holder oh)
    {
    assertIndex(i);
    bool fResult = getDelegate()->add(i + m_iOffset, oh);
    if (fResult)
        {
        ++m_iSize;
        }

    return fResult;
    }

bool SubList::addAll(size32_t i, Collection::View vc)
    {
    assertIndex(i);
    bool fResult = getDelegate()->addAll(i + m_iOffset, vc);
    if (fResult)
        {
        m_iSize += vc->size();
        }

    return fResult;
    }

Object::Holder SubList::get(size32_t i) const
    {
    assertIndex(i);

    return getDelegate()->get(i + m_iOffset);
    }

size32_t SubList::indexOf(Object::View v) const
    {
    size32_t i       = getDelegate()->indexOf(v);
    size32_t iOffset = m_iOffset;

    return i < iOffset || i > iOffset + m_iSize ? List::npos : i - iOffset;
    }

size32_t SubList::lastIndexOf(Object::View v) const
    {
    size32_t i       = getDelegate()->lastIndexOf(v);
    size32_t iOffset = m_iOffset;

    return i < iOffset || i > iOffset + m_iSize ? List::npos : i - iOffset;
    }

ListIterator::Handle SubList::listIterator(size32_t index) const
    {
    assertIndex(index);
    return SubListIterator::create(this,
            getDelegate()->listIterator(index + m_iOffset), index);
    }

ListMuterator::Handle SubList::listIterator(size32_t index)
    {
    assertIndex(index);
    return SubListIterator::create(this,
            getDelegate()->listIterator(index + m_iOffset), index);
    }

Object::Holder SubList::remove(size32_t index)
    {
    assertIndex(index);

    --m_iSize;
    return getDelegate()->remove(index + m_iOffset);
    }

Object::Holder SubList::set(size32_t idx, Object::Holder oh)
    {
    assertIndex(idx);

    return getDelegate()->set(idx + m_iOffset, oh);
    }


// ----- Collection interface -----------------------------------------------

size32_t SubList::size() const
    {
    return m_iSize;
    }

Iterator::Handle SubList::iterator() const
    {
    return SubListIterator::create(this, getDelegate()->listIterator(m_iOffset), 0);
    }

Muterator::Handle SubList::iterator()
    {
    return SubListIterator::create(this, getDelegate()->listIterator(m_iOffset), 0);
    }

bool SubList::add(Object::Holder oh)
    {
    bool fResult = getDelegate()->add(m_iOffset + m_iSize, oh);
    if (fResult)
        {
        ++m_iSize;
        }

    return fResult;
    }

bool SubList::addAll(Collection::View vc)
    {
    bool fResult = getDelegate()->addAll(m_iOffset + m_iSize, vc);
    if (fResult)
        {
        m_iSize += vc->size();
        }
    return fResult;
    }

bool SubList::remove(Object::View v)
    {
    bool fResult = getDelegate()->remove(v);
    if (fResult)
        {
        --m_iSize;
        }

    return fResult;
    }

// ----- helper methods -----------------------------------------------------

void SubList::assertIndex(size32_t i) const
    {
    size32_t iSize = m_iSize;
    if (i > iSize)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "index: " << i <<
                " size: " << iSize);
        }
    }

List::Handle SubList::getDelegate()
    {
    return cast<List::Handle>(f_ohList);
    }

List::View SubList::getDelegate() const
    {
    return f_ohList;
    }

COH_CLOSE_NAMESPACE2
