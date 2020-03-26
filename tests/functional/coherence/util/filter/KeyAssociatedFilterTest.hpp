/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/Invocable.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/KeyAssociatedFilter.hpp"
#include "common/TestClasses.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::CacheFactory;
using coherence::net::Invocable;
using coherence::net::NamedCache;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::KeyAssociatedFilter;
using common::test::KAFValidationInvocable;
using common::test::TestItem;
using common::test::TestItemKey;
using common::test::TestOrder;
using common::test::TestOrderKey;


/**
* Test Suite for the KeyAssociatedFilter.
*/
class KeyAssociatedFilterSuite : public CxxTest::TestSuite
  {
  public:
        /**
        * Test KeyAssociatedFilter
        */
        void testKeyAssociatedFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            TestOrder::Handle    hOrder1    = TestOrder::create(Integer32::create(123), "CD");
            TestOrderKey::Handle hOrderKey1 = TestOrderKey::create(Integer32::create(123));
            TestItem::Handle     hItem1     = TestItem::create(Integer32::create(1), 500.15f);
            TestItemKey::Handle  hItemKey1  = TestItemKey::create(Integer32::create(1), Integer32::create(123));

            TestOrder::Handle    hOrder2    = TestOrder::create(Integer32::create(124), "DVD");
            TestOrderKey::Handle hOrderKey2 = TestOrderKey::create(Integer32::create(124));
            TestItem::Handle     item2     = TestItem::create(Integer32::create(1), 675.10f);
            TestItemKey::Handle  itemKey2  = TestItemKey::create(Integer32::create(1), Integer32::create(124));

            TestOrder::Handle    order3    = TestOrder::create(Integer32::create(125), "USB");
            TestOrderKey::Handle orderKey3 = TestOrderKey::create(Integer32::create(125));
            TestOrder::Handle    order4    = TestOrder::create(Integer32::create(126), "Monitor");
            TestOrderKey::Handle orderKey4 = TestOrderKey::create(Integer32::create(126));

            //testing on remote cache
            hCache->put(hOrderKey1, hOrder1);
            hCache->put(hItemKey1, hItem1);
            hCache->put(hOrderKey2, hOrder2);
            hCache->put(itemKey2, item2);
            hCache->put(orderKey3, order3);
            hCache->put(orderKey4, order4);

            InvocationService::Handle hService = cast<InvocationService::Handle>(CacheFactory::getService("ExtendTcpInvocationService"));
            TS_ASSERT(hService != NULL);
            TS_ASSERT(instanceof<SafeInvocationService::View>(hService));
            SafeInvocationService::Handle hSafeService = cast<SafeInvocationService::Handle>(hService);

            Filter::Handle filter = KeyAssociatedFilter::create(AlwaysFilter::getInstance(), hItemKey1);
            Set::View keys = hCache->keySet(filter);

            // validating that all keyassociated keys have the same partitionID
            ObjectArray::Handle hKafTaskObjects = ObjectArray::create(6);
            hKafTaskObjects[0] = hOrderKey1;
            hKafTaskObjects[1] = hItemKey1;
            hKafTaskObjects[2] = hOrderKey2;
            hKafTaskObjects[3] = itemKey2;
            hKafTaskObjects[4] = orderKey3;
            hKafTaskObjects[5] = orderKey4;
                                       
            Invocable::Handle hKafTask = KAFValidationInvocable::create(hKafTaskObjects);

            Map::View vResults = hSafeService->query(hKafTask, NULL);

            TS_ASSERT(vResults != NULL);
            TS_ASSERT(vResults->size() == 1);
            Collection::View vColResults = vResults->values();
            TS_ASSERT(vColResults->size() == 1);
            Iterator::Handle colResultIter = vColResults->iterator();
            TS_ASSERT(colResultIter->hasNext());
            Object::Holder ohResult = colResultIter->next();
            Array<int32_t>::Handle oahResult = cast<Array<int32_t>::Handle>(ohResult);
            TS_ASSERT(oahResult->length == 6);

            //hOrderKey1 and hItemKey1 should have the same partitionID
            TS_ASSERT(oahResult[0] == oahResult[1]);

            //hOrderKey2 and itemKey2 should have the same partitionID
            TS_ASSERT(oahResult[2] == oahResult[3]);

            //all the rest should NOT have the same partitionID
            TS_ASSERT(oahResult[0] != oahResult[2]);
            TS_ASSERT(oahResult[0] != oahResult[3]);
            TS_ASSERT(oahResult[1] != oahResult[2]);
            TS_ASSERT(oahResult[1] != oahResult[3]);
            TS_ASSERT(oahResult[4] != oahResult[5]);
            TS_ASSERT(oahResult[4] != oahResult[5]);
            TS_ASSERT(oahResult[0] != oahResult[4]);
            TS_ASSERT(oahResult[0] != oahResult[5]);
            TS_ASSERT(oahResult[1] != oahResult[4]);
            TS_ASSERT(oahResult[1] != oahResult[5]);
            TS_ASSERT(oahResult[2] != oahResult[4]);
            TS_ASSERT(oahResult[2] != oahResult[5]);
            TS_ASSERT(oahResult[3] != oahResult[4]);
            TS_ASSERT(oahResult[3] != oahResult[5]);
            }
  };
