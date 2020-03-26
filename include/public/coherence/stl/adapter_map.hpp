/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ADAPTER_MAP_HPP
#define COH_ADAPTER_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/filter/NotFilter.hpp"
#include "coherence/util/filter/PresentFilter.hpp"
#include "coherence/util/processor/ConditionalPut.hpp"
#include "coherence/util/processor/ConditionalPutAll.hpp"

#include <functional>
#include <iterator>
#include <memory>
#include <ostream>

#include <utility>

COH_OPEN_NAMESPACE2(coherence,stl)

using coherence::util::Collection;
using coherence::util::Collections;
using coherence::util::HashMap;
using coherence::util::InvocableMap;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::filter::NotFilter;
using coherence::util::filter::PresentFilter;
using coherence::util::processor::ConditionalPut;
using coherence::util::processor::ConditionalPutAll;

// ----- pre-definitions ----------------------------------------------------

COH_OPEN_NAMESPACE(impl)

template<class K, class V> class _iterator;

template<class K, class V> class _const_iterator;

/**
* @internal
*/
template<class K, class V>
class _adapter_map_base
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * @internal
        */
        typedef K key_type;

        /**
        * @internal
        */
        typedef V data_type;

        /**
        * @internal
        */
        typedef V mapped_type;

        /**
        * @internal
        */
        typedef std::pair<const key_type, mapped_type> value_type;

        /**
        * @internal
        */
        typedef std::allocator<value_type> allocator_type;

        /**
        * @internal
        */
        typedef typename allocator_type::size_type size_type;

        /**
        * @internal
        */
        typedef typename allocator_type::difference_type difference_type;

        /**
        * @internal
        */
        typedef _iterator<K,V> iterator;

        /**
        * @internal
        */
        typedef _const_iterator<K,V> const_iterator;


    // ----- local class methods --------------------------------------------

    protected:
        /**
        * @internal
        */
        iterator _make_iterator(Map::Handle hMap, Iterator::Handle hIter);

        /**
        * @internal
        */
        iterator _make_lazy_iterator(Map::Handle hMap, Object::View vKey);

        /**
        * @internal
        */
        const_iterator _make_const_iterator(Map::View vMap,
                                            Iterator::Handle hIter) const;

        /**
        * @internal
        */
        const_iterator _make_lazy_const_iterator(Map::View vMap,
                                            Object::View vKey) const;
    };

    /**
    * @internal
    *
    * Iterator which represents the end of an iteration.
    */
    class EndIterator
        : public class_spec<EndIterator,
            extends<Object>,
            implements<Iterator> >
        {
        friend class factory<EndIterator>;

        public:
            static Handle getInstance()
                {
                static FinalHandle<EndIterator> hIter
                    (System::common(), create());
                return hIter;
                }

        protected:
            EndIterator()
                {
                }

        public:
            virtual bool hasNext() const
                {
                return false;
                }
            virtual Object::Holder next()
                {
                COH_THROW (NoSuchElementException::create());
                }
        };
    COH_STATIC_INIT(EndIterator::getInstance());

COH_CLOSE_NAMESPACE // impl


// ----- class: adapter_map -------------------------------------------------

