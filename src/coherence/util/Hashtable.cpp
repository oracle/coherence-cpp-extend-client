/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Hashtable.hpp"

#include "coherence/util/Collections.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/MapKeySet.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/lang/AssociativeArray.hpp"
#include "private/coherence/util/HashArray.hpp"
#include "private/coherence/util/SimpleMapEntry.hpp"

#include <sstream>

using coherence::util::Set;

COH_OPEN_NAMESPACE2(coherence,lang)
    // defined in Object.cpp
    extern Set::Handle coh_getThreadLocalSizeOfSet();
COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_TYPED_CLASS(Hashtable);

COH_OPEN_NAMESPACE_ANON(Hashtable)

// ----- nested class: key ----------------------------------------------

/**
* Key implementation.
*/
class key
    {
    public:
        key()
            : m_cpo(NULL), m_nHash(0)
            {}

        key(Object::View vKey)
            : m_cpo(get_pointer(vKey)),
              m_nHash(vKey == NULL ? 0 : vKey->hashCode())
            {}

        key(const key& that)
            : m_cpo(that.m_cpo), m_nHash(that.m_nHash)
            {}

        key& operator=(const key& that)
            {
            m_cpo   = that.m_cpo;
            m_nHash = that.m_nHash;
            return *this;
            }

        key& operator=(Object::View vKey)
            {
            const Object* cpo = m_cpo = get_pointer(vKey);
            m_nHash = (NULL == cpo ? 0 : cpo->hashCode());
            return *this;
            }

        bool operator==(const key& that) const
            {
            // deep equality
            const Object* cpoThis = m_cpo;
            const Object* cpoThat = that.m_cpo;

            if (cpoThis == cpoThat)
                {
                return true;
                }
            else if (cpoThis == NULL || cpoThat == NULL)
                {
                return false;
                }
            else
                {
                return m_nHash == that.m_nHash && cpoThis->equals(cpoThat);
                }
            }

        operator size_t() const
            {
            return m_nHash;
            }

        operator Object::View() const
            {
            return m_cpo;
            }

        void attach(bool fEscaped)
            {
            const Object* cpo = m_cpo;
            if (cpo != NULL)
                {
                cpo->_attach(fEscaped);
                }
            }

        void detach(bool fEscaped)
            {
            const Object* cpo = m_cpo;
            if (cpo != NULL)
                {
                cpo->_detach(fEscaped);
                }
            }

    private:
        const    Object* m_cpo;
        size32_t m_nHash;
    };


// ----- nested class: value --------------------------------------------

typedef HashArray::value value;

typedef AssociativeArray<key, value> Storage;

// ----- nested class: EntryIterator --------------------------------

