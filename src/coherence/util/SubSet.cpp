/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SubSet.hpp"

#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/NullImplementation.hpp"
#include "coherence/util/SortedSet.hpp"
#include "coherence/util/TreeSet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(SubSet)

// ----- helper class SubSetIterator ----------------------------------------

/**
* Iterator for the contents of a subset.
*/
class SubSetIterator
    : public class_spec<SubSetIterator,
        extends<Object>,
        implements<Muterator> >
    {
    friend class factory<SubSetIterator>;

    // ----- constructor --------------------------------------------

    protected:
        /**
        * Construct an iterator for a subset.
        */
        SubSetIterator(SubSet::Holder hSet)
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
            cast<SubSet::Handle>(f_ohOwner)->remove(vaObject[m_nIndex-1]);
            }

    // ------ data members ------------------------------------------

    protected:
        /**
        * The owning SubSet
        */
        FinalHolder<SubSet> f_ohOwner;

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

SubSet::SubSet(Set::Holder hSet)
        : f_ohSetOrig(self(), hSet),
          m_hSetMod(self()),
          m_fRetained(false)
    {
    }

SubSet::SubSet(const SubSet& that)
        : super(that),
          f_ohSetOrig(self(), cast<Set::Handle>(that.f_ohSetOrig->clone())),
          m_hSetMod(self()),
          m_fRetained(that.m_fRetained)
    {
    Set::View vSet = that.m_hSetMod;
    if (NULL != vSet)
        {
        m_hSetMod = cast<Set::Handle>(vSet->clone());
        }
    }


// ----- SubSet accessors ---------------------------------------------------

Set::View SubSet::getOriginal() const
    {
    return f_ohSetOrig;
    }

bool SubSet::isModified() const
    {
    Set::View vSetMod = m_hSetMod;
    return m_fRetained ? vSetMod == NULL || vSetMod->size() != f_ohSetOrig->size()
                       : vSetMod != NULL && !vSetMod->isEmpty();
    }

bool SubSet::isTrackingRetained() const
    {
    return m_fRetained;
    }

Set::View SubSet::getRetained() const
    {
    Set::View vSetMod = m_hSetMod;
    if (m_fRetained)
        {
        return vSetMod == NULL ?
                (Set::View)NullImplementation::getSet() : vSetMod;
        }
    else
        {
        // the set of modifications represents removed items
        if (vSetMod == NULL || vSetMod->isEmpty())
            {
            return f_ohSetOrig;
            }

        Set::View vSetOrig = f_ohSetOrig;
        if (vSetMod->size() == vSetOrig->size())
            {
            return NullImplementation::getSet();
            }

        Set::Handle hSetRemain = HashSet::create(vSetOrig);
        hSetRemain->removeAll(vSetMod);
        return hSetRemain;
        }
    }

bool SubSet::isTrackingRemoved() const
    {
    return !m_fRetained;
    }

Set::View SubSet::getRemoved() const
    {
    Set::View vSetMod = m_hSetMod;
    if (m_fRetained)
        {
        // the set of modifications represents retained items
        if (vSetMod == NULL || vSetMod->isEmpty())
            {
            return f_ohSetOrig;
            }

        Set::View vSetOrig = f_ohSetOrig;
        if (vSetMod->size() == vSetOrig->size())
            {
            return NullImplementation::getSet();
            }

        Set::Handle hSetRemove = HashSet::create(vSetOrig);
        hSetRemove->removeAll(vSetMod);
        return hSetRemove;
        }
    else
        {
        return vSetMod == NULL ?
                (Set::View) NullImplementation::getSet() : vSetMod;
        }
    }

Set::Handle SubSet::instantiateModificationSet(size32_t cSize) const
    {
        return instanceof<SortedSet::Handle>(f_ohSetOrig)
            ? (Set::Handle) TreeSet::create(cast<SortedSet::View>(f_ohSetOrig)->comparator())
            : (Set::Handle) (cSize == 0 ? HashSet::create() : HashSet::create(cSize));
    }

