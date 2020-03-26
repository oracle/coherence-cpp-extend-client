/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/LocalCache.hpp"

#include "coherence/net/cache/IterableCacheLoader.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::ArrayList;
using coherence::util::Collections;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Iterator;
using coherence::util::ReadOnlyArrayList;

COH_OPEN_NAMESPACE_ANON(LocalCache)

/**
* The default key mask that ignores nothing.
*/
class COH_EXPORT DefaultMask
    : public class_spec<DefaultMask,
        extends<LocalCache::KeyMask> >
    {
    friend class factory<DefaultMask>;

    public:
        DefaultMask()
            : m_fSynthetic(false)
            {
            }

    public:
        virtual bool isSynthetic()
                {
                return m_fSynthetic;
                }
        virtual void setSynthetic(bool fSynthetic)
                {
                m_fSynthetic = fSynthetic;
                }
    private:
        bool m_fSynthetic;
    };

class CacheLoaderKeyMask
    : public class_spec<CacheLoaderKeyMask,
        extends<LocalCache::KeyMask> >
    {
    friend class factory<CacheLoaderKeyMask>;

    protected:
        CacheLoaderKeyMask(Object::View vKey)
            : f_vKey(self(), vKey)
            {
            }
    public:
        bool isIgnored(Object::View vCheckKey)
            {
            return vCheckKey->equals(f_vKey);
            }

    protected:
        FinalView<Object> f_vKey;
    };

class COH_EXPORT LoadAllMask
    : public class_spec<LoadAllMask,
        extends<LocalCache::KeyMask> >
    {
    friend class factory<LoadAllMask>;

    protected:
        LoadAllMask(Set::View vKeys)
            : f_vSetKeys(self(), vKeys)
            {
            }

    public:
        bool isIgnored(Object::View vCheckKey)
        {
        return f_vSetKeys->contains(vCheckKey);
        }

    protected:
        FinalView<Set> f_vSetKeys;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const size32_t LocalCache::max_units)
COH_EXPORT_SPEC_MEMBER(const size32_t LocalCache::default_units)
COH_EXPORT_SPEC_MEMBER(const int32_t LocalCache::never_expire)
COH_EXPORT_SPEC_MEMBER(const int32_t LocalCache::default_expire)


LocalCache::KeyMask::Handle LocalCache::getDefaultKeyMask()
    {
    static FinalHandle<KeyMask> hMask(System::common(),
            DefaultMask::create());
    return hMask;
    }
COH_STATIC_INIT(LocalCache::getDefaultKeyMask());


// ----- constructors -------------------------------------------------------

LocalCache::LocalCache(int32_t cUnits,
        int32_t cExpiryMillis,
        float64_t dflPruneLevel,
        size32_t cInitialBuckets,
        float32_t flLoadFactor,
        float32_t flGrowthRate)
    : super(cUnits, cExpiryMillis, dflPruneLevel, cInitialBuckets,
      flLoadFactor, flGrowthRate), m_hLoader(self()), m_hStore(self()),
      m_hListener(self()), f_tloIgnore(self(), ThreadLocalReference::create())
    {
    }


// ----- Map interface ------------------------------------------------------

Object::Holder LocalCache::remove(Object::View vKey)
    {
    COH_SYNCHRONIZED(this)
        {
        // this method is only called as a result of a call from the cache
        // consumer, not from any internal eviction etc.

        // check for the specified entry; getEntryInternal() will only return an
        // entry if the entry exists and has not expired
        OldCache::Entry::Handle hEntry = cast<OldCache::Entry::Handle>(getEntryInternal(vKey));
        if (NULL == hEntry)
            {
            return NULL;
            }
        else
            {
            // if there is a CacheStore, tell it that the entry is being
            // erased
            CacheStore::Handle hStore = getCacheStore();
            if (hStore != NULL)
                {
                hStore->erase(vKey);
                }

            return super::remove(vKey);
            }
        }
    }

/**
 * Removes all mappings from this map.
 */
void LocalCache::clear()
    {
    COH_SYNCHRONIZED(this);
    // this method is only called as a result of a call from the cache
    // consumer, not from any internal eviction etc.

    // if there is a CacheStore, tell it that all entries are being erased
    CacheStore::Handle hStore = getCacheStore();
    if (hStore != NULL)
        {
        hStore->eraseAll(super::keySet());
        }

    super::clear();
    }


// ----- LocalCache interface -----------------------------------------------

