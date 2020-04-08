/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/DeltaSet.hpp"

#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/NullImplementation.hpp"


COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(DeltaSet)

// ----- helper class DeltaIterator ----------------------------------------

/**
* Iterator for the contents of a delta set.
*/
class DeltaIterator
    : public class_spec<DeltaIterator,
        extends<Object>,
        implements<Muterator> >
    {
    friend class factory<DeltaIterator>;

    // ----- constructor --------------------------------------------

    protected:
        /**
        * Construct an iterator for a delta set.
        */
        DeltaIterator(DeltaSet::Holder hSet)
            :  f_ohOwner(self(), hSet),
               f_haObject(self(), hSet->toArray()),
               m_nIndex(0),
               m_fRemovable(false)
            {
            }

    // ----- Iterator interface -------------------------------------

    public:
        /**
        * @internal
        */
        bool hasNext() const
            {
            return m_nIndex < f_haObject->length;
            }

        /**
        * @internal
        */
        Object::Holder next()
            {
            ObjectArray::View vaObject = f_haObject;
            if (m_nIndex < vaObject->length)
                {
                m_fRemovable = true;
                return vaObject[m_nIndex++];
                }

            m_fRemovable = false;
            COH_THROW (NoSuchElementException::create());
            }

        /**
        *  @internal
        */
        void remove()
            {
            if (!m_fRemovable)
                {
                COH_THROW (IllegalStateException::create());
                }

            m_fRemovable = false;
            ObjectArray::View vaObject = f_haObject;
            cast<DeltaSet::Handle>(f_ohOwner)->remove(vaObject[m_nIndex-1]);
            }

    // ------ data members ------------------------------------------

    protected:
        /**
        * The owning DeltaSet
        */
        FinalHolder<DeltaSet> f_ohOwner;

        /**
        * Array of objects to iterate.
        */
        FinalHandle<ObjectArray> f_haObject;

        /**
        * The next object to return.
        */
        size32_t m_nIndex;

        /**
        * True if last iterated item can be removed.
        */
        bool m_fRemovable;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

DeltaSet::DeltaSet(Set::Holder hSet)
        : f_ohSetOrig(self(), hSet),
          m_hSetAdded(self()),
          m_hSetRemoved(self())
    {
    }


// ----- DeltaSet accessors -------------------------------------------------

Set::View DeltaSet::getOriginal() const
    {
    return f_ohSetOrig;
    }

bool DeltaSet::isModified() const
    {
    return m_hSetAdded != NULL || m_hSetRemoved != NULL;
    }

Set::View DeltaSet::getAdded() const
    {
    Set::View vSet = m_hSetAdded;
    return vSet == NULL ? (Set::View) NullImplementation::getSet() : vSet;
    }

Set::View DeltaSet::getRemoved() const
    {
    Set::View vSet = m_hSetRemoved;
    return vSet == NULL ? (Set::View) NullImplementation::getSet() : vSet;
    }

Set::Handle DeltaSet::ensureAdded()
    {
    Set::Handle hSet = m_hSetAdded;
    if (hSet == NULL)
        {
        m_hSetAdded = hSet = HashSet::create();
        }
    return hSet;
    }

Set::Handle DeltaSet::ensureRemoved()
    {
    Set::Handle hSet = m_hSetRemoved;
    if (hSet == NULL)
        {
        m_hSetRemoved = hSet = HashSet::create();
        }
    return hSet;
    }

void DeltaSet::resolve()
    {
    Set::View   vSetAdded = getAdded();
    Set::Handle hSetOrig  = cast<Set::Handle>(f_ohSetOrig);

    if (!vSetAdded->isEmpty())
        {
        hSetOrig->addAll(vSetAdded);
        }

    Set::View vSetRemoved = getRemoved();
    if (!vSetRemoved->isEmpty())
        {
        hSetOrig->removeAll(vSetRemoved);
        }

    reset();
    }

void DeltaSet::reset()
    {
    m_hSetAdded   = NULL;
    m_hSetRemoved = NULL;
    }


// ----- Collection interface -----------------------------------------------

size32_t DeltaSet::size() const
    {
    return f_ohSetOrig->size() + getAdded()->size() - getRemoved()->size();
    }

bool DeltaSet::contains(Object::View v) const
    {
    return getAdded()->contains(v) || (!getRemoved()->contains(v) &&
            f_ohSetOrig->contains(v));
    }

Iterator::Handle DeltaSet::iterator() const
    {
    return isEmpty() ? NullImplementation::getIterator() :
            (Iterator::Handle) DeltaIterator::create(this);
    }

Muterator::Handle DeltaSet::iterator()
    {
    return isEmpty() ? NullImplementation::getMuterator() :
            (Muterator::Handle) DeltaIterator::create(this);
    }

ObjectArray::Handle DeltaSet::toArray(ObjectArray::Handle hao) const
    {
    size32_t c = size();
    if (c == 0)
        {
        return NullImplementation::getArray();
        }

    if (NULL == hao || hao->length < c)
        {
        hao = ObjectArray::create(c);
        }

    Set::View vSetAdded = getAdded();
    c = vSetAdded->size();
    if (c > 0)
        {
        hao = vSetAdded->toArray(hao);
        }

    Set::View vSetRemoved = getRemoved();
    for (Iterator::Handle hIter = f_ohSetOrig->iterator(); hIter->hasNext(); )
        {
        Object::View vo = hIter->next();
        if (!vSetRemoved->contains(vo))
            {
            hao[c++] = vo;
            }
        }

    if (c < hao->length)
        {
        hao[c] = NULL;
        }

    return hao;
    }

bool DeltaSet::add(Object::Holder oh)
    {
    if (getRemoved()->contains(oh))
        {
        ensureRemoved()->remove(oh);
        return true;
        }

    if (getAdded()->contains(oh) || f_ohSetOrig->contains(oh))
        {
        return false;
        }

    ensureAdded()->add(oh);
    return true;
    }

bool DeltaSet::remove(Object::View v)
    {
    if (getRemoved()->contains(v))
        {
        return false;
        }

    if (getAdded()->contains(v))
        {
        ensureAdded()->remove(v);
        return true;
        }

    if (f_ohSetOrig->contains(v))
        {
        ensureRemoved()->add(v);
        return true;
        }

    return false;
    }

void DeltaSet::clear()
    {
    reset();
    ensureRemoved()->addAll(f_ohSetOrig);
    }

COH_CLOSE_NAMESPACE2
