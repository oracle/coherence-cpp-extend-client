/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SortedBag.hpp"

#include "coherence/util/TreeMap.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"

using coherence::util::comparator::SafeComparator;

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(SortedBag)

// ---- constants -------------------------------------------------------

Object::View getNoValue()
    {
    static FinalView<Object> vNOVALUE(System::common(), Object::create());
    return vNOVALUE;
    }
COH_STATIC_INIT(getNoValue());

// ----- local class: BagIterator -----------------------------------

/**
* An iterator over the bag's contents.
*/
class COH_EXPORT BagIterator
    : public class_spec<BagIterator,
        extends<Object>,
        implements<Iterator> >
    {
    friend class factory<BagIterator>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new BagIterator.
        *
        * @param hBag  the tree map being iterated over
        */
        BagIterator(SortedBag::Holder ohBag, Iterator::Handle hIterKeys)
                : f_ohBag(self(), ohBag),
                  f_hIter(self(), hIterKeys)
            {
            }

    // ----- Iterator interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            return f_hIter->hasNext();
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            return f_ohBag->unwrap(f_hIter->next());
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * The sorted bag.
        */
        FinalHolder<SortedBag> f_ohBag;

        /**
        * The inclusive lower range of this iterator
        */
        FinalHandle<Iterator> f_hIter;
    };


// ----- local class: MutatingBagIterator ---------------------------

/**
* MutatingBagIterator is mutating bag iterator, that is it is capable of
* changing the SortedBag that it iterates.
*/
class MutatingBagIterator
    : public class_spec<MutatingBagIterator,
        extends<BagIterator>,
        implements<Muterator> >
    {
    friend class factory<MutatingBagIterator>;

    // ----- constructors -------------------------------------------

    protected:
        MutatingBagIterator(SortedBag::Handle hBag, Iterator::Handle hIterKeys)
            : super(hBag, hIterKeys)
            {
            }

    // ----- Muterator interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            if (!instanceof<Muterator::Handle> (f_hIter))
                {
                COH_THROW (UnsupportedOperationException::create());
                }

            (cast<Muterator::Handle>(f_hIter))->remove();
            }
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors ---------------------------------------------------

SortedBag::SortedBag()
        : m_hAtomicNonce(self(), AtomicCounter::create(0L)),
          m_vComparator(self(), SafeComparator::getInstance()),
          m_hMap(self(), instantiateInternalMap(m_vComparator))
    {
    }

SortedBag::SortedBag(Comparator::View vComparator)
        : m_hAtomicNonce(self(), AtomicCounter::create(0L)),
          m_vComparator(self(), vComparator == NULL
                                    ? (Comparator::View) SafeComparator::getInstance()
                                    : vComparator),
          m_hMap(self(), instantiateInternalMap(m_vComparator))
    {
    }


// ----- accessors ------------------------------------------------------

NavigableMap::Handle SortedBag::getInternalMap() const
    {
    return m_hMap;
    }

Comparator::View SortedBag::getComparator() const
    {
    return m_vComparator;
    }

AtomicCounter::Handle SortedBag::getNonceCounter()
    {
    return m_hAtomicNonce;
    }


// ----- AbstractCollection methods -------------------------------------

size32_t SortedBag::size() const
    {
    return getInternalMap()->size();
    }

bool SortedBag::isEmpty() const
    {
    return getInternalMap()->isEmpty();
    }

bool SortedBag::contains(Object::View v) const
    {
    Object::Holder ohCeiling = getInternalMap()->ceilingKey(v);
    return ohCeiling != NULL && Object::equals(v, unwrap(ohCeiling));
    }

Iterator::Handle SortedBag::iterator() const
    {
    return BagIterator::create(this, getInternalMap()->keySet()->iterator());
    }

Muterator::Handle SortedBag::iterator()
    {
    return MutatingBagIterator::create(this, getInternalMap()->keySet()->iterator());
    }

bool SortedBag::add(Object::Holder oh)
    {
    // loop around a "conditional" put here to allow for a "safe" extension
    NavigableMap::Handle hMap = getInternalMap();
    do
        {
        if (Object::equals(oh, unwrap(hMap->ceilingKey(oh))))
            {
            // does not need to be "conditional", as all wrapped values are unique
            hMap->put(wrap(oh), getNoValue());
            return true;
            }
        else if (hMap->put(oh, getNoValue()) == NULL)
            {
            return true;
            }
        }
    while (true);
    }

bool SortedBag::remove(Object::View v)
    {
    // loop around a "conditional" remove here to allow for a "safe" extension
    NavigableMap::Handle hMap = getInternalMap();
    do
        {
        Object::View ovCeiling = hMap->ceilingKey(v);
        if (Object::equals(v, unwrap(ovCeiling)))
            {
            if (hMap->remove(ovCeiling) != NULL)
                {
                return true;
                }
            }
        else
            {
            return false;
            }
        }
    while (true);
    }

SortedBag::Handle SortedBag::subBag(Object::View vFrom, Object::View vTo)
    {
    return ViewBag::create(this, vFrom, vTo);
    }

SortedBag::View SortedBag::subBag(Object::View vFrom, Object::View vTo) const
    {
    return ViewBag::create(this, vFrom, vTo);
    }

SortedBag::Handle SortedBag::headBag(Object::View vTo)
    {
    return ViewBag::create(this, (Object::View) NULL, vTo);
    }

SortedBag::View SortedBag::headBag(Object::View vTo) const
    {
    return ViewBag::create(this, (Object::View) NULL, vTo);
    }

SortedBag::Handle SortedBag::tailBag(Object::View vFrom)
    {
    return ViewBag::create(this, vFrom, (Object::View) NULL);
    }

