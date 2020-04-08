/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

using namespace coherence::lang;

COH_OPEN_NAMESPACE_ANON(FinalHandleTest)

class FinalHandleHolder
    : public class_spec<FinalHandleHolder>
    {
    friend class factory<FinalHandleHolder>;

    protected:
        FinalHandleHolder()
            : COH_NO_WARN(m_h1(self(), (Object::Handle) NULL))
            {
            }

        FinalHandleHolder(Object::Handle hObj)
            : COH_NO_WARN(m_h1(self(), hObj))
            {
            }

    public:
        FinalHandle<Object> m_h1;
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test Suite for the FinalHandle.
*/
class FinalHandleTest : public CxxTest::TestSuite, virtual Object
    {
    public:

    void testInitialize()
        {
        Object::Handle hObj = Object::create();
        FinalHandleHolder::Handle hHolder;

        // positive tests
        hHolder = FinalHandleHolder::create();
        TS_ASSERT(NULL == hHolder->m_h1);
        initialize(hHolder->m_h1, hObj);
        TS_ASSERT(hObj == hHolder->m_h1);

        hHolder = FinalHandleHolder::create();
        TS_ASSERT(NULL == hHolder->m_h1);
        initialize(hHolder->m_h1, (Object::Handle) NULL);
        TS_ASSERT(NULL == hHolder->m_h1);
        initialize(hHolder->m_h1, (Object::Handle) NULL);
        TS_ASSERT(NULL == hHolder->m_h1);
        initialize(hHolder->m_h1, hObj);
        TS_ASSERT(hObj == hHolder->m_h1);

        // negative tests that multiple initializations are disallowed
        hHolder = FinalHandleHolder::create();
        TS_ASSERT(NULL == hHolder->m_h1);
        initialize(hHolder->m_h1, hObj);
        TS_ASSERT(hObj == hHolder->m_h1);
        try
            {
            initialize(hHolder->m_h1, hObj);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        hHolder = FinalHandleHolder::create(hObj);
        TS_ASSERT(hObj == hHolder->m_h1);
        try
            {
            initialize(hHolder->m_h1, hObj);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        hHolder = FinalHandleHolder::create(hObj);
        TS_ASSERT(hObj == hHolder->m_h1);
        try
            {
            initialize(hHolder->m_h1, (Object::Handle) NULL);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        // test that escaped FinalHandle is still initializable
        hHolder = FinalHandleHolder::create();
        TS_ASSERT(NULL == hHolder->m_h1);
        MemberHandle<Object> h(System::common(), hHolder);
        initialize(hHolder->m_h1, hObj);
        }
    };
