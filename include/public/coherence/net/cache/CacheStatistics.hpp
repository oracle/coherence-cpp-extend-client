/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_STATISTICS_HPP
#define COH_CACHE_STATISTICS_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,net,cache)


/**
* An interface for exposing Cache statistics.
*
* @author tb  2008.06.12
*/
class COH_EXPORT CacheStatistics
    : public interface_spec<CacheStatistics>
    {
    // ----- CacheStatistics interface --------------------------------------

    public:
        /**
        * Determine the total number of get() operations since the cache
        * statistics were last reset.
        *
        * @return the total number of get() operations
        */
        virtual int64_t getTotalGets() const = 0;

        /**
        * Determine the total number of milliseconds spent on get()
        * operations since the cache statistics were last reset.
        *
        * @return the total number of milliseconds processing get()
        *         operations
        */
        virtual int64_t getTotalGetsMillis() const = 0;

        /**
        * Determine the average number of milliseconds per get() invocation
        * since the cache statistics were last reset.
        *
        * @return the average number of milliseconds per get() operation
        */
        virtual float64_t getAverageGetMillis() const = 0;

        /**
        * Determine the total number of put() operations since the cache
        * statistics were last reset.
        *
        * @return the total number of put() operations
        */
        virtual int64_t getTotalPuts() const = 0;

        /**
        * Determine the total number of milliseconds spent on put()
        * operations since the cache statistics were last reset.
        *
        * @return the total number of milliseconds processing put()
        *         operations
        */
        virtual int64_t getTotalPutsMillis() const = 0;

        /**
        * Determine the average number of milliseconds per put() invocation
        * since the cache statistics were last reset.
        *
        * @return the average number of milliseconds per put() operation
        */
        virtual float64_t getAveragePutMillis() const = 0;

        /**
        * Determine the rough number of cache hits since the cache statistics
        * were last reset.
        *
        * A cache hit is a read operation invocation (i.e. get()) for which
        * an entry exists in this map.
        *
        * @return the number of get() calls that have been served by
        *         existing cache entries
        */
        virtual int64_t getCacheHits() const = 0;

        /**
        * Determine the total number of milliseconds (since that last
        * statistics reset) for the get() operations for which an entry
        * existed in this map.
        *
        * @return the total number of milliseconds for the get() operations
        *         that were hits
        */
        virtual int64_t getCacheHitsMillis() const = 0;

        /**
        * Determine the average number of milliseconds per get() invocation
        * that is a hit.
        *
        * @return the average number of milliseconds per cache hit
        */
        virtual float64_t getAverageHitMillis() const = 0;

        /**
        * Determine the rough number of cache misses since the cache
        * statistics were last reset.
        *
        * A cache miss is a get() invocation that does not have an entry in
        * this map.
        *
        * @return the number of get() calls that failed to find an existing
        *         cache entry because the requested key was not in the cache
        */
        virtual int64_t getCacheMisses() const = 0;

        /**
        * Determine the total number of milliseconds (since that last
        * statistics reset) for the get() operations for which no entry
        * existed in this map.
        *
        * @return the total number of milliseconds (since that last
        *         statistics reset) for the get() operations that were misses
        */
        virtual int64_t getCacheMissesMillis() const = 0;

        /**
        * Determine the average number of milliseconds per get() invocation
        * that is a miss.
        *
        * @return the average number of milliseconds per cache miss
        */
        virtual float64_t getAverageMissMillis() const = 0;

        /**
        * Determine the rough probability (0 <= p <= 1) that the next
        * invocation will be a hit, based on the statistics collected since
        * the last reset of the cache statistics.
        *
        * @return the cache hit probability (0 <= p <= 1)
        */
        virtual float64_t getHitProbability() const = 0;

        /**
        * Determine the rough number of cache pruning cycles since the cache
        * statistics were last reset.
        *
        * For the LocalCache implementation, this refers to the number of
        * times that the <tt>prune()</tt> method is executed.
        *
        * @return the total number of cache pruning cycles (since that last
        *         statistics reset)
        */
        virtual int64_t getCachePrunes() const = 0;

        /**
        * Determine the total number of milliseconds (since that last
        * statistics reset) spent on cache pruning.
        *
        * For the LocalCache implementation, this refers to the time spent in
        * the <tt>prune()</tt> method.
        *
        * @return the total number of milliseconds (since that last statistics
        *         reset) for cache pruning operations
        */
        virtual int64_t getCachePrunesMillis() const = 0;

        /**
        * Determine the average number of milliseconds per cache pruning.
        *
        * @return the average number of milliseconds per cache pruning
        */
        virtual float64_t getAveragePruneMillis() const = 0;

        /**
        * Reset the cache statistics.
        */
        virtual void resetHitStatistics() = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CACHE_STATISTICS_HPP