SortedBag::View SortedBag::tailBag(Object::View vFrom) const
    {
    return ViewBag::create(this, vFrom, (Object::View) NULL);
    }

Object::Holder SortedBag::first() const
    {
    return unwrap(getInternalMap()->firstKey());
    }

Object::Holder SortedBag::last() const
    {
    return unwrap(getInternalMap()->lastKey());
    }

Object::Holder SortedBag::removeFirst()
    {
    Map::Entry::Holder hEntryRemoved = getInternalMap()->pollFirstEntry();
    return hEntryRemoved == NULL ? NULL : unwrap(hEntryRemoved->getKey());
    }

Object::Holder SortedBag::removeLast()
    {
    Map::Entry::Holder hEntryRemoved = getInternalMap()->pollLastEntry();
    return hEntryRemoved == NULL ? NULL : unwrap(hEntryRemoved->getKey());
    }


// ----- helper methods -------------------------------------------------

NavigableMap::Handle SortedBag::instantiateInternalMap(Comparator::View vComparator)
    {
    return TreeMap::create(WrapperComparator::create(vComparator));
    }

SortedBag::UniqueElement::View SortedBag::wrap(Object::Holder oh)
    {
    return UniqueElement::create(this, oh);
    }

Object::Holder SortedBag::unwrap(Object::View v) const
    {
    return instanceof<UniqueElement::View>(v) ?
            (cast<UniqueElement::View> (v))->f_ohElem : v;
    }


// ----- inner class: WrapperComparator ---------------------------------

SortedBag::WrapperComparator::WrapperComparator(Comparator::View vComparator)
        : f_vComparator(self(), vComparator)
    {
    }

int32_t SortedBag::WrapperComparator::compare(Object::View v1, Object::View v2) const
    {
    if (instanceof<UniqueElement::View>(v1))
        {
        return (cast<UniqueElement::View>(v1))->compareTo(v2);
        }
    else if (instanceof<UniqueElement::View>(v2))
        {
        return -(cast<UniqueElement::View>(v2))->compareTo(v1);
        }
    else
        {
        // v1 and v2 are both "real" elements
        return f_vComparator->compare(v1, v2);
        }
    }


// ----- inner class: UniqueElement -------------------------------------

SortedBag::UniqueElement::UniqueElement(SortedBag::Handle hBag, Object::Holder oh)
        : f_nUniqueId(hBag->getNonceCounter()->increment()), f_ohElem(self(), oh),
          f_hBagOuter(self(), hBag)
    {
    }

int32_t SortedBag::UniqueElement::compareTo(Object::View v) const
    {
    if (!instanceof<UniqueElement::View>(v))
        {
        int32_t nCompare = f_hBagOuter->getComparator()->compare(f_ohElem, v);

        // this must be comparing to a "plain" or "actual" element;
        // order the actual elements first if they are equivalent
        return nCompare < 0 ? -1 : 1;
        }
    else
        {
        UniqueElement::View vThat = cast<UniqueElement::View>(v);
        int32_t nCompare = f_hBagOuter->getComparator()->compare(f_ohElem, vThat->f_ohElem);
        int64_t nIdThis  = f_nUniqueId;
        int64_t nIdThat  = vThat->f_nUniqueId;

        return nCompare == 0
                ? nIdThis == nIdThat
                    ? 0
                    : nIdThis < nIdThat
                        ? -1 : 1
                : nCompare;
        }
    }


// ----- inner class: ViewBag -------------------------------------------

SortedBag::ViewBag::ViewBag(SortedBag::Holder ohBag, Object::View vFrom, Object::View vTo)
        : super(), f_ohBag(self(), ohBag), f_vFrom(self(), vFrom), f_vTo(self(), vTo)
    {
    NavigableMap::Handle hMapInternal = ohBag->getInternalMap();

    m_hMap = vFrom == NULL
            ? hMapInternal->headMap(vTo, false)
            : vTo == NULL
                ? hMapInternal->tailMap(vFrom, true)
                : hMapInternal->subMap(vFrom, true, vTo, false);
    m_hAtomicNonce = ohBag->m_hAtomicNonce;
    m_vComparator  = ohBag->m_vComparator;
    }

bool SortedBag::ViewBag::add(Object::Holder oh)
    {
    checkRange(oh);

    return super::add(oh);
    }

SortedBag::Handle SortedBag::ViewBag::subBag(Object::View vFrom, Object::View vTo)
    {
    checkRange(vFrom);
    checkRange(vTo);

    return super::subBag(vFrom, vTo);
    }

SortedBag::View SortedBag::ViewBag::subBag(Object::View vFrom, Object::View vTo) const
    {
    checkRange(vFrom);
    checkRange(vTo);

    return super::subBag(vFrom, vTo);
    }

SortedBag::Handle SortedBag::ViewBag::headBag(Object::View vTo)
    {
    checkRange(vTo);

    return super::headBag(vTo);
    }

SortedBag::View SortedBag::ViewBag::headBag(Object::View vTo) const
    {
    checkRange(vTo);

    return super::headBag(vTo);
    }

SortedBag::Handle SortedBag::ViewBag::tailBag(Object::View vFrom)
    {
    checkRange(vFrom);

    return super::tailBag(vFrom);
    }

SortedBag::View SortedBag::ViewBag::tailBag(Object::View vFrom) const
    {
    checkRange(vFrom);

    return super::tailBag(vFrom);
    }

void SortedBag::ViewBag::checkRange(Object::View v) const
    {
    Comparator::View comparator = getComparator();
    if ((f_vFrom != NULL && comparator->compare(v, f_vFrom) < 0) ||
        (f_vTo   != NULL && comparator->compare(v, f_vTo) >= 0))
        {
        COH_THROW (IllegalStateException::create());
        }
    }

COH_CLOSE_NAMESPACE2
