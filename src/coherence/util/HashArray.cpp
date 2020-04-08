/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/HashArray.hpp"

#include "coherence/util/Collections.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/Set.hpp"

using coherence::util::Set;

COH_OPEN_NAMESPACE2(coherence,lang)
    // defined in Object.cpp
    extern Set::Handle coh_getThreadLocalSizeOfSet();
COH_CLOSE_NAMESPACE2


COH_OPEN_NAMESPACE2(coherence,util)

// ----- constructors -------------------------------------------------------

HashArray::HashArray(size32_t cEstimate, float32_t flLoadFactor,
        float32_t flGrowthRate)
    : m_lMax(-1),
      f_hStorage(self(), Storage::create(value(), cEstimate,
              flLoadFactor, flGrowthRate), /*fMutable*/ true)
    {
    }

HashArray::~HashArray()
    {
    clear(); // else we will leak
    }


// ----- LongArray interface ------------------------------------------------

Object::Holder HashArray::get(int64_t lIndex) const
    {
    COH_SYNCHRONIZED_MEMBER_READ
        {
        // note we can't move the implicit attach outside of sync block
        // as there would be no guarentee that the object would not be
        // concurrently removed by another thread and potentially deleted
        return f_hStorage->get(lIndex);
        }
    }

Object::Holder HashArray::set(int64_t lIndex, Object::Holder ohValue)
    {
    bool  fEscaped = false;
    value valNew(ohValue);
    value valOld;

    COH_SYNCHRONIZED_MEMBER_WRITE
        {
        if (lIndex > m_lMax)
            {
            m_lMax = lIndex;
            }
        value& valCurr = f_hStorage->at(lIndex);

        // attach to new
        fEscaped = _isEscaped();
        valNew.attach(fEscaped);

        valOld  = valCurr; // record old
        valCurr = valNew;  // update storage
        }

    Object::Holder ohOld = valOld;

    // detach old; outside of sync for performance
    valOld.detach(fEscaped);

    return ohOld;
    }

int64_t HashArray::add(Object::Holder ohValue)
    {
    bool    fEscaped = false;
    value   valNew(ohValue);
    value   valOld;
    int64_t lIndex;

    COH_SYNCHRONIZED_MEMBER_WRITE
        {
        lIndex = ++m_lMax; // incr/assign index

        value& valCurr = f_hStorage->at(lIndex);

        // attach to new
        fEscaped = _isEscaped();
        valNew.attach(fEscaped);

        valOld  = valCurr; // record old
        valCurr = valNew;  // update storage
        }

    // detach old; outside of sync for performance
    valOld.attach(fEscaped);

    return lIndex;
    }

bool HashArray::exists(int64_t lIndex) const
    {
    COH_SYNCHRONIZED_MEMBER_READ
        {
        return f_hStorage->containsKey(lIndex);
        }
    }

Object::Holder HashArray::remove(int64_t lIndex)
    {
    bool fEscaped = false;
    value valOld;
    COH_SYNCHRONIZED_MEMBER_WRITE
        {
        if (lIndex == m_lMax)
            {
            --m_lMax; // just a guess
            }
        valOld   = f_hStorage->erase(lIndex);
        fEscaped = _isEscaped();
        }

    Object::Holder ohOld = valOld;

    // detach old; outside of sync for performance
    valOld.detach(fEscaped);

    return ohOld;
    }

bool HashArray::contains(Object::View vElement) const
    {
    value val(get_pointer(vElement));
    COH_SYNCHRONIZED_MEMBER_READ
        {
        return f_hStorage->containsValue(val);
        }
    }

void HashArray::clear()
    {
    Storage::Handle hStorage = f_hStorage;
    COH_SYNCHRONIZED_MEMBER_WRITE
        {
        // detach from all objects
        bool              fEscaped = _isEscaped();
        Storage::Element* aElement = hStorage->m_aElements;
        size32_t          cSlots   = hStorage->getSlots();
        for (size_t i = 0; i < cSlots; ++i)
            {
            if (aElement[i].state == Storage::mapped)
                {
                aElement[i].value.detach(fEscaped);
                }
            }

        m_lMax = -1;
        f_hStorage->clear();
        }
    }

bool HashArray::isEmpty() const
    {
    return f_hStorage->size() == 0;
    }

size32_t HashArray::getSize() const
    {
    return f_hStorage->size();
    }

LongArrayIterator::Handle HashArray::iterator()
    {
    return HashIterator::create(this);
    }

LongArrayIterator::Handle HashArray::iterator() const
    {
    return HashIterator::create(this);
    }

LongArrayIterator::Handle HashArray::iterator(int64_t /*lIndex*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

LongArrayIterator::Handle HashArray::iterator(int64_t /*lIndex*/) const
    {
    COH_THROW (UnsupportedOperationException::create());
    }

int64_t HashArray::getFirstIndex() const
    {
    COH_THROW (UnsupportedOperationException::create());
    }