CacheLoader::Handle LocalCache::getCacheLoader()
    {
    return m_hLoader;
    }

CacheLoader::View LocalCache::getCacheLoader() const
    {
    return m_hLoader;
    }

void LocalCache::setCacheLoader(CacheLoader::Handle hLoader)
    {
    COH_SYNCHRONIZED(this)
        {
        if (hLoader != m_hLoader)
            {
            // unconfigure the old loader
            m_hLoader    = NULL;
            m_hStore     = NULL;

            MapListener::Handle hListener = m_hListener;
            if (hListener != NULL)
                {
                removeFilterListener(hListener);
                m_hListener = NULL;
                }

            // configure with the new loader
            m_hLoader = hLoader;
            if (instanceof<CacheStore::Handle>(hLoader))
                {
                m_hStore    = cast<CacheStore::Handle>(hLoader);
                m_hListener = hListener = instantiateInternalListener();
                addFilterListener(hListener);
                }
            }
        }
    }


Object::Holder LocalCache::get(Object::View vKey) const
    {
    return super::get(vKey);
    }

SafeHashMap::Entry::View LocalCache::getEntry(Object::View vKey) const
    {
    SafeHashMap::Entry::View vEntry = super::getEntry(vKey);

    if (NULL == vEntry)
        {
        int64_t ldtStart = System::safeTimeMillis();

        load(vKey);

        // use getEntryInternal() instead of get() to avoid screwing
        // up stats
        vEntry = getEntryInternal(vKey);
        f_hStats->registerMisses(0, ldtStart);
        }

    return vEntry;
    }

SafeHashMap::Entry::Handle LocalCache::getEntry(Object::View vKey)
    {
    SafeHashMap::Entry::Handle hEntry = super::getEntry(vKey);

    if (NULL == hEntry)
        {
        int64_t ldtStart = System::safeTimeMillis();

        load(vKey);

        // use getEntryInternal() instead of get() to avoid screwing
        // up stats
        hEntry = getEntryInternal(vKey);
        f_hStats->registerMisses(0, ldtStart);
        }

    return hEntry;
    }

Map::View LocalCache::getAll(Collection::View vColKeys) const
    {
    int64_t ldtStart = 0;

    // first, get all of the requested keys that are already loaded
    // into the map
    Map::Handle hMap   = peekAll(vColKeys);
    int32_t     cTotal = vColKeys->size();
    int32_t     cHits  = hMap->size();
    if (cHits < cTotal)
        {
        // load the remaining keys
        CacheLoader::View vLoader = getCacheLoader();
        if (vLoader != NULL)
            {
            ldtStart = System::safeTimeMillis();

            // build a list of the missing keys to load
            Set::Handle hSetRequest = HashSet::create(cTotal);
            hSetRequest->addAll(vColKeys);
            hSetRequest->removeAll(hMap->keySet());

            // load the missing keys
            loadAll(hSetRequest);

            // whichever ones are now loaded, add their values to the
            // result
            hMap->putAll(peekAll(hSetRequest));
            }
        }

    // update stats
    f_hStats->registerHits(cHits, ldtStart);
    f_hStats->registerMisses(cTotal - cHits, ldtStart);

    return hMap;
    }

void LocalCache::load(Object::View vKey) const
    {
    /*
    * We're going to cast away constness in load so that we can support cache
    * loading from const methods. In particular getEntry.
    */
    LocalCache::Handle hThis = const_cast<LocalCache*>(this);
    COH_ENSURE(hThis != NULL)

    hThis->load(vKey);
    }

void LocalCache::load(Object::View vKey)
    {
    CacheLoader::Handle hLoader = getCacheLoader();
    if (hLoader != NULL && getEntryInternal(vKey) == NULL)
        {
        Object::Holder ohValue = hLoader->load(vKey);
        if (ohValue != NULL)
            {
            CacheLoaderKeyMask::Handle hMask = CacheLoaderKeyMask::create(vKey);
            setKeyMask(hMask);

            struct loadFinally
                {
                loadFinally(LocalCache::Handle _hCache)
                    : hCache(_hCache)
                    {
                    }

                ~loadFinally()
                    {
                    hCache->setKeyMask(NULL);
                    }

                LocalCache::Handle hCache;
                } finally(this);


            super::put(vKey, ohValue);
            }
        }
    }

