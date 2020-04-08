/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/CircularArrayList.hpp"

#include "coherence/util/ConcurrentModificationException.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_TYPED_CLASS(CircularArrayList);


// ------ helper class: CircularArrayListIterator ---------------------------

class COH_EXPORT CircularArrayListIterator
    : public class_spec<CircularArrayListIterator,
        extends<Object>,
        implements<ListMuterator> >
    {
    friend class factory<CircularArrayListIterator>;

    // ----- constructor ------------------------------------------------

    protected:
        CircularArrayListIterator(CircularArrayList::Holder ohList,
                size32_t index)
            : m_iReturned(List::npos), m_iNext(index),
              m_cModExpected(ohList->m_cMod), f_ohList(self(), ohList)
            {
            if (index > ohList->size())
                {
                COH_THROW_STREAM(IndexOutOfBoundsException,
                        "Bad Index: " << index);
                }
            }


    // ----- Iterator interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasPrevious() const
            {
            return m_iNext != 0;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t nextIndex() const
            {
            return m_iNext;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t previousIndex() const
            {
            size32_t iNext = m_iNext;
            return iNext == 0 ? List::npos : iNext - 1;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder previous()
            {
            size32_t iNext = m_iNext;
            if (iNext == 0)
                {
                COH_THROW (NoSuchElementException::create());
                }

            CircularArrayList::View vList   = getList();
            size32_t                iReturn = (List::npos == iNext)
                    ? vList->size() - 1
                    : iNext - 1;

            checkForConcurrentModification();
            m_iNext = m_iReturned = iReturn;

            return vList->get(iReturn);
            }

        /**
        * {@inheritDoc}
        */
        virtual void add(Object::Holder oh)
            {
            checkForConcurrentModification();
            CircularArrayList::Handle hList = getMutableList();
            hList->add(m_iNext, oh);
            ++m_iNext;
            m_iReturned    = List::npos;
            m_cModExpected = hList->m_cMod;
            }

        /**
        * {@inheritDoc}
        */
        virtual void set(Object::Holder oh)
            {
            checkForConcurrentModification();
            size32_t iReturned = m_iReturned;
            COH_ENSURE(List::npos != iReturned);
            CircularArrayList::Handle hList = getMutableList();
            hList->set(iReturned, oh);
            m_cModExpected = hList->m_cMod;
            }


    // ----- Iterator interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            return m_iNext != getList()->size();
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            CircularArrayList::View vList = getList();
            if (m_iNext == vList->size())
                {
                COH_THROW (NoSuchElementException::create());
                }

            checkForConcurrentModification();

            size32_t iReturned = m_iReturned = m_iNext++;
            return vList->get(iReturned);
            }

        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            size32_t iReturned = m_iReturned;
            COH_ENSURE(List::npos != iReturned);
            checkForConcurrentModification();
            CircularArrayList::Handle hList = getMutableList();

            hList->remove(iReturned);
            m_cModExpected = hList->m_cMod;

            if (iReturned < m_iNext)
                {
                --m_iNext;
                }
            m_iReturned = List::npos;
            }

    // ----- helper methods ----------------------------------------

    protected:
        /**
        * Return the mutable List
        *
        * @throws UnsupportedOperationException if the list is not mutable
        */
        CircularArrayList::Handle getMutableList()
            {
            CircularArrayList::Handle hList =
                cast<CircularArrayList::Handle>(f_ohList, /*fThrow*/ false);
            if (NULL == hList)
                {
                COH_THROW (UnsupportedOperationException::create());
                }
            return hList;
            }

        /**
        * Return the List
        */
        CircularArrayList::View getList() const
            {
            return f_ohList;
            }

        /**
        * Test for concurrent modifications
        */
        void checkForConcurrentModification()
            {
            if (m_cModExpected != f_ohList->m_cMod)
                {
                COH_THROW (ConcurrentModificationException::create());
                }
            }

    // ----- data members ------------------------------------------

    protected:
        /**
        * The last index of the element returned from the iterator, or
        * List::npos if none has been returned, or if an add or remove was
        * performed since the last element was retruned.
        */
        size32_t m_iReturned;

        /**
        * The current Index of this iterator in the list.
        */
        size32_t m_iNext;

        /**
        * The expected modification count which is used
        * .
        */
        size32_t m_cModExpected;

        /**
        * Reference to the list.
        */
        FinalHolder<CircularArrayList> f_ohList;
    };



// ----- nested class: SubCircularArrayList ---------------------------------

// ----- constructor --------------------------------------------------------

CircularArrayList::SubCircularArrayList::SubCircularArrayList(
        size32_t fromIndex, size32_t toIndex, List::Holder ohList)
    : super(fromIndex, toIndex, ohList)
    {
    }


// ----- List interface -----------------------------------------------------

bool CircularArrayList::SubCircularArrayList::retainAll(
        Collection::View vColl)
    {
    CircularArrayList::Handle hList    = cast<CircularArrayList::Handle>(
            getDelegate());
    ObjectArray::Handle       haoData  = hList->m_haoData;
    size32_t                  cSlots   = hList->m_haoData->length;
    size32_t                  iFirst   = hList->m_iFirst;
    bool                      fChanged = false;
    for (size32_t i = m_iOffset, cChecked = 0, c = m_iSize;
        cChecked < c; ++cChecked)
        {
        if (vColl->contains(haoData[(iFirst + i) % cSlots]))
            {
            ++i;
            }
        else
            {
            hList->remove(i);
            haoData  = hList->m_haoData;
            cSlots   = hList->m_haoData->length;
            iFirst   = hList->m_iFirst;
            fChanged = true;
            --m_iSize;
            }
        }

    return fChanged;
    }

void CircularArrayList::SubCircularArrayList::clear()
    {
    cast<CircularArrayList::Handle>(getDelegate())->removeRange(
            m_iOffset, m_iOffset + m_iSize);
    m_iSize   = 0;
    m_iOffset = 0;
    }

List::View CircularArrayList::SubCircularArrayList::
        subList(size32_t fromIndex, size32_t toIndex) const
    {
    if (toIndex > m_iSize || fromIndex > toIndex)
        {
        COH_THROW(IndexOutOfBoundsException::create());
        }
    size32_t iOffset = m_iOffset;
    return SubCircularArrayList::create(iOffset + fromIndex,
            iOffset + toIndex, cast<CircularArrayList::View>(getDelegate()));
    }

List::Handle CircularArrayList::SubCircularArrayList::
        subList(size32_t fromIndex, size32_t toIndex)
    {
    if (toIndex > m_iSize || fromIndex > toIndex)
        {
        COH_THROW(IndexOutOfBoundsException::create());
        }
    size32_t iOffset = m_iOffset;
    return SubCircularArrayList::create(iOffset + fromIndex,
            iOffset + toIndex, cast<CircularArrayList::Handle>(getDelegate()));
    }


// ----- constructor --------------------------------------------------------

CircularArrayList::CircularArrayList(size32_t cInitialElements)
    : m_haoData(self(), ObjectArray::create(cInitialElements)),
      m_iFirst(0), m_iLast(0), m_cElements(0), m_cMod(0)
    {
    }

CircularArrayList::CircularArrayList(Collection::View vc)
    : m_haoData(self(), ObjectArray::create(vc->size() + 1)),
      m_iFirst(0), m_iLast(0), m_cElements(0), m_cMod(0)
    {
    addAll(vc);
    }

CircularArrayList::CircularArrayList(const CircularArrayList& that)
    : m_haoData(self(), cast<ObjectArray::Handle>(that.m_haoData->clone())),
      m_iFirst(that.m_iFirst), m_iLast(that.m_iLast),
      m_cElements(that.m_cElements), m_cMod(0)
    {
    }


// ----- CircularArrayList interface ----------------------------------------

void CircularArrayList::trimToSize()
    {
    ++m_cMod;
    size32_t cElements = m_cElements;
    if (cElements + 1 < m_haoData->length)
        {
        ObjectArray::Handle haoNewData =  ObjectArray::create(cElements + 1);
        toArray(haoNewData);
        m_haoData = haoNewData;
        m_iFirst  = 0;
        m_iLast   = cElements;
        }
    }

bool CircularArrayList::ensureCapacity(size32_t cMin)
    {
    size32_t cOld = m_haoData->length;
    if (cMin > cOld)
        {
        size32_t cNew = std::max(cMin, ((cOld * 3) / 2 + 1));

        m_haoData = toArray(ObjectArray::create(cNew));
        m_iLast   = m_cElements;
        m_iFirst  = 0;

        ++m_cMod;
        return true;
        }

    return false;
    }


// ----- List interface -----------------------------------------------------

bool CircularArrayList::add(size32_t i, Object::Holder oh)
    {
    size32_t cElements = m_cElements;
    if (i == cElements)
        {
        // append to tail
        return add(oh);
        }

    rangeCheck(i);
    ensureCapacity(1 + cElements + 1); // must have one empty slot

    ObjectArray::Handle haoData = m_haoData;
    size32_t            cSlots  = haoData->length;
    size32_t            iFirst  = m_iFirst;
    size32_t            iLast   = m_iLast;
    size32_t            iEff    = effectiveIndex(i);

    if (iEff == iFirst)
        {
        // insert before head
        m_iFirst = iFirst = (iFirst - 1 + cSlots) % cSlots;
        haoData[iFirst] = oh;
        }
    else if (((iFirst > iLast) && (iEff > iFirst)) || iLast == cSlots - 1)
        {
        // ... m_iFirst o o o indx o o o m_iLast
        // o o o m_iLast ... m_iFirst o o o indx o o o
        // so we have space in front of m_iFirst

        // shift head to the right
        ObjectArray::copy(haoData, iFirst,
                haoData, iFirst - 1, iEff - iFirst);
        --m_iFirst;
        haoData[iEff - 1] = oh;
        ++m_cMod;
        }
    else // iEff < iLast
        {
        // m_iFirst o o o indx o o o m_iLast ...
        // ... m_iFirst o o o indx o o o m_iLast ...
        // or o o o indx o o o m_iLast ... m_iFirst o o o

        // shift tail to the left
        ObjectArray::copy(haoData, iEff, haoData, iEff + 1, iLast - iEff);
        m_iLast = (iLast + 1) % cSlots;
        haoData[iEff] = oh;
        ++m_cMod;
        }
    ++m_cElements;
    return true;
    }

bool CircularArrayList::addAll(size32_t index, Collection::View vc)
    {
    size32_t cElements = m_cElements;
    if (index == cElements)
        {
        // append to tail
        return addAll(vc);
        }

    rangeCheck(index);

    size32_t cNew = vc->size();
    ensureCapacity(1 + cElements + cNew); // must one empty slot

    ObjectArray::Handle haoData = m_haoData;
    size32_t            cSlots  = haoData->length;
    size32_t            iFirst  = m_iFirst;
    size32_t            iLast   = m_iLast;
    size32_t            iEff    = effectiveIndex(index);
    size32_t            iNew    = 0;
    Iterator::Handle    hIter   = vc->iterator();

    if (iEff == iFirst)
        {
        // insert at head (no shifting required)
        m_iFirst = (iFirst - cNew + cSlots) % cSlots;
        }
    else if ((iFirst > iLast && iEff > iFirst) ||
             (iFirst < iLast && iFirst >= cNew))
        {
        // o o o m_iLast ... m_iFirst o o o iEff o o o
        // or ... (lots of room) ... iFirst o o o iLast ...
        // there is guaranteed room before m_iFirst
        // shift head to the left
        ObjectArray::copy(haoData, iFirst,
                haoData, iFirst - cNew, iEff - iFirst);
        m_iFirst = (iFirst - cNew + cSlots) % cSlots;
        }
    // ... (too small) ... m_iFirst o o o iEff o o o m_iLast ...
    // or o o o iEff o o o m_iLast ... m_iFirst o o o
    else if (iLast + cNew <= cSlots) // || (iFirst > iLast)
        {
        // if o o o iEff o o o m_iLast ... m_iFirst o o o
        //     there is guaranteed room after m_iLast
        // if ... m_iFirst o o o iEff o o o m_iLast ...
        //     room in this case too otherwise test would have failed

        // shift tail to the right
        ObjectArray::copy(haoData, iEff,
               haoData, iEff + cNew, iLast - iEff);
        m_iLast = (iLast + cNew) % cSlots;
        }
    else // must be ... m_iFirst o o o iEff o o o m_iLast ...
        {
        // shoot, we already knew there was no room after iLast,
        // therefore we must do two moves
        // shift head to the left; tail to the right
        ObjectArray::copy(haoData, iFirst,
              haoData, 0, iEff - iFirst);
        size32_t cRight = cNew - iFirst;
        ObjectArray::copy(haoData, iEff,
              haoData, iEff + cRight, iLast - iEff);

        m_iFirst = 0;
        m_iLast  = iLast + cRight;
        }

    iEff = effectiveIndex(index); // m_iFirst might have moved
    for (; iNew < cNew && hIter->hasNext(); ++iNew)
        {
        haoData[iEff] = hIter->next();
        iEff         = (iEff + 1) % cSlots;
        }
    m_cElements += cNew;
    ++m_cMod;

    if (iNew != cNew) // collection lost elements somehow
        {
        removeRange(index + iNew, index + cNew + 1);
        }

    return true;
    }

Object::Holder CircularArrayList::get(size32_t i) const
    {
    return ((ObjectArray::View) m_haoData)[ensureEffectiveIndex(i)];
    }

size32_t CircularArrayList::indexOf(Object::View v) const
    {
    ObjectArray::View vaoData = m_haoData;
    size32_t          cSlots  = vaoData->length;
    size32_t          iFirst  = m_iFirst;

    for (size32_t i = 0, c = m_cElements; i < c; ++i)
        {
        if (Object::equals(v, vaoData[(iFirst + i) % cSlots]))
            {
            return i;
            }
        }

    return List::npos;
    }

size32_t CircularArrayList::lastIndexOf(Object::View v) const
    {
    size32_t          cElements = m_cElements;
    size32_t          iFirst    = m_iFirst;
    ObjectArray::View vaoData   = m_haoData;
    size32_t          cSlots    = vaoData->length;

    for (size32_t i = cElements; i != 0; i--)
        {
        if (Object::equals(v, vaoData[(i + iFirst - 1) % cSlots]))
            {
            return i-1;
            }
        }
    return List::npos;
   }

ListIterator::Handle CircularArrayList::listIterator(size32_t index) const
    {
    return CircularArrayListIterator::create(View(this), index);
    }

ListMuterator::Handle CircularArrayList::listIterator(size32_t index)
    {
    return CircularArrayListIterator::create(Handle(this), index);
    }

Object::Holder CircularArrayList::remove(size32_t index)
    {
    ObjectArray::Handle haoData    = m_haoData;
    size32_t            iEff       = ensureEffectiveIndex(index);
    size32_t            iFirst     = m_iFirst;
    size32_t            iLast      = m_iLast;
    Object::Holder      hoOldValue = haoData[iEff];

    // optimized for removing from front.  This test and code block
    // are really the only reason we wrote this class
    if (iEff == iFirst)
        {
        haoData[iEff] = NULL;
        m_iFirst      = (iFirst + 1) % haoData->length;
        }
    else if ((iFirst > iLast) && (iEff > iFirst))
        {
        // o o o m_LastIndex ... m_iFirst o o o iEff o o o

        // shift head to the right (overwritting iEff)
        ObjectArray::copy(haoData, iFirst,
                haoData, iFirst + 1, iEff - iFirst);
        haoData[m_iFirst++] = NULL;
        }
    else // iEff < iLast
        {
        // ... m_iFirst o o o indx o o o m_iLast
        // m_iFirst o o o indx o o o m_iLast ...
        // ... m_iFirst o o o indx o o o m_iLast ...
        // o o o indx o o o m_iLast ... m_iFirst o o o

        // shift tail to the left (overitting iEff)
        ObjectArray::copy(haoData, iEff + 1,
                haoData, iEff, iLast - iEff - 1);
        haoData[--m_iLast] = NULL;
        }

    --m_cElements;
    ensureCompactness();
    ++m_cMod;

    return hoOldValue;
    }

Object::Holder CircularArrayList::set(size32_t idx, Object::Holder oh)
    {
    ObjectArray::Handle haoData    = m_haoData;
    size32_t            iEff       = ensureEffectiveIndex(idx);
    Object::Holder      hoOldValue = haoData[iEff];

    haoData[iEff] = oh;
    return hoOldValue;
    }

List::View CircularArrayList::subList(size32_t fromIndex,
        size32_t toIndex) const
    {
    rangeCheck(fromIndex);
    rangeCheck(toIndex);

    return SubCircularArrayList::create(fromIndex, toIndex, View(this));
    }

List::Handle CircularArrayList::subList(size32_t fromIndex, size32_t toIndex)
    {
    rangeCheck(fromIndex);
    rangeCheck(toIndex);

    return SubCircularArrayList::create(fromIndex, toIndex, Handle(this));
    }


// ----- Collection interface -----------------------------------------------

size32_t CircularArrayList::size() const
    {
    return m_cElements;
    }

Iterator::Handle CircularArrayList::iterator() const
    {
    return CircularArrayListIterator::create(View(this), 0);
    }

Muterator::Handle CircularArrayList::iterator()
    {
    return CircularArrayListIterator::create(Handle(this), 0);
    }

ObjectArray::Handle CircularArrayList::toArray(ObjectArray::Handle hao) const
    {
    ObjectArray::View haoData   = m_haoData;
    size32_t          cSlots    = haoData->length;
    size32_t          iFirst    = m_iFirst;
    size32_t          iLast     = m_iLast;
    size32_t          cElements = m_cElements;

    if (hao == NULL || hao->length < cElements)
        {
        hao = ObjectArray::create(cElements);
        }
    else if (hao->length > cElements)
        {
        hao[cElements] = NULL;
        }

    if (cElements > 0)
        {
        if (iFirst < iLast)
            {
            ObjectArray::copy(haoData, iFirst, hao, 0, cElements);
            }
        else
            {
            ObjectArray::copy(haoData, iFirst, hao, 0, cSlots  - iFirst);
            ObjectArray::copy(haoData, 0, hao, cSlots - iFirst, iLast);
            }
        }

    return hao;
    }

bool CircularArrayList::add(Object::Holder oh)
    {
    // ensureCapacity may increments modCount
    ensureCapacity(1 + m_cElements + 1); // must have one extra slot

    ObjectArray::Handle haoData = m_haoData;
    size32_t            iLast   = m_iLast;

    haoData[iLast] = oh;
    m_iLast        = (iLast + 1) % haoData->length;

    ++m_cElements;
    return true;
    }

bool CircularArrayList::addAll(Collection::View vc)
    {
    // we could have called addAll(m_cElements, c)
    // but this is a common case and the size is optimizable friendly in java
    size32_t cNew = vc->size();
    if (cNew == 0)
        {
        return false;
        }

    ensureCapacity(1 + m_cElements + cNew); // must have one extra slot

    ObjectArray::Handle haoData = m_haoData;
    size32_t            cSlots  = haoData->length;
    size32_t            iLast   = m_iLast;
    size32_t            iNew    = 0;
    Iterator::Handle    hIter   = vc->iterator();

    for (; iNew < cNew && hIter->hasNext(); ++iNew)
        {
        haoData[iLast] = hIter->next();
        iLast         = (iLast + 1) % cSlots;
        }

    m_iLast      = iLast;
    m_cElements += iNew;
    return true;
    }

bool CircularArrayList::remove(Object::View v)
    {
    size32_t index = indexOf(v);
    if (index == List::npos)
        {
        return false;
        }
    remove(index);
    return true;
    }

bool CircularArrayList::removeAll(Collection::View vColl)
    {
    bool               fResult = false;
    Iterator::Handle    hIter  = vColl->iterator();

    while (hIter->hasNext())
        {
        fResult |= remove(hIter->next());
        }

    return fResult;
    }

bool CircularArrayList::retainAll(Collection::View vColl)
    {
    ObjectArray::Handle haoData = m_haoData;
    size32_t            cSlots  = haoData->length;
    size32_t            iFirst  = m_iFirst;
    bool                fChanged  = false;
    for (size32_t i = 0, cChecked = 0, c = m_cElements; cChecked < c;
            ++cChecked)
        {
        if (vColl->contains(haoData[(iFirst + i) % cSlots]))
            {
            ++i;
            }
        else
            {
            remove(i);
            haoData  = m_haoData;
            cSlots   = haoData->length;
            iFirst   = m_iFirst;
            fChanged = true;
            }
        }

    return fChanged;
    }

void CircularArrayList::clear()
    {
    ObjectArray::Handle haoData = m_haoData;
    size32_t            cSlots  = haoData->length;
    size32_t            iFirst  = m_iFirst;
    size32_t            iLast   = m_iLast;

    m_iFirst = m_iLast = m_cElements = 0;

    ensureCompactness();

    // if ensure didn't create a new array, null out old data
    if (m_haoData == haoData)
        {
        for (size32_t i = iFirst; i != iLast; i = (i + 1) % cSlots)
            {
            haoData[i] = NULL;
            }
        }
    ++m_cMod;
    }


// ----- helper methods -----------------------------------------------------

size32_t CircularArrayList::effectiveIndex(size32_t index) const
    {
    return (m_iFirst + index) % m_haoData->length;
    }

void CircularArrayList::rangeCheck(size32_t index) const
    {
    if (index >= m_cElements)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "Bad Index: " << index);
        }
    }