/**
* adapter_map provides an STL-like "pair associative container" wrapper
* around coherence::util::Map implementations.
*
* An example usage examples may look as follows:
* @code
* typedef adapter_map<String::View, Integer32::View> month_cache;
*
* month_cache months(CacheFactory::getCache("dist-months"));
*
* months["january"]  = Integer32::valueOf(31);
* months["february"] = Integer32::valueOf(28);
* months["march"]    = Integer32::valueOf(31);
* ...
*
* for (month_cache::iterator i = months.begin(), e = months.end(); i != e; ++i)
*     {
*     std::cout << i->first << " = " << i->second << std::endl;
*     }
* @endcode
*
* adapter_map only supports keys and values which are handles, for instance
* Integer32::View. See boxing_map for a variant which supports keys and values
* of non-managed types, including primitives.
*
* @see boxing_map
* @see Map
* @see TypedCollections
*/
template<class K, class V>
class adapter_map
        : private impl::_adapter_map_base<K,V>
    {
    // ----- typedefs -------------------------------------------------------

    private:
        /**
        * Super type
        */
        typedef impl::_adapter_map_base<K,V> super;

    public:
        /**
        * This type.
        */
        typedef adapter_map<K, V> this_type;

        /**
        * Key type of the map. Should be handle-to-const type.
        */
        typedef typename super::key_type key_type;

        /**
        * Value type of the map. Should be handle-to-const type.
        */
        typedef typename super::data_type data_type;

        /**
        * Value type of the map. Should be handle-to-const type.
        */
        typedef typename super::mapped_type mapped_type;

        /**
        * Entry type of the map. value_type is
        * @code std::pair<const key_type, mapped_type> @endcode.
        */
        typedef typename super::value_type value_type;

        /**
        * Allocator type for this map. allocator_type is
        * @code std::allocator<value_type> @endcode.
        */
        typedef typename super::allocator_type allocator_type;

        /**
        * Signed integral type. Identical to the difference type of iterator
        * and const_iterator.
        */
        typedef typename super::difference_type difference_type;

        /**
        * Unsigned integral type. size_type can represent any non-negative
        * value of difference_type.
        */
        typedef typename super::size_type size_type;

        /**
        * Iterator type of the <i>forward iterator category</i> which
        * iterates key-value std::pairs of this map. Convertible to
        * const_iterator.
        */
        typedef typename super::iterator iterator;

        /**
        * Const iterator type of the <i>forward iterator category</i> which
        * iterates key-value std::pairs of this map.
        */
        typedef typename super::const_iterator const_iterator;

        /**
        * Type of the values returned by @c iterator.operator-\>
        */
        typedef typename iterator::pointer pointer;

        /**
        * Type of the values returned by @c const_iterator.operator-\>
        */
        typedef typename const_iterator::pointer const_pointer;

        /**
        * Type of the values returned by @c iterator.operator*
        */
        typedef typename iterator::reference reference;

        /**
        * Type of the values returned by @c const_iterator.operator*
        */
        typedef typename const_iterator::reference const_reference;

        /**
        * Type of the values returned by adapter_map.operator[]()
        * operator[]
        */
        typedef typename iterator::mapped_reference mapped_reference;


    // ---- hash map-specific -----------------------------------------------

    public:
        class hasher;
        class key_equal;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create new adapter_map from the given Coherence Map.
        *
        * @param ohMap the Map to be delegated by the adapter_map.
        */
        adapter_map(Map::Holder ohMap = NULL)
            : f_ohMap(System::common(), ohMap)
            {
            }

        /**
        * Create a adapter_map copy. The new adapter_map will reference the
        * same Map, as the original adapter_map.
        */
        adapter_map(const this_type& that)
            : f_ohMap(System::common(), that.f_ohMap)
            {
            }

        /**
        * destructor
        */
        virtual ~adapter_map()
            {
            }

        /**
        * Reassign this adapter_map to reference the same Map as another
        * adapter_map.
        */
        this_type& operator=(const this_type& that)
            {
            initialize(f_ohMap, (Map::Holder) that.f_ohMap);
            return *this;
            }

        /**
        * Reassign this adapter_map to reference another Map.
        */
        this_type& operator=(Map::Holder ohMap)
            {
            initialize(f_ohMap, ohMap);
            return *this;
            }


    // ---- delegate access -------------------------------------------------

    public:
        /**
        * Return the coherence::util::Map to which this adapter_map delegates.
        *
        * @return the associated coherence::util::Map::Handle
        *
        * @throws UnsupportedOperationException if the adapater_map is
        *         delegating to a Map::View, rather then Map::Handle
        */
        Map::Handle delegate()
            {
            Map::Holder ohMap = f_ohMap;
            Map::Handle hMap  = cast<Map::Handle>(ohMap, false);
            if (NULL == hMap && NULL != ohMap)
                {
                COH_THROW (UnsupportedOperationException::create());
                }
            return hMap;
            }

        /**
        * Return the coherence::util::Map to which this adapter_map delegates.
        *
        * @return the associated coherence::util::Map::View
        */
        Map::View delegate() const
            {
            return f_ohMap;
            }


    // ---- capacity --------------------------------------------------------

    public:
        /**
        * @return the number of the key-value std::pairs in the map
        */
        size_type size() const
            {
            return delegate()->size();
            }

        /**
        * @return size() of the largest possible map
        */
        size_type max_size() const
            {
            return static_cast<size_type>(-1);
            }

        /**
        * @return true only iff this map is empty
        */
        bool empty() const
            {
            return (size() == 0);
            }


    // ----- iterators ------------------------------------------------------

    public:
        /**
        * @return a const_iterator referring to the first element in the
        *         container; if the map is empty, then
        *         @code cbegin() == cend() @endcode
        */
        const_iterator cbegin() const
            {
            Map::View vMap = delegate();
            return super::_make_const_iterator
                (vMap, vMap->keySet()->iterator());
            }

        /**
        * @return a const_iterator referring to the first element in the
        *         container; if the map is empty, then
        *         @code begin() == end() @endcode
        */
        const_iterator begin() const
            {
            return cbegin();
            }

        /**
        * @return an iterator referring to the first element in the
        *         container; if the map is empty, then
        *         @code begin() == end() @endcode
        */
        iterator begin()
            {
            Map::Handle hMap = delegate();
            return super::_make_iterator(hMap, hMap->keySet()->iterator());
            }

        /**
        * @return a const_iterator which is the past-the-end value for the
        *         container; if the map is empty, then
        *         @code cbegin() == cend() @endcode
        */
        const_iterator cend() const
            {
            return super::_make_const_iterator(delegate(),
                    impl::EndIterator::getInstance());
            }

        /**
        * @return a const_iterator which is the past-the-end value for the
        *         container; if the map is empty, then
        *         @code begin() == end() @endcode
        */
        const_iterator end() const
            {
            return cend();
            }

        /**
        * @return an iterator which is the past-the-end value for the
        *         container; if the map is empty, then
        *         @code begin() == end()  @endcode
        */
        iterator end()
            {
            return super::_make_iterator(delegate(),
                    impl::EndIterator::getInstance());
            }


    // ----- element access -------------------------------------------------

    public:
        /**
        * Return a reference to the value mapped to the key.
        *
        * @param key a key to search in the map
        *
        * @return a reference to x.second, where x is the (unique) element
        *         whose key is equal to @a key
        */
        mapped_reference operator[](const key_type& key);

        /**
        * Return a reference to the value mapped to the key.
        *
        * @param key a key to search in the map
        *
        * @return a reference to x.second, where x is the (unique) element
        *         whose key is equal to @a key
        */
        mapped_type operator[](const key_type& key) const;


    // ----- modifiers ------------------------------------------------------

    public:
        /**
        * Insert @a x if and only if there is no element in the map with key
        * equal to the key of @a x. The bool component of the returned
        * std::pair is true if and only if the insertion takes place, and the
        * iterator component of the std::pair points to the element with key
        * equal to the key of argument.
        *
        * All insert methods do not affect the validity of references to map
        * elements, but may invalidate all iterators to the map.
        *
        * @param x the key-value std::pair to insert into this map
        *
        * @return the std::pair which indicates the insertion status and points to
        *         the corresponding map entry
        */
        std::pair<iterator, bool> insert(const value_type& x);

        /**
        * Insert @a x if and only if there is no element in the map with key
        * equal to the key of @a x.
        *
        * All insert methods do not affect the validity of references to map
        * elements, but may invalidate all iterators to the map.
        *
        * @param hint a hint pointing to where the insert should start to
        *             search. Implementation is permitted to ignore the hint.
        * @param x    the key-value std::pair to insert into this map.
        *
        * @return iterator pointing to the element with key equal to the key
        *         of @a x.
        */
        iterator insert(iterator /*hint*/, const value_type& x)
            {
            return insert(x).first;
            }

        /**
        * Insert each element from the range [@a first, @a last) if and only
        * if there is no element with key equal to the key of that element in
        * this map.
        *
        * All insert methods do not affect the validity of references to map
        * elements, but may invalidate all iterators to the map.
        *
        * @note it is required that @a first and @a last are not iterators
        *       into this map. Otherwise, behavior of
        *       @code insert(first, last) @endcode is undefined.
        */
        template<class InputIterator>
        void insert(InputIterator first, InputIterator last);

        /**
        * Erase an element pointed to by @a position.
        *
        * All erase methods invalidate only iterators and references to the
        * erased elements.
        *
        * @param position an iterator specifying the element to erase
        */
        void erase(iterator position);

        /**
        * Erase all elements in the range <tt>[@a first, last)</tt>.
        *
        * All erase methods invalidate only iterators and references to the
        * erased elements.
        */
        void erase(iterator first, iterator last);

        /**
        * Erase an element in the map with key equal to the argument.
        *
        * All erase methods invalidate only iterators and references to the
        * erased elements.
        *
        * @param k key whose mapping is to be removed from the map.
        *
        * @return  the number of erased elements (0 or 1).
        */
        size_type erase(const key_type& k)
            {
            return delegate()->keySet()->remove(k) ? 1 : 0;
            }

        /**
        * Remove all elements from this map.
        */
        void clear()
            {
            delegate()->clear();
            }

        /**
        * Swap this map and argument map.
        */
        void swap(this_type& x)
            {
            std::swap(f_ohMap, x.f_ohMap);
            }


    // ----- map operations -------------------------------------------------

    public:

        /**
        * @return an iterator pointing to an element with the key equal to
        * @a k, or this-\>end() if such an element is not found.
        */
        iterator find(const key_type& k)
            {
            Map::Handle hMap = delegate();
            if (!hMap->containsKey(k))
                {
                return end();
                }

            return this->_make_lazy_iterator(hMap, k);
            }

        /**
        * @return an iterator pointing to an element with the key equal to
        * @a k, or this-\>end() if such an element is not found.
        */
        const_iterator find(const key_type& k) const
            {
            Map::View vMap = delegate();
            if (!vMap->containsKey(k))
                {
                return end();
                }

            return this->_make_lazy_const_iterator(vMap, k);
            }

        /**
        * @return the number of element with key equal to @a k (0 or 1).
        */
        size_type count(const key_type& k) const
            {
            return delegate()->containsKey(k) ? 1 : 0;
            }

        // sorted map-specific methods lower_bound() and upper_bound() are
        // not supported

        /**
        * @return a std::pair of iterators which locate the element with key @a k.
        *         If such element is found then the first component of the
        *         returned std::pair points to the element and the second
        *         component of the std::pair points to the element immediately
        *         following the found one in the iterator's traverse order.
        *         Otherwise returns a this-\>end(), this-\>;end() std::pair.
        *
        * In other words, equal_range always returns the range [x, y) which
        * contains the found element only.
        */
        std::pair<iterator,iterator> equal_range(const key_type& k);

        /**
        * @return a std::pair of iterators which locate the element with key @a k.
        *         If such element is found then the first component of the
        *         returned std::pair points to the element and the second
        *         component of the std::pair points to the element immediately
        *         following the found one in the iterator's traverse order.
        *         Otherwise returns an this-\>end(), this-\>end() std::pair.
        *
        * In other words, equal_range always returns the range [x, y) which
        * contains the found element only.
        */
        std::pair<const_iterator,const_iterator> equal_range(const key_type& k)
            const;


    // ----- other methods --------------------------------------------------

    public:
        /**
        * @return a copy of the Allocator object
        */
        allocator_type get_allocator() const
            {
            return allocator_type();
            }


    // ----- observers: unordered map ---------------------------------------

    public:
        /**
        * @return hash function
        */
        hasher hash_function() const
            {
            return hasher();
            }

        /**
        * @return key equality predicate
        */
        key_equal key_eq() const
            {
            return key_equal();
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Coherence Map that is delegated by the adapter_map.
        */
        FinalHolder<Map> f_ohMap;
    };

//operators:

// operators ==,!=,<,>,<=,>= are NOT defined for adapter_map
// as adapter_map can be sorted(ordered) or unordered map and
// these operators are not required to be defined for unordered associative containers.
// (see TR1:6.3.1.2 [tr.unord.req.2])

//specialized algorithms:

/** @relates adapter_map
* Swaps two argument maps.
*/
template <class K, class V>
inline void swap(adapter_map<K,V>& x, adapter_map<K,V>& y)
    {
    x.swap(y);
    }

/**
* Copy the contents of one map to another.
*
* @param mapSrc  the source map
* @param mapDes  the destination map
*/
template <class MapSrc, class MapDes> void copy_map(const MapSrc& mapSrc, MapDes& mapDes)
    {
    for (typename MapSrc::const_iterator i = mapSrc.begin(), e = mapSrc.end(); i != e; ++i)
        {
        mapDes[i->first] = i->second;
        }
    }

// ----- implementation -----------------------------------------------------

COH_OPEN_NAMESPACE(impl)


// ----- class _array_proxy -------------------------------------------------

/**
* @internal
*/
template<class T>
class _arrow_proxy
    {
    private:
        T value;

    public:
        /**
        * @internal
        */
        _arrow_proxy(const T& x)
                : value(x)
            {
            }

        /**
        * @internal
        */
        const T* operator->() const
            {
            return &value;
            }

        // This function is needed for MWCW and BCC, which won't call
        // operator-> again automatically per 13.3.1.2 para 8
        /**
        * @internal
        */
        operator const T*() const
            {
            return &value;
            }
    };

/**
* @internal
*/
template<class V>
class _mapped_proxy : public V
    {
    private:
        const Map::Handle   m_hMap; // on stack
        Object::View        m_vKey; // on stack

    public: //not for user!
        /**
        * @internal
        */
        _mapped_proxy(Map::Handle hMap, Object::View vKey, V v)
                : V(v), m_hMap(hMap), m_vKey(vKey)
            {
            COH_ENSURE_PARAM(hMap);
            }

    public:
        /**
        * @internal
        */
        const _mapped_proxy& operator=(const V& v) const
            {
            m_hMap->putAll(Collections::singletonMap(m_vKey, v));
            return *this;
            }

        /**
        * @internal
        */
        const _mapped_proxy& operator=(const _mapped_proxy& that) const
            {
            m_hMap->put(Collections::singletonMap(m_vKey, that.m_vValue));
            return *this;
            }
    };

/**
* @internal
*/
template<class V>
bool operator==(const _mapped_proxy<V>& x, const _mapped_proxy<V>& y)
    {
    return V(x) == V(y);
    }

/**
* @internal
*/
template<class V>
bool operator==(const _mapped_proxy<V>& x, const V& y)
    {
    return V(x) == y;
    }

/**
* @internal
*/
template<class V>
bool operator==(const V& x, const _mapped_proxy<V>& y)
    {
    return x == V(y);
    }

/**
* @internal
*/
template<class V>
bool operator!=(const _mapped_proxy<V>& x, const _mapped_proxy<V>& y)
    {
    return !(x == y);
    }

/**
* @internal
*/
template<class V>
bool operator!=(const _mapped_proxy<V>& x, const V& y)
    {
    return !(x == y);
    }

/**
* @internal
*/
template<class V>
bool operator!=(const V& x, const _mapped_proxy<V>& y)
    {
    return !(x == y);
    }

/**
* @internal
*/
template <typename Char, typename Traits, class V>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const _mapped_proxy<V>& proxy)
    {
    return out << V(proxy);
    }

