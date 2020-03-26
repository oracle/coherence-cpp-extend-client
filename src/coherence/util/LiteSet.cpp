/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/LiteSet.hpp"

#include "coherence/util/Arrays.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/NullImplementation.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(LiteSet)

// ----- helper class LiteSetIterator -----------------------------------

/**
* Implementing the Muterator interface over the elements in this Set.
*/
class LiteSetIterator
    : public class_spec<LiteSetIterator,
            extends<Object>,
            implements<Muterator> >
    {
    friend class factory<LiteSetIterator>;


    // ----- constructors -------------------------------------------

    protected:
        LiteSetIterator(LiteSet::Holder hSet)
            : f_haOwner(self(), hSet),
              f_haVals(self(), hSet->toArray()),
              m_iPrev(-1), m_fCanRemove(false)
        {
        }


    // ----- Muterator Interface ------------------------------------

    public:
        /**
        * Returns <tt>true</tt> if the iteration has more elements. (In
        * other words, returns <tt>true</tt> if <tt>next</tt> would
        * return an element rather than throwing an exception.)
        *
        * @return <tt>true</tt> if the iterator has more elements
        */
        virtual bool hasNext() const
            {
            return (m_iPrev + 1 < f_haVals->length);
            }

        /**
        * Returns the next element in the iteration.
        *
        * @return the next element in the iteration
        *
        * @throws NoSuchElementException iteration has no more
        *            elements
        */
        virtual Object::Holder next()
            {
            ObjectArray::View vaObject = f_haVals;
            size32_t iNext = m_iPrev + 1;
            if (iNext < f_haVals->length)
                {
                m_iPrev      = iNext;
                m_fCanRemove = true;
                return vaObject[iNext];
                }
            else
                {
                COH_THROW (NoSuchElementException::create());
                }
            }

        /**
        * Removes from the underlying set the last element
        * returned by the iterator.  This method can be called only once
        * per call to <tt>next</tt>.  The behavior of an iterator is
        * unspecified if the underlying set is modified while the
        * iteration is in progress in any way other than by calling this
        * method.
        *
        * @throws IllegalStateException if the <tt>next</tt> method
        *         has not yet been called, or the <tt>remove</tt>
        *         method has already been called after the last call
        *            to the <tt>next</tt> method
        */
        virtual void remove()
            {
            if (m_fCanRemove)
                {
                m_fCanRemove = false;
                ObjectArray::View vaObject = f_haVals;
                cast<LiteSet::Handle>(f_haOwner)->remove(
                        vaObject[m_iPrev]);
                }
            else
                {
                COH_THROW (IllegalStateException::create());
                }
            }


    // ----- data members -------------------------------------------

    protected:

        /**
        * The owning LiteSet
        */
        FinalHolder<LiteSet>     f_haOwner;
        FinalHandle<ObjectArray> f_haVals;
        size32_t                 m_iPrev;
        bool                     m_fCanRemove;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

LiteSet::LiteSet()
    : m_nImpl(i_empty), m_hContents(self())
    {
    }

LiteSet::LiteSet(Collection::View vCollection)
    : m_nImpl(i_empty), m_hContents(self())
    {
    addAll(vCollection);
    }

/**
* Copy constructor
*/
LiteSet::LiteSet(const LiteSet& that)
    : m_nImpl(that.m_nImpl), m_hContents(self(),
            that.m_hContents->clone())
    {
    }


// ----- Set interface --------------------------------------------------

bool LiteSet::isEmpty() const
    {
    return m_nImpl == i_empty;
    }

size32_t LiteSet::size() const
    {
    switch (m_nImpl)
        {
        case i_empty:
            return 0;

        case i_single:
            return 1;

        case i_array_1: case i_array_2: case i_array_3: case i_array_4:
        case i_array_5: case i_array_6: case i_array_7: case i_array_8:
            return m_nImpl - i_array_1 + 1;

        case i_other:
            return cast<Set::View>(m_hContents)->size();

        default:
            COH_THROW (IllegalStateException::create());
        }
    }


bool LiteSet::contains(Object::View v) const
    {
    switch (m_nImpl)
        {
        case i_empty:
            return false;

        case i_single:
            return v->equals(m_hContents);

        case i_array_1: case i_array_2: case i_array_3: case i_array_4:
        case i_array_5: case i_array_6: case i_array_7: case i_array_8:
            {
            // "ObjectArray" implementation
            ObjectArray::View va = cast<ObjectArray::View>(m_hContents);
            size32_t c  = m_nImpl - i_array_1 + 1;
            return indexOf(va, c, v) != npos;
            }

        case i_other:
            return cast<Set::View>(m_hContents)->contains(v);

        default:
            COH_THROW (IllegalStateException::create());
        }
    }

Iterator::Handle LiteSet::iterator() const
    {
    return isEmpty()
            ? NullImplementation::getIterator()
            : (Iterator::Handle) LiteSetIterator::create(this);
    }

Muterator::Handle LiteSet::iterator()
    {
    return isEmpty()
            ? NullImplementation::getMuterator()
            : (Muterator::Handle) LiteSetIterator::create(this);
    }

ObjectArray::Handle LiteSet::toArray(ObjectArray::Handle haDest) const
    {
    size32_t cSrc = size();
    if (cSrc == 0 && NULL == haDest)
        {
        haDest = NullImplementation::getArray();
        }
    else if (NULL == haDest || haDest->length < cSrc)
        {
        haDest = ObjectArray::create(cSrc);
        }

    size32_t cDest = haDest->length;
    switch (m_nImpl)
        {
        case i_empty:
            if (cDest > 0)
                haDest[0] = NULL;
            return haDest;

        case i_single:
            haDest[0] = m_hContents;
            if (cDest > cSrc)
                {
                haDest[cSrc] = NULL;
                }
            return haDest;

        case i_array_1: case i_array_2: case i_array_3: case i_array_4:
        case i_array_5: case i_array_6: case i_array_7:
            {
            // "ObjectArray" implementation
            ObjectArray::View va = cast<ObjectArray::View>(m_hContents);
            size32_t          c  = m_nImpl - i_array_1 + 1;
            ObjectArray::copy(va, 0, haDest, 0, c);
            if (cDest > cSrc)
                {
                haDest[cSrc] = NULL;
                }
            return haDest;
            }

        case i_array_8:
            {
            ObjectArray::Handle haResult;
            haResult = cast<ObjectArray::Handle>(m_hContents->clone());
            ObjectArray::copy(haResult, 0, haDest, 0, cSrc);
            if (cDest > cSrc)
                {
                haDest[cSrc] = NULL;
                }
            return haDest;
            }

        case i_other:
            return cast<Set::Handle>(m_hContents)->toArray(haDest);

        default:
            COH_THROW (IllegalStateException::create());
        }
    }

bool LiteSet::add(Object::Holder oh)
    {
    switch (m_nImpl)
        {
        case i_empty:
            // growing from an empty set to the "single entry"
            // implementation
            m_nImpl     = i_single;
            m_hContents = oh;
            return true;

        case i_single:
            {
            // check if this set already contains the object
            Object::View vContents = m_hContents;
            if (oh->equals(vContents))
                {
                return false;
                }

            // growing from a "single entry" set to an "ObjectArray"
            // implementation
            ObjectArray::Handle ha = ObjectArray::create(threshold);
            ha[0]                  = vContents;
            ha[1]                  = oh;
            m_nImpl                = i_array_2;
            m_hContents            = cast<Object::Holder>(ha);
            return true;
            }

        case i_array_1: case i_array_2: case i_array_3: case i_array_4:
        case i_array_5: case i_array_6: case i_array_7: case i_array_8:
            {
            // "ObjectArray" implementation
            size32_t            nImpl = m_nImpl;
            ObjectArray::Handle ha    = cast<ObjectArray::Handle>(m_hContents);
            size32_t            c     = nImpl - i_array_1 + 1;
            if (indexOf(ha, c, oh) != npos)
                {
                return false;
                }

            // check if adding the object exceeds the "lite" threshold
            if (c >= threshold)
                {
                // time to switch to a different set implementation
                Set::Handle hSet = instantiateSet();
                hSet->addAll(Arrays::asList(ha));
                hSet->add(oh);
                m_nImpl     = i_other;
                m_hContents = cast<Object::Holder>(hSet);
                }
            else
                {
                // use the next available element in the array
                ha[c]   = oh;
                m_nImpl = nImpl + 1;
                }

            return true;
            }

        case i_other:
            return cast<Set::Handle>(m_hContents)->add(oh);

        default:
            COH_THROW (IllegalStateException::create());
        }
    }

bool LiteSet::remove(Object::View v)
    {
    switch (m_nImpl)
        {
        case i_empty:
            return false;

        case i_single:
            {
            if (v->equals(m_hContents))
                {
                // shrink to an "empty set"
                m_nImpl     = i_empty;
                m_hContents = NULL;
                return true;
                }
            }
            return false;

        case i_array_1: case i_array_2: case i_array_3: case i_array_4:
        case i_array_5: case i_array_6: case i_array_7: case i_array_8:
            {
            // "ObjectArray" implementation
            size32_t            nImpl = m_nImpl;
            ObjectArray::Handle ha    = cast<ObjectArray::Handle>(m_hContents);
            size32_t            c     = nImpl - i_array_1 + 1;
            size32_t            i     = indexOf(ha, c, v);
            if (i == npos)
                {
                return false;
                }

            if (c == 1)
                {
                m_nImpl     = i_empty;
                m_hContents = NULL;
                }
            else
                {
                ObjectArray::copy(ha, i + 1, ha, i, c - i - 1);
                ha[c-1] = NULL;
                m_nImpl = --nImpl;
                }

            return true;
            }

        case i_other:
            {
            Set::Handle hSet = cast<Set::Handle>(m_hContents);
            bool fRemoved    = hSet->remove(v);
            if (fRemoved)
                {
                checkShrinkFromOther();
                }
            return fRemoved;
            }

        default:
            COH_THROW (IllegalStateException::create());
        }
    }

bool LiteSet::containsAll(Collection::View vCollection) const
    {
    switch (m_nImpl)
        {
        case i_empty:
            // since this set is empty, so must the other be
            return vCollection->isEmpty();

        case i_other:
            // (assume the delegatee is more efficient)
            return cast<Set::View>(m_hContents)->containsAll(vCollection);

        default:
            return super::containsAll(vCollection);
        }
    }

bool LiteSet::addAll(Collection::View vCollection)
    {
    switch (m_nImpl)
        {
        case i_empty:
            // "empty set" implementation (adding all to nothing is easy)
            {
            size32_t c = vCollection->size();
            switch (c)
                {
                case 0:
                    return false;

                case 1:
                    {
                    // growing from an empty set to the "single entry"
                    // implementation
                    m_nImpl     = i_single;
                    m_hContents = cast<Object::Holder>(
                            vCollection->iterator()->next());
                    }
                    return true;

                default:
                    return super::addAll(vCollection);
                }
            }

        case i_other:
            // (assume the delegatee is more efficient)
            return cast<Set::Handle>(m_hContents)->addAll(vCollection);

        default:
            return super::addAll(vCollection);
        }
    }

bool LiteSet::retainAll(Collection::View vCollection)
    {
    switch (m_nImpl)
        {
        case i_empty:
            return false;

        case i_other:
            // (assume the delegatee is more efficient)
            {
            bool fChanged = cast<Set::Handle>(m_hContents)->retainAll(
                    vCollection);
            if (fChanged)
                {
                checkShrinkFromOther();
                }
            return fChanged;
            }

        default:
            return super::retainAll(vCollection);
        }
    }

bool LiteSet::removeAll(Collection::View vCollection)
    {
    switch (m_nImpl)
        {
        case i_empty:
            return false;

        case i_other:
            // (assume the delegatee is more efficient)
            {
            bool fChanged = cast<Set::Handle>(m_hContents)->removeAll(
                    vCollection);
            if (fChanged)
                {
                checkShrinkFromOther();
                }
            return fChanged;
            }

        default:
            return super::removeAll(vCollection);
        }
    }

void LiteSet::clear()
    {
    m_nImpl     = i_empty;
    m_hContents = NULL;
    }


// ----- internal methods -----------------------------------------------

Set::Handle LiteSet::instantiateSet()
    {
    return cast<Set::Handle>(HashSet::create());
    }

Set::View LiteSet::instantiateSet() const
    {
    return cast<Set::View>(HashSet::create());
    }

size32_t LiteSet::indexOf(ObjectArray::View va, size32_t c,
        Object::View v)
    {
    // first quick-scan by reference
    for (size32_t i = 0; i < c; ++i)
        {
        if (v == va[i])
            {
            return i;
            }
        }

    // slow scan by equals()
    if (v != NULL)
        {
        for (size32_t i = 0; i < c; ++i)
            {
            if (v->equals(va[i]))
                {
                return i;
                }
            }
        }

    return npos;
    }

void LiteSet::initFromArray(ObjectArray::View va, size32_t c)
    {
    switch (c)
        {
        case 0:
            m_hContents = NULL;
            m_nImpl     = i_empty;
            break;

        case 1:
            m_hContents = cast<Object::Holder>(va[0]);
            m_nImpl     = i_single;
            break;

        case 2: case 3: case 4: case 5: case 6: case 7: case 8:
            {
            ObjectArray::Handle haPresize;
            if (va->length != threshold)
                {
                haPresize = ObjectArray::create(threshold);
                ObjectArray::copy(va, 0, haPresize, 0, c);
                }
            m_hContents = cast<Object::Holder>(haPresize);
            m_nImpl     = i_array_1 + c - 1;
            }
            break;

        default:
            {
            Set::Handle hSet = instantiateSet();
            for (size32_t i = 0; i < c; ++i)
                {
                hSet->add(va[i]);
                }
            m_hContents = cast<Object::Holder>(hSet);
            m_nImpl     = i_other;
            }
            break;
        }
    }

void LiteSet::checkShrinkFromOther()
    {
    // Todo:
    // assert m_nImpl == i_other;

    // check if the set is now significantly below the "lite"
    // threshold
    Set::View vSet = cast<Set::View>(m_hContents);
    size32_t c = vSet->size();
    switch (c)
        {
        case 0:
            m_nImpl     = i_empty;
            m_hContents = NULL;
            break;

        case 1:
            m_nImpl     = i_single;
            m_hContents = cast<Object::Holder>(vSet);
            break;

        case 2: case 3: case 4:
            {
            // shrink to "ObjectArray" implementation
            ObjectArray::Handle ha = vSet->toArray(
                    ObjectArray::create(threshold));
            m_nImpl     = i_array_1 + c - 1;
            m_hContents = cast<Object::Holder>(ha);
            }
            break;
        }
    }

COH_CLOSE_NAMESPACE2
