/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/LinkedList.hpp"

#include "coherence/util/Set.hpp"

#include "coherence/util/ConcurrentModificationException.hpp"

using coherence::util::Set;

COH_OPEN_NAMESPACE2(coherence,lang)
    // defined in Object.cpp
    extern Set::Handle coh_getThreadLocalSizeOfSet();
COH_CLOSE_NAMESPACE2


COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_TYPED_CLASS(LinkedList);


// ------ helper class: LinkedListIterator ------------------------------

class COH_EXPORT LinkedListIterator
    : public class_spec<LinkedListIterator,
        extends<Object>,
        implements<ListMuterator> >
    {
    friend class factory<LinkedListIterator>;

    // ----- constructor ------------------------------------------------

    protected:
        LinkedListIterator(LinkedList::Holder ohList,
                LinkedList::ListElement::Holder ohElement, size32_t index)
            : f_ohList(self(), ohList), m_ohNextElement(self(), ohElement),
              m_ohReturnedElement(self()),
              m_iNextIndex(index),
              m_fViewer(!instanceof<LinkedList::Handle>(ohList))
            {
            }


    // ----- Iterator interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasPrevious() const
            {
            return m_iNextIndex != 0;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t nextIndex() const
            {
            return m_iNextIndex;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t previousIndex() const
            {
            size32_t iNext = m_iNextIndex;
            return iNext == 0 ? List::npos : iNext - 1;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder previous()
            {
            if (m_iNextIndex == 0)
                {
                COH_THROW (NoSuchElementException::create());
                }

            LinkedList::ListElement::Holder ohNext = m_ohNextElement;
            LinkedList::ListElement::Holder ohReturn;
            if (NULL == ohNext)
                {
                if (m_fViewer)
                    {
                    ohReturn = (LinkedList::ListElement::View) getList()->m_hTail;
                    }
                else
                    {
                    ohReturn = getMutableList()->m_hTail;
                    }
                }
            else
                {
                if (m_fViewer)
                    {
                    ohReturn = (LinkedList::ListElement::View) ohNext->previous;
                    }
                else
                    {
                    ohReturn = cast<LinkedList::ListElement::Handle>(ohNext)->previous;
                    }
                }

            if (NULL == ohReturn)
                {
                COH_THROW (ConcurrentModificationException::create());
                }

            m_ohReturnedElement = ohReturn;
            m_ohNextElement     = ohReturn;
            --m_iNextIndex;

            return ohReturn->value;
            }

        /**
        * {@inheritDoc}
        */
        virtual void add(Object::Holder oh)
            {
            getMutableList()->addElement(cast
                    <LinkedList::ListElement::Handle>(m_ohNextElement), oh);
            ++m_iNextIndex;
            m_ohReturnedElement = NULL;
            }

        /**
        * {@inheritDoc}
        */
        virtual void set(Object::Holder oh)
            {
            LinkedList::ListElement::Holder ohReturned = m_ohReturnedElement;
            if (NULL == ohReturned)
                {
                COH_THROW (IllegalStateException::create());
                }
            LinkedList::ListElement::Handle hPrior =
                cast<LinkedList::ListElement::Handle>(ohReturned);
            if (NULL == hPrior)
                {
                COH_THROW (UnsupportedOperationException::create());
                }
            hPrior->value = oh;
            }

    // ----- Iterator interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            return m_iNextIndex != getList()->size();
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            if (m_iNextIndex == getList()->size())
                {
                COH_THROW (NoSuchElementException::create());
                }

            LinkedList::ListElement::Holder ohReturn = m_ohNextElement;
            if (NULL == ohReturn)
                {
                COH_THROW (ConcurrentModificationException::create());
                }

            m_ohReturnedElement = ohReturn;
            if (m_fViewer)
                {
                m_ohNextElement = ohReturn->next;
                }
            else
                {
                m_ohNextElement = cast<LinkedList::ListElement::Handle>(ohReturn)->next;
                }
            ++m_iNextIndex;

            return ohReturn->value;
            }

        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            LinkedList::ListElement::Holder ohReturned = m_ohReturnedElement;
            if (NULL == ohReturned)
                {
                COH_THROW (IllegalStateException::create());
                }

            bool     fForward = ohReturned->next == m_ohNextElement;
            size32_t iRemove  = fForward ? m_iNextIndex - 1 : m_iNextIndex;
            getMutableList()->removeElement(
                    cast<LinkedList::ListElement::Handle>(ohReturned),
                    iRemove);

            if (fForward)
                {
                --m_iNextIndex;
                }
            m_ohReturnedElement = NULL;
            }

    // ----- helper methods ----------------------------------------

    protected:
        /**
        * Return the mutable List
        *
        * @throws UnsupportedOperationException if the list is not mutable
        */
        LinkedList::Handle getMutableList()
            {
            LinkedList::Handle hList = cast<LinkedList::Handle>(f_ohList, false);
            if (NULL == hList)
                {
                COH_THROW (UnsupportedOperationException::create());
                }
            return hList;
            }

        /**
        * Return the List
        */
        LinkedList::View getList() const
            {
            return f_ohList;
            }

    // ----- data members ------------------------------------------

    protected:
        /**
        * Reference to the list.
        */
        FinalHolder<LinkedList> f_ohList;

        /**
        * The current element for this iterator
        */
        MemberHolder<LinkedList::ListElement> m_ohNextElement;

        /**
        * The last element returned from the iterator, or NULL if none
        * has been returned, or if an add or remove was performed since
        * the last element was retruned.
        */
        MemberHolder<LinkedList::ListElement> m_ohReturnedElement;

        /**
        * The current Index of this iterator in the list.
        */
        size32_t m_iNextIndex;

        /**
        * True if this Muterator can only be used as Iterator.
        */
        const bool m_fViewer;
    };


// ----- nested class: ListElement ------------------------------------------

// ----- constructor ----------------------------------------------------

LinkedList::ListElement::ListElement(ListElement::Handle hNext,
        ListElement::Handle hPrev, Object::Holder hObj)
    : next(self(), hNext, /*fMutable*/ true), previous(self(), hPrev),
      value(self(), hObj)
    {
    }


// ----- ListElement interface ------------------------------------------

template<class T>
TypedHandle<T> LinkedList::ListElement::nextElement(
    const TypedHandle<T>& thElement, size32_t c)
    {
    TypedHandle<T> result = thElement;
    while (c != 0)
        {
        if (NULL == result)
            {
            return NULL;
            }

        result = result->next;
        --c;
        }

    return result;
    }

size64_t LinkedList::ListElement::sizeOf(bool fDeep) const
    {
    if (!fDeep)
        {
        return sizeof(LinkedList::ListElement);
        }

    // Avoid the recursive traversal which the default impl would trigger.
    // With the linked list this would cause stack overflow for lists with a
    // few thousand elements

    size64_t    cb   = 0;
    Set::Handle hSet = coh_getThreadLocalSizeOfSet();
    if (hSet->add(this))
        {
        bool fRoot = hSet->size() == 1;
        try
            {
            cb = sizeof(LinkedList::ListElement);
            Object::View v = value;
            if (v != NULL)
                {
                cb += v->sizeOf(/*fDeep*/ true);
                }

            // iterate backwards until previous == null
            for (ListElement::View vElm = previous; vElm != NULL; vElm = vElm->previous)
                {
                if (hSet->add(vElm))
                    {
                    cb += sizeof(LinkedList::ListElement);
                    v   = vElm->value;
                    if (v != NULL)
                        {
                        cb += v->sizeOf(/*fDeep*/ true);
                        }
                    }
                }

            // iterate forwards until next == null
            for (ListElement::View vElm = next; vElm != NULL; vElm = vElm->next)
                {
                if (hSet->add(vElm))
                    {
                    cb += sizeof(LinkedList::ListElement);
                    v   = vElm->value;
                    if (v != NULL)
                        {
                        cb += v->sizeOf(/*fDeep*/ true);
                        }
                    }
                }
            }
        catch (...)
            {
            if (fRoot)
                {
                hSet->clear();
                }
            throw;
            }
        if (fRoot)
            {
            hSet->clear();
            }
        }

    return cb;
    }


// ----- nested class: SubLinkedList ----------------------------------------

// ----- constructor ----------------------------------------------------

LinkedList::SubLinkedList::SubLinkedList(size32_t fromIndex, size32_t toIndex,
        List::Holder ohList)
    : super(fromIndex, toIndex, ohList)
    {
    }


// ----- List interface -------------------------------------------------

bool LinkedList::SubLinkedList::retainAll(Collection::View vColl)
    {
    LinkedList::Handle hList = cast<LinkedList::Handle>(getDelegate());

    size32_t idx = 0, iSize = m_iSize;
    for (ListElement::Handle hElement = ListElement::nextElement(
            (ListElement::Handle) hList->m_hHead, m_iOffset);
         NULL != hElement && idx < iSize; hElement = hElement->next)
        {
        if (!vColl->contains(hElement->value))
            {
            hList->removeElement(hElement, idx);
            --m_iSize;
            --iSize;
            }
        else
            {
            ++idx;
            }
        }

    return true;
    }

void LinkedList::SubLinkedList::clear()
    {
    getDelegate()->removeAll(this);
    m_iSize = 0;
    m_iOffset = 0;
    }

List::View LinkedList::SubLinkedList::
        subList(size32_t fromIndex, size32_t toIndex) const
    {
    return SubLinkedList::create(fromIndex, toIndex, View(this));
    }

List::Handle LinkedList::SubLinkedList::
        subList(size32_t fromIndex, size32_t toIndex)
    {
    return SubLinkedList::create(fromIndex, toIndex, Handle(this));
    }


// ----- constructor --------------------------------------------------------

LinkedList::LinkedList()
    : m_cElements(0),
      m_hHead(self(), NULL, /*fMutable*/ true),
      m_hTail(self(), NULL, /*fMutable*/ true)
    {
    }

LinkedList::LinkedList(Collection::View vc)
    : m_cElements(0),
      m_hHead(self(), NULL, /*fMutable*/ true),
      m_hTail(self(), NULL, /*fMutable*/ true)
    {
    addAll(vc);
    }

LinkedList::LinkedList(const LinkedList& that)
    : super(that),
      m_cElements(0),
      m_hHead(self(), NULL, /*fMutable*/ true),
      m_hTail(self(), NULL, /*fMutable*/ true)
    {
    Iterator::Handle hIterator = that.iterator();
    while (hIterator->hasNext())
        {
        add(hIterator->next()->clone());
        }
    }

LinkedList::~LinkedList()
    {
    clear(); // elements will be leaked otherwise
    }


// ----- List interface -----------------------------------------------------

bool LinkedList::add(size32_t i, Object::Holder oh)
    {
    assertIndex(i);
    ListElement::Handle hNewElement;

    ListElement::Handle hTail = m_hTail;
    ListElement::Handle hHead = m_hHead;

    // insert at the end
    if (i == size())
        {
        hNewElement = ListElement::create((ListElement::Handle) NULL, hTail, oh);
        if (is_null(hTail))
            {
            m_hTail = hNewElement;
            }
        else
            {
            hTail->next = hNewElement;
            m_hTail = hNewElement;
            }

        // handle the case of the first insert
        if (is_null(hHead))
            {
            m_hHead = hNewElement;
            }
        }

    // insert at the head
    else if (i == 0)
        {
        hNewElement = ListElement::create(hHead, (ListElement::Handle) NULL, oh);

        if (is_null(hHead))
            {
            m_hHead = hNewElement;
            }
        else
            {
            hHead->previous = hNewElement;
            m_hHead = hNewElement;
            }
        }
    // insert somewhere in the middle
    else
        {
        ListElement::Handle hInsertionPoint = ListElement::nextElement(
                (ListElement::Handle) m_hHead, i);
        hNewElement = ListElement::create(hInsertionPoint,
                (ListElement::Handle) hInsertionPoint->previous, oh);

        if (!is_null(hInsertionPoint->previous))
            {
            hInsertionPoint->previous->next = hNewElement;
            }

        hInsertionPoint->previous = hNewElement;
        }

    // increment the collection count
    ++m_cElements;

    return true;
    }

bool LinkedList::addAll(size32_t i, Collection::View vc)
    {
    Iterator::Handle hIter = vc->iterator();
    bool fResult = false;

    while(hIter->hasNext())
        {
        fResult |= add(i++, hIter->next());
        }

    return fResult;
    }

Object::Holder LinkedList::get(size32_t i) const
    {
    assertIndex(i);

    return ListElement::nextElement((ListElement::View)m_hHead, i)->value;
    }

size32_t LinkedList::indexOf(Object::View v) const
    {
    size32_t of = 0;
    for (ListElement::View vElement = m_hHead; NULL != vElement;
         vElement = vElement->next)
        {
        if (Object::equals(v, vElement->value))
            {
            return of;
            }
        ++of;
        }

    return List::npos;
    }

size32_t LinkedList::lastIndexOf(Object::View v) const
    {
    size32_t of = size() - 1;
    for (ListElement::View vElement = m_hTail; NULL != vElement;
         vElement = vElement->previous)
        {
        if (Object::equals(v, vElement->value))
            {
            return of;
            }
        --of;
        }

    return List::npos;
    }

ListIterator::Handle LinkedList::listIterator(size32_t index) const
    {
    return LinkedListIterator::create(View(this), ListElement::nextElement(
            (ListElement::View) m_hHead, index), index);
    }

ListMuterator::Handle LinkedList::listIterator(size32_t index)
    {
    return LinkedListIterator::create(Handle(this), ListElement::nextElement(
            (ListElement::Handle) m_hHead, index), index);
    }

Object::Holder LinkedList::remove(size32_t index)
    {
    ListElement::Handle hElement = ListElement::nextElement(
            (ListElement::Handle) m_hHead, index);
    Object::Holder hResult = removeElement(hElement, index);

    return hResult;
    }

Object::Holder LinkedList::set(size32_t idx, Object::Holder oh)
    {
    ListElement::Handle hElement = ListElement::nextElement(
            (ListElement::Handle) m_hHead, idx);
    Object::Holder hResult = hElement->value;
    hElement->value = oh;

    return hResult;
    }

List::View LinkedList::subList(size32_t fromIndex, size32_t toIndex) const
    {
    assertIndex(fromIndex);
    assertIndex(toIndex);

    return SubLinkedList::create(fromIndex, toIndex, View(this));
    }

List::Handle LinkedList::subList(size32_t fromIndex, size32_t toIndex)
    {
    assertIndex(fromIndex);
    assertIndex(toIndex);

    return SubLinkedList::create(fromIndex, toIndex, Handle(this));
    }


// ----- Collection interface -----------------------------------------------

size32_t LinkedList::size() const
    {
    return m_cElements;
    }

Iterator::Handle LinkedList::iterator() const
    {
    return LinkedListIterator::create(View(this), (ListElement::View) m_hHead, 0);
    }

Muterator::Handle LinkedList::iterator()
    {
    return LinkedListIterator::create(Handle(this), (ListElement::Handle) m_hHead, 0);
    }

bool LinkedList::add(Object::Holder oh)
    {
    return add(size(), oh);
    }

bool LinkedList::addAll(Collection::View vc)
    {
    Iterator::Handle hIter   = vc->iterator();
    bool             fResult = false;

    while (hIter->hasNext())
        {
        fResult |= add(hIter->next());
        }

    return fResult;
    }

bool LinkedList::remove(Object::View v)
    {
    size32_t idx     = 0;
    bool     fResult = false;

    for (ListElement::Handle hElement = m_hHead; NULL != hElement;
            hElement = hElement->next)
        {
        if (v->equals(hElement->value))
            {
            fResult |= (NULL != removeElement(hElement, idx));
            break;
            }
        else
            {
            ++idx;
            }
        }

    return fResult;
    }

bool LinkedList::removeAll(Collection::View vColl)
    {
    int32_t idx     = 0;
    bool    fResult = false;

    for (ListElement::Handle hElement = m_hHead; NULL != hElement;
            hElement = hElement->next)
        {
        if (vColl->contains(hElement->value))
            {
            fResult |= (NULL != removeElement(hElement, idx));
            }
        else
            {
            ++idx;
            }
        }
    return fResult;
    }


bool LinkedList::retainAll(Collection::View vColl)
    {
    size32_t idx = 0;
    for (ListElement::Handle hElement = m_hHead; hElement != NULL;
          hElement = hElement->next)
        {
        if (!vColl->contains(hElement->value))
            {
            removeElement(hElement, idx);
            }
        else
            {
            ++idx;
            }
        }

    return true;
    }

void LinkedList::clear()
    {
    m_cElements = 0;
    m_nHashCode = 0;

    // unlink the list elements, the list is doubly linked and unless we
    // unlink them they will be leaked, this manual unlinking is far cheaper
    // then using WeakHandles to link the ListElements
    ListElement::Handle hCurr, hNext;
    for (hCurr = m_hHead; hCurr != NULL; hCurr = hNext)
        {
        hNext           = hCurr->next;
        hCurr->previous = hCurr->next = NULL;
        }

    m_hHead = NULL;
    m_hTail = NULL;
    }

void LinkedList::onEscape(bool fEscaped) const
    {
    // Avoid the recursive traversal which the default impl would trigger.
    // With the linked list this would cause stack overflow for lists with a
    // few thousand elements
    if (fEscaped)
        {
        ListElement::Handle hHead = m_hHead;
        ListElement::Handle hTail = m_hTail;
        m_hHead = m_hTail = NULL; // unlink list

        // allow structure, but not ListElements to escape
        super::onEscape(fEscaped);

        // now add one element at a time back, causing iterative rather
        // then recursive escaping
        if (hHead != NULL)
            {
            ListElement::Handle hCurr = hHead->next;
            hHead->next = NULL;  // temp unlink next
            m_hHead     = hHead; // escape just head

            for (ListElement::Handle hNext; hCurr != NULL; hCurr = hNext)
                {
                hNext                 = hCurr->next;
                hCurr->next           = NULL;  // temp unlink next
                hCurr->previous->next = hCurr; // relink/escape just curr
                }

            m_hTail = hTail; // re-link tail
            }
        }
    else
        {
        // will never happen on a deep list as the doubly linked list will
        // prevent the refcount on the ListElements from being one unless
        // it is a list of just one element
        super::onEscape(fEscaped);
        }
    }


// ----- helper methods -----------------------------------------------------

void LinkedList::assertIndex(size32_t i) const
    {
    if (i > size())
        {
        COH_THROW (IndexOutOfBoundsException::create());
        }
    }

Object::Holder LinkedList::removeElement(ListElement::Handle hElement,
        size32_t index)
    {
    ListElement::Handle hPrevious = hElement->previous;
    ListElement::Handle hNext = hElement->next;

    if (NULL != hPrevious)
        {
        hPrevious->next = hNext;
        }

    if (NULL != hNext)
        {
        hNext->previous = hPrevious;
        }

     // Handle head or tail removal
    if (index == 0)
        {
        m_hHead = hNext;
        }

    if (index == size() - 1)
        {
        m_hTail = hPrevious;
        }

    --m_cElements;
    return hElement->value;
    }

void LinkedList::addElement(ListElement::Handle hNext, Object::Holder oh)
    {
    ListElement::Handle hAdd;
    if (NULL == hNext)
        {
        // add at tail
        m_hTail = hAdd = ListElement::create(hNext,
                (ListElement::Handle) m_hTail, oh);
        }
    else
        {
        hNext->previous = hAdd = ListElement::create(hNext,
                (ListElement::Handle) hNext->previous, oh);
        }

    if (hAdd->previous == NULL)
        {
        m_hHead = hAdd;
        }
    else
        {
        hAdd->previous->next = hAdd;
        }
    ++m_cElements;
    }

COH_CLOSE_NAMESPACE2

