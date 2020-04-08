/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_CACHE_STATISTICS_HPP
#define COH_SIMPLE_CACHE_STATISTICS_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/CacheStatistics.hpp"



COH_OPEN_NAMESPACE3(coherence,net,cache)


/**
* Implementation of the CacheStatistics class intended for use by a cache
* to maintain its statistics.
*
* @author tb  2008.06.12
*/
class COH_EXPORT SimpleCacheStatistics
    : public class_spec<SimpleCacheStatistics,
        extends<Object>,
        implements<CacheStatistics> >
    {
    friend class factory<SimpleCacheStatistics>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a SimpleCacheStatistics object.
        */
        SimpleCacheStatistics();


    // ----- CacheStatistics interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int64_t getTotalGets() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getTotalGetsMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getAverageGetMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getTotalPuts() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getTotalPutsMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getAveragePutMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getCacheHits() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getCacheHitsMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getAverageHitMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getCacheMisses() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getCacheMissesMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getAverageMissMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getHitProbability() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getCachePrunes() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getCachePrunesMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getAveragePruneMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual void resetHitStatistics();


    // ----- accessors, mutators and helpers --------------------------------

    public:
        /**
        * Register a cache hit (no timing information).
        */
        virtual void registerHit();

        /**
        * Register a cache hit.
        *
        * @param lStartMillis  the time when the get operation started
        */
        virtual void registerHit(int64_t lStartMillis);

        /**
        * Register a multiple cache hit.
        *
        * @param cHits         the number of hits
        * @param lStartMillis  the time when the get operation started
        */
        virtual void registerHits(int32_t cHits, int64_t lStartMillis);

        /**
        * Register a cache miss (no timing information).
        */
        virtual void registerMiss();

        /**
        * Register a cache miss.
        *
        * @param lStartMillis  the time when the get operation started
        */
        virtual void registerMiss(int64_t lStartMillis);

        /**
        * Register a multiple cache miss.
        *
        * @param cMisses       the number of misses
        * @param lStartMillis  the time when the get operation started
        */
        virtual void registerMisses(int32_t cMisses, int64_t lStartMillis);

        /**
        * Register a cache put.
        *
        * @param lStartMillis  the time when the put operation started
        */
        virtual void registerPut(int64_t lStartMillis);

        /**
        * Register a multiple cache put.
        *
        * @param cPuts         the number of puts
        * @param lStartMillis  the time when the put operation started
        */
        virtual void registerPuts(int32_t cPuts, int64_t lStartMillis);

        /**
        * Register a cache prune.
        *
        * @param lStartMillis  the time when the prune operation started
        */
        virtual void registerCachePrune(int64_t lStartMillis);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The rough (ie unsynchronized) number of calls that could be
        * answered from the front or the back and were answered by data in
        * the front map.
        */
        /*volatile stat*/ int64_t m_cCacheHits;

        /**
        * The rough (ie unsynchronized) number of calls that could be
        * answered from the front or the back and were answered by data in
        * the back map.
        */
        /*volatile stat*/ int64_t m_cCacheMisses;

        /**
        * Total number of milliseconds used for get operations that were hits
        * since the last statistics reset.
        */
        /*volatile stat*/ int64_t m_cHitsMillis;

        /**
        * Total number of milliseconds used for get operations that were
        * misses since the last statistics reset.
        */
        /*volatile stat*/ int64_t m_cMissesMillis;

        /**
        * Total number of put operations since the last statistics reset.
        */
        /*volatile stat*/ int64_t m_cCachePuts;

        /**
        * Total number of milliseconds used for put operations since the last
        * statistics reset.
        */
        /*volatile stat*/ int64_t m_cPutsMillis;

        /**
        * Total number of evictions triggered based on the size of the cache
        * since the last statistics reset.
        */
        /*volatile stat*/ int64_t m_cCachePrunes;

        /**
        * Total number of milliseconds used for prune operations since the
        * last statistics reset.
        */
        /*volatile stat*/ int64_t m_cCachePrunesMillis;

    };

COH_CLOSE_NAMESPACE3

#endif // COH_SIMPLE_CACHE_STATISTICS_HPP