void LocalCache::loadAll()
    {
    CacheLoader::Handle hLoader = getCacheLoader();
    if (instanceof<IterableCacheLoader::Handle>(hLoader))
        {
        Iterator::Handle hIter = cast<IterableCacheLoader::Handle>(hLoader)->keys();

        size32_t cMaxUnits = getHighUnits();
        if (cMaxUnits > 0 && cMaxUnits < UnitCalculator::npos)
            {
            size32_t cTarget  = std::max(getLowUnits(), (size32_t) (0.9 * cMaxUnits));
            size32_t cCurrent = getUnits();
            while (hIter->hasNext() && cCurrent < cTarget)
                {
                load(hIter->next());

                size32_t cUnits = getUnits();
                if (cUnits < cCurrent)
                    {
                    // cache is already starting to prune itself for
                    // some reason; assume that eviction occurred
                    // which is an indication that we've warmed the
                    // cache suitably
                    break;
                    }

                cCurrent = cUnits;
                }
            }
        else
            {
            // TODO Port SimpleEnumerator to C++
            // loadAll(ReadOnlyArrayList::create(SimpleEnumerator::toArray(hIter)));
            ArrayList::Handle hList = ArrayList::create();
            while(hIter->hasNext())
                {
                hList->add(hIter->next());
                }
            loadAll(ReadOnlyArrayList::create(hList->toArray()));
            }
        }
    }

void LocalCache::loadAll(Collection::View vColKeys) const
    {
    /*
    * We're going to cast away constness in load so that we can support cache
    * loading from const methods. In particular getEntry.
    */
    LocalCache::Handle hThis = const_cast<LocalCache*>(this);
    COH_ENSURE(hThis != NULL)

    hThis->loadAll(vColKeys);
    }


void LocalCache::loadAll(Collection::View vColKeys)
    {
    CacheLoader::Handle hLoader = getCacheLoader();
    if (hLoader != NULL && !vColKeys->isEmpty())
        {
        Set::Handle hSetRequest = HashSet::create(vColKeys);
        hSetRequest->removeAll(peekAll(vColKeys)->keySet());
        if (!hSetRequest->isEmpty())
            {
            Map::View vMap = hLoader->loadAll(hSetRequest);
            if (!vMap->isEmpty())
                {
                Set::View vSetKeys = vMap->keySet();

                KeyMask::Handle hMask = LoadAllMask::create(vSetKeys);
                setKeyMask(hMask);

                struct loadAllFinally
                    {
                    loadAllFinally(LocalCache::Handle _hCache)
                        : hCache(_hCache)
                        {
                        }

                    ~loadAllFinally()
                        {
                        hCache->setKeyMask(NULL);
                        }

                    LocalCache::Handle hCache;
                    } finally(this);

                super::putAll(vMap);
                }
            }
        }
    }

Object::Holder LocalCache::peek(Object::View vKey) const
    {
    // avoid super.get() because it affects statistics
    OldCache::Entry::Handle hEntry = cast<OldCache::Entry::Handle>(getEntryInternal(vKey));
    return NULL == hEntry ? NULL : hEntry->getValue();
    }

Map::Handle LocalCache::peekAll(Collection::View vColKeys) const
    {
    Map::Handle hMap = HashMap::create(vColKeys->size());
    for (Iterator::Handle hIter = vColKeys->iterator(); hIter->hasNext(); )
        {
        Object::View            vKey   = hIter->next();
        OldCache::Entry::Handle hEntry = cast<OldCache::Entry::Handle>(getEntryInternal(vKey));
        if (hEntry != NULL)
            {
            hMap->put(vKey, hEntry->getValue());
            }
        }
    return hMap;
    }


// ----- internal -----------------------------------------------------------

CacheStore::View LocalCache::getCacheStore() const
    {
    return m_hStore;
    }

CacheStore::Handle LocalCache::getCacheStore()
    {
    return m_hStore;
    }

LocalCache::KeyMask::View LocalCache::getKeyMask() const
    {
    // NSA TODO - come back here for TLO
    KeyMask::View vMask = cast<KeyMask::View>(f_tloIgnore->get());
    return vMask == NULL ? (KeyMask::View)getDefaultKeyMask() : vMask;
    }

LocalCache::KeyMask::Handle LocalCache::getKeyMask()
    {
    KeyMask::Handle hMask = cast<KeyMask::Handle>(f_tloIgnore->get());
    return NULL == hMask ? getDefaultKeyMask() : hMask;
    }