/**
* The type of the entryset iterator.  The class is both an Iterator
* and a Map::Entry which means that each call to Iterator::next()
* simply returns a reference to the iterator.  This optimization
* reduces temporary object (Map::Entry) creation, but forces manual
* creation of Entries if they need to be retained externally from
* the iteration.
*/
class COH_EXPORT EntryIterator
    : public cloneable_spec<EntryIterator,
        extends<Object>,
        implements<Muterator, Map::Entry> >
    {
    friend class factory<EntryIterator>;

    // ----- constructors ---------------------------------------

    protected:
        EntryIterator(Storage::Holder ohStorage)
            : f_ohStorage(self(), ohStorage),
              m_cSlots(ohStorage->getSlots()), m_ofNext(0),
              m_vKey(self()), m_ohVal(self())
            {
            }

        EntryIterator(const EntryIterator& that)
            : super(), f_ohStorage(self(),
                    (Storage::Holder) NULL), m_cSlots(0),
              m_ofNext(1), m_vKey(self(), that.getKey()->clone()),
              m_ohVal(self(), that.getValue()->clone())
            {
            }

    // ----- Muterator interface --------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            cast<Map::Handle>(getMutableDelegate())->remove(getKey());
            }

    // ----- Iterator interface ---------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            size32_t ofNext = m_ofNext;
            return ofNext == 0
                ? getDelegate()->size() > 0
                : ofNext < m_cSlots;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            Storage::View vStorage = getDelegate();
            size32_t          ofNext   = m_ofNext;

            COH_SYNCHRONIZED (vStorage)
                {
                size32_t cSlots = m_cSlots;
                if (vStorage->getSlots() != cSlots)
                    {
                    COH_THROW (ConcurrentModificationException::create());
                    }

                Storage::Element* aElement = vStorage->m_aElements;
                for (; ofNext < cSlots; ++ofNext)
                    {
                    if (aElement[ofNext].state == Storage::mapped)
                        {
                        m_vKey  = aElement[ofNext].key;
                        m_ohVal = aElement[ofNext].value;

                        // advance until next mapped element
                        for (++ofNext; ofNext < cSlots &&
                             aElement[ofNext].state != Storage::mapped;
                             ++ofNext)
                            {
                            }
                        m_ofNext = ofNext;

                        return this;
                        }
                    }
                }

            COH_THROW (NoSuchElementException::create());
            }

    // ----- Map::Entry interface -------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View getKey() const
            {
            COH_ENSURE(m_ofNext != 0);
            return m_vKey;
            }


        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getValue() const
            {
            COH_ENSURE(m_ofNext != 0);
            return m_ohVal;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getValue()
            {
            COH_ENSURE(m_ofNext != 0);
            return m_ohVal;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder setValue(Object::Holder ohValue)
            {
            Object::Holder ohOld = cast<Map::Handle>(getMutableDelegate())
                ->put(getKey(), ohValue);
            m_ohVal = ohValue;
            return ohOld;
            }

    // ----- helper methods -------------------------------------

    private:
        Storage::Handle getMutableDelegate()
            {
            Storage::Handle hStorage =
                cast<Storage::Handle>(f_ohStorage,
                        /*fThrow*/ false);
            if (NULL == hStorage)
                {
                COH_THROW (UnsupportedOperationException::create());
                }
            return hStorage;
            }

        Storage::View getDelegate() const
            {
            return f_ohStorage;
            }


    // ----- Object interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return Collections::toString((Map::Entry::View) this);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            return Object::hashCode(getKey());
            }

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View vThat) const
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(vThat);
            return Object::equals(getKey(), vEntry->getKey()) &&
                   Object::equals(getValue(), vEntry->getValue());
            }

    // ----- data members ---------------------------------------

    private:
        /**
        * The underlying storage.
        */
        FinalHolder<Storage> f_ohStorage;

        /**
        * The number of slots at the start of the iteration.
        */
        const size32_t m_cSlots;

        /**
        * The next storage array index
        */
        size32_t m_ofNext;

        /**
        * The current key.
        */
        MemberView<Object> m_vKey;

        /**
        * The current value.
        */
        MemberHolder<Object> m_ohVal;
    };


// ----- nested class: HashStorage --------------------------------------