size32_t CircularArrayList::ensureEffectiveIndex(size32_t index) const
    {
    if (index >= m_cElements)
        {
        COH_THROW_STREAM(IndexOutOfBoundsException, "Bad Index: " << index);
        }
    return (m_iFirst + index) % m_haoData->length;
    }

bool CircularArrayList::ensureCompactness()
    {
    return false;
    }

void CircularArrayList::removeRange(size32_t fromIndex, size32_t toIndex)
    {
    rangeCheck(fromIndex);
    rangeCheck(toIndex - 1);

    if (fromIndex >= toIndex)
        {
        return;
        }

    ObjectArray::Handle haoData  = m_haoData;
    size32_t            cSlots   = haoData->length;
    size32_t            iFrom    = effectiveIndex(fromIndex);
    size32_t            iTo      = effectiveIndex(toIndex);
    size32_t            iFirst   = m_iFirst;
    size32_t            iLast    = m_iLast;
    size32_t            cRemoved = iTo - iFrom;

    if (iFirst < iLast || (iFrom < iLast && iTo < iLast))
        {
        // ... iFirst o o o [from o o o to) o o o iLast ...
        // or
        // o o o [from o o o to) o o o iLast ... iFirst o o o
        size32_t cMoved   = iLast - iTo;
        size32_t iNewLast = iLast - cRemoved;
        ObjectArray::copy(haoData, iTo, haoData, iFrom, cMoved);
        while (iLast != iNewLast)
            {
            haoData[--iLast] = NULL;
            }
        m_iLast = iLast;
        }
    else if (iFrom >= iFirst && iTo > iFirst)
        {
        // o o o iLast ... iFirst o o o [from o o o to) o o o
        size32_t cMoved    = iFrom - iFirst;
        size32_t iNewFirst = iTo   - cMoved;
        ObjectArray::copy(haoData, iFirst,
                haoData, iNewFirst, cMoved);
        while (iFirst != iNewFirst)
            {
            haoData[iFirst++] = NULL;
            }
        m_iFirst = iFirst;
        }
    else // if (iFrom >= iFirst && iTo <= iLast)
        {
        // o o o to) o o o iLast ... iFirst o o o [from o o o

        // shift head to the right
        size32_t cMovedEnd = iFrom  - iFirst;
        size32_t iNewFirst = cSlots - cMovedEnd;

        if (cMovedEnd > 0)
            {
            ObjectArray::copy(haoData, iFirst,
                    haoData, iNewFirst, cMovedEnd);
            }

        while (iFirst != iNewFirst)
            {
            haoData[iFirst++] = NULL;
            }

        // iFrom could have been == iFirst
        m_iFirst = iNewFirst % cSlots;

        if (iTo != 0)
            {
            // shift tail to the left
            size32_t iNewLast = iLast - iTo;

            ObjectArray::copy(haoData, iTo, haoData, 0, iNewLast);

            while (iLast != iNewLast)
                {
                haoData[--iLast] = NULL;
                }
            m_iLast = iLast;
            }
        }

    m_cElements -= cRemoved;
    ensureCompactness();
    ++m_cMod;
    }

COH_CLOSE_NAMESPACE2