int64_t HashArray::getLastIndex() const
    {
    COH_THROW (UnsupportedOperationException::create());
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> HashArray::toString() const
    {
    return Collections::toString((Iterator::Handle) iterator());
    }

void HashArray::onEscape(bool fEscaped) const
    {
    super::onEscape(fEscaped);

    // single thread, no need for locking
    Storage::Handle    hStorage = f_hStorage;
    Storage::Element*  aElement = hStorage->m_aElements;
    size32_t           cSlots   = hStorage->getSlots();
    for (size_t i = 0; i < cSlots; ++i)
        {
        if (aElement[i].state == Storage::mapped)
            {
            aElement[i].value.attach(fEscaped);
            aElement[i].value.detach(!fEscaped);
            }
        }
    }

size64_t HashArray::sizeOf(bool fDeep) const
    {
    if (fDeep)
        {
        Set::Handle hSet = coh_getThreadLocalSizeOfSet();
        size64_t    cb   = 0;
        if (hSet->add(this))
            {
            bool fRoot = hSet->size() == 1;
            try
                {
                ObjectArray::Handle haStorage;
                size32_t            cSlots;

                // copy values into external array while read sync'd
                COH_SYNCHRONIZED_MEMBER_READ
                    {
                    Storage::Handle   hStorage = f_hStorage;
                    Storage::Element* aElement = hStorage->m_aElements;

                    cSlots    = hStorage->getSlots();
                    haStorage = ObjectArray::create(cSlots);
                    for (size32_t i = 0; i < cSlots; ++i)
                        {
                        if (aElement[i].state == Storage::mapped)
                            {
                            haStorage[i] = aElement[i].value;
                            }
                        }
                    }

                // compute size of values outside of sync to protect against
                // deadlock if another thread performing sizeOf reaches
                // this impl through locks held on one of the values
                cb = sizeOf(false) + (cSlots * sizeof(Storage::Element));
                for (size32_t i = 0 ; i < cSlots; ++i)
                    {
                    Object::Holder v = haStorage[i];
                    if (v != NULL)
                        {
                        cb += v->sizeOf(true);
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
    else
        {
        return sizeof(*this);
        }
    }


// ----- nested class: HashIterator -----------------------------------------

HashArray::HashIterator::HashIterator(HashArray::Holder ohHa)
    : f_ohHa(self(), ohHa), m_cSlots(ohHa->f_hStorage->getSlots()),
      m_ofNext(0), m_lCurr(-1), m_ohCurr(self())
    {
    }

// ----- LongArrayIterator interface ----------------------------------------

int64_t HashArray::HashIterator::getIndex() const
    {
    COH_ENSURE(m_ofNext != 0);
    return m_lCurr;
    }

Object::Holder HashArray::HashIterator::getValue() const
    {
    COH_ENSURE(m_ofNext != 0);
    return m_ohCurr;
    }

Object::Holder HashArray::HashIterator::setValue(Object::Holder ohValue)
    {
    return getMutableDelegate()->set(getIndex(), ohValue);
    }

void HashArray::HashIterator::remove()
    {
    getMutableDelegate()->remove(getIndex());
    }

// ----- Iterator interface ---------------------------------

bool HashArray::HashIterator::hasNext() const
    {
    size32_t ofNext = m_ofNext;
    return ofNext == 0 ? !getDelegate()->isEmpty() : ofNext < m_cSlots;
    }

Object::Holder HashArray::HashIterator::next()
    {
    HashArray::View vHa      = getDelegate();
    Storage::View   vStorage = vHa->f_hStorage;
    size32_t        ofNext   = m_ofNext;

    // sync block
        {
        SynchronizedMemberReadBlock(*getDelegate());
        if (vStorage->getSlots() != m_cSlots)
            {
            COH_THROW (ConcurrentModificationException::create());
            }

        HashArray::Storage::Element* aElement = vStorage->m_aElements;
        size32_t                     cSlots   = m_cSlots;
        for (; ofNext < cSlots; ++ofNext)
            {
            if (aElement[ofNext].state == HashArray::Storage::mapped)
                {
                m_lCurr  = aElement[ofNext].key;
                m_ohCurr = aElement[ofNext].value;

                // advance until next mapped element
                for (++ofNext; ofNext < cSlots &&
                     aElement[ofNext].state != HashArray::Storage::mapped;
                     ++ofNext)
                    {
                    }
                m_ofNext = ofNext;

                return m_ohCurr;
                }
            }
        }

    COH_THROW (NoSuchElementException::create());
    }

HashArray::Handle HashArray::HashIterator::getMutableDelegate()
    {
    HashArray::Handle hha = cast<HashArray::Handle>(f_ohHa, /*fThrow*/ false);
    if (NULL == hha)
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    return hha;
    }

HashArray::View HashArray::HashIterator::getDelegate() const
    {
    return f_ohHa;
    }

COH_CLOSE_NAMESPACE2
