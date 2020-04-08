/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

using namespace coherence::lang;

/**
* Test Suite for the System class.
*/
class SystemTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test that currentTimeMillis();
        */
        void testTime()
            {
            int64_t cMillisStart = System::currentTimeMillis();
            Thread::sleep(123);
            int64_t cMillis = System::currentTimeMillis() - cMillisStart;

            // assume time and sleep to be roughly accurate
            TS_ASSERT_LESS_THAN(100,     cMillis);
            TS_ASSERT_LESS_THAN(cMillis, 1000);
            }

        /**
        * Test safeTimeMillis();
        */
        void testSafeTimeMillis()
            {
            int64_t cMillisStart = System::safeTimeMillis();
            Thread::sleep(150);
            int64_t cMillisStop = System::safeTimeMillis();
            int64_t cMillis = cMillisStop - cMillisStart;

            // assume time and sleep to be roughly accurate
            TS_ASSERT_LESS_THAN(50,      cMillis);
            TS_ASSERT_LESS_THAN(cMillis, 1000);
            }

        void testCommonMonitor()
            {
            size32_t i = 0;

            // loop over all CMs until we find CM at 0 again
            for (Object::View vCommon = System::getCommonMonitor(i++);
                 System::getCommonMonitor(i) != vCommon; ++i)
                {
                }

            // ensure that we looped
            TS_ASSERT(i > 1);
            }

        void testCommon()
            {
            Boolean::Handle hb = Boolean::create(true);
            MemberHolder<Boolean> mh(System::common(), hb);
            TS_ASSERT(mh == hb);
            mh = Boolean::create(false);
            TS_ASSERT(mh != hb);
            TS_ASSERT(mh->equals(Boolean::create(false)));
            }

        int _setenv(const char *name, const char *value)
            {
            System::setProperty(name, value);
            int result = System::getProperty(name)->compareTo(String::create(value));
            TS_ASSERT(result == 0);
            return result;
            }

        void testGetPropertyCacheConfigVariants()
            {
            // test -DTangosolCoherenceCacheConfig="my-coherence-cache-config.xml"
            int result = _setenv("TangosolCoherenceCacheConfig", "my-coherence-cache-config.xml");
            if (result == 0)
                {
                String::View vsValueTmp = System::getProperty("TangosolCoherenceCacheConfig", "default");
                TS_ASSERT(vsValueTmp->equals("my-coherence-cache-config.xml"));

                String::View vsValue = System::getProperty("coherence.cacheconfig",
                    System::getProperty("coherence.cache.config", "coherence-cache-config.xml"));
                TS_ASSERT(vsValue->equals("my-coherence-cache-config.xml"));
                vsValue = System::getProperty("tangosol.coherence.cacheconfig",
                        System::getProperty("tangosol.coherence.cache.config","coherence-cache-config.xml"));
                TS_ASSERT(vsValue->equals("my-coherence-cache-config.xml"));
                System::clearProperty("TangosolCoherenceCacheConfig");
                }
             TS_ASSERT(System::getProperty("TangosolCoherenceCacheConfig", "default")->equals("default"));

             // test -DCoherenceCacheConfig="my-coherence-cache-config.xml"
             result = _setenv("CoherenceCacheConfig", "my-coherence-cache-config.xml");
             if (result == 0)
                 {
                 String::View vsValue = System::getProperty("coherence.cacheconfig",
                         System::getProperty("coherence.cache.config","coherence-cache-config.xml"));
                 TS_ASSERT(vsValue->equals("my-coherence-cache-config.xml"));
                 vsValue = System::getProperty("tangosol.coherence.cacheconfig",
                         System::getProperty("tangosol.coherence.cache.config","coherence-cache-config.xml"));
                 TS_ASSERT(vsValue->equals("my-coherence-cache-config.xml"));
                 System::clearProperty("CoherenceCacheConfig");
                 }
             TS_ASSERT(System::getProperty("CoherenceCacheConfig", "default")->equals("default"));

             // test -DTangosolCoherenceCacheconfig="another-coherence-cache-config.xml"
             result = _setenv("TangosolCoherenceCacheconfig", "another-coherence-cache-config.xml");
             if (result == 0)
                 {
                 String::View vsValue = System::getProperty("coherence.cacheconfig",
                         System::getProperty("coherence.cache.config","coherence-cache-config.xml"));
                 TS_ASSERT(vsValue->equals("another-coherence-cache-config.xml"));
                 vsValue = System::getProperty("tangosol.coherence.cacheconfig",
                                          System::getProperty("tangosol.coherence.cache.config","coherence-cache-config.xml"));
                 TS_ASSERT(vsValue->equals("another-coherence-cache-config.xml"));
                 System::clearProperty("TangosolCoherenceCacheconfig");
                 }
             TS_ASSERT(System::getProperty("TangosolCoherenceCacheconfig", "default")->equals("default"));

             // test -DCoherenceCacheconfig="another-coherence-cache-config.xml"
             result = _setenv("CoherenceCacheconfig", "another-coherence-cache-config.xml");
             if (result == 0)
                 {
                 String::View vsValue = System::getProperty("coherence.cacheconfig",
                         System::getProperty("coherence.cache.config","coherence-cache-config.xml"));
                 TS_ASSERT(vsValue->equals("another-coherence-cache-config.xml"));
                 vsValue = System::getProperty("tangosol.coherence.cacheconfig",
                                                         System::getProperty("tangosol.coherence.cache.config","coherence-cache-config.xml"));
                 TS_ASSERT(vsValue->equals("another-coherence-cache-config.xml"));
                 System::clearProperty("CoherenceCacheconfig");
                 }
             TS_ASSERT(System::getProperty("CoherenceCacheconfig", "default")->equals("default"));
            }

        void testGetProperty()
            {
            String::View vsValue = System::getProperty("coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("default"));
            vsValue = System::getProperty("tangosol.coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("default"));

            // test -Dtangosol.coherence.system.property="backwards"
            int result = _setenv("tangosol.coherence.system.property", "backwards");
            if (result == 0)
                {
                vsValue = System::getProperty("coherence.system.property", "default");
                TS_ASSERT(vsValue->equals("backwards"));
                vsValue = System::getProperty("tangosol.coherence.system.property", "default");
                TS_ASSERT(vsValue->equals("backwards"));
                System::clearProperty("tangosol.coherence.system.property");
                }

            // test -DTangosolCoherenceSystemProperty="backwards"
            result = _setenv("TangosolCoherenceSystemProperty", "backwards");
            TS_ASSERT(result == 0);
            vsValue = System::getProperty("coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("backwards"));
            vsValue = System::getProperty("tangosol.coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("backwards"));
            System::clearProperty("TangosolCoherenceSystemProperty");
            vsValue = System::getProperty("coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("default"));

            // test -Dcoherence.system.property="current"
            result = _setenv("coherence.system.property", "current");
            if (result == 0)
                {
                vsValue = System::getProperty("coherence.system.property", "default");
                TS_ASSERT(vsValue->equals("current"));
                vsValue = System::getProperty("tangosol.coherence.system.property", "default");
                TS_ASSERT(vsValue->equals("current"));
                System::clearProperty("coherence.system.property");
                }

            vsValue = System::getProperty("coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("default"));

            // test -DCoherenceSystemProperty="current"
            result = _setenv("CoherenceSystemProperty", "current");
            TS_ASSERT(result == 0);
            vsValue = System::getProperty("coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("current"));
            vsValue = System::getProperty("tangosol.coherence.system.property", "default");
            TS_ASSERT(vsValue->equals("current"));
            System::clearProperty("CoherenceSystemProperty");
            }
    };