/**
* The storage for Hashtables.  This is the real implementation and sits
* on top of the private AssociateArray class.  This class makes
* AssociativeArray thread-safe and adapts it to partially implement the
* Map interface.  The portions not implemented here are implemented by
* Hashtable via inheritance from AbstractMap.
*/
class COH_EXPORT HashStorage
    : public cloneable_spec<HashStorage,
        extends<Storage>,
        implements<Map> >
    {
    friend class factory<HashStorage>;

    // ----- handle definitions (needed for nested classes) -------------
    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

    // ----- constructors -----------------------------------------------

    protected:
        HashStorage(size32_t cEstimate, float32_t flLoadFactor,
                float32_t flGrowth)
            : super(value(), cEstimate, flLoadFactor, flGrowth)
            {
            }

        HashStorage(const HashStorage& that)
            : super(value(), that.size(), that.m_flLoadFactor,
                    that.m_flGrowth)
            {
            bool fEscaped = _isEscaped();
            for (Iterator::Handle hIter = that.iterator();
                hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                key              keyNew(vEntry->getKey());
                value            valNew(vEntry->getValue());

                at(keyNew) = valNew;

                keyNew.attach(fEscaped);
                valNew.attach(fEscaped);
                }
            }

        virtual ~HashStorage()
            {
            bool fEscaped = _isEscaped(/*fCapture*/ false);
            // detach from all objects
            Storage::Element* aElement = m_aElements;
            size32_t          cSlots   = getSlots();
            for (size_t i = 0; i < cSlots; ++i)
                {
                if (aElement[i].state == Storage::mapped)
                    {
                    aElement[i].key.detach(fEscaped);
                    aElement[i].value.detach(fEscaped);
                    }
                }
            }

    // ----- Map interface ----------------------------------------------

    public:
        virtual size32_t size() const
            {
            return Storage::size(); // dirty read
            }

        virtual bool isEmpty() const
            {
            return size() == 0;
            }

        virtual bool containsKey(Object::View vKey) const
            {
            COH_SYNCHRONIZED (this)
                {
                return Storage::containsKey(key(vKey));
                }
            }
        using Storage::containsKey;

        virtual bool containsValue(Object::View vValue) const
            {
            COH_SYNCHRONIZED (this)
                {
                return Storage::containsValue(value(vValue));
                }
            }
        using Storage::containsValue;

        virtual Object::Holder get(Object::View vKey) const
            {
            COH_SYNCHRONIZED (this)
                {
                return Storage::get(key(vKey));
                }
            }

        virtual Object::Holder get(Object::View vKey)
            {
            COH_SYNCHRONIZED (this)
                {
                return Storage::get(key(vKey));
                }
            }
        using Storage::get;

        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue)
            {
            bool  fEscaped = false;
            key   keyNew(vKey);
            value valNew(ohValue);
            value valOld;

            COH_SYNCHRONIZED (this)
                {
                size32_t cmPre    = getModCount();
                value&   valCurr  = at(keyNew);

                fEscaped = _isEscaped();
                valNew.attach(fEscaped); // attach value
                if (getModCount() != cmPre)
                    {
                    // new key; attach
                    keyNew.attach(fEscaped);
                    }

                valOld  = valCurr; // record old
                valCurr = valNew;  // update storage
                }

            Object::Holder ohOld = valOld;

            // detach old; outside of sync for performance
            valOld.detach(fEscaped);

            return ohOld;
            }
        using Storage::put;

        virtual void putAll(Map::View vMap)
            {
            Iterator::Handle hIter = vMap->entrySet()->iterator();
            COH_SYNCHRONIZED (this)
                {
                bool     fEscaped = _isEscaped();
                size32_t cmPre    = getModCount();
                while (hIter->hasNext())
                    {
                    Map::Entry::View vEntry = cast<Map::Entry::View>(
                            hIter->next());
                    Object::View   vKey  = vEntry->getKey();
                    Object::Holder ohVal = vEntry->getValue();

                    key   keyNew(vKey);
                    value valNew(ohVal);

                    valNew.attach(fEscaped); // attach new value

                    value& valCur = at(keyNew);

                    valCur.detach(fEscaped); // detach old value
                    valCur = valNew;         // update storage

                    size32_t cmPost = getModCount();
                    if (cmPre != cmPost)
                        {
                        // insert; attach new key
                        keyNew.attach(fEscaped);
                        cmPre = cmPost;
                        }
                    }
                }
            }

        virtual Object::Holder remove(Object::View vKey)
            {
            key   keyRem(vKey);
            value valRem;

            bool fDetach  = false;
            bool fEscaped = false;
            COH_SYNCHRONIZED (this)
                {
                // can't use erase as we need access to the stored key
                // so we can detach it
                Element* pElement = Storage::find(keyRem,
                        /*fConst*/ true, /*fGrow*/ false);
                if (pElement != NULL)
                    {
                    keyRem   = pElement->key; // the key to detach
                    valRem   = pElement->value;

                    // recycle the element
                    pElement->value = m_valDefault;
                    pElement->state = recycled;
                    --m_cMappings;

                    fDetach  = true;
                    fEscaped = _isEscaped();
                    }
                }

            Object::Holder ohOld = valRem; // retain removed value

            if (fDetach)
                {
                // detach removed key and value
                keyRem.detach(fEscaped);
                valRem.detach(fEscaped);
                }

            return ohOld;
            }
        using Map::remove;

        virtual void clear()
            {
            COH_SYNCHRONIZED (this)
                {
                // detach from all objects
                bool fEscaped = _isEscaped();
                Storage::Element* aElement = m_aElements;
                size32_t          cSlots   = getSlots();
                for (size_t i = 0; i < cSlots; ++i)
                    {
                    if (aElement[i].state == Storage::mapped)
                        {
                        aElement[i].key.detach(fEscaped);
                        aElement[i].value.detach(fEscaped);
                        }
                    }

                Storage::clear();
                }
            }

        virtual Set::View keySet() const
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        virtual Set::Handle keySet()
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        virtual Collection::View values() const
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        virtual Collection::Handle values()
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        virtual Set::View entrySet() const
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        virtual Set::Handle entrySet()
            {
            COH_THROW (UnsupportedOperationException::create());
            }



