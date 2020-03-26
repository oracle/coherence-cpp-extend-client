/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Collections.hpp"

#include "coherence/util/AbstractList.hpp"
#include "coherence/util/AbstractMap.hpp"
#include "coherence/util/AbstractSet.hpp"
#include "coherence/util/Arrays.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/LongArrayIterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/SortedMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(Collections)

/*
 * Tuning parameters for algorithms - Many of the List algorithms have
 * two implementations, one of which is appropriate for small sequential access
 * lists. The tuning parameters below determine the cutoff point for what
 * constitutes a "small" sequential access list for each algorithm.
 * Those doing performance work on this code would
 * do well to validate the values of these parameters from time to time.
 * (The first word of each tuning parameter name is the algorithm to which
 * it applies.)
 */
static const size32_t coh_reverse_threshold = 18;

// ----- local class: SingletonIterator ---------------------------------

/**
* Immutable iterator for singleton collections
* @author tb  2008.04.08
*/
class COH_EXPORT SingletonIterator
    : public cloneable_spec<SingletonIterator,
        extends<Object>,
        implements<Muterator> >
    {
    friend class factory<SingletonIterator>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * @internal
        */
        SingletonIterator(Object::Holder hElement)
                : f_hElement(self(), hElement),
                  m_fHasNext(true)
            {
            }

        /**
        * Copy constructor.
        */
        SingletonIterator(const SingletonIterator& that)
                : f_hElement(self(), that.f_hElement),
                  m_fHasNext(that.m_fHasNext)
            {
            }

    // ----- Iterator interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            return m_fHasNext;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            if (m_fHasNext)
                {
                m_fHasNext = false;
                return f_hElement;
                }
            COH_THROW (NoSuchElementException::create());
            }

        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            COH_THROW (UnsupportedOperationException::create());
            }


    // ----- data members -------------------------------------------

    public:
        /**
        * The associated object.
        */
        FinalHolder<Object> f_hElement;

        /**
        * true if the iterator has a next element.
        */
        bool m_fHasNext;
    };


// ----- local class: SingletonListIterator -----------------------------

