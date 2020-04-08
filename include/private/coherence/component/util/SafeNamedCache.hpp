/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_NAMED_CACHE_HPP
#define COH_SAFE_NAMED_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/ValueExtractor.hpp"

#include "private/coherence/component/util/SafeCacheService.hpp"
#include "coherence/util/MapListenerSupport.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::security::auth::Subject;
using coherence::util::Collection;
using coherence::util::Comparator;
using coherence::util::Filter;
using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::MapListenerSupport;
using coherence::util::Set;
using coherence::util::ValueExtractor;


/**
* "Safe" wrapper for RemoteNamedCache.
*
* @author tb  2008.15.02
*/
class COH_EXPORT SafeNamedCache
    : public class_spec<SafeNamedCache,
        extends<Object>,
        implements<NamedCache, MapListener> >
    {
    friend class factory<SafeNamedCache>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new SafeNamedCache.
        */
        SafeNamedCache(MapListenerSupport::Handle hSupport = NULL);


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
        *
        * @deprecated as of Coherence 12.1
        */
        virtual bool lock(Object::View vKey, int64_t cWait) const;

        /**
        * {@inheritDoc}
        *
        * @deprecated as of Coherence 12.1
        */
        virtual bool lock(Object::View vKey) const;

        /**
        * {@inheritDoc}
        *
        * @deprecated as of Coherence 12.1
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


    // ----- MapListener interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void entryInserted(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryUpdated(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryDeleted(MapEvent::View vEvent);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Returns the running wrapped named cache.
        *
        * @returns the wrapped named cache.
        */
        virtual NamedCache::Handle getRunningNamedCache() const;

        /**
        * Restarts underlying RemoteNamedCache.
        *
        * @return the active instance of SafeNamedCache
        */
        virtual NamedCache::Handle restartNamedCache() const;

        /**
        * Restarts underlying RemoteNamedCache.
        *
        * @return the active instance of SafeNamedCache
        */
        virtual NamedCache::Handle restartNamedCache();

        /**
        * Convert the specified map event into another MapEvent,  set the new
        * events source to this and fire the new event.
        *
        * @param vEvt  the event that is to be converted
        */
        virtual void translateMapEvent(MapEvent::View vEvt);

        /**
        * Release active listeners.
        */
        virtual void releaseListeners();


    // ----- property getters/setters ---------------------------------------

    public:
        /**
        * Get the MapListenerSupport instance used to add/remove map events.
        *
        * @return the MapListenerSupport instance
        */
        virtual MapListenerSupport::Handle getListenerSupport();

        /**
        * Get the MapListenerSupport instance used to add/remove map events.
        *
        * @return the MapListenerSupport instance
        */
        virtual MapListenerSupport::View getListenerSupport() const;

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

        /**
        * Set the actual (wrapped) named cache.
        *
        * @param hNamesCache the wrapped named cache
        */
        virtual void setNamedCache(NamedCache::Handle hNamedCache);

        /**
        * Set the cache name.
        *
        * @param vsCacheName  the cache name
        */
        virtual void setCacheName(String::View vsCacheName);

        /**
        * Sets the remote cache service that this named cache is a part of.
        */
        virtual void setSafeCacheService(SafeCacheService::Handle hCacheService);

        /**
        * Gets the remote cache service that this named cache is a part of.
        *
        * @return the cache service
        */
        virtual SafeCacheService::View getSafeCacheService() const;

        /**
        * Specifies whether or not the underlying named cache has been
        * explicitly released.
        *
        * @return true iff the underlying cache has been released
        */
        virtual bool isReleased() const;

        /**
        * Set whether or not the underlying named cache has been released.
        *
        * @param fReleased  true if the underlying cache has been released
        */
        virtual void setReleased(bool fReleased);

        /**
        * Get the optional identity associated with this cache.
        */
        virtual Subject::View getSubject() const;

        /**
        * Set the optional identity associated with this cache.
        *
        * @param vSubject  the identity
        */
        virtual void setSubject(Subject::View vSubject);

    protected:
        /**
        * Set the MapListenerSupport instance which is used to add/remove map
        * listeners.
        *
        * @param hListenerSupport  the MapListenerSupport instance
        */
        virtual void setListenerSupport(
                MapListenerSupport::Handle hListenerSupport);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Actual (wrapped) NamedCache.
        */
        mutable MemberHandle<NamedCache> m_hNamedCache;

        /**
        * The cache name.
        */
        FinalView<String> f_vsCacheName;

        /**
        * SafeCacheService this NamedCache is part of.
        */
        mutable MemberHandle<SafeCacheService> m_hCacheService;

        /**
        * Specifies whether or not the underlying NamedCache has been
        * explicitly released.
        */
        Volatile<bool> m_fIsReleased;

        /**
        * Used to add/remove map listeners.
        */
        FinalHandle<MapListenerSupport> f_hListenerSupport;

        /**
        * The identity associated with this cache.
        */
        FinalView<Subject> f_vSubject;
};

COH_CLOSE_NAMESPACE3

#endif // COH_SAFE_NAMED_CACHE_HPP