Set::Handle SubSet::ensureRetained()
    {
    Set::Handle hSetMod = m_hSetMod;
    if (m_fRetained)
        {
        if (hSetMod == NULL)
            {
            m_hSetMod = hSetMod = HashSet::create();
            }
        }
    else
        {
        Set::View   vSetOrig    = f_ohSetOrig;
        Set::Handle hSetRemoved = hSetMod;

        // new set to manage retained items
        m_hSetMod   = hSetMod = HashSet::create();
        m_fRetained = true;

        if (hSetRemoved == NULL || hSetRemoved->isEmpty())
            {
            // nothing has been removed; implies everything is retained
            hSetMod->addAll(vSetOrig);
            }
        else
            {
            // some items may have been removed
            size32_t cOrig    = vSetOrig->size();
            size32_t cRemoved = hSetRemoved->size();
            if (cOrig != cRemoved)
                {
                // if more than 25% have been removed, then filter them
                // out while building the retained set, otherwise make
                // the retained set by copying the original set and
                // removing just those items that have been removed from
                // the SubSet
                if (cRemoved << 2 > cOrig)
                    {
                    for (Iterator::Handle hIter = vSetOrig->iterator();
                            hIter->hasNext(); )
                        {
                        Object::View o = hIter->next();
                        if (!hSetRemoved->contains(o))
                            {
                            hSetMod->add(o);
                            }
                        }
                    }
                else
                    {
                    hSetMod->addAll(vSetOrig);
                    hSetMod->removeAll(hSetRemoved);
                    }
                }
            }
        }

    return hSetMod;
    }

Set::Handle SubSet::ensureRemoved()
    {
    if (m_fRetained)
        {
        COH_THROW (IllegalStateException::create());

        }
    Set::Handle hSetMod = m_hSetMod;
    if (hSetMod == NULL)
        {
        // removed set doesn't have to be ordered
        m_hSetMod = hSetMod = HashSet::create();
        }

    return hSetMod;
    }

void SubSet::resolve()
    {
    if (isModified())
        {
        Set::Handle hSetMod  = m_hSetMod;
        Set::Handle hSetOrig = cast<Set::Handle>(f_ohSetOrig);
        if (m_fRetained)
            {
            if (hSetMod == NULL || hSetMod->isEmpty())
                {
                hSetOrig->clear();
                }
            else
                {
                hSetOrig->retainAll(hSetMod);
                }
            }
        else
            {
            if (hSetMod != NULL && !hSetMod->isEmpty())
                {
                hSetOrig->removeAll(hSetMod);
                }
            }
        }

    reset();
    }

void SubSet::reset()
    {
    m_hSetMod   = NULL;
    m_fRetained = false;
    }


// ----- Collection interface -----------------------------------------------

size32_t SubSet::size() const
    {
    Set::View vSetMod = m_hSetMod;
    if (m_fRetained)
        {
        return vSetMod == NULL ? 0 : vSetMod->size();
        }
    else
        {
        return f_ohSetOrig->size() - (vSetMod == NULL ? 0 : vSetMod->size());
        }
    }

bool SubSet::isEmpty() const
    {
    Set::View vSetOrig = f_ohSetOrig;
    Set::View vSetMod  = m_hSetMod;

    // if the original set is empty, then the subset must be empty
    if (vSetOrig->isEmpty())
        {
        return true;
        }

    // if this set is retaining a subset, then this set is empty
    // only if nothing has been retained
    if (m_fRetained)
        {
        return vSetMod == NULL || vSetMod->isEmpty();
        }

    // this set is tracking removed items; if nothing has been removed
    // then this subset definitely is NOT empty
    if (vSetMod == NULL || vSetMod->isEmpty())
        {
        return false;
        }

    // otherwise, this set is empty only if all items have been removed
    return vSetMod->size() == vSetOrig->size();
    }

bool SubSet::contains(Object::View v) const
    {
    Set::View vSetMod = m_hSetMod;
    if (m_fRetained)
        {
        return vSetMod != NULL && vSetMod->contains(v);
        }
    else
        {
        return (vSetMod == NULL || !vSetMod->contains(v)) &&
                f_ohSetOrig->contains(v);
        }
    }