/**
* @internal
*/
template<class K, class V>
struct _value_proxy
        : public std::pair< K, _mapped_proxy<V> >
    {
        /**
        * @internal
        */
        typedef std::pair< K, _mapped_proxy<V> > super;

        /**
        * @internal
        */
        _value_proxy()
                : super()
            {
            }

        /**
        * @internal
        */
        _value_proxy(const K& key, const _mapped_proxy<V>& proxy)
                : super(key, proxy)
            {
            }
    };


// ----- iterators ----------------------------------------------------------

/**
* @internal
*/
template<typename difference_type>
class _iterator_base
    {
    protected:
        /**
        * The associated Map.
        */
        FinalView<Map> f_vMap;

        /**
        * The delegate iterator. A value of NULL indicates that the iterator
        * has been created lazily, and the first call to _next() will advance
        * the iterator to m_vKey.
        */
        MemberHandle<Iterator> m_hIter;

        /**
        * The current key associated with the iterator.
        */
        MemberView<Object> m_vKey;

        /**
        * @internal
        */
        _iterator_base(Map::View vMap, Object::View vKey)
                : f_vMap(System::common(), vMap),
                  m_hIter(System::common(), NULL),
                  m_vKey(System::common(), vKey)
            {
            }

        /**
        * @internal
        */
        _iterator_base(Map::View vMap, Iterator::Handle hIter)
                : f_vMap(System::common(), vMap),
                  m_hIter(System::common(), hIter),
                  m_vKey(System::common(), hIter->hasNext() ? hIter->next() : NULL)
            {
            }

        /**
        * @internal
        */
        _iterator_base(const _iterator_base& that)
                : f_vMap(System::common(), that.f_vMap),
                  m_hIter(System::common(), NULL),
                  m_vKey(System::common(), that.m_vKey)
            {
            Iterator::Handle endIterator = EndIterator::getInstance();
            if (endIterator == that.m_hIter)
                {
                m_hIter = endIterator;
                }
            }

        /**
        * @internal
        */
        _iterator_base& operator=(const _iterator_base& that)
            {
            if (this != &that)
                {
                f_vMap  = that.f_vMap;
                m_vKey  = that.m_vKey;

                Iterator::View vIterThat = that.m_hIter;
                m_hIter = vIterThat == NULL || vIterThat->hasNext()
                        ? NULL /*lazy*/
                        : EndIterator::getInstance();
                }
            return *this;
            }

        /**
        * @internal
        */
        void _next()
            {
            Iterator::Handle hIter = m_hIter;
            if (NULL == hIter)
                {
                // lazy create, and advance the iterator past the stored key
                // this is potentially expensive, but also quite rare
                hIter = f_vMap->keySet()->iterator();
                Object::View vKey = m_vKey;

                while (hIter->hasNext() && !Object::equals(vKey, hIter->next()));
                m_hIter = hIter;
                }
            else if (!hIter->hasNext())
                {
                m_hIter = EndIterator::getInstance();
                m_vKey  = NULL;
                return;
                }


            if (hIter->hasNext())
                {
                m_vKey = hIter->next();
                }
            else
                {
                m_vKey = NULL;
                }
            }

        /**
        * @internal
        */
        void _incr(difference_type i)
            {
            COH_ENSURE(i >= 0);
            for ( ; i; --i)
                {
                _next();
                }
            }

    public:
        /**
        * @internal
        */
        bool operator==(const _iterator_base& that) const
            {
            if (f_vMap == that.f_vMap)
                {
                if (m_hIter == EndIterator::getInstance())
                    {
                    return that.m_hIter == EndIterator::getInstance();
                    }
                return Object::equals(m_vKey, that.m_vKey);
                }
            return false;
            }

        /**
        * @internal
        */
        bool operator!=(const _iterator_base& i) const
            {
            return !(*this == i);
            }
    };

