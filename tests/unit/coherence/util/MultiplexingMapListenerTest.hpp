/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "mock/CommonMocks.hpp"

#include "coherence/util/MultiplexingMapListener.hpp"

using coherence::util::HashMap;


class TestMultiplexingMapListener
    : public class_spec<TestMultiplexingMapListener,
        extends<MultiplexingMapListener> >
{
friend class factory<TestMultiplexingMapListener>;

protected:
    TestMultiplexingMapListener(): m_nOnMapEventCallCount(0){}

public:
    virtual void onMapEvent(MapEvent::View /*vEvent*/)
        {
        ++m_nOnMapEventCallCount;
        }
    int32_t m_nOnMapEventCallCount;
};


class MultiplexingMapListenerTest : public CxxTest::TestSuite
    {
    public:

    void testEntryInserted()
        {
        TestMultiplexingMapListener::Handle hListener =
                TestMultiplexingMapListener::create();

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 0 );

        hListener->entryInserted( NULL );

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 1 );

        hListener->entryInserted( NULL );

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 2 );
        }


    void testEntryUpdated()
        {
        TestMultiplexingMapListener::Handle hListener =
                TestMultiplexingMapListener::create();

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 0 );

        hListener->entryUpdated( NULL );

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 1 );

        hListener->entryUpdated( NULL );

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 2 );
        }

    void testEntryDeleted()
        {
        TestMultiplexingMapListener::Handle hListener =
                TestMultiplexingMapListener::create();

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 0 );

        hListener->entryDeleted( NULL );

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 1 );

        hListener->entryDeleted( NULL );

        TS_ASSERT(hListener->m_nOnMapEventCallCount == 2 );
        }

    };