bool SubSet::containsAll(Collection::View vCol) const
    {
    Set::View vSetMod = m_hSetMod;
    if (m_fRetained)
        {
        return vSetMod != NULL && vSetMod->containsAll(vCol);
        }
    else
        {
        // determine if there is any intersection between the removed
        // items managed by this SubSet and the passed collection
        if (vSetMod != NULL)
            {
            Collection::View vColSmaller;
            Collection::View vColLarger;
            if (vCol->size() > vSetMod->size())
                {
                vColLarger  = vCol;
                vColSmaller = vSetMod;
                }
            else
                {
                vColLarger  = vSetMod;
                vColSmaller = vCol;
                }
            for (Iterator::Handle hIter = vColSmaller->iterator(); hIter->hasNext(); )
                {
                if (vColLarger->contains(hIter->next()))
                    {
                    return false;
                    }
                }
            }

        return f_ohSetOrig->containsAll(vCol);
        }
    }

Iterator::Handle SubSet::iterator() const
    {
    return isEmpty() ? NullImplementation::getIterator() :
            (Iterator::Handle) SubSetIterator::create(this);
    }

Muterator::Handle SubSet::iterator()
    {
    return isEmpty() ? NullImplementation::getMuterator() :
            (Muterator::Handle) SubSetIterator::create(this);
    }

ObjectArray::Handle SubSet::toArray(
        ObjectArray::Handle hao) const
    {
    // if this SubSet is tracking retained items, then simply get an
    // array of the retained items
    Set::View vSetMod = m_hSetMod;
    if (m_fRetained)
        {
        return vSetMod == NULL || vSetMod->isEmpty() ?
                NullImplementation::getArray() : vSetMod->toArray();
        }

    // this SubSet is tracking removed items; if nothing has been
    // removed, then simply get an array of the original items
    Set::View vSetOrig = f_ohSetOrig;
    if (vSetMod == NULL || vSetMod->isEmpty())
        {
        return vSetOrig->toArray();
        }

    // otherwise it is necessary to get all original items that have NOT
    // been removed
    size32_t i  = 0;
    size32_t c  = vSetOrig->size() - vSetMod->size();

    if (NULL == hao || hao->length < c)
        {
        hao = ObjectArray::create(c);
        }

    for (Iterator::Handle hIter = vSetOrig->iterator(); hIter->hasNext(); )
        {
        Object::View o = hIter->next();
        if (!vSetMod->contains(o))
            {
            if (i < c)
                {
                hao[i] = o;
                }
            i++;
            }
        }

    if (i != c)
        {
        COH_THROW_STREAM (ConcurrentModificationException,
                "while assembling the array of objects in the SubSet, "
                << i << " objects were determined to be in the SubSet, but "
                << c << " objects were expected.");
        }

    if (c < hao->length)
        {
        hao[c] = NULL;
        }

    return hao;
    }

bool SubSet::add(Object::Holder oh)
    {
    if (!f_ohSetOrig->contains(oh))
        {
        COH_THROW_STREAM (UnsupportedOperationException,
                "attempt to add an item to the SubSet that was not" <<
                " in the original set; item=\"" << oh << "\"");
        }

    if (m_fRetained)
        {
        return ensureRetained()->add(oh);
        }
    else
        {
        return ensureRemoved()->remove(oh);
        }
    }

bool SubSet::addAll(Collection::View vCol)
    {
    if (!f_ohSetOrig->containsAll(vCol))
        {
        COH_THROW_STREAM (UnsupportedOperationException,
                "attempt to add items to the SubSet" <<
                " that were not in the original set; item collection=\"" <<
                vCol << "\"");
        }

    if (m_fRetained)
        {
        return ensureRetained()->addAll(vCol);
        }
    else
        {
        return ensureRemoved()->removeAll(vCol);
        }
    }

bool SubSet::remove(Object::View v)
    {
    if (m_fRetained)
        {
        Set::Handle hSetMod = m_hSetMod;
        return hSetMod == NULL ? false : hSetMod->remove(v);
        }
    else
        {
        return f_ohSetOrig->contains(v) ? ensureRemoved()->add(v) : false;
        }
    }

