/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

using namespace coherence::lang;

COH_OPEN_NAMESPACE_ANON(FinalViewTest)

class FinalViewHolder
    : public class_spec<FinalViewHolder>
    {
    friend class factory<FinalViewHolder>;

    protected:
        FinalViewHolder()
            : COH_NO_WARN(m_v1(self(), (Object::View) NULL))
            {
            }

        FinalViewHolder(Object::View vObj)
            : COH_NO_WARN(m_v1(self(), vObj))
            {
            }

    public:
        FinalView<Object> m_v1;
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test Suite for the FinalView.
*/
class FinalViewTest : public CxxTest::TestSuite, virtual Object
    {
    public:

    void testInitialize()
        {
        Object::View vObj = Object::create();
        FinalViewHolder::Handle hHolder;

        // positive tests
        hHolder = FinalViewHolder::create();
        TS_ASSERT(NULL == hHolder->m_v1);
        initialize(hHolder->m_v1, vObj);
        TS_ASSERT(vObj == hHolder->m_v1);

        hHolder = FinalViewHolder::create();
        TS_ASSERT(NULL == hHolder->m_v1);
        initialize(hHolder->m_v1, (Object::View) NULL);
        TS_ASSERT(NULL == hHolder->m_v1);
        initialize(hHolder->m_v1, (Object::View) NULL);
        TS_ASSERT(NULL == hHolder->m_v1);
        initialize(hHolder->m_v1, vObj);
        TS_ASSERT(vObj == hHolder->m_v1);

        // negative tests that multiple initializations are disallowed
        hHolder = FinalViewHolder::create();
        TS_ASSERT(NULL == hHolder->m_v1);
        initialize(hHolder->m_v1, vObj);
        TS_ASSERT(vObj == hHolder->m_v1);
        try
            {
            initialize(hHolder->m_v1, vObj);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        hHolder = FinalViewHolder::create(vObj);
        TS_ASSERT(vObj == hHolder->m_v1);
        try
            {
            initialize(hHolder->m_v1, vObj);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        hHolder = FinalViewHolder::create(vObj);
        TS_ASSERT(vObj == hHolder->m_v1);
        try
            {
            initialize(hHolder->m_v1, (Object::View) NULL);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        // test that escaped FinalView is still initializable
        hHolder = FinalViewHolder::create();
        TS_ASSERT(NULL == hHolder->m_v1);
        MemberHandle<Object> h(System::common(), hHolder);
        initialize(hHolder->m_v1, vObj);
        }
    };
