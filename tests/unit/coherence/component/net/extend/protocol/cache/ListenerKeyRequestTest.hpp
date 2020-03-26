/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"

#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/filter/FilterTrigger.hpp"

#include <sstream>

#include "private/coherence/component/net/extend/protocol/cache/ListenerKeyRequest.hpp"

using coherence::component::net::extend::protocol::cache::ListenerKeyRequest;
using coherence::util::MapListener;
using coherence::util::filter::FilterTrigger;

using namespace std;
using namespace mock;

class ListenerKeyRequestTest : public CxxTest::TestSuite
    {
    public:

        void testGetTypeId()
            {
            ListenerKeyRequest::View vRequest = ListenerKeyRequest::create();
            TS_ASSERT(11 == vRequest->getTypeId());
            }

        void testAddProperty()
            {
            ListenerKeyRequest::Handle hRequest = ListenerKeyRequest::create();
            TS_ASSERT(!hRequest->isAdd());
            hRequest->setAdd(true);
            TS_ASSERT(hRequest->isAdd());
            }

        void testLiteProperty()
            {
            ListenerKeyRequest::Handle hRequest = ListenerKeyRequest::create();
            TS_ASSERT(!hRequest->isLite());
            hRequest->setLite(true);
            TS_ASSERT(hRequest->isLite());
            }

        void testTriggerProperty()
            {
            ListenerKeyRequest::Handle hRequest = ListenerKeyRequest::create();
            TS_ASSERT(hRequest->getTrigger() == NULL);

            MapTrigger::Handle hTrigger = FilterTrigger::create();
            hRequest->setTrigger(hTrigger);
            TS_ASSERT(hRequest->getTrigger() == hTrigger);
            }

        void testKeyProperty()
            {
            ListenerKeyRequest::Handle hRequest = ListenerKeyRequest::create();
            TS_ASSERT(hRequest->getKey() == NULL);

            String::View vsKey = String::create("key");
            hRequest->setKey(vsKey);
            TS_ASSERT(hRequest->getKey() == vsKey);
            }

        void testIdProperty()
            {
            ListenerKeyRequest::Handle hRequest = ListenerKeyRequest::create();
            TS_ASSERT(hRequest->getId() == 0);

            hRequest->setId(1);
            TS_ASSERT(hRequest->getId() == 1);
            }

        void testOutputDescription()
            {
            ListenerKeyRequest::Handle hRequest = ListenerKeyRequest::create();
            hRequest->setLite(true);
            hRequest->setAdd(true);
            hRequest->setTrigger(FilterTrigger::create());
            hRequest->setKey(String::create("key"));
            hRequest->setId(5);

            stringstream ss;
            ss << hRequest->getDescription();
            TS_ASSERT(string::npos != ss.str().find(
                "Type=11, Channel=NULL, Id=5, Status=NULL, Key=key, Add=1, Lite=1, Trigger=FilterTrigger(NULL, action_rollback)"));
            }

        //TODO
//      void testReadWriteExternal()
//          {
//          }

    };
