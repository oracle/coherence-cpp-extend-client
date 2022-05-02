/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REMOTE_NAMED_CACHE_HPP
#define COH_REMOTE_NAMED_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/cache/CacheStore.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"

#include "private/coherence/component/net/extend/AbstractPartialResponse.hpp"
#include "private/coherence/component/util/AbstractSerializationConverter.hpp"
#include "private/coherence/component/util/QueueProcessor.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "coherence/util/MapListenerSupport.hpp"
#include "private/coherence/util/PagedIterator.hpp"




COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::component::net::extend::AbstractPartialResponse;
using coherence::component::util::AbstractSerializationConverter;
using coherence::component::util::QueueProcessor;
using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::net::cache::CacheStore;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Message;
using coherence::net::messaging::Protocol;
using coherence::util::Binary;
using coherence::util::Collection;
using coherence::util::Converter;
using coherence::util::Comparator;
using coherence::util::Describable;
using coherence::util::Filter;
using coherence::util::Iterator;
using coherence::util::LongArray;
using coherence::util::Listeners;
using coherence::util::Map;
using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::MapListenerSupport;
using coherence::util::MapTrigger;
using coherence::util::Muterator;
using coherence::util::PagedIterator;
using coherence::util::Set;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;


/**
* NamedCache implementation that delegates to a remote NamedCache using a
* Channel.
*
* @author jh  2008.03.07
*/
class COH_EXPORT RemoteNamedCache
    : public class_spec<RemoteNamedCache,
        extends<Describable>,
        implements<NamedCache, Channel::Receiver> >
    {
    friend class factory<RemoteNamedCache>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new RemoteNamedCache.
        */
        RemoteNamedCache();

    private:
        /**
        * Blocked copy constructor.
        */
        RemoteNamedCache(const RemoteNamedCache&);


    // ----- inner class: ConverterBinaryToDecoratedBinary ------------------

    public:
        /**
        * Converter implementation that deserializes a Binary object using
        * the RemoteNamedCache Channel's Serializer and decorates the Binary
        * using the associated key.
        */
        class COH_EXPORT ConverterBinaryToDecoratedBinary
            : public class_spec<ConverterBinaryToDecoratedBinary,
                extends<AbstractSerializationConverter> >
            {
            friend class factory<ConverterBinaryToDecoratedBinary>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new ConverterBinaryToDecoratedBinary.
                *
                * @param vCache  the RemoteNamedCache for which the
                *                ConverterBinaryToDecoratedBinary is being
                *                created
                */
                ConverterBinaryToDecoratedBinary(
                        RemoteNamedCache::View vCache);


            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder convert(Object::Holder oh) const;


            // ----- data members ---------------------------------------

            protected:
                /**
                * The RemoteNamedCache.
                */
                WeakView<RemoteNamedCache> m_wvCache;
            };


    // ----- inner class: ConverterBinaryToUndecoratedBinary ----------------

    public:
        /**
        * Converter implementation that removes an int decoration from a
        * Binary if present.
        */
        class COH_EXPORT ConverterBinaryToUndecoratedBinary
            : public class_spec<ConverterBinaryToUndecoratedBinary,
                extends<AbstractSerializationConverter> >
            {
            friend class factory<ConverterBinaryToUndecoratedBinary>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new ConverterBinaryToUndecoratedBinary.
                *
                * @param vCache  the RemoteNamedCache for which the
                *                ConverterBinaryToUndecoratedBinary is being
                *                created
                */
                ConverterBinaryToUndecoratedBinary(
                        RemoteNamedCache::View vCache);


            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder convert(Object::Holder oh) const;


            // ----- data members ---------------------------------------

            protected:
                /**
                * The RemoteNamedCache.
                */
                WeakView<RemoteNamedCache> m_wvCache;
            };


    // ----- inner class: ConverterFromBinary -------------------------------

    public:
        /**
        * Converter implementation that converts Objects from a Binary
        * representation via the RemoteNamedCache Channel's Serializer.
        */
        class COH_EXPORT ConverterFromBinary
            : public class_spec<ConverterFromBinary,
                extends<AbstractSerializationConverter> >
            {
            friend class factory<ConverterFromBinary>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new ConverterFromBinary.
                */
                ConverterFromBinary();

            private:
                /**
                * Blocked copy constructor.
                */
                ConverterFromBinary(const ConverterFromBinary&);


            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder convert(Object::Holder oh) const;
            };


    // ----- inner class: ConverterKeyToBinary ------------------------------

    public:
        /**
        * Converter implementation that converts keys into their Binary
        * representation via the RemoteNamedCache Channel's Serializer.
        */
        class COH_EXPORT ConverterKeyToBinary
            : public class_spec<ConverterKeyToBinary,
                extends<AbstractSerializationConverter> >
            {
            friend class factory<ConverterKeyToBinary>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new ConverterKeyToBinary.
                *
                * @param vCache  the RemoteNamedCache for which the
                *                ConverterKeyToBinary is being created
                */
                ConverterKeyToBinary(RemoteNamedCache::View vCache);


            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder convert(Object::Holder oh) const;


            // ----- data members ---------------------------------------

            protected:
                /**
                * The RemoteNamedCache.
                */
                WeakView<RemoteNamedCache> m_wvCache;
            };


    // ----- inner class: ConverterValueToBinary ----------------------------

    public:
        /**
        * Converter implementation that converts values into their Binary
        * representation via the RemoteNamedCache Channel's Serializer.
        */
        class COH_EXPORT ConverterValueToBinary
            : public class_spec<ConverterValueToBinary,
                extends<AbstractSerializationConverter> >
            {
            friend class factory<ConverterValueToBinary>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new ConverterValueToBinary.
                */
                ConverterValueToBinary();

            private:
                /**
                * Blocked copy constructor.
                */
                ConverterValueToBinary(const ConverterValueToBinary&);


            // ----- Converter interface --------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder convert(Object::Holder oh) const;
            };


    // ----- inner class: BinaryCache ---------------------------------------

    public:
        /**
        * The internal view of the RemoteNamedCache.
        */
        class COH_EXPORT BinaryCache
            : public class_spec<BinaryCache,
                extends<Object>,
                implements<NamedCache, CacheStore> >
            {
            friend class factory<BinaryCache>;

            // ----- handle definitions (needed for nested classes) -----

            public:
                typedef this_spec::Handle Handle;
                typedef this_spec::View   View;
                typedef this_spec::Holder Holder;


            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new BinaryCache.
                *
                * @param hCache    the RemoteNamedCache for which the
                *                  BinaryCache is being created
                * @param hSupport  the MapListenerSupport, or NULL to create
                */
                BinaryCache(RemoteNamedCache::Handle hCache, MapListenerSupport::Handle hSupport = NULL);

            // ----- BinaryCache interface ------------------------------

            public:
                /**
                * Determine if the remote NamedCache contains the specified
                * keys.
                *
                * @param vColKeys  the keys
                *
                * @return true if the NamedCache contains the specified keys
                */
                virtual bool containsAll(Collection::View vColKeys) const;

                /**
                * Associate the specified value with the specified key and
                * expiry delay in the remote NamedCache.
                *
                * @param vKey     the entry key
                * @param ohValue  the entry value
                * @param cMillis  the entry expiry delay
                * @param fReturn  if true, the old value will be returned
                *
                * @return the old value associated with the given key; only
                *         applicable if fReturn is true
                */
                virtual Object::Holder put(Object::View vKey,
                        Object::Holder ohValue, int64_t cMillis,
                        bool fReturn);

                /**
                * Remove the entry with the given key from the remote
                * NamedCache.
                *
                * @param vKey     the key to remove
                * @param fReturn  if true, the removed value will be
                *                 returned
                *
                * @return the removed value; only applicable if fReturn is
                *         true
                */
                virtual Object::Holder remove(Object::View vKey,
                        bool fReturn);

                /**
                * Remove the entries with the specified keys from the remote
                * NamedCache.
                *
                * @param vColKeys  the keys to remove
                *
                * @return true if the NamedCache was modified as a result of
                *         this call
                */
                virtual bool removeAll(Collection::View vColKeys);

                /**
                * Dispatch a MapEvent created using the supplied information
                * to the MapListeners registered with this BinaryCache.
                *
                * @param nEventId         the identifier of the MapEvent
                * @param valFilterIds     the positive unique identifier(s) of the
                *                         Filters that caused this MapEvent to be
                *                         dispatched
                * @param vKey             the key associated with the MapEvent
                * @param vValueOld        the old value associated with the
                *                         MapEvent
                * @param vValueNew        the new value associated with the
                *                         MapEvent
                * @param fSynthetic       true if the MapEvent occured because of
                *                         internal cache processing
                * @param nTransformState  the transformation state of
                *                         this event
                * @param fPriming         true if the MapEvent is a priming event
                * @param fExpired         true if the MapEvent results from a
                *                         time-based eviction event
                */
                virtual void dispatch(int32_t nEventId, Array<int64_t>::View valFilterIds,
                        Object::View vKey, Object::View vValueOld,
                        Object::View vValueNew, bool fSynthetic, int32_t nTransformState,
                        bool fPriming, bool fExpired);

            // ----- CacheMap interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Map::View getAll(Collection::View vColKeys) const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder put(Object::View vKey,
                        Object::Holder ohValue, int64_t cMillis);

            // ----- CacheStore interface -------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void store(Object::View vKey, Object::Holder ohValue);

                /**
                * {@inheritDoc}
                */
                virtual void storeAll(Map::View vMapEntries);

                /**
                * {@inheritDoc}
                */
                virtual void erase(Object::View vKey);

                /**
                * {@inheritDoc}
                */
                virtual void eraseAll(Collection::View vColKeys);

            // ----- ConcurrentMap interface ----------------------------

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

            // ----- InvocableMap interface -----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder invoke(Object::View vKey,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Map::View invokeAll(Collection::View vCollKeys,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Map::View invokeAll(Filter::View vFilter,
                        InvocableMap::EntryProcessor::Handle hAgent);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder aggregate(Collection::View vCollKeys,
                        InvocableMap::EntryAggregator::Handle hAgent) const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder aggregate(Filter::View vFilter,
                        InvocableMap::EntryAggregator::Handle hAgent) const;

            // ----- QueryMap interface ---------------------------------

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
                virtual void addIndex(ValueExtractor::View vExtractor,
                        bool fOrdered, Comparator::View vComparator);

                /**
                * {@inheritDoc}
                */
                virtual void removeIndex(ValueExtractor::View vExtractor);

             // ----- CacheLoader interface -----------------------------

             public:
                 /**
                 * {@inheritDoc}
                 */
                 virtual Object::Holder load(Object::View vKey);

                 /**
                 * {@inheritDoc}
                 */
                 virtual Map::View loadAll(Collection::View vColKeys);

             // ----- NamedCache interface ------------------------------

             public:
                 /**
                 * {@inheritDoc}
                 */
                 virtual String::View getCacheName() const;

                 /**
                 * {@inheritDoc}
                 */
                 virtual CacheService::Handle getCacheService();

                 /**
                 * {@inheritDoc}
                 */
                 virtual CacheService::View getCacheService() const;

                 /**
                 * {@inheritDoc}
                 */
                 virtual bool isActive() const;

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


            // ----- ObservableMap interface ----------------------------

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

            // ----- Map interface --------------------------------------

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
                virtual Object::Holder put(Object::View vKey,
                        Object::Holder ohValue);

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

            // ----- Object interface -----------------------------------

            protected:
                /**
                * {@inheritDoc}
                */
                virtual void onInit();

            // ----- internal methods -----------------------------------

            protected:
                /**
                * Return the Channel used by this RemoteNamedCache. If
                * the Channel is NULL or is not open, this method throws
                * an exception.
                *
                * @return a Channel that can be used to exchange Messages
                *         with the remote ProxyService
                */
                virtual Channel::Handle ensureChannel() const;

                /**
                * Set the MapListenerSupport instance which is used to
                * add/remove map listeners.
                *
                * @param hListenerSupport  the MapListenerSupport instance
                */
                virtual void setMapListenerSupport(
                        MapListenerSupport::Handle hListenerSupport);

                /**
                * Send a request to the remote NamedCacheProxy to register a
                * MapListener.
                *
                * @param vFilter    filter used to register the remote
                *                   MapListener
                * @param lFilterId  the unqiue positive identifier for the
                *                   specified Filter
                * @param fLite      if the remote MapListener should be
                *                   "lite"
                * @param hTrigger   the optional MapTrigger to associate with
                *                   the request
                * @param fPriming   if the call is for a NearCache "priming" listener
                */
                virtual void addRemoteMapListener(Filter::View vFilter,
                        int64_t lFilterId, bool fLite,
                        MapTrigger::Handle hTrigger, bool fPriming);

                /**
                * Send a request to the remote NamedCacheProxy to register a
                * MapListener.
                *
                * @param vKey      the key used to register the remote
                *                  MapListener
                * @param fLite     if the remote MapListener should be "lite"
                * @param hTrigger  the optional MapTrigger to associate with
                *                  the request
                * @param fPriming  if the call is for a NearCache "priming" listener
                */
                virtual void addRemoteMapListener(Object::View vKey,
                        bool fLite, MapTrigger::Handle hTrigger, bool fPriming);

                /**
                * Send a request to the remote NamedCacheProxy to unregister
                * a MapListener.
                *
                * @param vFilter    the Filter used to unregister the remote
                *                   MapListener
                * @param lFilterId  the unqiue positive identifier for the
                *                   specified Filter
                * @param fSync      if the local MapListener is a
                *                   SynchronousListener
                * @param hTrigger   the optional MapTrigger to associate with
                *                   the request
                * @param fPriming   if the call is for a NearCache "priming" listener
                */
                virtual void removeRemoteMapListener(Filter::View vFilter,
                        int64_t lFilterId, bool fSync,
                        MapTrigger::Handle hTrigger, bool fPriming);

                /**
                * Send a request to the remote NamedCacheProxy to unregister
                * a MapListener.
                *
                * @param vKey      the key used to unregister the remote
                *                  MapListener
                * @param fSync     if the local MapListener is a
                *                  SynchronousListener
                * @param hTrigger  the optional MapTrigger to associate with
                *                  the request
                * @param fPriming  if the call is for a NearCache "priming" listener
                */
                virtual void removeRemoteMapListener(Object::View vKey,
                        bool fSync, MapTrigger::Handle hTrigger, bool fPriming);

                /**
                * Return the unique positive identifier that the specified
                * Filter was registered with or 0 if the specified Filter has
                * not been registered.
                *
                * Note: all calls to this method should be synchronized using
                * the MapListenerSupport object.
                *
                * @param vFilter  the Filter
                *
                * @return the unique identifier that the specified Filter was
                *         registered with
                */
                virtual int64_t getFilterId(Filter::View vFilter);

                /**
                * Create a unqiue positive identifier for the specified
                * Filter.
                *
                * Note: all calls to this method should be synchronized using
                * the MapListenerSupport object.
                *
                * @param vFilter  the Filter
                *
                * @return the unique identifier that the specified Filter was
                *         registered with
                */
                virtual int64_t registerFilter(Filter::View vFilter);

                /**
                * Return the next page of keys.
                *
                * @param vBinCookie  the optional opaque cookie returned from
                *                    the last call to this method
                *
                * @return a PartialResponse containing the next set of keys
                */
                virtual AbstractPartialResponse::Handle keySetPage(
                        Binary::View vBinCookie) const;

                /**
                * Return the result associated with the given Response.
                *
                * @param hResponse  the Response to process
                *
                * @return the result associated with the given Response
                *
                * @throws RuntimeException if the Response was a failure
                */
                virtual Object::Holder processResponse(
                        AbstractPofResponse::Handle hResponse) const;

                /**
                * Perform a remote query.
                *
                * @param vFilter    the Filter used in the query
                * @param fKeysOnly  if true, only the keys from the result
                * will be returned; otherwise, the entries will be returned
                *
                * @return the result of the query
                */
                virtual Set::View query(Filter::View vFilter, bool fKeysOnly) const;

            // ----- accessors ------------------------------------------

            public:				
                /**
                * Return the RemoteNamedCache that created this BinaryCache.
                *
                * @return the RemoteNamedCache
                */
                virtual RemoteNamedCache::View getRemoteNamedCache() const;

                /**
                * Return the ConverterBinaryToDecoratedBinary.
                *
                * @return the ConverterBinaryToDecoratedBinary
                */
                virtual RemoteNamedCache::ConverterBinaryToDecoratedBinary::View
                        getConverterBinaryToDecoratedBinary() const;

                /**
                * Return the ConverterBinaryToDecoratedBinary.
                *
                * @return the ConverterBinaryToDecoratedBinary
                */
                virtual RemoteNamedCache::ConverterBinaryToDecoratedBinary::Handle
                        getConverterBinaryToDecoratedBinary();

                /**
                * Set the ConverterBinaryToDecoratedBinary
                *
                * @param hConverter  the ConverterBinaryToDecoratedBinary to set
                */
                virtual void setConverterBinaryToDecoratedBinary(
                        RemoteNamedCache::ConverterBinaryToDecoratedBinary::Handle hConverter);

                /**
                * Return the ConverterBinaryToUndecoratedBinary.
                *
                * @return the ConverterBinaryToUndecoratedBinary
                */
                virtual RemoteNamedCache::ConverterBinaryToUndecoratedBinary::View
                        getConverterBinaryToUndecoratedBinary() const;

                /**
                * Set the ConverterBinaryToUndecoratedBinary
                *
                * @param vConv  the ConverterBinaryToUndecoratedBinary to set
                */
                virtual void setConverterBinaryToUndecoratedBinary(
                        RemoteNamedCache::ConverterBinaryToUndecoratedBinary::View vConv);

                /**
                * Set the Channel for this BinaryCache
                *
                * @param hChannel the Channel for this BinaryCache
                */
                virtual void setChannel(Channel::Handle hChannel);

                /**
                * Return the Channel used to communicate with the remote
                * ProxyService.
                *
                * @return the Channel
                */
                virtual Channel::Handle getChannel() const;

                /**
                * Get the MapListenerSupport instance used to add/remove map
                * events.
                *
                * @return the MapListenerSupport instance
                */
                virtual MapListenerSupport::Handle getMapListenerSupport();

                /**
                * Get the MapListenerSupport instance used to add/remove map
                * events.
                *
                * @return the MapListenerSupport instance
                */
                virtual MapListenerSupport::View getMapListenerSupport() const;

                /**
                * Get the LongArray of Filter objects indexed by the unique
                * filter id. These filter id values are used by the MapEvent
                * message to specify what filters caused a cache event.
                *
                * @return the array of filters
                */
                virtual LongArray::Handle getFilterArray();

                /**
                * Configure the QueueProcessor used to dispatch MapEvents.
                *
                * @param hDispacher the event dispatcher QueueProcessor
                */
                virtual void setEventDispatcher(
                        QueueProcessor::Handle hDispacher);

                /**
                * Return the QueueProcessor used to dispatch MapEvents.
                *
                * @return the QueueProcessor
                */
                virtual QueueProcessor::Handle getEventDispatcher();

                /**
                * Return the QueueProcessor used to dispatch MapEvents.
                *
                * @return the QueueProcessor
                */
                virtual QueueProcessor::View getEventDispatcher() const;

            // ----- inner class: EntrySet ------------------------------

            public:
                /**
                * Entry Set implementation for the RemoteNamedCache.
                */
                class COH_EXPORT EntrySet
                    : public class_spec<EntrySet,
                        extends<Object>,
                        implements<Set> >
                    {
                    friend class factory<EntrySet>;

                    // ----- constructors ---------------------------

                    protected:
                        /**
                        * Create a new EntrySet object for the given
                        * RemoteNamedCache.
                        *
                        * @param hCache  the RemoteNamedCache for which the
                        *                EntrySet is being created
                        *
                        */
                        EntrySet(BinaryCache::Handle hCache);

                    // ----- Set interface --------------------------

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
                        virtual bool contains(Object::View v) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool containsAll(
                                Collection::View vCol) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual Iterator::Handle iterator() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual Muterator::Handle iterator();

                        /**
                        * {@inheritDoc}
                        */
                        virtual ObjectArray::Handle toArray(
                                ObjectArray::Handle hao = NULL) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool add(Object::Holder oh);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool addAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool remove(Object::View v);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool removeAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool retainAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual void clear();

                    // ----- Object interface -----------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual bool equals(Object::View v) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual size32_t hashCode() const;

                    // ----- inner class: EntryIterator -------------

                    public:
                        /**
                        * Iterator implementation for the EntrySet class.
                        */
                        class COH_EXPORT EntryIterator
                            : public class_spec<EntryIterator,
                                extends<Object>,
                                implements<Muterator> >
                            {
                            friend class factory<EntryIterator>;

                            // ----- constructors ---------------

                            protected:
                                /**
                                * Create a new EntryIterator object for the
                                * given RemoteNamedCache.
                                *
                                * @param ohCache  the RemoteNamedCache over
                                *                 which the EntryInterator
                                *                 will iterate
                                *
                                */
                                EntryIterator(BinaryCache::Holder ohCache);

                            // ----- Iterator interface ---------

                            public:
                                /**
                                * @internal
                                */
                                virtual bool hasNext() const;

                                /**
                                * @internal
                                */
                                virtual Object::Holder next();

                                /**
                                * @internal
                                */
                                virtual void remove();

                            // ----- data members ---------------

                            protected:
                                /**
                                * The last Map::Entry that was iterated.
                                */
                                MemberView<Map::Entry> m_vEntry;

                                /**
                                * An iterator over the keys returned by
                                * RemoteNamedCache::keySet().
                                */
                                FinalHandle<Iterator> f_hIterKeys;

                                /**
                                * The BinaryCache.
                                */
                                FinalHolder<BinaryCache> f_ohCache;
                            };

                    // ----- data members ---------------------------

                    protected:
                        /**
                        * The RemoteNamedCache.
                        */
                        WeakHandle<BinaryCache> m_whCache;
                    };


            // ----- inner class: KeySet --------------------------------

            public:
                /**
                * Key Set implementation for the RemoteNamedCache.
                */
                class COH_EXPORT KeySet
                    : public class_spec<KeySet,
                        extends<Object>,
                        implements<Set> >
                    {
                    friend class factory<KeySet>;

                    // ----- constructors ---------------------------

                    protected:
                        /**
                        * Create a new KeySet object for the given
                        * RemoteNamedCache.
                        *
                        * @param hCache  the RemoteNamedCache for which the
                        *                KeySet is being created
                        *
                        */
                        KeySet(BinaryCache::Handle hCache);

                    // ----- Set interface --------------------------

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
                        virtual bool contains(Object::View v) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool containsAll(Collection::View vCol) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual Iterator::Handle iterator() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual Muterator::Handle iterator();

                        /**
                        * {@inheritDoc}
                        */
                        virtual ObjectArray::Handle toArray(
                                ObjectArray::Handle hoa = NULL) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool add(Object::Holder oh);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool addAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool remove(Object::View v);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool removeAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool retainAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual void clear();

                    // ----- Object interface -----------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual bool equals(Object::View v) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual size32_t hashCode() const;

                    // ----- inner class: KeyIterator ---------------

                    public:
                        /**
                        * Iterator implementation for the KeySet class.
                        */
                        class COH_EXPORT KeyIterator
                            : public class_spec<KeyIterator,
                                extends<Object>,
                                implements<Muterator> >
                            {
                            friend class factory<KeyIterator>;

                            // ----- constructors ---------------

                            protected:
                                /**
                                * Create a new KeyIterator object for the
                                * given RemoteNamedCache.
                                *
                                * @param ohCache  the RemoteNamedCache over
                                *                 which the KeyIterator will
                                *                 iterate
                                *
                                */
                                KeyIterator(BinaryCache::Holder ohCache);

                            // ----- Iterator interface ---------

                            public:
                                /**
                                * @internal
                                */
                                virtual bool hasNext() const;

                                /**
                                * @internal
                                */
                                virtual Object::Holder next();

                                /**
                                * @internal
                                */
                                virtual void remove();

                            // ----- data members ---------------

                            protected:
                                /**
                                * The last key that was iterated.
                                */
                                MemberView<Object> m_vKey;

                                /**
                                * An iterator over the keys returned by
                                * RemoteNamedCache::keySet(NULL).
                                */
                                FinalHandle<Iterator> f_hIterKeys;

                                /**
                                * The RemoteNamedCache.
                                */
                                FinalHolder<BinaryCache> f_ohCache;
                            };

                    // ----- inner class: KeyAdvancer ---------------

                    public:
                        /**
                        * Advancer for the parent KeySet.
                        */
                        class COH_EXPORT KeyAdvancer
                            : public class_spec<KeyAdvancer,
                                extends<Object>,
                                implements<PagedIterator::Advancer> >
                            {
                            friend class factory<KeyAdvancer>;

                            // ----- constructors ---------------

                            protected:
                                /**
                                * Create a new KeyAdvancer object
                                *
                                * @param ohCache  the RemoteNamedCache
                                */
                                KeyAdvancer(BinaryCache::Holder ohCache);

                            // ----- Advancer interface ---------

                            public:
                                /**
                                * {@inheritDoc}
                                */
                                virtual Collection::Handle nextPage();

                                /**
                                * {@inheritDoc}
                                */
                                virtual void remove(Object::View vCurr);

                            // ----- data members ---------------

                            protected:
                                /**
                                * Property BinaryCache
                                *
                                * The BinaryCache that is being iterated.
                                */
                                FinalHolder<BinaryCache> f_ohCache;

                                /**
                                * Property Cookie
                                *
                                * Opaque cookie used for streaming.
                                */
                                MemberView<Binary> m_vCookie;

                                /**
                                * Property Exhausted
                                *
                                * True iff the Advancer has been exhausted.
                                */
                                bool m_fExhausted;
                            };

                    // ----- data members ---------------------------

                    protected:
                        /**
                        * The RemoteNamedCache.
                        */
                        WeakHandle<BinaryCache> m_whCache;
                    };


            // ----- inner class: Values --------------------------------

            public:
                /**
                * Value Collection implementation for the RemoteNamedCache.
                */
                class COH_EXPORT Values
                    : public class_spec<Values,
                        extends<Object>,
                        implements<Collection> >
                    {
                    friend class factory<Values>;

                    // ----- constructors ---------------------------

                    protected:
                        /**
                        * Create a new Values object for the given
                        * RemoteNamedCache.
                        *
                        * @param hCache  the RemoteNamedCache for which the
                        *                Values is being created
                        *
                        */
                        Values(BinaryCache::Handle hCache);

                    // ----- Collection interface -------------------

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
                        virtual bool contains(Object::View v) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool containsAll(Collection::View vCol) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual Iterator::Handle iterator() const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual Muterator::Handle iterator();

                        /**
                        * {@inheritDoc}
                        */
                        virtual ObjectArray::Handle toArray(
                                ObjectArray::Handle hoa = NULL) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool add(Object::Holder oh);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool addAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool remove(Object::View v);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool removeAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual bool retainAll(Collection::View vCol);

                        /**
                        * {@inheritDoc}
                        */
                        virtual void clear();

                    // ----- Object interface -----------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual bool equals(Object::View v) const;

                        /**
                        * {@inheritDoc}
                        */
                        virtual size32_t hashCode() const;

                    // ----- inner class: ValueIterator -------------

                    public:
                        /**
                        * Iterator implementation for the Values class.
                        */
                        class COH_EXPORT ValueIterator
                            : public class_spec<ValueIterator,
                                extends<Object>,
                                implements<Muterator> >
                            {
                            friend class factory<ValueIterator>;

                            // ----- constructors ---------------

                            protected:
                                /**
                                * Create a new ValueIterator object for the
                                * given RemoteNamedCache.
                                *
                                * @param ohCache  the RemoteNamedCache over
                                *                 which the ValueIterator
                                *                 will iterate
                                *
                                */
                                ValueIterator(BinaryCache::Holder ohCache);

                            // ----- Iterator interface ---------

                            public:
                                /**
                                * @internal
                                */
                                virtual bool hasNext() const;

                                /**
                                * @internal
                                */
                                virtual Object::Holder next();

                                /**
                                * @internal
                                */
                                virtual void remove();

                            // ----- data members ---------------

                            protected:
                                /**
                                * The last value that was iterated.
                                */
                                MemberHolder<Object> m_ohValue;

                                /**
                                * An iterator over the keys returned by
                                * RemoteNamedCache::keySet().
                                */
                                FinalHandle<Iterator> f_hIterKeys;

                                /**
                                * The RemoteNamedCache.
                                */
                                FinalHolder<BinaryCache> f_ohCache;
                            };

                    // ----- data members ---------------------------

                    protected:
                        /**
                        * The RemoteNamedCache.
                        */
                        WeakHandle<BinaryCache> m_whCache;
                    };

            // ----- data members ---------------------------------------

            protected:
                /**
                * The RemoteNamedCache.
                */
                WeakHandle<RemoteNamedCache> m_whCache;

                /**
                * The Channel used to exchange Messages with a remote
                * ProxyService.
                */
                mutable FinalHandle<Channel> f_hChannel;

                /**
                * The parent ConverterBinaryToDecoratedBinary instance.
                */
                FinalHandle<RemoteNamedCache::ConverterBinaryToDecoratedBinary>
                        m_hConverterBinaryToDecoratedBinary;

                /**
                * The parent ConverterBinaryToUndecoratedBinary instance.
                */
                FinalView<RemoteNamedCache::ConverterBinaryToUndecoratedBinary>
                        m_vConverterBinaryToUndecoratedBinary;

                /**
                * The child EntrySet instance.
                */
                FinalHandle<EntrySet> f_hEntrySet;

                /**
                * The QueueProcessor used to dispatch MapEvents.
                */
                FinalHandle<QueueProcessor> f_hEventDispatcher;

                /**
                * The child KeySet instance.
                */
                FinalHandle<KeySet> f_hKeySet;

                /**
                * The child Values instance.
                */
                FinalHandle<Values> f_hValues;

                /**
                * Used to add/remove map listeners.
                */
                FinalHandle<MapListenerSupport> f_hListenerSupport;

                /**
                * A LongArray of Filter objects indexed by the unique filter
                * id. These filter id values are used by the MapEvent message
                * to specify what filters caused a cache event.
                * <p>
                * Note: all access (for update) to this array should be
                * synchronized on the MapListenerSupport object.
                */
                FinalHandle<LongArray> f_hlaFilters;
            };


    // ----- Inner class: ConverterListener -----------------------------

    public:
        /**
        * MapListener implementation that wraps another MapListener and
        * converts dispatched MapEvents using the RemoteNamedCache's
        * Converters.
        */
        class COH_EXPORT ConverterListener
            : public class_spec<ConverterListener,
                extends<Object>,
                implements<MapListener> >
            {
            friend class factory<ConverterListener>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new ConverterListener.
                */
                ConverterListener();

            // ----- MapListener interface ------------------------------

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

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Set the converter used to convert MapEvents.
                *
                * @param vConverter  converter used to convert MapEvents
                */
                virtual void setConverter(Converter::View vConverter);

                /**
                * Get the converter used to convert MapEvents.
                *
                * @return converter used to convert MapEvents
                */
                virtual Converter::View getConverter() const;

                /**
                * Set the NamedCache that is the source of converted
                * MapEvents.
                *
                * @param hNamedCache  the NamedCache
                */
                virtual void setNamedCache(NamedCache::Handle hNamedCache);

                /**
                * Get the NamedCache that is the source of converted
                * MapEvents.
                *
                * @return the NamedCache
                */
                virtual NamedCache::Handle getNamedCache();

                /**
                * Get the NamedCache that is the source of converted
                * MapEvents.
                *
                * @return the NamedCache
                */
                virtual NamedCache::View getNamedCache() const;

                /**
                * Set the delegate MapListener.
                *
                * @param hListener  the delegate MapListener
                */
                virtual void setListener(MapListener::Handle hListener);

                /**
                * Get the delegate MapListener.
                *
                * @return the delegate MapListener
                */
                virtual MapListener::Handle getListener();

                /**
                * Get the delegate MapListener.
                *
                * @return the delegate MapListener
                */
                virtual MapListener::View getListener() const;

            // ----- internal methods -----------------------------------

            protected:
                /**
                * Dispatch the given MapEvent to the delegate MapListener.
                *
                * @param vEvent  the MapEvent to dispatch
                */
                virtual void dispatch(MapEvent::View vEvent);

            // ----- data members ---------------------------------------

            protected:
                /**
                * The converter used to convert MapEvents.
                */
                FinalView<Converter> f_vConverter;

                /**
                * The NamedCache that is the source of converted MapEvents.
                */
                FinalHandle<NamedCache> f_hNamedCache;

                /**
                * The delegate MapListener.
                */
                FinalHandle<MapListener> f_hListener;
            };


    // ----- NamedCache interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getCacheName() const;

        /**
        * {@inheritDoc}
        */
        virtual CacheService::Handle getCacheService();

        /**
        * {@inheritDoc}
        */
        virtual CacheService::View getCacheService() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isActive() const;

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


    // ----- CacheMap interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Map::View getAll(Collection::View vColKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue,
                int64_t cMillis);


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
                InvocableMap::EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Map::View invokeAll(Collection::View vCollKeys,
                InvocableMap::EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Map::View invokeAll(Filter::View vFilter,
                InvocableMap::EntryProcessor::Handle hAgent);

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


    // ----- CacheStore interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void store(Object::View vKey, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual void storeAll(Map::View vMapEntries);

        /**
        * {@inheritDoc}
        */
        virtual void erase(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual void eraseAll(Collection::View vColKeys);


    // ----- CacheLoader interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder load(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual Map::View loadAll(Collection::View vColKeys);


    // ----- Receiver interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getName() const;

        /**
        * {@inheritDoc}
        */
        virtual Protocol::View getProtocol() const;

        /**
        * {@inheritDoc}
        */
        virtual void registerChannel(Channel::Handle hChannel);

        /**
        * {@inheritDoc}
        */
        virtual void onMessage(Message::Handle hMessage);

        /**
        * {@inheritDoc}
        */
        virtual void unregisterChannel(Channel::Handle hChannel);

        /**
        * {@inheritDoc}
        */
        virtual void onChannelClosed(Channel::Handle hChannel);

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


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- accessors ------------------------------------------------------

    protected:
        /**
        * Configure the Channel used to communicate with the remote
        * ProxyService.
        *
        * @param hChannel  the Channel
        */
        virtual void setChannel(Channel::Handle hChannel);

        /**
        * Set the DeactivationListeners
        *
        * @param hListener  the DeactivationListeners to set
        */
        virtual void setDeactivationListeners(Listeners::Handle hListeners);

    public:
        /**
        * Return the BinaryCache instance.
        *
        * @return the BinaryCache instance
        */
        virtual BinaryCache::View getBinaryCache() const;

        /**
        * Return the BinaryCache instance.
        *
        * @return the BinaryCache instance
        */
        virtual BinaryCache::Handle getBinaryCache();

        /**
        * Set the BinaryCache instance
        *
        * @param hCache  the BinaryCache to set
        */
        virtual void setBinaryCache(BinaryCache::Handle hCache);

        /**
        * Configure the name of this NamedCache instance.
        *
        * @param vsName  the name of this NamedCache
        */
        virtual void setCacheName(String::View vsName);

        /**
        * Configure the CacheService that created this NamedCache instance.
        *
        * @param vsName  the CacheService
        */
        virtual void setCacheService(CacheService::Handle hService);

        /**
        * Return the Channel used to communicate with the remote
        * ProxyService.
        *
        * @return the Channel
        */
        virtual Channel::Handle getChannel() const;

        /**
        * Return the ConverterCache.
        *
        * @return the ConverterCache
        */
        virtual NamedCache::View getConverterCache() const;

        /**
        * Return the ConverterCache.
        *
        * @return the ConverterCache
        */
        virtual NamedCache::Handle getConverterCache();

        /**
        * Set the ConverterCache.
        *
        * @param hCache  the ConverterCache to set
        */
        virtual void setConverterCache(NamedCache::Handle hCache);

        /**
        * Return the ConverterBinaryToDecoratedBinary.
        *
        * @return the ConverterBinaryToDecoratedBinary
        */
        virtual ConverterBinaryToDecoratedBinary::View
                getConverterBinaryToDecoratedBinary() const;

        /**
        * Return the ConverterBinaryToDecoratedBinary.
        *
        * @return the ConverterBinaryToDecoratedBinary
        */
        virtual ConverterBinaryToDecoratedBinary::Handle
                getConverterBinaryToDecoratedBinary();

        /**
        * Set the ConverterBinaryToDecoratedBinary
        *
        * @param hConverter  the ConverterBinaryToDecoratedBinary to set
        */
        virtual void setConverterBinaryToDecoratedBinary(
                ConverterBinaryToDecoratedBinary::Handle hConverter);

        /**
        * Return the ConverterBinaryToUndecoratedBinary.
        *
        * @return the ConverterBinaryToUndecoratedBinary
        */
        virtual ConverterBinaryToUndecoratedBinary::View
                getConverterBinaryToUndecoratedBinary() const;

        /**
        * Return the ConverterBinaryToUndecoratedBinary.
        *
        * @return the ConverterBinaryToUndecoratedBinary
        */
        virtual ConverterBinaryToUndecoratedBinary::Handle
                getConverterBinaryToUndecoratedBinary();

        /**
        * Set the ConverterBinaryToUndecoratedBinary
        *
        * @param hConverter  the ConverterBinaryToUndecoratedBinary to set
        */
        virtual void setConverterBinaryToUndecoratedBinary(
                ConverterBinaryToUndecoratedBinary::Handle hConverter);

        /**
        * Return the ConverterFromBinary.
        *
        * @return the ConverterFromBinary
        */
        virtual ConverterFromBinary::View getConverterFromBinary() const;

        /**
        * Return the ConverterFromBinary.
        *
        * @return the ConverterFromBinary
        */
        virtual ConverterFromBinary::Handle getConverterFromBinary();

        /**
        * Set the ConverterFromBinary
        *
        * @param hConverter  the ConverterFromBinary to set
        */
        virtual void setConverterFromBinary(ConverterFromBinary::Handle hConverter);

        /**
        * Return the ConverterKeyToBinary.
        *
        * @return the ConverterKeyToBinary
        */
        virtual ConverterKeyToBinary::View getConverterKeyToBinary() const;

        /**
        * Return the ConverterKeyToBinary.
        *
        * @return the ConverterKeyToBinary
        */
        virtual ConverterKeyToBinary::Handle getConverterKeyToBinary();

        /**
        * Set the ConverterKeyToBinary
        *
        * @param hConverter  the ConverterKeyToBinary to set
        */
        virtual void setConverterKeyToBinary(ConverterKeyToBinary::Handle hConverter);

        /**
        * Return the ConverterValueToBinary.
        *
        * @return the ConverterValueToBinary
        */
        virtual ConverterValueToBinary::View getConverterValueToBinary() const;

        /**
        * Return the ConverterValueToBinary.
        *
        * @return the ConverterValueToBinary
        */
        virtual ConverterValueToBinary::Handle getConverterValueToBinary();

        /**
        * Set the ConverterValueToBinary
        *
        * @param hConverter  the ConverterValueToBinary to set
        */
        virtual void setConverterValueToBinary(ConverterValueToBinary::Handle hConverter);

        /**
        * Return the DeactivationListeners.
        *
        * @return the DeactivationListeners
        */
        virtual Listeners::Handle getDeactivationListeners();

        /**
        * Determine whether a key should be checked for KeyAssociation by the
        * extend client (false) or deferred until the key is received by the
        * PartionedService (true).
        *
        * @return whether or not to defer the KeyAssociation check
        */
        virtual bool isDeferKeyAssociationCheck() const;

        /**
        * Configure whether a key should be checked for KeyAssociation by the
        * extend client (false) or deferred until the key is received by the
        * PartionedService (true).
        *
        * @param fDefer  whether or not to defer the KeyAssociation check
        */
        virtual void setDeferKeyAssociationCheck(bool fDefer);

        /**
        * Return the QueueProcessor used to dispatch MapEvents.
        *
        * @return the QueueProcessor
        */
        virtual QueueProcessor::Handle getEventDispatcher();

        /**
        * Return the QueueProcessor used to dispatch MapEvents.
        *
        * @return the QueueProcessor
        */
        virtual QueueProcessor::View getEventDispatcher() const;

        /**
        * Configure the QueueProcessor used to dispatch MapEvents.
        *
        * @param hDispacher the event dispatcher QueueProcessor
        */
        virtual void setEventDispatcher(QueueProcessor::Handle hDispacher);


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Instantiate and configure a new ConverterListener for the given
        * MapListener.
        *
        * @param hListener  the MapListener to wrap
        *
        * @return a new ConverterListener that wraps the given MapListener
        */
        virtual MapListener::Handle instantiateConverterListener(
                MapListener::Handle hListener);

        /**
         * Instantiate a Binary cache view for this RemoteNamedCache.
         */
        virtual BinaryCache::Handle instantiateBinaryCache();

        /**
         * Print only once a warning message for deprecated lock API.
         */
        virtual void printLockDeprecatedMessage() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Child BinaryCache instance.
        */
        FinalHandle<BinaryCache> f_hBinaryCache;

        /**
        * The name of this NamedCache instance.
        */
        FinalView<String> f_vsCacheName;

        /**
        * The CacheService that created this NamedCache instance.
        */
        WeakHandle<CacheService> m_whCacheService;

        /**
        * The Channel used to exchange Messages with a remote ProxyService.
        */
        mutable MemberHandle<Channel> m_hChannel;

        /**
        * The client view of the BinaryCache.
        */
        FinalHandle<NamedCache> f_hConverterCache;

        /**
        * Child ConverterBinaryToDecoratedBinary instance.
        */
        FinalHandle<ConverterBinaryToDecoratedBinary>
                m_hConverterBinaryToDecoratedBinary;

        /**
        * Child ConverterBinaryToUndecoratedBinary instance.
        */
        FinalHandle<ConverterBinaryToUndecoratedBinary>
                m_hConverterBinaryToUndecoratedBinary;
\
        /**
        * Child DeactivationListeners instance.
        */
        MemberHandle<Listeners> m_hDeactivationListeners;

        /**
        * Child ConverterFromBinary instance.
        */
        FinalHandle<ConverterFromBinary> f_hConverterFromBinary;

        /**
        * Child ConverterKeyToBinary instance.
        */
        FinalHandle<ConverterKeyToBinary> f_hConverterKeyToBinary;

        /**
        * Child ConverterValueToBinary instance.
        */
        FinalHandle<ConverterValueToBinary> f_hConverterValueToBinary;

        /**
        * Whether a key should be checked for KeyAssociation by the extend
        * client (false) or deferred until the key is received by the
        * PartionedService (true).
        */
        bool fDeferKeyAssociationCheck;

        /**
        * Whether we have warned user about the deprecated lock methods.
        */
        mutable bool m_fLockDeprecateWarned;

        /**
        * The QueueProcessor used to dispatch MapEvents.
        */
        FinalHandle<QueueProcessor> f_hEventDispatcher;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_REMOTE_NAMED_CACHE_HPP