/**
* @internal
*/
template<class K, class V>
class _const_iterator
        : public _iterator_base<typename _adapter_map_base<K,V>::difference_type>
    {
        friend class _adapter_map_base<K,V>;
        friend class _iterator<K,V>;
        typedef _iterator_base<typename _adapter_map_base<K,V>::difference_type>
            super;

    public:
        /**
        * @internal
        */
        typedef std::forward_iterator_tag iterator_category;

        /**
        * @internal
        */
        typedef typename _adapter_map_base<K,V>::difference_type difference_type;

        /**
        * @internal
        */
        typedef typename _adapter_map_base<K,V>::value_type value_type;

        /**
        * @internal
        */
        typedef value_type reference;

        /**
        * @internal
        */
        typedef _arrow_proxy<const value_type> pointer;

        /**
        * @internal
        */
        _const_iterator(const _const_iterator& that)
            : super(that)
            {
            }

        /**
        * @internal
        */
        reference operator*() const
            {
            K key = cast<K>(super::m_vKey);
            return _make_pair(key, cast<V>(super::f_vMap->get(key)));
            }

        /**
        * @internal
        */
        pointer operator->() const
            {
            return pointer(operator*());
            }

        /**
        * @internal
        */
        _const_iterator& operator++()
            {
            super::_next();
            return *this;
            }

        /**
        * @internal
        */
        _const_iterator operator++(int)
            {
            _const_iterator r = *this;
            super::_next();
            return r;
            }

        /**
        * @internal
        */
        _const_iterator& operator+=(difference_type i)
            {
            _incr(i);
            return *this;
            }

        /**
        * @internal
        */
        _const_iterator operator+(difference_type i) const
            {
            _const_iterator r = *this;
            return r += i;
            }

        /**
        * @internal
        */
        reference operator[](difference_type i)
            {
            return *(*this + i);
            }

    private:
        /**
        * @internal
        */
        _const_iterator(Map::View vMap, Iterator::Handle hIter)
                : super(vMap, hIter)
            {
            }

        /**
        * @internal
        */
        _const_iterator(Map::View vMap, Object::View vKey)
                : super(vMap, vKey)
            {
            }

        /**
        * @internal
        */
        reference _make_pair(K key, V value) const
            {
            return value_type(key, value);
            }
    };