bool SubSet::removeAll(Collection::View vCol)
    {
    Set::Handle hSetMod = m_hSetMod;
    if (m_fRetained)
        {
        return hSetMod == NULL || hSetMod->isEmpty() ? false :
                hSetMod->removeAll(vCol);
        }
    else
        {
        // determine if this should be switched to a retain model
        Set::View vSetOrig = f_ohSetOrig;
        size32_t cOrig      = vSetOrig->size();
        size32_t cRemoved   = hSetMod == NULL ? 0 : hSetMod->size();
        size32_t cRemove    = vCol->size();

        if (cRemove == 0)
            {
            return false;
            }

        // if the percentage of removed is high enough (25% threshold)
        // switch to "retained"
        if (cOrig > 64 && (cRemove + cRemoved) > cOrig >> 2)
            {
            return ensureRetained()->removeAll(vCol);
            }

        // relatively small number of removes; stick with "removing"
        hSetMod = ensureRemoved();

        bool fMod = false;
        for (Iterator::Handle hIter = vCol->iterator(); hIter->hasNext(); )
            {
            Object::View o = hIter->next();
            if (!hSetMod->contains(o) && vSetOrig->contains(o))
                {
                hSetMod->add(o);
                fMod = true;
                }
            }
        return fMod;
        }
    }

bool SubSet::retainAll(Collection::View vCol)
    {
    Set::Handle hSetMod     = m_hSetMod;
    Set::View   vSetPassed  = instanceof<Set::View>(vCol) ? cast<Set::View>(vCol) : NULL;
    size32_t    cPassed     = vCol->size();

    if (m_fRetained)
        {
        hSetMod = ensureRetained();

        size32_t cPrevSize = hSetMod->size();
        if (vSetPassed == NULL || cPrevSize >= cPassed * 2)
            {
            // as the original set is at least twice the size of the passed
            // set it is cheaper to create a new retained set than remove
            // from the original
            retainAllInternal(vCol, hSetMod, NULL);
            return cPrevSize != m_hSetMod->size();
            }
        else
            {
            for (Muterator::Handle hMuter = hSetMod->iterator(); hMuter->hasNext(); )
                {
                if (!vSetPassed->contains(hMuter->next()))
                    {
                    hMuter->remove();
                    }
                }
            return cPrevSize != hSetMod->size();
            }
        }

    // switch to retain regardless
    m_fRetained = true;

    Set::View   vSetOrig    = f_ohSetOrig;
    size32_t    cOrig       = vSetOrig->size();
    Set::Handle hSetRemoved = NULL;
    size32_t    cRemoved    = 0;

    if (hSetMod != NULL && !hSetMod->isEmpty())
        {
        hSetRemoved = hSetMod;
        cRemoved    = hSetRemoved->size();
        }

    // optimization: decide on the cheaper walk (original or passed)
    if (vSetPassed == NULL || cOrig >= cPassed)
        {
        retainAllInternal(vCol, vSetOrig, hSetRemoved);
        }
    else
        {
        retainAllInternal(vSetOrig, vSetPassed, hSetRemoved);
        }

    return cOrig - cRemoved != m_hSetMod->size();
    }

void SubSet::clear()
    {
    m_fRetained = true;
    m_hSetMod   = NULL;
    }


    // ----- helpers --------------------------------------------------------

void SubSet::retainAllInternal(Collection::View vColOuter,
        Set::View vSetMatch, Set::View vSetExclude)
    {
    Set::Handle hSetMod = m_hSetMod = instantiateModificationSet(vColOuter->size());

    for (Iterator::Handle hIter = vColOuter->iterator(); hIter->hasNext(); )
        {
        Object::View v = hIter->next();
        if (vSetExclude == NULL || !vSetExclude->contains(v))
            {
            if (vSetMatch->contains(v))
                {
                hSetMod->add(v);
                }
            }
        }
    }


COH_CLOSE_NAMESPACE2
