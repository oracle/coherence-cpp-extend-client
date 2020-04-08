/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/ReadOnlyMultiList.hpp"

#include "coherence/util/HashSet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// ----- nested class: MultiIterator ----------------------------------------

/**
* ListMuterator implementation based on the ReadOnlyMultiList.
*/
class MultiIterator
    : public class_spec<MultiIterator,
        extends<Object>,
        implements<ListMuterator> >
    {
    friend class factory<MultiIterator>;

    // ----- constructors -----------------------------------------------

    protected:
        MultiIterator(ObjectArray::View vaao, size32_t cTotal, size32_t i = 0)
            : f_vaao(self(), vaao), m_cTotal(cTotal), m_i(0), m_ia(0), m_iaa(0)
            {
            // advance array as needed
            if (i == List::npos)
                {
                i = cTotal;
                }
            while (i-- > 0)
                {
                next();
                }
            }

    // ----- Iterator interface -----------------------------------------
    public:
        virtual void remove()
            {
            COH_THROW (UnsupportedOperationException::create(
                    "collection is read-only"));
            }

        virtual void add(Object::Holder /*oh*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "collection is read-only"));
            }

        virtual void set(Object::Holder /*oh*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "collection is read-only"));
            }

        virtual bool hasNext() const
            {
            return m_i < m_cTotal;
            }

        virtual Object::Holder next()
            {
            if (m_i == m_cTotal)
                {
                COH_THROW (NoSuchElementException::create());
                }
            ++m_i;

            ObjectArray::View vaao = f_vaao;
            size32_t          ia   = m_ia;
            ObjectArray::View vao  = cast<ObjectArray::View>(vaao[m_iaa]);

            while (ia == vao->length)
                {
                // no more elements in this array; move on to the next
                // populated array
                vao = cast<ObjectArray::View>(vaao[++m_iaa]);
                ia  = 0;
                }

            m_ia = ia + 1; // prepare for the next iteration
            return vao[ia];
            }

        virtual size32_t nextIndex() const
            {
            return m_i;
            }

        virtual size32_t previousIndex() const
            {
            return m_i - 1;
            }

        virtual bool hasPrevious() const
            {
            return m_i > 0;
            }

        virtual Object::Holder previous()
            {
            if (m_i == 0)
                {
                COH_THROW (NoSuchElementException::create());
                }
            --m_i;

            ObjectArray::View vaao = f_vaao;
            size32_t          ia   = m_ia;
            ObjectArray::View vao  = cast<ObjectArray::View>(vaao[m_iaa]);

            while (ia == 0)
                {
                // no more elements in this array; move on to the previous
                // populated array
                vao = cast<ObjectArray::View>(vaao[--m_iaa]);
                ia  = vao->length;
                }

            m_ia = --ia; // prepare for the next iteration
            return vao[ia];
            }


    private:
        FinalView<ObjectArray> f_vaao;   // array of array of elements
        size32_t               m_cTotal; // total number of elements
        size32_t               m_i;      // index of next to be returned
        size32_t               m_ia;     // index into current array
        size32_t               m_iaa;    // index into array of arrays
    };


// ----- constructors -------------------------------------------------------

ReadOnlyMultiList::ReadOnlyMultiList(ObjectArray::View vaao)
    : f_vaao(self(), vaao), f_vSet(self()), m_cTotal(calculateTotalLength(vaao))
    {
    }


// ----- accessors ----------------------------------------------------------

List::View ReadOnlyMultiList::getList() const
    {
    return ListView::create(View(this));
    }

List::Handle ReadOnlyMultiList::getList()
    {
    return ListView::create(Handle(this));
    }

Set::View ReadOnlyMultiList::getSet() const
    {
    return SetView::create(View(this));
    }

Set::Handle ReadOnlyMultiList::getSet()
    {
    return SetView::create(Handle(this));
    }


// ----- List interface -----------------------------------------------------

size32_t ReadOnlyMultiList::size() const
    {
    return m_cTotal;
    }

Object::Holder ReadOnlyMultiList::get(size32_t i) const
    {
    ObjectArray::View vaao = f_vaao;
    for (size32_t iaa = 0, caa = vaao->length; iaa < caa; ++iaa)
        {
        ObjectArray::View va = cast<ObjectArray::View>(vaao[iaa]);
        size32_t c = va->length;
        if (i < c)
            {
            return va[i];
            }
        i -= c;
        }

    COH_THROW (IndexOutOfBoundsException::create());
    }

Object::Holder ReadOnlyMultiList::set(size32_t /*i*/, Object::Holder /*oh*/)
    {
    COH_THROW (UnsupportedOperationException::create("collection is read-only"));
    }

bool ReadOnlyMultiList::add(size32_t /*i*/, Object::Holder /*oh*/)
    {
    COH_THROW (UnsupportedOperationException::create("collection is read-only"));
    }

bool ReadOnlyMultiList::addAll(size32_t /*i*/, Collection::View /*vColl*/)
    {
    COH_THROW (UnsupportedOperationException::create("collection is read-only"));
    }

Object::Holder ReadOnlyMultiList::remove(size32_t /*i*/)
    {
    COH_THROW (UnsupportedOperationException::create("collection is read-only"));
    }

size32_t ReadOnlyMultiList::indexOf(Object::View v) const
    {
    size32_t          i    = 0;
    ObjectArray::View vaao = f_vaao;
    for (size32_t iaa = 0, caa = vaao->length; iaa < caa; ++iaa)
        {
        ObjectArray::View vao = cast<ObjectArray::View>(vaao[iaa]);
        for (size32_t ia = 0, ca = vao->length; ia < ca; ++ia, ++i)
            {
            if (Object::equals(vao[ia], v))
                {
                return i;
                }
            }
        }
    return npos;
    }

size32_t ReadOnlyMultiList::lastIndexOf(Object::View v) const
    {
    size32_t          i    = m_cTotal - 1;
    ObjectArray::View vaao = f_vaao;
    for (size32_t iaa = vaao->length - 1; iaa != npos; --iaa)
        {
        ObjectArray::View vao = cast<ObjectArray::View>(vaao[iaa]);
        for (size32_t ia = vao->length - 1; ia != npos; --ia, --i)
            {
            if (Object::equals(vao[ia], v))
                {
                return i;
                }
            }
        }
    return npos;
    }

bool ReadOnlyMultiList::contains(Object::View v) const
    {
    Set::View vSet = f_vSet;
    if (vSet == NULL)
        {
        size32_t cTotal = m_cTotal;
        if (cTotal < 32)
            {
            return indexOf(v) != npos;
            }
        // We have a decent number of elements and it appears that we're
        // being accessed as a Set-> The ReadOnlyMultiList data-structure
        // is sub-optimal for Set based operations, and thus for large
        // sets we inflate and delegate to a real Set implementation->
        Set::Handle hSet = HashSet::create(cTotal);
        hSet->addAll(this);
        vSet = hSet;
        initialize(f_vSet, vSet);
        }
    return vSet->contains(v);
    }

ObjectArray::Handle ReadOnlyMultiList::toArray(ObjectArray::Handle hao) const
    {
    return flatten(f_vaao, m_cTotal, hao);
    }

Iterator::Handle ReadOnlyMultiList::iterator() const
    {
    return MultiIterator::create(f_vaao, m_cTotal);
    }

Muterator::Handle ReadOnlyMultiList::iterator()
    {
    return MultiIterator::create(f_vaao, m_cTotal);
    }

ListIterator::Handle ReadOnlyMultiList::listIterator(size32_t i) const
    {
    return MultiIterator::create(f_vaao, m_cTotal, i);
    }

ListMuterator::Handle ReadOnlyMultiList::listIterator(size32_t i)
    {
    return MultiIterator::create(f_vaao, m_cTotal, i);
    }


template<class T>
List::Holder ReadOnlyMultiList::subList(T list, size32_t iFrom, size32_t iTo)
    {
    size32_t cTotal = list->m_cTotal;
    if (iTo == npos)
        {
        iTo = cTotal;
        }
    COH_ENSURE_PARAM_RELATION(iFrom, <, iTo);
    COH_ENSURE_PARAM_RELATION(iFrom, <, cTotal);
    COH_ENSURE_PARAM_RELATION(iTo, <=, cTotal);

    ObjectArray::View vaao = list->f_vaao;

    // find the array for the "from" element
    size32_t          iaaFrom = 0;
    ObjectArray::View vaFrom  = cast<ObjectArray::View>(vaao[iaaFrom]);
    for (; iFrom >= vaFrom->length; vaFrom = cast<ObjectArray::View>(vaao[++iaaFrom]))
        {
        iFrom -= vaFrom->length;
        }

    // find the array for the "to" element
    size32_t          iaaTo = iaaFrom;
    ObjectArray::View vaTo  = vaFrom;
    for (; iTo > vaTo->length; vaTo = cast<ObjectArray::View>(vaao[++iaaTo]))
        {
        iTo -= vaTo->length;
        }

    size32_t caaSub = iaaTo - iaaFrom + 1;
    if (iFrom == 0 && iTo == vaTo->length)
        {
        // neither edge array requires trimming, this allows us to avoid
        // creating new array storage and we can just use a delegating
        // sub-array
        return ReadOnlyMultiList::create(vaao->subArray(iaaFrom, iaaTo + 1));
        }
    else if (caaSub == 1)
        {
        // all elements are from a single sub-array; but requires trimming
        ObjectArray::Handle haaSub = ObjectArray::create(1);
        haaSub[0] = vaFrom->subArray(iFrom, iTo);
        return ReadOnlyMultiList::create(haaSub);
        }
    else
        {
        // construct multi-array sub-list; and trim the edge arrays
        ObjectArray::Handle haaSub = ObjectArray::create(caaSub);
        ObjectArray::copy(vaao, iaaFrom, haaSub, 0, caaSub);
        if (iFrom != 0)
            {
            haaSub[0] = vaFrom->subArray(iFrom, vaFrom->length);
            }
        if (iTo != vaTo->length)
            {
            haaSub[caaSub - 1] = vaTo->subArray(0, iTo);
            }
        return ReadOnlyMultiList::create(haaSub);
        }
    }

List::View ReadOnlyMultiList::subList(size32_t iFrom, size32_t iTo) const
    {
    return subList(this, iFrom, iTo);
    }

List::Handle ReadOnlyMultiList::subList(size32_t iFrom, size32_t iTo)
    {
    return cast<List::Handle>(subList(this, iFrom, iTo));
    }


// ----- Object methods -----------------------------------------------------

bool ReadOnlyMultiList::equals(Object::View v) const
    {
    if (v == this)
        {
        return true;
        }
    else if (instanceof<List::View>(v))
        {
        return AbstractList::equals(v);
        }
    else if (instanceof<Collection::View>(v))
        {
        Collection::View that = cast<Collection::View>(v);
        return this->size() == that->size()
            && this->containsAll(that);
        }
    else
        {
        return false;
        }
    }


// ----- helpers ------------------------------------------------------------

size32_t ReadOnlyMultiList::calculateTotalLength(ObjectArray::View vaao)
    {
    size32_t cnt = 0;
    for (size32_t i = 0, c = vaao->length; i < c; ++i)
        {
        cnt += cast<ObjectArray::View>(vaao[i])->length;
        }
    return cnt;
    }

ObjectArray::Handle ReadOnlyMultiList::flatten(ObjectArray::View vaaoFrom, size32_t cTotal, ObjectArray::Handle haoTo)
    {
    if (cTotal == npos)
        {
        cTotal = calculateTotalLength(vaaoFrom);
        }

    if (haoTo == NULL)
        {
        // implied ObjectArray::View type
        haoTo = ObjectArray::create(cTotal);
        }
    else if (haoTo->length < cTotal)
        {
        // if it is not big enough
        haoTo = ObjectArray::create(cTotal);
        }
    else if (haoTo->length > cTotal)
        {
        // if the collection fits in the specified array with room to
        // spare, the element in the array immediately following the
        // end of the collection is set to NULL
        haoTo[cTotal] = NULL;
        }

    for (size32_t i = 0, of = 0, c = vaaoFrom->length; i < c; ++i)
        {
        ObjectArray::View vaoNext = cast<ObjectArray::View>(vaaoFrom[i]);
        size32_t          cNext   = vaoNext->length;
        ObjectArray::copy(vaoNext, 0, haoTo, of, cNext);
        of += cNext;
        }
    return haoTo;
    }


// ----- inner class: ListView ----------------------------------------------


ReadOnlyMultiList::ListView::ListView(ReadOnlyMultiList::Holder ohList)
    : super(ohList)
    {
    }

bool ReadOnlyMultiList::ListView::equals(Object::View v) const
    {
    List::View vThat = cast<List::View>(v, false);

    if (NULL == vThat)
        {
        return false;
        }
    else if (this == vThat)
        {
        return true;
        }

    if (size() != vThat->size())
        {
        return false;
        }

    for (Iterator::Handle hIterThis = iterator(), hIterThat = vThat->iterator();
            hIterThis->hasNext(); )
        {
        if (!Object::equals(hIterThis->next(), hIterThat->next()))
            {
            return false;
            }
        }

    return true;
    }

size32_t ReadOnlyMultiList::ListView::hashCode() const
    {
    size32_t nHash = 1;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        Object::Holder oh = hIter->next();
        if (NULL != oh)
            {
            nHash = 31* nHash + oh->hashCode();
            }
        }
    return nHash;
    }


// ----- inner class: SetView -----------------------------------------------

ReadOnlyMultiList::SetView::SetView(ReadOnlyMultiList::Holder ohSet)
    : super(ohSet)
    {
    }

bool ReadOnlyMultiList::SetView::equals(Object::View v) const
    {
    Set::View vThat = cast<Set::View>(v, false);

    if(vThat == this)
        {
        return true;
        }

    if (NULL == vThat )
        {
        return false;
        }

    if(vThat->size() != size())
        {
        return false;
        }

    return containsAll(vThat);
    }

size32_t ReadOnlyMultiList::SetView::hashCode() const
    {
    size32_t nHash = 0;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        Object::Holder oh = hIter->next();
        if (NULL != oh)
            {
            nHash += oh->hashCode();
            }
        }
    return nHash;
    }


COH_CLOSE_NAMESPACE2
