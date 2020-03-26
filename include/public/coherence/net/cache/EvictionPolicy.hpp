/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVICTION_POLICY_HPP
#define COH_EVICTION_POLICY_HPP

#include "coherence/lang.ns"

#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Map;


/**
* An eviction policy is an object that the cache provides with access
* information, and when requested, the eviction policy selects and
* evicts entries from the cache. If the eviction policy needs to be
* aware of changes to the cache, it must implement the MapListener
* interface; if it does, it will automatically be registered to receive
* MapEvents.
*
* @author nsa 2008.06.23
*/
class EvictionPolicy
    : public interface_spec<EvictionPolicy>
    {
    // ----- constants ------------------------------------------------------

    public:
        /**
        * EvictionPolicyType describes the eviction policies available
        */
        typedef enum
            {
            /**
            * By default, the cache prunes based on a hybrid LRU+LFU
            * algorithm.
            */
            eviction_policy_hybrid   = 0,
            /**
            * The cache can prune based on a pure Least Recently Used (LRU)
            * algorithm.
            */
            eviction_policy_lru      = 1,
            /**
            * The cache can prune based on a pure Least Frequently Used (LFU)
            * algorithm.
            */
            eviction_policy_lfu      = 2,
            /**
            * The cache can prune using an external eviction policy.
            */
            eviction_policy_external = 3
            } EvictionPolicyType;

    // ----- EvictionPolicy interface ---------------------------------------

    public:
        /**
        * This method is called by the cache to indicate that an entry has
        * been touched.
        *
        * @param hEntry  the Cache Entry that has been touched
        */
        virtual void entryTouched(Map::Entry::Handle hEntry) = 0; // TODO NSA - Have Mark Review

        /**
        * This method is called by the cache when the cache requires the
        * eviction policy to evict entries.
        *
        * @param cMaximum  the maximum number of units that should remain
        *                  in the cache when the eviction is complete
        */
        virtual void requestEviction(int32_t cMaximum) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // #define COH_EVICTION_POLICY_HPP