/**
* @internal
*/
template<class K, class V>
class _iterator
        : public _const_iterator<K,V>
    {
        friend class _adapter_map_base<K,V>;

    private:
        typedef _const_iterator<K,V> super;

    public:
        /**
        * @internal
        */
        _iterator(const _iterator& that)
            : super(that), f_hMap(System::common(), that.f_hMap, true)
            {
            }

        /**
        * @internal
        */
        typedef typename super::difference_type difference_type;

        /**
        * @internal
        */
        typedef const _mapped_proxy<V> mapped_reference;

        /**
        * @internal
        */
        typedef const _value_proxy<const K, V> reference;

        /**
        * @internal
        */
        typedef const _arrow_proxy<reference> pointer;

        /**
        * @internal
        */
        reference operator*() const
            {
            K key = cast<K>(super::m_vKey);
            return _make_ref(key, cast<V>(f_hMap->get(key)));
            }

        /**
        * @internal
        */
        pointer operator->() const
            {
            return pointer(operator*());
            }

        /**
        * @internal
        */
        _iterator& operator++()
            {
            super::_next();
            return *this;
            }

        /**
        * @internal
        */
        _iterator operator++(int)
            {
            _iterator r = *this;
            super::_next();
            return r;
            }

        /**
        * @internal
        */
        _iterator& operator+=(difference_type i)
            {
            _incr(i);
            return *this;
            }

        /**
        * @internal
        */
        _iterator operator+(difference_type i) const
            {
            _iterator r = *this;
            return r += i;
            }

        /**
        * @internal
        */
        reference operator[](difference_type i)
            {
            return *(*this + i);
            }

    private:
        /**
        * @internal
        */
        mutable FinalHandle<Map> f_hMap;

        /**
        * @internal
        */
        _iterator(Map::Handle hMap, Iterator::Handle hIter)
            : super(hMap, hIter), f_hMap(System::common(), hMap, true)
            {
            }

        /**
        * @internal
        */
        _iterator(Map::Handle hMap, Object::View vKey)
                : super(hMap, vKey), f_hMap(System::common(), hMap, true)
            {
            }

        /**
        * @internal
        */
        reference _make_ref(K key, V value) const
            {
            return reference(key, _mapped_proxy<V>(f_hMap, key, value));
            }
    };