// ----- helper methods ----------------------------------------------

public:
    virtual Iterator::Handle iterator() const
        {
        return EntryIterator::create(this);
        }

    virtual Muterator::Handle iterator()
        {
        return EntryIterator::create(this);
        }

    virtual ObjectArray::Handle toArray(
            ObjectArray::Handle hao = NULL) const
        {
        // can't use Collections::toArray, as we must shallow clone
        // the Entries
        COH_SYNCHRONIZED (this)
            {
            size32_t ce = Storage::size();
            if (hao == NULL || hao->length < ce)
                {
                hao = ObjectArray::create(ce);
                }
            else if (hao->length > ce)
                {
                hao[ce] = NULL;
                }


            Storage::Element* aElement = m_aElements;
            size32_t ofOut = 0;
            for (size32_t i = 0, c = getSlots(); i < c; ++i)
                {
                if (aElement[i].state == HashStorage::mapped)
                    {
                    hao[ofOut++] = SimpleMapEntry::create(
                            aElement[i].key, aElement[i].value);
                    }
                }

            return hao;
            }
        }

    // ----- Object interface -------------------------------------------

    protected:

        virtual void onEscape(bool fEscaped) const
            {
            super::onEscape(fEscaped);

            // single thread, no need for locking
            Storage::Element*  aElement = m_aElements;
            size32_t           cSlots   = getSlots();
            for (size_t i = 0; i < cSlots; ++i)
                {
                if (aElement[i].state == Storage::mapped)
                    {
                    aElement[i].key.attach(fEscaped);
                    aElement[i].key.detach(!fEscaped);

                    aElement[i].value.attach(fEscaped);
                    aElement[i].value.detach(!fEscaped);
                    }
                }
            }

    public:
        virtual size64_t sizeOf(bool fDeep = false) const
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
                        // copy contents into local array to avoid possible
                        // deadlock if we sync'd and called into sizeOf
                        ObjectArray::Handle ha = toArray();

                        cb = sizeOf(false) + (getSlots() * sizeof(Storage::Element));
                        for (size32_t i = 0, c = ha->length; i < c; ++i)
                            {
                            SimpleMapEntry::View e = cast<SimpleMapEntry::View>(ha[i]);
                            if (e != NULL)
                                {
                                Object::View k = e->getKey();
                                if (k != NULL)
                                    {
                                    cb += k->sizeOf(true);
                                    }
                                Object::Holder v = e->getValue();
                                if (v != NULL)
                                    {
                                    cb += v->sizeOf(true);
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
            else
                {
                // Note we don't include the size of the array as it
                // is variable length and the object's shallow size
                // must be fixed
                return sizeof(*this);
                }
            }
    };

/**
* Helper intermediate class which allows a class to implement both
* both the Set and Map interfaces.
*
* Note: this is a bit of a hack as this is treated as an interface but
* actually has implementations as well.
*/
class SetMapAdapter
    : public interface_spec<SetMapAdapter,
        implements<Set> >
    {
    // ----- Collection interface ---------------------------------------

    public:
        virtual bool remove(Object::View v)
            {
            return removeElement(v);
            }


    // ----- SetMapAdapter interface ------------------------------------

    public:
        virtual bool removeElement(Object::View) = 0;
    };

// ----- nested class: HashEntrySet -------------------------------------

class COH_EXPORT HashEntrySet
    : public cloneable_spec<HashEntrySet,
        extends<HashStorage>,
        implements<Set, SetMapAdapter> >
    {
    friend class factory<HashEntrySet>;

    // ----- constructors -----------------------------------------------

    protected:
        HashEntrySet(size32_t cInitial, float32_t flLoadFactor,
                float32_t flGrowth)
            : super(cInitial, flLoadFactor, flGrowth)
            {}

        HashEntrySet(const HashEntrySet& that)
            : super(that)
            {}

    // ----- Collection interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return HashStorage::size();
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const
            {
            return HashStorage::isEmpty();
            }

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const
            {
            if (instanceof<Map::Entry::View>(v))
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(v);
                Object::View     vValue = HashStorage::get(vEntry->getKey());
                return Object::equals(vValue, vEntry->getValue());
                }
            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool containsAll(Collection::View vCol) const
            {
            for (Iterator::Handle hIter = vCol->iterator();
                hIter->hasNext(); )
                {
                if (!contains(hIter->next()))
                    {
                    return false;
                    }
                }
            return true;
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            return HashStorage::iterator();
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            return HashStorage::iterator();
            }

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle toArray(
                ObjectArray::Handle hao = NULL) const
            {
            return HashStorage::toArray(hao);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool add(Object::Holder /*oh*/)
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(Collection::View /*vCol*/)
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        /**
        * {@inheritDoc}
        */
        virtual bool removeElement(Object::View v)
            {
            Map::Entry::View vEntry  = cast<Map::Entry::View>(v);
            Object::View     vKey    = vEntry->getKey();
            Object::Holder   ohValue = vEntry->getValue();

            COH_SYNCHRONIZED (this)
                {
                if (Object::equals(HashStorage::get(vKey), ohValue))
                    {
                    size32_t cEntries = HashStorage::size();
                    HashStorage::remove(vKey);
                    return cEntries != HashStorage::size();
                    }
                return false;
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual bool removeAll(Collection::View vCol)
            {
            bool fChanged = false;
            for (Iterator::Handle hIter = vCol->iterator();
                hIter->hasNext(); )
                {
                fChanged |= removeElement(hIter->next());
                }
            return fChanged;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool retainAll(Collection::View vCol)
            {
            bool fChanged = false;
            for (Muterator::Handle hIter = iterator(); hIter->hasNext(); )
                {
                Object::Holder ohNext = hIter->next();
                if (!vCol->contains(ohNext))
                    {
                    hIter->remove();
                    fChanged = true;
                    }
                }
            return fChanged;
            }

        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            HashStorage::clear();
            }

    // ----- Object interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return Collections::toString(Collection::View(this));
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            size32_t nHash = 0;
            for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
                {
                nHash += Object::hashCode(hIter->next());
                }
            return nHash;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const
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
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

Hashtable::Hashtable(size32_t cInitialBuckets,
        float32_t flLoadFactor, float32_t flGrowthRate)
    : f_hMapStorage(self()), f_hSetStorage(self())
    {
    HashEntrySet::Handle hMapSet = HashEntrySet::create(cInitialBuckets,
            flLoadFactor, flGrowthRate);
    initialize(f_hMapStorage, hMapSet);
    initialize(f_hSetStorage, hMapSet);
    }

Hashtable::Hashtable(Map::View vMap)
    : f_hMapStorage(self()), f_hSetStorage(self())
    {
    HashEntrySet::Handle hMapSet = HashEntrySet::create(vMap->size(),
            0.75F, 3.0F);
    initialize(f_hMapStorage, hMapSet);
    initialize(f_hSetStorage, hMapSet);
    putAll(vMap);
    }

Hashtable::Hashtable(const Hashtable& that)
    : f_hMapStorage(self()), f_hSetStorage(self())
    {
    HashEntrySet::Handle hMapSet = cast<HashEntrySet::Handle>(that.f_hMapStorage->clone());
    initialize(f_hMapStorage, hMapSet);
    initialize(f_hSetStorage, hMapSet);
    }

// ----- Map interface ------------------------------------------------------

size32_t Hashtable::size() const
    {
    return f_hMapStorage->size();
    }

bool Hashtable::containsKey(Object::View vKey) const
    {
    return f_hMapStorage->containsKey(vKey);
    }

Object::Holder Hashtable::get(Object::View vKey) const
    {
    return f_hMapStorage->get(vKey);
    }

Object::Holder Hashtable::put(Object::View vKey, Object::Holder ohValue)
    {
    return f_hMapStorage->put(vKey, ohValue);
    }

void Hashtable::putAll(Map::View vMap)
    {
    return f_hMapStorage->putAll(vMap);
    }

Object::Holder Hashtable::remove(Object::View vKey)
    {
    return f_hMapStorage->remove(vKey);
    }

void Hashtable::clear()
    {
    f_hMapStorage->clear();
    }

Set::View Hashtable::entrySet() const
    {
    return f_hSetStorage;
    }

Set::Handle Hashtable::entrySet()
    {
    return f_hSetStorage;
    }

COH_CLOSE_NAMESPACE2


