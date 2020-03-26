/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/pof/PortableObjectSerializer.hpp"
#include "coherence/io/pof/SimplePofContext.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/Member.hpp"
#include "coherence/tests/IncrementorInvocable.hpp"
#include "coherence/tests/MBeanInvocable.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/processor/ConditionalPut.hpp"

#include "common/PortablePerson.hpp"

using namespace coherence::lang;

using coherence::io::pof::PortableObjectSerializer;
using coherence::io::pof::SimplePofContext;
using coherence::net::CacheFactory;
using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::InvocationService;
using coherence::net::Member;
using coherence::tests::IncrementorInvocable;
using coherence::tests::MBeanInvocable;
using coherence::util::Map;
using coherence::util::processor::ConditionalPut;

using common::test::PortablePerson;

/**
* InvocationService Test Suite.
*/
class InvocationServiceTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test query().
        */
        void testQuery()
            {
            InvocationService::Handle hService = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpInvocationService"));

            Member::View vMember =
                    cast<DefaultConfigurableCacheFactory::Handle>
                    (CacheFactory::getConfigurableCacheFactory())
                    ->ensureOperationalContext()->getLocalMember();

            IncrementorInvocable::Handle hInvocable = IncrementorInvocable::create(1);

            Map::View vMap = hService->query(hInvocable, NULL);
            TS_ASSERT(vMap->size() == 1);

            Integer32::View vI = cast<Integer32::View>(vMap->get(vMember));
            TS_ASSERT(vI->getInt32Value() == 2);
            }

    /**
    * Test MBean query().
    */
    void testMBeanQuery()
        {
        InvocationService::Handle hService = cast<InvocationService::Handle>
                (CacheFactory::getService("ExtendTcpInvocationService"));

        Member::View vMember =
                cast<DefaultConfigurableCacheFactory::Handle>
                (CacheFactory::getConfigurableCacheFactory())
                ->ensureOperationalContext()->getLocalMember();

        MBeanInvocable::Handle hInvocable = MBeanInvocable::create();
        Map::View              vMap       = hService->query(hInvocable, NULL);
        TS_ASSERT(vMap->size() == 1);

        String::View vs = cast<String::View>(vMap->get(vMember));
        TS_ASSERT(vs->length() > 0);
        }

    /**
    * Test POF circular references.
    */
    void testPofCircularReference()
        {
        CacheService::Handle hService    = cast<CacheService::Handle>
                (CacheFactory::getService("ExtendTcpCacheService"));
        Serializer::View     vSerializer = hService->getSerializer();
        if (instanceof<SimplePofContext::View>(vSerializer))
            {
            cast<SimplePofContext::View>(vSerializer)->setReferenceEnabled(true);
            }

        NamedCache::Handle hCache = hService->ensureCache("dist-extend-direct");

        PortablePerson::Handle hJoe  = PortablePerson::create(
                    String::create("Joe Smith"), 30);
        PortablePerson::Handle hJane = PortablePerson::create(
                    String::create("Jane Smith"), 28);
        hJoe->setSpouse(hJane);
        hJane->setSpouse(hJoe);

        hCache->put(Integer32::create(1), hJoe);
        hCache->invoke(Integer32::create(1), ConditionalPut::create(AlwaysFilter::create(), hJoe, false));

        if (instanceof<SimplePofContext::View>(vSerializer))
            {
            cast<SimplePofContext::View>(vSerializer)->setReferenceEnabled(false);
            }
        }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }
    };
