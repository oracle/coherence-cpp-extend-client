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

#include "coherence/util/MapTriggerListener.hpp"

#include "coherence/tests/SimplePerson.hpp"
#include "coherence/tests/SimplePersonMapTrigger.hpp"

#include "common/TestUtils.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::run::xml::XmlHelper;
using coherence::util::MapTriggerListener;
using coherence::tests::SimplePerson;
using coherence::tests::SimplePersonMapTrigger;

namespace MapTriggers
    {
    class TestMapTriggerFactory : public class_spec<TestMapTriggerFactory,
          extends<Object>  >
        {
        friend class factory<TestMapTriggerFactory>;

        protected:
            TestMapTriggerFactory()
               {}

        public:
            static MapTriggerListener::Handle createTriggerListener(String::View vsCacheName)
                {
                if (vsCacheName->equals("dist-extend-trigger-listener"))
                    {
                    return MapTriggerListener::create(SimplePersonMapTrigger::create());
                    }
                throw IllegalArgumentException::create(COH_TO_STRING("Unknown cache name " << vsCacheName));
                }
        };
COH_REGISTER_TYPED_CLASS(TestMapTriggerFactory);
COH_REGISTER_METHOD(TestMapTriggerFactory, COH_STATIC_METHOD1(MapTriggerListener::Handle, TestMapTriggerFactory, createTriggerListener, String::View));
    }

/**
* A functional tests for a Coherence*Extend client that uses
* configured trigger listeners.
*
* @author par  2013.09.23
*
* @since Coherence 12.1.3
*/
class MapTriggerListenerTest : public CxxTest::TestSuite
    {
    // ----- Test Methods -----------------------------------------------------

    public:
        /**
        * Test configured MapTrigger operation.
        */
        void testConfiguredTriggerListener()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-extend-trigger-listener",
                    "defaultconfig/extend-maptrigger-cache-config.xml");

            TS_ASSERT(hCache->isEmpty());

            ArrayList::Handle hasChildrenIds = ArrayList::create(1);
            hasChildrenIds->add(String::create("456-78-9123"));
            SimplePerson::View vIn = SimplePerson::create(String::create("123-45-6789"),
                    String::create("Eddie"), String::create("Van Halen"), 1955, 
                    String::create("987-65-4321"), hasChildrenIds);

            Integer32::View vKey = Integer32::valueOf(1);
            hCache->put(vKey, vIn);
            Thread::sleep(1000);

            TS_ASSERT(hCache->containsKey(vKey));

            SimplePerson::View hOut = cast<SimplePerson::View>(hCache->get(vKey));

            CacheFactory::destroyCache(hCache);

            // restore the default cache configure
            setDefaultCacheConfigure();

            TS_ASSERT(hOut->getLastName()->equals(XmlHelper::toUpperCase(vIn->getLastName())));
            }
    };
