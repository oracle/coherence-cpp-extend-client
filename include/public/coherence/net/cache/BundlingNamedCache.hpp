/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BUNDLING_NAMED_CACHE_HPP
#define COH_BUNDLING_NAMED_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/net/cache/AbstractBundler.hpp"
#include "coherence/net/cache/AbstractEntryBundler.hpp"
#include "coherence/net/cache/AbstractKeyBundler.hpp"
#include "coherence/net/cache/WrapperNamedCache.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::net::NamedCache;
using coherence::util::Collection;
using coherence::util::Collections;
using coherence::util::Map;

/**
 * Bundling NamedCache implementation.
 *
 * @see AbstractBundler
 * @author gg 2007.02.13
 * @author lh 2012.06.05
 * @since Coherence 12.1.2
 */
class COH_EXPORT BundlingNamedCache
    : public class_spec<BundlingNamedCache,
        extends<WrapperNamedCache> >
    {
    friend class factory<BundlingNamedCache>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Construct a BundlingNamedCache based on the specified NamedCache.
         *
         * @param hCache  the NamedCache that will be wrapped by this 
         *                BundlingNamedCache
         */
        BundlingNamedCache(NamedCache::Handle hCache);

    private:
        /**
         * Blocked copy constructor.
         */
        BundlingNamedCache(const BundlingNamedCache&);

    // ----- initiators ------------------------------------------------------

    public:
        /**
         * Configure the bundler for the "get" operations. If the bundler does
         * not exist and bundling is enabled, it will be instantiated.
         *
         * @param cBundleThreshold  the bundle size threshold; pass zero to
         *                          disable "get" operation bundling
         *
         * @return the "get" bundler or NULL if bundling is disabled
         */
        virtual AbstractBundler::Handle ensureGetBundler(int32_t cBundleThreshold);

        /**
         * Configure the bundler for the "put" operations. If the bundler does
         * not exist and bundling is enabled, it will be instantiated.
         *
         * @param cBundleThreshold  the bundle size threshold; pass zero to
         *                          disable "put" operation bundling
         *
         * @return the "put" bundler or NULL if bundling is disabled
         */
        virtual AbstractBundler::Handle ensurePutBundler(int32_t cBundleThreshold);

        /**
         * Configure the bundler for the "remove" operations. If the bundler
         * does not exist and bundling is enabled, it will be instantiated.
         *
         * @param cBundleThreshold  the bundle size threshold; pass zero to
         *                          disable "remove" operation bundling
         *
         * @return the "remove" bundler or NULL if bundling is disabled
         */
        virtual AbstractBundler::Handle ensureRemoveBundler(int32_t cBundleThreshold);

    // ----- accessors -------------------------------------------------------

    public:
        /**
         * Obtain the bundler for the "get" operations.
         *
         * @return the "get" bundler
         */
        virtual AbstractBundler::Handle getGetBundler();

        /**
         * Obtain the bundler for the "put" operations.
         *
         * @return the "put" bundler
         */
        virtual AbstractBundler::Handle getPutBundler();

        /**
         * Obtain the bundler for the "remove" operations.
         *
         * @return the "remove" bundler
         */
        virtual AbstractBundler::Handle getRemoveBundler();
        
    // ----- various bundleable NamedCache methods ---------------------------

    public:
        using WrapperNamedCache::get;
        using WrapperNamedCache::getAll;
        using WrapperNamedCache::put;

        /**
         * {@inheritDoc}
         */
        virtual Object::Holder get(Object::View vKey);

        /**
         * {@inheritDoc}
         */
        virtual Map::View getAll(Collection::View vColKeys);

        /**
         * {@inheritDoc}
         *
         * <b>Note:</b> this method always returns NULL.
         */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue);

        /**
         * {@inheritDoc}
         */
        virtual void putAll(Map::View vMap);

        /**
         * {@inheritDoc}
         *
         * <b>Note:</b> this method always returns NULL.
         */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

    // ----- NamedCache interface --------------------------------------------

        /**
         * {@inheritDoc}
         */
        virtual void release();

        /**
         * {@inheritDoc}
         */
        virtual void destroy();

    // ----- inner classes ---------------------------------------------------

    public:
        class COH_EXPORT GetBundler
            : public class_spec<GetBundler,
                extends<AbstractKeyBundler> >
            {
            friend class factory<GetBundler>;
            
            // ----- constructors ---------------------------------------

            protected:
                /**
                 * @param hBundlingNamedCache  the BundlingNamedCache this 
                 *                             bundler is associated with
                 */
                GetBundler(BundlingNamedCache::Handle hBundlingNamedCache);

            // ----- bundle operations --------------------------------------

            protected:
                /**
                 * A pass through the underlying "getAll" operation.
                 *
                 * @param vColKeys  a collection of keys for the "getAll"
                 *                  operation
                 *
                 * @return the results.
                 */
                virtual Map::View bundle(Collection::View vColKeys);

                /**
                 * A pass through the underlying "get" operation.
                 *
                 * @param vKey  the entry key
                 */
                virtual Object::Holder unbundle(Object::View vKey) const;

                /**
                 * A pass through the underlying "get" operation.
                 *
                 * @param vKey  the entry key
                 */
                virtual Object::Holder unbundle(Object::View vKey);

            // ----- accessors ------------------------------------------

            public:
                /**
                 * Obtain the BundlingNamedCache for this bundler.
                 *
                 * @return the BundlingNamedCache
                 */
                virtual BundlingNamedCache::Handle getBundlingNamedCache();

                /**
                 * Obtain the BundlingNamedCache for this bundler.
                 *
                 * @return the BundlingNamedCache
                 */
                virtual BundlingNamedCache::View getBundlingNamedCache() const;

            // ----- data fields ----------------------------------------

            private:
                /**
                 * The BundlingNamedCache for this bundler.
                 */
                FinalHandle<BundlingNamedCache> f_hBundlingNamedCache;
            };

        class COH_EXPORT PutBundler
            : public class_spec<PutBundler,
                extends<AbstractEntryBundler> >
            {
            friend class factory<PutBundler>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                 * @param hBundlingNamedCache  the BundlingNamedCache this 
                 *                             bundler is associated with
                 */
                PutBundler(BundlingNamedCache::Handle hBundlingNamedCache);

            // ----- bundle operations ----------------------------------

            protected:
                /**
                 * A pass through the underlying "putAll" operation.
                 *
                 * @param vMap  the collection of entries
                 */
                virtual void bundle(Map::View vMap);

            // ----- accessors ------------------------------------------

            public:
                /**
                 * Obtain the BundlingNamedCache for this bundler.
                 *
                 * @return the BundlingNamedCache
                 */
                virtual BundlingNamedCache::Handle getBundlingNamedCache();

                /**
                 * Obtain the BundlingNamedCache for this bundler.
                 *
                 * @return the BundlingNamedCache
                 */
                virtual BundlingNamedCache::View getBundlingNamedCache() const;

            // ----- data fields ----------------------------------------

            private:
                /**
                 * The BundlingNamedCache for this bundler.
                 */
                FinalHandle<BundlingNamedCache> f_hBundlingNamedCache;
            };

        class COH_EXPORT RemoveBundler
            : public class_spec<RemoveBundler,
                extends<AbstractKeyBundler> >
            {
            friend class factory<RemoveBundler>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                 * @param hBundlingNamedCache  the BundlingNamedCache this 
                 *                             bundler is associated with
                 */
                RemoveBundler(BundlingNamedCache::Handle hBundlingNamedCache);

            // ----- bundle operations ----------------------------------

            protected:
                /**
                 * A pass through the underlying keySet()->removeAll() operation.
                 *
                 * @param vColKeys  the collection of keys to perform the
                 *                  bundled operation for
                 */
                virtual Map::View bundle(Collection::View vColKeys);

                /**
                 * A pass through the underlying "remove" operation.
                 *
                 * @param vKey  the entry key
                 */
                virtual Object::Holder unbundle(Object::View vKey);

            // ----- accessors ------------------------------------------

            public:
                /**
                 * Obtain the BundlingNamedCache for this bundler.
                 *
                 * @return the BundlingNamedCache
                 */
                virtual BundlingNamedCache::Handle getBundlingNamedCache();

                /**
                 * Obtain the BundlingNamedCache for this bundler.
                 *
                 * @return the BundlingNamedCache
                 */
                virtual BundlingNamedCache::View getBundlingNamedCache() const;

            // ----- data fields ----------------------------------------

            private:
                /**
                 * The BundlingNamedCache for this bundler.
                 */
                FinalHandle<BundlingNamedCache> f_hBundlingNamedCache;
            };

    // ----- data fields -----------------------------------------------------

    private:
        /**
         * The bundler for get() operations.
         */
        MemberHandle<GetBundler> m_hGetBundler;

        /**
         * The bundler for put() operations.
         */
        MemberHandle<PutBundler> m_hPutBundler;

        /**
         * The bundler for remove() operations.
         */
        MemberHandle<RemoveBundler> m_hRemoveBundler;
    };
    
COH_CLOSE_NAMESPACE3

#endif // COH_BUNDLING_NAMED_CACHE_HPP
