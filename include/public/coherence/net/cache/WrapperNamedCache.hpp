/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WRAPPER_NAMED_CACHE_HPP
#define COH_WRAPPER_NAMED_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::util::Collection;
using coherence::util::Comparator;
using coherence::util::Filter;
using coherence::util::MapListener;
using coherence::util::Set;
using coherence::util::ValueExtractor;

/**
 * A wrapper for the given NamedCache.
 *
 * @author lh  2012.06.18
 * @since Coherence 12.1.2
 */
class COH_EXPORT WrapperNamedCache
    : public class_spec<WrapperNamedCache,
        extends<Object>,
        implements<NamedCache> >
    {
    friend class factory<WrapperNamedCache>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Create a new WrapperNamedCache.
         *
         * @param hCache  the NamedCache that will be wrapped by this 
         *                WrapperNamedCache
         */
        WrapperNamedCache(NamedCache::Handle hCache);

    // ----- Map interface --------------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual size32_t size() const;

        /**
         * {@inheritDoc}
         */
        virtual bool isEmpty() const;

        /**
         * {@inheritDoc}
         */
        virtual bool containsKey(Object::View vKey) const;

        /**
         * {@inheritDoc}
         */
        virtual bool containsValue(Object::View vValue) const;

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder get(Object::View vKey) const;

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder get(Object::View vKey);

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue);

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

        /**
         * {@inheritDoc}
         */
        virtual void putAll(Map::View vMap);

        /**
         * {@inheritDoc}
         */
        virtual void clear();

        /**
         * {@inheritDoc}
         */
        virtual Set::View keySet() const;

        /**
         * {@inheritDoc}
         */
        virtual Set::Handle keySet();

        /**
         * {@inheritDoc}
         */
        virtual Collection::View values() const;

        /**
         * {@inheritDoc}
         */
        virtual Collection::Handle values();

        /**
         * {@inheritDoc}
         */
        virtual Set::View entrySet() const;

        /**
         * {@inheritDoc}
         */
        virtual Set::Handle entrySet();

    // ----- CacheMap interface ---------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual Map::View getAll(Collection::View vKeys) const;

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue,
                int64_t cMillis);

    // ----- NamedCache interface -------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual void release();

        /**
         * {@inheritDoc}
         */
        virtual void destroy();

        /**
         * {@inheritDoc}
         */
        virtual void truncate();

        /**
         * {@inheritDoc}
         */
        virtual String::View getCacheName() const;

        /**
         * {@inheritDoc}
         */
        virtual bool isActive() const;

        /**
         * {@inheritDoc}
         */
        virtual NamedCache::CacheServiceHandle getCacheService();

        /**
         * {@inheritDoc}
         */
        virtual NamedCache::CacheServiceView getCacheService() const;

    // ----- ConcurrentMap interface ----------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual bool lock(Object::View vKey, int64_t cWait) const;

        /**
         * {@inheritDoc}
         */
        virtual bool lock(Object::View vKey) const;

        /**
         * {@inheritDoc}
         */
        virtual bool unlock(Object::View vKey) const;

    // ----- InvocableMap interface -----------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual Object::Holder invoke(Object::View vKey,
                EntryProcessor::Handle hAgent);

        /**
         * {@inheritDoc}
         */
        virtual Map::View invokeAll(Collection::View vCollKeys,
                EntryProcessor::Handle hAgent);

        /**
         * {@inheritDoc}
         */
        virtual Map::View invokeAll(Filter::View vFilter,
                EntryProcessor::Handle hAgent);

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder aggregate(Collection::View vCollKeys,
                EntryAggregator::Handle hAgent) const;

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder aggregate(Filter::View vFilter,
                EntryAggregator::Handle hAgent) const;

    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual Set::View keySet(Filter::View vFilter) const;

        /**
         * {@inheritDoc}
         */
        virtual Set::View entrySet(Filter::View vFilter) const;

        /**
         * {@inheritDoc}
         */
        virtual Set::View entrySet(Filter::View vFilter,
                Comparator::View vComparator) const;

        /**
         * {@inheritDoc}
         */
        virtual void addIndex(ValueExtractor::View vExtractor, bool fOrdered,
                Comparator::View vComparator);

        /**
         * {@inheritDoc}
         */
        virtual void removeIndex(ValueExtractor::View vExtractor);

    // ----- ObservableMap interface ----------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual void addKeyListener(MapListener::Handle hListener,
                Object::View vKey, bool fLite);

        /**
         * {@inheritDoc}
         */
        virtual void removeKeyListener(MapListener::Handle hListener,
                Object::View vKey);

        /**
         * {@inheritDoc}
         */
        virtual void addMapListener(MapListener::Handle hListener);

        /**
        * {@inheritDoc}
        */
        virtual void removeMapListener(MapListener::Handle hListener);
        
        /**
         * {@inheritDoc}
         */
        virtual void addFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL, bool fLite = false);

        /**
         * {@inheritDoc}
         */
        virtual void removeFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL);

    // ----- Object interface -----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual TypedHandle<const String> toString() const;

    // ----- property getters/setters ---------------------------------------

    public:
        /**
         * Get the actual (wrapped) named cache.
         *
         * @return the wrapped cache
         */
        virtual NamedCache::Handle getNamedCache();

        /**
         * Get the actual (wrapped) named cache.
         *
         * @return the wrapped cache
         */
        virtual NamedCache::View getNamedCache() const;

    // ----- data members ---------------------------------------------------

    protected:
        /**
         * Actual (wrapped) NamedCache.
         */
        FinalHandle<NamedCache> f_hNamedCache;
};

COH_CLOSE_NAMESPACE3

#endif // COH_WRAPPER_NAMED_CACHE_HPP