/**
* Immutable iterator for singleton lists
* @author tb  2008.04.08
*/
class COH_EXPORT SingletonListIterator
    : public cloneable_spec<SingletonListIterator,
        extends<Object>,
        implements<ListMuterator> >
    {
    friend class factory<SingletonListIterator>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new SingletonListIterator instance.
        */
        SingletonListIterator(Object::Holder hElement)
                : f_hElement(self(), hElement),
                  m_fHasNext(true)
            {
            }

        /**
        * Copy constructor.
        */
        SingletonListIterator(const SingletonListIterator& that)
                : f_hElement(self(), that.f_hElement),
                  m_fHasNext(that.m_fHasNext)
            {
            }

    // ----- ListIterator interface ---------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t nextIndex() const
            {
            return m_fHasNext ? 0 : 1;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t previousIndex() const
            {
            return m_fHasNext ? List::npos : 0;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool hasPrevious() const
            {
            return !m_fHasNext;
            }

        /**
        * {@inheritDoc}
        */
         virtual Object::Holder previous()
             {
             if (!m_fHasNext)
                 {
                 m_fHasNext = true;
                 return f_hElement;
                 }
             COH_THROW (NoSuchElementException::create());
             }

         /**
         * {@inheritDoc}
         */
         virtual Object::Holder peekPrevious() const
             {
             if (!m_fHasNext)
                 {
                 return f_hElement;
                 }
             COH_THROW (NoSuchElementException::create());
             }

         /**
         * {@inheritDoc}
         */
         virtual bool hasNext() const
             {
             return m_fHasNext;
             }

         /**
         * {@inheritDoc}
         */
         virtual Object::Holder next()
             {
             if (m_fHasNext)
                 {
                 m_fHasNext = false;
                 return f_hElement;
                 }
             COH_THROW (NoSuchElementException::create());
             }

         /**
         * {@inheritDoc}
         */
         virtual void remove()
             {
             COH_THROW (UnsupportedOperationException::create());
             }

         /**
         * {@inheritDoc}
         */
         virtual void add(Object::Holder /*oh*/)
             {
             COH_THROW (UnsupportedOperationException::create());
             }

         /**
         * {@inheritDoc}
         */
         virtual void set(Object::Holder /*oh*/)
             {
             COH_THROW (UnsupportedOperationException::create());
             }


    // ----- data members -------------------------------------------

    public:
        /**
        * The associated object.
        */
        FinalHolder<Object> f_hElement;

        /**
        * true if the iterator has a next element.
        */
        bool m_fHasNext;
    };


// ----- local class: SingletonSet --------------------------------------

/**
* Immutable set containing only a single specified object.
*
* @author tb  2008.04.08
*/
class COH_EXPORT SingletonSet
    : public cloneable_spec<SingletonSet,
        extends<AbstractSet> >
    {
    friend class factory<SingletonSet>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new SingletonSet instance.
        */
        SingletonSet(Object::Holder hElement)
                : f_hElement(self(), hElement)
            {
            }

        /**
        * Copy constructor.
        */
        SingletonSet(const SingletonSet& that)
                : f_hElement(self(), that.f_hElement)
            {
            }

    // ----- Set interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return 1;
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            return SingletonIterator::create(f_hElement);
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            return SingletonIterator::create(f_hElement);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const
            {
            return (NULL == f_hElement ? NULL == v :
                    f_hElement->equals(v));
            }


    // ----- data members -------------------------------------------

    public:
        /**
        * The set's single object.
        */
        FinalHolder<Object> f_hElement;
    };


// ----- local class: SingletonList -------------------------------------

/**
* Immutable list containing only a single specified object.
*
* @author tb  2008.04.08
*/
class COH_EXPORT SingletonList
    : public cloneable_spec<SingletonList,
        extends<AbstractList> >
    {
    friend class factory<SingletonList>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new SingletonList instance.
        */
        SingletonList(Object::Holder hElement)
                : f_hElement(self(), hElement)
            {
            }

        /**
        * Copy constructor.
        */
        SingletonList(const SingletonList& that)
                : f_hElement(self(), that.f_hElement)
            {
            }

    // ----- List interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return 1;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(size32_t i) const
            {
            if (i != 0)
                {
                COH_THROW_STREAM(
                        IndexOutOfBoundsException, "index=" << i <<
                        ", length()=1");
                }
            return f_hElement;
            }

        /**
        * {@inheritDoc}
        */
        using List::get;

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const
            {
            return (NULL == f_hElement ? NULL == v :
                    f_hElement->equals(v));
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            return SingletonIterator::create(f_hElement);
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            return SingletonIterator::create(f_hElement);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool add(size32_t /*i*/, Object::Holder /*oh*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The list does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(size32_t /*i*/, Collection::View /*vc*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The list does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t indexOf(Object::View v) const
            {
            return contains(v) ? 0 : -1;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t lastIndexOf(Object::View v) const
            {
            return indexOf(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual ListIterator::Handle listIterator(size32_t i) const
            {
            if (i != 0)
                {
                COH_THROW_STREAM(IndexOutOfBoundsException, "index=" <<
                        i <<  ", length()=1");
                }
            return SingletonListIterator::create(f_hElement);
            }

        /**
        * {@inheritDoc}
        */
        virtual ListMuterator::Handle listIterator(size32_t i)
            {
            if (i != 0)
                {
                COH_THROW_STREAM(IndexOutOfBoundsException, "index=" <<
                        i <<  ", length()=1");
                }
            return SingletonListIterator::create(f_hElement);
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(size32_t /*i*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The list does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder set(size32_t /*i*/, Object::Holder /*oh*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The list does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual List::View subList(size32_t iFromIndex, size32_t iToIndex) const
            {
            if (iFromIndex != 0 || iToIndex != 1)
                {
                COH_THROW_STREAM(IndexOutOfBoundsException,
                        "fromIndex=" << iFromIndex <<  ", toIndex=" <<
                        iToIndex << ", length()=1");
                }
            return this;
            }

        /**
        * {@inheritDoc}
        */
        virtual List::Handle subList(size32_t iFromIndex, size32_t iToIndex)
            {
            if (iFromIndex != 0 || iToIndex != 1)
                {
                COH_THROW_STREAM(IndexOutOfBoundsException,
                        "fromIndex=" << iFromIndex <<  ", toIndex=" <<
                        iToIndex << ", length()=1");
                }
            return this;
            }


    // ----- Collection interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        using Collection::add;

        /**
        * {@inheritDoc}
        */
        using Collection::addAll;

        /**
        * {@inheritDoc}
        */
        using Collection::remove;


    // ----- data members -------------------------------------------

    public:
        /**
        * The list's single object.
        */
        FinalHolder<Object> f_hElement;
    };

// ----- local class: ImmutableEntry ------------------------------------

/**
* Immutable entry for singleton maps
*
* @author tb  2008.04.08
*/
class COH_EXPORT ImmutableEntry
    : public cloneable_spec<ImmutableEntry,
        extends<Object>,
        implements<Map::Entry> >
            {
    friend class factory<ImmutableEntry>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new ImmutableEntry instance.
        */
        ImmutableEntry(Object::View vKey, Object::Holder ohValue)
                : f_vKey(self(), vKey),
                  f_ohValue(self(), ohValue)
            {
            }

        /**
        * Copy constructor.
        */
        ImmutableEntry(const ImmutableEntry& that)
                : f_vKey(self(), that.f_vKey),
                  f_ohValue(self(), that.f_ohValue)
            {
            }

    // ----- Map::Entry interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View getKey() const
            {
            return f_vKey;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getValue() const
            {
            return f_ohValue;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getValue()
            {
            return f_ohValue;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder setValue(Object::Holder /*ohValue*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The entry does not support modification operations"));
            }

    // ----- Object interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return Collections::toString(this);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            return Object::hashCode(getKey());
            }

    // ----- data members -------------------------------------------

    public:
        /**
        * The single key.
        */
        FinalView<Object> f_vKey;

        /**
        * The single value.
        */
        FinalHolder<Object> f_ohValue;
    };


/**
* Helper intermediate class which allows a class to implement both
* both the Set and Map interfaces.
*
* Note: this is a bit of a hack as this is treated as an interface but
* actually has implementations as well.
*/
class MapSetAdapter
    : public virtual Map
        {
        public:
            virtual Object::Holder remove(Object::View v)
                {
                return removeEntry(v);
                }
            using Map::remove;

            virtual Object::Holder removeEntry(Object::View) = 0;
        };

// ----- local class: SingletonMap --------------------------------------

/**
* Immutable map containing only a single specified key/value pair.
*
* SingletonMap implements Map, Set, and Map::Entry, effectively reducing
* the number of dynamic allocations by 2x when creating them for the
* purpose of calling putAll on a Map.  When used in such a fashion the
* dynamic allocations are 2 per putAll, one is the SingletonMap, the
* other is the Iterator.  The original implementation would create the
* SingletonMap, a SingletonSet, an ImmutableEntry, and finally an Iterator.
*
* @author tb/mf  2008.04.08
*/
class COH_EXPORT SingletonMap
    :
    protected virtual AbstractSet,    // dynamic allocation optimization
    protected virtual ImmutableEntry, // dynamic allocation optimization
    protected virtual MapSetAdapter,  // name collision resolution
    public cloneable_spec<SingletonMap,
        extends<AbstractMap>,
        implements<Map::Entry, Set> >   // for fast cast of above cheats
    {
    friend class factory<SingletonMap>;

    // ----- typedefs -----------------------------------------------

    public:
        typedef TypedHandle<SingletonMap> Handle;


    // ----- constructors -------------------------------------------

    public:
        static Handle create(Object::View vKey,
                Object::Holder ohValue)
            {
            return new SingletonMap(vKey, ohValue);
            }

        static Handle create(const SingletonMap& that)
            {
            return new SingletonMap(that);
            }

    protected:
        /**
        * Create a new SingletonMap instance.
        */
        SingletonMap(Object::View vKey, Object::Holder ohValue)
            : ImmutableEntry(vKey, ohValue)
            {
            }

        /**
        * Copy constructor.
        */
        SingletonMap(const SingletonMap& that)
            : ImmutableEntry(that)
            {
            }

    // ----- Map interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View /*vKey*/,
                Object::Holder /*ohValue*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder removeEntry(Object::View /*vKey*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const
            {
            return (Set::View) this;
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet()
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }


    // ----- Set interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return 1;
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            return SingletonIterator::create((Map::Entry::View) this);
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            return SingletonIterator::create((Map::Entry::View) this);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const
            {
            if (instanceof<Map::Entry::View>(v))
                {
                Map::Entry::View vE = cast<Map::Entry::View>(v);
                return Object::equals(getKey(), vE->getKey()) &&
                       Object::equals(getValue(), vE->getValue());
                }
            return false;
            }

    // ----- Object interface -------------------------------------------

    public:
        virtual bool equals(Object::View vThat) const
            {
            if (this == vThat)
                {
                return true;
                }
            else if (NULL == vThat)
                {
                return false;
                }

            // vThat is allowed to be a Map, a Set, or a Map::Entry
            if (instanceof<Map::View>(vThat))
                {
                return AbstractMap::equals(vThat);
                }
            else if (instanceof<Set::View>(vThat))
                {
                return AbstractSet::equals(vThat);
                }
            else if (instanceof<Map::Entry::View>(vThat))
                {
                return ImmutableEntry::equals(vThat);
                }
            return false;
            }

        virtual size64_t sizeOf(bool fDeep = false) const
            {
            return fDeep
                    ? Object::sizeOf(true)
                    : sizeof(SingletonMap);
            }

        virtual size32_t hashCode() const
            {
            return ImmutableEntry::hashCode();
            }

        virtual TypedHandle<const String> toString() const
            {
            return Collections::toString((Map::View) this);
            }

        virtual Object::Handle clone() const
            {
            return new SingletonMap(*this);
            }

        virtual void* _cast(coh_class_id pInfo) const
            {
            return cloneable_spec<SingletonMap,
                    extends<AbstractMap>,
                    implements<Map::Entry, Set> >::_cast(pInfo);
            }

        COH_GENERATE_CLASS_ID(SingletonMap)
        virtual coh_class_id _getClassId() const
            {
            return COH_CLASS_ID(SingletonMap);
            }
    };


// ----- local class: SynchronizedMap -------------------------------

/**
* Returns a synchronized (thread-safe) map backed by the specified
* map.
*
* @author lh  2010.09.21
*/
class COH_EXPORT SynchronizedMap
    : public cloneable_spec<SynchronizedMap,
        extends<Object>,
        implements<Map> >
    {
    friend class factory<SynchronizedMap>;


    // ----- constructors -------------------------------------------

    protected:
        /**
        * Construct a synchronized map wrapper for the specified map.
        *
        * @param hMap  the map to be "wrapped" in a synchronized map
        */
        SynchronizedMap(Map::Holder ohMap)
            : f_ohMap(self(), ohMap)
            {
            }

        /**
        * Copy constructor.
        */
        SynchronizedMap(const SynchronizedMap& that)
            : f_ohMap(self())
            {
            COH_SYNCHRONIZED (&that)
                {
                initialize(f_ohMap, cast<Map::Holder>(
                        that.f_ohMap->clone()));
                }
            }


    // ----- Map interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->size();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->isEmpty();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual bool containsKey(Object::View vKey) const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->containsKey(vKey);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual bool containsValue(Object::View vValue) const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->containsValue(vValue);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->get(vKey);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey)
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->get(vKey);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey,
                Object::Holder ohValue)
            {
            COH_SYNCHRONIZED (this)
                {
                Map::Handle hMap = cast<Map::Handle>(f_ohMap);
                return hMap->put(vKey, ohValue);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(Object::View vKey)
            {
            COH_SYNCHRONIZED (this)
                {
                Map::Handle hMap = cast<Map::Handle>(f_ohMap);
                return hMap->remove(vKey);
                }
            }
        using Map::remove;

        /**
        * {@inheritDoc}
        */
        virtual void putAll(Map::View vMap)
            {
            COH_SYNCHRONIZED (this)
                {
                Map::Handle hMap = cast<Map::Handle>(f_ohMap);
                return hMap->putAll(vMap);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            COH_SYNCHRONIZED (this)
                {
                Map::Handle hMap = cast<Map::Handle>(f_ohMap);
                hMap->clear();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::View keySet() const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->keySet();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle keySet()
            {
            COH_SYNCHRONIZED (this)
                {
                Map::Handle hMap = cast<Map::Handle>(f_ohMap);
                return hMap->keySet();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Collection::View values() const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->values();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Collection::Handle values()
            {
            COH_SYNCHRONIZED (this)
                {
                Map::Handle hMap = cast<Map::Handle>(f_ohMap);
                return hMap->values();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->entrySet();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet()
            {
            COH_SYNCHRONIZED (this)
                {
                Map::Handle hMap = cast<Map::Handle>(f_ohMap);
                return hMap->entrySet();
                }
            }


    // ----- Object interface ---------------------------------------

    public:
        virtual bool equals(Object::View vThat) const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->equals(vThat);
                }
            }

        virtual size64_t sizeOf(bool fDeep = false) const
            {
            return fDeep
                    ? Object::sizeOf(true)
                    : sizeof(SynchronizedMap);
            }

        virtual size32_t hashCode() const
            {
            COH_SYNCHRONIZED (this)
                {
                return f_ohMap->hashCode();
                }
            }

        virtual TypedHandle<const String> toString() const
            {
            COH_SYNCHRONIZED (this)
                {
                return Collections::toString((Map::View) this);
                }
            }


    // ----- data members -------------------------------------------

    protected:
        /**
        * The map to be synchronized.
        */
        FinalHolder<Map> f_ohMap;
    };


// ----- local class: SynchronizedSortedMap -----------------------------

/**
* Returns a synchronized (thread-safe) sorted map backed by the specified
* sorted map.
*
* @author lh  2010.09.21
*/
class COH_EXPORT SynchronizedSortedMap
    : public cloneable_spec<SynchronizedSortedMap,
        extends<SynchronizedMap>,
        implements<SortedMap> >
    {
    friend class factory<SynchronizedSortedMap>;


    // ----- constructors -------------------------------------------

    protected:
        /**
        * Construct a synchronized wrapper for the specified sorted map.
        *
        * @param hSortedMap  the sorted map to be "wrapped" in a
        *                    synchronized sorted map
        */
        SynchronizedSortedMap(SortedMap::Holder hSortedMap)
            : super(hSortedMap)
            {
            }

        /**
        * Copy constructor.
        */
        SynchronizedSortedMap(const SynchronizedSortedMap& that)
            : super(that)
            {
            }


    // ----- SortedMap interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Comparator::View comparator() const
            {
            COH_SYNCHRONIZED (this)
                {
                return cast<SortedMap::View>(f_ohMap)->comparator();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::View firstKey() const
            {
            COH_SYNCHRONIZED (this)
                {
                return cast<SortedMap::View>(f_ohMap)->firstKey();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::View lastKey() const
            {
            COH_SYNCHRONIZED (this)
                {
                return cast<SortedMap::View>(f_ohMap)->lastKey();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View headMap(Object::View vToKey) const
            {
            COH_SYNCHRONIZED (this)
                {
                return SynchronizedSortedMap::create(
                    cast<SortedMap::View>(f_ohMap)->headMap(vToKey));
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle headMap(Object::View vToKey)
            {
            COH_SYNCHRONIZED (this)
                {
                return SynchronizedSortedMap::create(
                    cast<SortedMap::Handle>(f_ohMap)->headMap(vToKey));
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View subMap(Object::View vFromKey,
                Object::View vToKey) const
            {
            COH_SYNCHRONIZED (this)
                {
                return SynchronizedSortedMap::create(
                    cast<SortedMap::View>(f_ohMap)->subMap(vFromKey,
                            vToKey));
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle subMap(Object::View vFromKey,
                Object::View vToKey)
            {
            COH_SYNCHRONIZED (this)
                {
                return SynchronizedSortedMap::create(
                    cast<SortedMap::Handle>(f_ohMap)->subMap(vFromKey,
                            vToKey));
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::View tailMap(Object::View vFromKey) const
            {
            COH_SYNCHRONIZED (this)
                {
                return SynchronizedSortedMap::create(
                    cast<SortedMap::View>(f_ohMap)->tailMap(vFromKey));
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual SortedMap::Handle tailMap(Object::View vFromKey)
            {
            COH_SYNCHRONIZED (this)
                {
                return SynchronizedSortedMap::create(
                    cast<SortedMap::Handle>(f_ohMap)->tailMap(vFromKey));
                }
            }
    };


// ----- local class: UnmodifiableCollection ------------------------

/**
* A Collection wrapper that provides an unmodifiable view of a
* specified collection.
*
* @author tb  2008.06.23
*/
class COH_EXPORT UnmodifiableCollection
    : public cloneable_spec<UnmodifiableCollection,
        extends<AbstractCollection> >
    {
    friend class factory<UnmodifiableCollection>;

    // ----- constructors -----------------------------------------------

    protected:
        /**
        * Create a new UnmodifiableCollection instance.
        *
        * @param vCollection  the Collection to be wrapped
        */
        UnmodifiableCollection(Collection::View vCollection)
                : f_vCollection(self(), vCollection)
            {
            }

        /**
        * Copy constructor.
        */
        UnmodifiableCollection(const UnmodifiableCollection& that)
                : f_vCollection(self(), that.f_vCollection)
            {
            }

    // ----- Collection interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return f_vCollection->size();
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            return f_vCollection->iterator();
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            COH_THROW(UnsupportedOperationException::create(
                    "The collection does not support modification operations"));
            }

    // ----- data members -------------------------------------------

    protected:
        /**
        * The wrapped collection.
        */
        FinalView<Collection> f_vCollection;
    };


// ----- local class: UnmodifiableSet -----------------------------------

/**
* A Set wrapper that provides an unmodifiable view of a
* specified set.
*
* @author tb  2008.06.23
*/
class COH_EXPORT UnmodifiableSet
    : public cloneable_spec<UnmodifiableSet,
        extends<UnmodifiableCollection>,
        implements<Set> >
    {
    friend class factory<UnmodifiableSet>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * @internal
        */
        UnmodifiableSet(Set::View vSet)
                : super(vSet)
            {
            }

        /**
        * Copy constructor.
        */
        UnmodifiableSet(const UnmodifiableSet& that)
                : super(that.f_vCollection)
            {
            }

    // ----- Object interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const
            {
            return f_vCollection->equals(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            return f_vCollection->hashCode();
            }
    };


// ----- local class: EmptyIterator -------------------------------------

/**
* Immutable iterator for empty collections
* @author tb  2008.07.30
*/
class EmptyIterator
    : public cloneable_spec<EmptyIterator,
        extends<Object>,
        implements<Muterator> >
    {
    friend class factory<EmptyIterator>;

    // ----- constructors/destructor --------------------------------

    protected:
        /**
        * Create a new EmptyIterator instance.
        */
        EmptyIterator()
            {
            }

        /**
        * Copy constructor.
        */
        EmptyIterator(const EmptyIterator& /*that*/)
            {
            }

    // ----- Iterator interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            COH_THROW (NoSuchElementException::create());
            }

        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            COH_THROW (UnsupportedOperationException::create());
            }
    };


// ----- local class: EmptySet ------------------------------------------

/**
* The empty set (immutable).
*
* @author tb  2008.07.30
*/
class EmptySet
    : public cloneable_spec<EmptySet,
        extends<AbstractSet> >
    {
    friend class factory<EmptySet>;

    // ----- constructors -----------------------------------------

    protected:
        /**
        * Create a new EmptySet instance.
        */
        EmptySet()
            {
            }

        /**
        * Copy constructor.
        */
        EmptySet(const EmptySet& /*that*/)
            {
            }

    // ----- Set interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return 0;
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            return EmptyIterator::create();
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            return EmptyIterator::create();
            }

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View /*v*/) const
            {
            return false;
            }
    };


// ----- local class: EmptyMap ------------------------------------------

/**
* The empty map (immutable).
*
* @author tb  2008.07.30
*/
class EmptyMap
    : public cloneable_spec<EmptyMap,
        extends<AbstractMap> >
    {
    friend class factory<EmptyMap>;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Create a new EmptyMap instance.
        */
        EmptyMap()
            {
            }

        /**
        * Copy constructor.
        */
        EmptyMap(const EmptyMap& /*that*/)
            {
            }

    // ----- Map interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View /*vKey*/,
                Object::Holder /*ohValue*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(Object::View /*vKey*/)
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }
        using Map::remove;

        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const
            {
            return Collections::emptySet();
            }

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet()
            {
            COH_THROW (UnsupportedOperationException::create(
                    "The map does not support modification operations"));
            }
    };

COH_CLOSE_NAMESPACE_ANON

// ----- Collections interface ----------------------------------------------

void Collections::reverse(List::Handle hList)
    {
    size32_t cSize = hList->size();
    if (cSize < coh_reverse_threshold)
        {
        for (size32_t i = 0, mid = cSize >> 1, j = cSize - 1; i < mid; i++, j--)
            swap(hList, i, j);
        }
    else
        {
        ListMuterator::Handle hFwd = hList->listIterator();
        ListMuterator::Handle hRev = hList->listIterator(cSize);
        for (size32_t i = 0, mid = hList->size() >> 1; i < mid; i++)
            {
            Object::Holder ohTmp = hFwd->next();
            hFwd->set(hRev->previous());
            hRev->set(ohTmp);
            }
        }
    }

void Collections::sort(List::Handle hList)
    {
    size32_t            cElements = hList->size();
    ObjectArray::Handle hArr      = ObjectArray::create(cElements);

    ListIterator::Handle hIterator = hList->listIterator();
    size32_t i;
    for (i = 0; hIterator->hasNext(); ++i)
        {
        hArr[i] = hIterator->next();
        }

    Arrays::sort(hArr);

    for (i = 0; i < cElements; ++i)
        {
        hList->set(i, hArr[i]);
        }
    }

void Collections::sort(List::Handle hList, Comparator::Handle hComparator)
    {
    size32_t            cElements = hList->size();
    ObjectArray::Handle hArr      = ObjectArray::create(cElements);

    ListIterator::Handle hIterator = hList->listIterator();
    size32_t i;
    for (i = 0; hIterator->hasNext(); ++i)
        {
        hArr[i] = hIterator->next();
        }

    Arrays::sort(hArr, hComparator);

    for (i = 0; i < cElements; ++i)
        {
        hList->set(i, hArr[i]);
        }
    }

void Collections::swap(List::Handle hList, size32_t i, size32_t j)
    {
    hList->set(i, hList->set(j, hList->get(i)));
    }

Set::View Collections::singleton(Object::Holder ohElement)
    {
    return SingletonSet::create(ohElement);
    }

List::View Collections::singletonList(Object::Holder ohElement)
    {
    return SingletonList::create(ohElement);
    }

Map::View Collections::singletonMap(Object::View vKey,
        Object::Holder ohValue)
    {
    return SingletonMap::create(vKey, ohValue);
    }

Map::Handle Collections::synchronizedMap(Map::Handle hMap)
    {
    return SynchronizedMap::create(hMap);
    }

Map::View Collections::synchronizedMap(Map::View vMap)
    {
    return SynchronizedMap::create(vMap);
    }

SortedMap::Handle Collections::synchronizedSortedMap(SortedMap::Handle hSortedMap)
    {
    return SynchronizedSortedMap::create(hSortedMap);
    }

SortedMap::View Collections::synchronizedSortedMap(SortedMap::View vSortedMap)
    {
    return SynchronizedSortedMap::create(vSortedMap);
    }

Collection::Handle Collections::unmodifiableCollection(Collection::View
        vCollection)
    {
    return UnmodifiableCollection::create(vCollection);
    }

Set::Handle Collections::unmodifiableSet(Set::View vSet)
    {
    return UnmodifiableSet::create(vSet);
    }

Set::View Collections::emptySet()
    {
    static FinalView<Set> v(System::common(), EmptySet::create());
    return v;
    }
COH_STATIC_INIT(Collections::emptySet());

Map::View Collections::emptyMap()
    {
    static FinalView<Map> v(System::common(), EmptyMap::create());
    return v;
    }
COH_STATIC_INIT(Collections::emptyMap());

String::View Collections::toString(Iterator::Handle hIter)
    {
    if (NULL == hIter)
        {
        return "NULL";
        }
    LongArrayIterator::Handle hIterLong =
        cast<LongArrayIterator::Handle>(hIter, /*fThrow*/ false);
    size32_t cOut   = 0;
    int64_t  lIndex = 0;
    String::View vs = "{";
    for (bool fMore = hIter->hasNext(); fMore; ++cOut)
        {
        Object::View vObj = hIter->next();

        if (NULL != hIterLong)
            {
            // index must be pulled before calling hasNext()
            lIndex = hIterLong->getIndex();
            }

        fMore = hIter->hasNext();
        if (cOut > 0)
            {
            vs = COH_TO_STRING(vs << ",\n\t");
            }
        else if (fMore) // first but more coming
            {
            vs = COH_TO_STRING(vs << "\n\t");
            }

        if (NULL != hIterLong)
            {
            vs = COH_TO_STRING(vs << lIndex << '=');
            }

        if (instanceof<Map::Entry::View>(vObj))
            {
            vs = COH_TO_STRING(vs << toString(cast<Map::Entry::View>(vObj)));
            }
        else
            {
            vs = COH_TO_STRING(vs << vObj);
            }
        }

    if (cOut > 1)
        {
        return COH_TO_STRING(vs << "\n\t}");
        }
    else
        {
        return COH_TO_STRING(vs << '}');
        }
    }

String::View Collections::toString(Collection::View vCol)
    {
    if (NULL == vCol)
        {
        return "NULL";
        }

    return toString(vCol->iterator());
    }

String::View Collections::toString(Map::View vMap)
    {
    if (NULL == vMap)
        {
        return "NULL";
        }

    return toString(vMap->entrySet()->iterator());
    }

String::View Collections::toString(Map::Entry::View vEntry)
    {
    return COH_TO_STRING("key=\"" << vEntry->getKey() << '\"' << ", value=\""
        << vEntry->getValue() << '\"');
    }

ObjectArray::Handle Collections::toArray(Collection::View vCol,
        ObjectArray::Handle hao)
    {
    size32_t c = vCol->size();
    if (hao == NULL || hao->length < c)
        {
        hao = ObjectArray::create(c);
        }
    else if (hao->length > c)
        {
        hao[c] = NULL;
        }

    Iterator::Handle hIter = vCol->iterator();
    for (size32_t i = 0; i < c; ++i)
        {
        try
            {
            hao[i] = hIter->next();
            }
        catch (Exception::View e) // NoSuchElement; IndexOutOfBounds
            {
            COH_THROW (ConcurrentModificationException::create(
                    String::null_string, e));
            }
        }

    return hao;
    }


COH_CLOSE_NAMESPACE2
