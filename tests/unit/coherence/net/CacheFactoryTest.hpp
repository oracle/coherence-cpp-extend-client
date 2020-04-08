/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/net/cache/NearCache.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/component/net/extend/RemoteInvocationService.hpp"
#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"

#include "private/coherence/component/util/SafeNamedCache.hpp"

#include "private/coherence/net/cache/LocalNamedCache.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <sstream>

using namespace coherence::lang;
using namespace coherence::net;
using namespace coherence::net::cache;
using namespace coherence::util;
using namespace std;

using coherence::component::net::extend::RemoteInvocationService;
using coherence::component::net::extend::RemoteNamedCache;
using coherence::component::util::SafeNamedCache;
using coherence::util::logging::Logger;

class ThreadDumper
    : public Runnable
    {
    public:
        void run()
            {
            while (true)
                {
                Thread::sleep(5000);
                std::cout << Thread::formatStacks();
                }
            }
    };

COH_PRAGMA_PUSH
class CustomLocalCache
    : public class_spec<CustomLocalCache,
        extends<LocalNamedCache> >
    {
    friend class factory<CustomLocalCache>;

    protected:
        CustomLocalCache()
             : super()
            {
            }
    };
COH_REGISTER_TYPED_CLASS(CustomLocalCache);
COH_PRAGMA_POP

/**
* CacheFactory test suite
*/
class CacheFactoryTest : public CxxTest::TestSuite
    {
    public:

        void testConfigureLogger()
            {
            Logger::Handle hLogger = Logger::getLogger();
            int32_t nLevel = hLogger->getLevel();

            hLogger->setLevel(1);

            int32_t nLevelNew = 0;
            stringstream ss;
            ss << "<coherence>"
               << "  <logging-config>"
               << "    <severity-level>" << nLevelNew << "</severity-level>"
               << "  </logging-config>"
               << "</coherence>";

            CacheFactory::configure(NULL, CacheFactory::loadXml(ss));

            // Logger will have been shut down and restarted.  Refetch logger
            hLogger = Logger::getLogger();
            TS_ASSERT(hLogger->getLevel() == nLevelNew);
            hLogger->setLevel(nLevel);
            }

        void testCreateLocalCache()
            {
            stringstream ss;
            ss << "<cache-config>"
               << "  <caching-scheme-mapping>"
               << "    <cache-mapping>"
               << "      <cache-name>local-*</cache-name>"
               << "      <scheme-name>example-local</scheme-name>"
               << "    </cache-mapping>"
               << "  </caching-scheme-mapping>"
               << "  <caching-schemes>"
               << "    <local-scheme>"
               << "      <scheme-name>example-local</scheme-name>"
               << "    </local-scheme>"
               << "  </caching-schemes>"
               << "</cache-config>";

            CacheFactory::configure(CacheFactory::loadXml(ss));

            NamedCache::Handle hCache = CacheFactory::getCache("local-foo");

            // validate name, and type
            TS_ASSERT(hCache != NULL);
            TS_ASSERT(cast<LocalCache::Handle>(hCache) != NULL);
            TS_ASSERT(hCache->getCacheName()->equals("local-foo"));

            // validate caching
            TS_ASSERT(hCache == CacheFactory::getCache("local-foo"));
            TS_ASSERT(hCache != CacheFactory::getCache("local-bar"));

            // just for fun, use the thing
            hCache->put(Boolean::create(true), Boolean::create(false));
            TS_ASSERT (hCache->get(Boolean::create(true))->equals(Boolean::create(false)));
            }

        void testCreateThroughRef()
            {
            stringstream ss;
            ss << "<cache-config>"
               << "  <caching-scheme-mapping>"
               << "    <cache-mapping>"
               << "      <cache-name>local-*</cache-name>"
               << "      <scheme-name>example-local</scheme-name>"
               << "    </cache-mapping>"
               << "  </caching-scheme-mapping>"
               << "  <caching-schemes>"
               << "    <local-scheme>"
               << "      <scheme-name>example-local</scheme-name>"
               << "      <scheme-ref>ref-local</scheme-ref>"
               << "    </local-scheme>"
               << "    <local-scheme>"
               << "      <scheme-name>ref-local</scheme-name>"
               << "    </local-scheme>"
               << "  </caching-schemes>"
               << "</cache-config>";

            CacheFactory::configure(CacheFactory::loadXml(ss));

            NamedCache::Handle hCache = CacheFactory::getCache("local-foo");
            // validate name, and type
            TS_ASSERT(hCache != NULL);
            TS_ASSERT(cast<LocalCache::Handle>(hCache) != NULL);
            }

        void testCustomLocalCache()
            {
            stringstream ss;
            ss << "<cache-config>"
               << "  <caching-scheme-mapping>"
               << "    <cache-mapping>"
               << "      <cache-name>local-*</cache-name>"
               << "      <scheme-name>example-local</scheme-name>"
               << "    </cache-mapping>"
               << "  </caching-scheme-mapping>"
               << "  <caching-schemes>"
               << "    <local-scheme>"
               << "      <scheme-name>example-local</scheme-name>"
               << "      <class-name>CustomLocalCache</class-name>"
               << "    </local-scheme>"
               << "  </caching-schemes>"
               << "</cache-config>";

            CacheFactory::configure(CacheFactory::loadXml(ss));

            NamedCache::Handle hCache = CacheFactory::getCache("local-foo");

            // validate name, and type
            TS_ASSERT(hCache != NULL);
            TS_ASSERT(cast<CustomLocalCache::Handle>(hCache) != NULL);
            TS_ASSERT(hCache->getCacheName()->equals("local-foo"));
            }


        void testCreateNearCache()
            {
            stringstream ss;
            ss << "<cache-config>"
               << "  <caching-scheme-mapping>"
               << "    <cache-mapping>"
               << "      <cache-name>near-*</cache-name>"
               << "      <scheme-name>example-near</scheme-name>"
               << "    </cache-mapping>"
               << "  </caching-scheme-mapping>"
               << "  <caching-schemes>"
               << "    <near-scheme>"
               << "      <scheme-name>example-near</scheme-name>"
               << "      <front-scheme>"
               << "        <local-scheme>"
               << "          <scheme-ref>example-local-1</scheme-ref>"
               << "        </local-scheme>"
               << "      </front-scheme>"
               << "      <back-scheme>"
               << "        <local-scheme>"
               << "          <scheme-ref>example-local-2</scheme-ref>"
               << "        </local-scheme>"
               << "      </back-scheme>"
               << "    </near-scheme>"
               << "    <local-scheme>"
               << "      <scheme-name>example-local-1</scheme-name>"
               << "    </local-scheme>"
               << "    <local-scheme>"
               << "      <scheme-name>example-local-2</scheme-name>"
               << "    </local-scheme>"
               << "  </caching-schemes>"
               << "</cache-config>";

            CacheFactory::configure(CacheFactory::loadXml(ss));

            NamedCache::Handle hCache = CacheFactory::getCache("near-foo");

            // validate name, and type
            TS_ASSERT(hCache != NULL);
            TS_ASSERT(cast<NearCache::Handle>(hCache) != NULL);
            TS_ASSERT(hCache->getCacheName()->equals("near-foo"));

            // validate caching
            TS_ASSERT(hCache == CacheFactory::getCache("near-foo"));
            TS_ASSERT(hCache != CacheFactory::getCache("near-bar"));

            // just for fun, use the thing
            hCache->put(Boolean::create(true), Boolean::create(false));
            TS_ASSERT (hCache->get(Boolean::create(true))->equals(Boolean::create(false)));
            }

    };
