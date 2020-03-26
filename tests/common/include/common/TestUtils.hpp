/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_UTILS
#define COH_TEST_UTILS

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"

using namespace coherence::lang;

using coherence::net::CacheFactory;
using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::NamedCache;

/**
* Simple macro to do polling assertions.
* 
* @param EXPR           boolean expression to test
* @param TIMEOUT        max time, in milliseconds, to prove the assertion
* @param POLL_INTERVAL  how often, in milliseconds, to test the assertion

* @since 12.2.1.4
*/
#define COH_EVENTUALLY_ASSERT(EXPR, TIMEOUT, POLL_INTERVAL) \
try \
    { \
    COH_TIMEOUT_AFTER(TIMEOUT) \
        { \
        while (!(EXPR)) \
            { \
            Thread::sleep(POLL_INTERVAL); \
            } \
        } \
    } \
catch (InterruptedException::View ve) \
    { \
    COH_THROW_STREAM(InterruptedException, \
        "Timed out waiting for condition \"" << #EXPR << "\" to evaluate to true"); \
    } \

/**
 * Simple macro to do polling assertions.  This macro will default the 
 * timeout to <code>60000ms</code> and polls every <code>500ms</code>
 * 
 * @since 12.2.1.4
 */
#define COH_EVENTUALLY_ASSERT_THAT(EXPR) COH_EVENTUALLY_ASSERT(EXPR, 60000, 500)

/**
* Ensure that a clean cache has been retrieved from the CacheFactory.
*
* @param vsCacheName  the name of the cache you want to create
*/
NamedCache::Handle ensureCleanCache(String::View vsCacheName)
    {
    NamedCache::Handle hCache = CacheFactory::getCache(vsCacheName);
    hCache->clear();
    return hCache;
    }

/**
* Ensure that a clean cache has been retrieved from the CacheFactory.
*
* @param vsCacheName    the name of the cache you want to create
* @param vsCacheConfig  the cache configure file to use
*/
NamedCache::Handle ensureCleanCache(String::View vsCacheName, String::View vsCacheConfig)
    {
    if (NULL != vsCacheConfig)
        {
        CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
        }
    return ensureCleanCache(vsCacheName);
    }

/**
* Set the default cache configure with DefaultConfigurableCacheFactory's configure.
*/
void setDefaultCacheConfigure()
    {
    CacheFactory::configure(DefaultConfigurableCacheFactory::getDefaultCacheConfig());
    }

#endif