/**
* @internal
*/
template<class K, class V>
inline _const_iterator<K,V> operator+
        (typename _const_iterator<K,V>::difference_type n,
        const _const_iterator<K,V>& x)
    {
    return x + n;
    }

/**
* @internal
*/
template<class K, class V>
bool operator==(const _value_proxy<K,V>& x, const std::pair<K,V>& y)
    {
    return (x.first == y.first) && (x.second == y.second);
    }

/**
* @internal
*/
template<class K, class V>
bool operator==(const std::pair<K,V>& x, const _value_proxy<K,V>& y)
    {
    return (x.first == y.first) && (x.second == y.second);
    }

/**
* @internal
*/
template<class K, class V>
inline _iterator<K,V>
operator+(typename _iterator<K,V>::difference_type n, const _iterator<K,V>& x)
    {
    return x + n;
    }

/**
* @internal
*/
template<class K, class V>
inline typename _adapter_map_base<K,V>::iterator
_adapter_map_base<K,V>::_make_iterator(Map::Handle hMap, Iterator::Handle hIter)
    {
    return iterator(hMap, hIter);
    }

/**
* @internal
*/
template<class K, class V>
inline typename _adapter_map_base<K,V>::iterator
_adapter_map_base<K,V>::_make_lazy_iterator(Map::Handle hMap, Object::View vKey)
    {
    return iterator(hMap, vKey);
    }