void LocalCache::setKeyMask(KeyMask::Handle hMask)
    {
    f_tloIgnore->set(hMask);
    }

void LocalCache::removeExpired(OldCache::Entry::Handle hEntry,
        bool fRemoveInternal)
    {
    COH_SYNCHRONIZED(this)
        {
        KeyMask::Handle hMask = getKeyMask();
        bool            fPrev = hMask->ensureSynthetic();

        struct removeExpiredFinally
            {
            removeExpiredFinally(LocalCache::KeyMask::Handle _hCache, bool _fPrev)
                : hMask(_hCache), fPrev(_fPrev)
                {
                }

            ~removeExpiredFinally()
                {
                hMask->setSynthetic(fPrev);
                }

            LocalCache::KeyMask::Handle hMask;
            bool                        fPrev;
            } finally(hMask, fPrev);

        super::removeExpired(hEntry, fRemoveInternal);
        }
    }

MapEvent::Handle LocalCache::instantiateMapEvent(int32_t nId,
        Object::View vKey, Object::Holder ohValueOld,
        Object::Holder ohValueNew)
    {
    return CacheEvent::create(this, nId, vKey, ohValueOld, ohValueNew,
            getKeyMask()->isSynthetic());
    }


// ----- inner class: InternalListener --------------------------------------

/**
* Factory pattern: Instantiate an internal MapListener to listen to this
* cache and report changes to the CacheStore.
*
* @return  a new MapListener instance
*/
MapListener::Handle LocalCache::instantiateInternalListener()
    {
    return InternalListener::create(this);
    }

LocalCache::InternalListener::InternalListener(LocalCache::Handle hCache)
    : m_hCache(self(), hCache)
    {
    }

void LocalCache::InternalListener::entryInserted(MapEvent::View vEvt)
    {
    onModify(vEvt);
    }

/**
* Invoked when a map entry has been updated.
*
* @param evt  the MapEvent carrying the update information
*/
void LocalCache::InternalListener::entryUpdated(MapEvent::View vEvt)
    {
    onModify(vEvt);
    }

void LocalCache::InternalListener::entryDeleted(MapEvent::View)
    {
    // deletions are handled by the clear() and remove(Object)
    // methods, and are ignored by the listener, because they
    // include evictions, which may be impossible to differentiate
    // from client-invoked removes and clears
    }

/**
* A value modification event (insert or update) has occurred.
*
* @param evt  the MapEvent object
*/
void LocalCache::InternalListener::onModify(MapEvent::View vEvt)
    {
    if (!m_hCache->getKeyMask()->isIgnored(vEvt->getKey()))
        {
        CacheStore::Handle hStore = m_hCache->getCacheStore();
        if (hStore != NULL)
            {
            hStore->store(vEvt->getKey(), vEvt->getNewValue());
            }
        }
    }


// ----- inner class: KeyMask -----------------------------------------------

LocalCache::KeyMask::KeyMask()
    {
    }

// ----- KeyMask interface ----------------------------------------------

bool LocalCache::KeyMask::isIgnored(Object::View /*vKey*/)
    {
    return false;
    }

bool LocalCache::KeyMask::isSynthetic()
    {
    return true;
    }

void LocalCache::KeyMask::setSynthetic(bool /*fSynthetic*/)
    {
    }

bool LocalCache::KeyMask::ensureSynthetic()
    {
    bool f = isSynthetic();
    if (!f)
        {
        setSynthetic(true);
        }
    return f;
    }


// ----- inner class: Entry -------------------------------------------------

SafeHashMap::Entry::Handle LocalCache::instantiateEntry(
        Object::View vKey, Object::Holder ohValue, size32_t nHash)
    {
    return Entry::create(vKey, ohValue, nHash, this);
    }

SafeHashMap::Entry::Handle LocalCache::instantiateEntry(
        SafeHashMap::Entry::View vEntry)
    {
    return Entry::create(cast<LocalCache::Entry::View>(vEntry));
    }

LocalCache::Entry::Entry(Object::View vKey, Object::Holder ohValue,
        size32_t nHash, LocalCache::Handle hCache)
    : super(vKey, ohValue, nHash, hCache)
    {
    }

LocalCache::Entry::Entry(const Entry& that)
    : super(that)
    {
    }

LocalCache::Entry::Entry(Entry::View vThat)
    : super(vThat)
    {
    }


COH_CLOSE_NAMESPACE3
