/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"

#include "coherence/util/Filter.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/FilterTrigger.hpp"

#include "private/coherence/component/net/extend/protocol/cache/ListenerFilterRequest.hpp"

using coherence::component::net::extend::protocol::cache::ListenerFilterRequest;
using coherence::util::Filter;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::FilterTrigger;

using namespace std;
using namespace mock;

class ListenerFilterRequestTest : public CxxTest::TestSuite
    {
    public:

        void testGetTypeId()
            {
            ListenerFilterRequest::View vRequest = ListenerFilterRequest::create();
            TS_ASSERT(12 == vRequest->getTypeId());
            }

        void testAddProperty()
            {
            ListenerFilterRequest::Handle hRequest = ListenerFilterRequest::create();
            TS_ASSERT(!hRequest->isAdd());
            hRequest->setAdd(true);
            TS_ASSERT(hRequest->isAdd());
            }

        void testLiteProperty()
            {
            ListenerFilterRequest::Handle hRequest = ListenerFilterRequest::create();
            TS_ASSERT(!hRequest->isLite());
            hRequest->setLite(true);
            TS_ASSERT(hRequest->isLite());
            }

        void testTriggerProperty()
            {
            ListenerFilterRequest::Handle hRequest = ListenerFilterRequest::create();
            TS_ASSERT(hRequest->getTrigger() == NULL);

            FilterTrigger::Handle hTrigger = FilterTrigger::create();
            hRequest->setTrigger(hTrigger);
            TS_ASSERT(hRequest->getTrigger() == hTrigger);
            }

        void testFilterProperty()
            {
            ListenerFilterRequest::Handle hRequest = ListenerFilterRequest::create();
            TS_ASSERT(hRequest->getFilter() == NULL);

            Filter::View vFilter = MockFilter::create();
            hRequest->setFilter(vFilter);
            TS_ASSERT(hRequest->getFilter() == vFilter);
            }

        void testFilterIdProperty()
            {
            ListenerFilterRequest::Handle hRequest = ListenerFilterRequest::create();
            TS_ASSERT(hRequest->getFilterId() == 0);

            hRequest->setFilterId(5);
            TS_ASSERT(hRequest->getFilterId() == 5);
            }

        void testIdProperty()
            {
            ListenerFilterRequest::Handle hRequest = ListenerFilterRequest::create();
            TS_ASSERT(hRequest->getId() == 0);

            hRequest->setId(1);
            TS_ASSERT(hRequest->getId() == 1);
            }

        void testOutputDescription()
            {
            ListenerFilterRequest::Handle hRequest = ListenerFilterRequest::create();
            hRequest->setLite(true);
            hRequest->setAdd(true);
            hRequest->setTrigger(FilterTrigger::create());
            hRequest->setFilter(AlwaysFilter::create());
            hRequest->setFilterId(1000);
            hRequest->setId(5);

            stringstream ss;
            ss << hRequest->getDescription();
            TS_ASSERT(string::npos != ss.str().find(
                "Type=12, Channel=NULL, Id=5, Status=NULL, Filter=AlwaysFilter, FilterId=1000, Add=1, Lite=1, Trigger=FilterTrigger(NULL, action_rollback)"));
            }


        //TODO
//      void testReadWriteExternal()
//          {
//          }

    };