/**
* @internal
*/
template<class K, class V>
inline typename _adapter_map_base<K,V>::const_iterator
_adapter_map_base<K,V>::_make_const_iterator(Map::View vMap,
                                         Iterator::Handle hIter) const
    {
    return const_iterator(vMap, hIter);
    }

/**
* @internal
*/
template<class K, class V>
inline typename _adapter_map_base<K,V>::const_iterator
_adapter_map_base<K,V>::_make_lazy_const_iterator(Map::View vMap,
                                         Object::View vKey) const
    {
    return const_iterator(vMap, vKey);
    }

COH_CLOSE_NAMESPACE // impl


// ----- Observers -----------------------------------------------------------

/** @relates adapter_map
* Hasher function.
*
* A function object that takes a single argument of type key_type and
* returns a value of type size32_t.
*/
template<class K, class V>
class adapter_map<K,V>::hasher
        : public std::unary_function<key_type, size32_t>
    {
    public:
        /**
        * Determine a hash code corresponding to a given key object.
        *
        * @param k the handle to the key Object, may be @c NULL.
        *
        * @return a hash code of the key object, or 0 if @c NULL handle
        *         was provided
        */
        size32_t operator()(const key_type& k) const
            {
            return Object::hashCode(k);
            }
    };

/** @relates adapter_map
* Key equality relation.
*
* Binary predicate that takes two arguments of type key_type.
*/
template<class K, class V>
class adapter_map<K,V>::key_equal
        : public std::binary_function<key_type, key_type, bool>
    {
    public:
        /**
        * Compare two key objects for equality.
        *
        * @param x the handle to the first key Object
        * @param y the handle to the second key Object
        *
        * @return whether two keys are equal
        */
        bool operator()(const key_type& x, const key_type& y) const
            {
            return Object::equals(x, y);
            }
    };


// ----- adapter_map methods -----------------------------------------------------

//element access:
template<class K, class V>
inline typename adapter_map<K,V>::mapped_reference
adapter_map<K,V>::operator[](const key_type& key)
    {
    // operator[] is not part of the associative container concept, but is
    // defined within map implementations. As it is not part of the concept
    // we have some leverage with respect to the implementation. Most notably
    // we are not required to delegate to the insert method, which can be
    // expensive as it requires an iteration scan. Additionally we are not
    // required to insert of test, i.e. v = m[k], does not have to set a value
    // for k if it did not already exist

    Map::Handle hMap = delegate();
    return mapped_reference(hMap, key, cast<V>(hMap->get(key)));
    }

template<class K, class V>
inline typename adapter_map<K,V>::mapped_type
adapter_map<K,V>::operator[](const key_type& key) const
    {
    // see note from non-const version
    return cast<V>(delegate()->get(key));
    }

//modifiers:
template<class K, class V>
inline std::pair<typename adapter_map<K,V>::iterator, bool>
adapter_map<K,V>::insert(const value_type& p)
    {
    Map::Handle hMap = delegate();
    if (instanceof<InvocableMap::Handle>(hMap))
        {
        // perform conditional put
        if (NULL == (cast<InvocableMap::Handle>(hMap)->
                invoke(p.first, ConditionalPut::create(NotFilter::create(
                        PresentFilter::getInstance()), p.second, true))))
            {
            // insert occurred
            return std::pair<iterator, bool>(find(p.first), false);
            }
        }
    else if (!hMap->containsKey(p.first))
        {
        // key not found, insert element
        hMap->put(p.first, p.second);
        return std::pair<iterator, bool>(find(p.first), false);
        }

    // key already exists in the map
    return std::pair<iterator, bool>(find(p.first), true);
    }

template<class K, class V>
template <class InputIterator>
inline void adapter_map<K,V>::insert(InputIterator first, InputIterator last)
    {
    Map::Handle hMapDelegate = delegate();
    if (instanceof<InvocableMap::Handle>(hMapDelegate))
        {
        Map::Handle hMapTmp = HashMap::create();
        for ( ;first != last; ++first)
            {
            hMapTmp->put(K(first->first), V(first->second));
            }

        cast<InvocableMap::Handle>(hMapDelegate)->invokeAll(
                (Collection::View) hMapTmp->keySet(),
                ConditionalPutAll::create(NotFilter::create(
                        PresentFilter::getInstance()),
                        hMapTmp));
        }
    else
        {
        for ( ;first != last; ++first)
            {
            K key = K(first->first);
            if (!hMapDelegate->containsKey(key))
                {
                hMapDelegate->put(key, V(first->second));
                }
            }
        }
    }

template<class K, class V>
inline void adapter_map<K,V>::erase(iterator position)
    {
    erase(position->first);
    }

template<class K, class V>
inline void adapter_map<K,V>::erase(iterator first, iterator last)
    {
    if (first == begin() && last == end())
        {
        clear();
        }
    else
        {
        while (first != last)
            {
            erase(first++);
            }
        }
    }

//map operations:
template<class K, class V>
inline std::pair< typename adapter_map<K,V>::iterator,
             typename adapter_map<K,V>::iterator >
adapter_map<K,V>::equal_range(const key_type& x)
    {
    iterator         a  = find(x);
    iterator         b  = a == end() ? end() : a + 1;
    return make_pair(a, b);
    }

template<class K, class V>
inline std::pair< typename adapter_map<K,V>::const_iterator,
             typename adapter_map<K,V>::const_iterator >
adapter_map<K,V>::equal_range(const key_type& x) const
    {
    const_iterator   a  = find(x);
    const_iterator   b  = a == end() ? end() : a + 1;
    return make_pair(a, b);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_ADAPTER_MAP_HPP
