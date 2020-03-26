/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

using namespace coherence::lang;

COH_OPEN_NAMESPACE_ANON(FinalHolderTest)

class FinalHolderHolder
    : public class_spec<FinalHolderHolder>
    {
    friend class factory<FinalHolderHolder>;

    protected:
        FinalHolderHolder()
            : COH_NO_WARN(m_oh1(self(), (Object::View) NULL))
            {
            }

        FinalHolderHolder(Object::View ohObj)
            : COH_NO_WARN(m_oh1(self(), ohObj))
            {
            }

    public:
        FinalHolder<Object> m_oh1;
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test Suite for the FinalHolder.
*/
class FinalHolderTest : public CxxTest::TestSuite, virtual Object
    {
    public:

    void testInitialize()
        {
        Object::Holder ohObj     = Object::Holder(Object::create());
        Object::Holder ohObjView = Object::Holder(Object::View(Object::create()));
        FinalHolderHolder::Handle hHolder;

        // positive tests
        hHolder = FinalHolderHolder::create();
        TS_ASSERT(NULL == hHolder->m_oh1);
        initialize(hHolder->m_oh1, ohObj);
        TS_ASSERT(ohObj == hHolder->m_oh1);

        hHolder = FinalHolderHolder::create();
        TS_ASSERT(NULL == hHolder->m_oh1);
        initialize(hHolder->m_oh1, (Object::Holder) NULL);
        TS_ASSERT(NULL == hHolder->m_oh1);
        initialize(hHolder->m_oh1, (Object::Holder) NULL);
        TS_ASSERT(NULL == hHolder->m_oh1);
        initialize(hHolder->m_oh1, ohObj);
        TS_ASSERT(ohObj == hHolder->m_oh1);

        // negative tests that multiple initializations are disallowed
        hHolder = FinalHolderHolder::create();
        TS_ASSERT(NULL == hHolder->m_oh1);
        initialize(hHolder->m_oh1, ohObj);
        TS_ASSERT(ohObj == hHolder->m_oh1);
        try
            {
            initialize(hHolder->m_oh1, ohObj);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        hHolder = FinalHolderHolder::create(ohObj);
        TS_ASSERT(ohObj == hHolder->m_oh1);
        try
            {
            initialize(hHolder->m_oh1, ohObj);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        hHolder = FinalHolderHolder::create(ohObj);
        TS_ASSERT(ohObj == hHolder->m_oh1);
        try
            {
            initialize(hHolder->m_oh1, (Object::Holder) NULL);
            TS_ASSERT(false);
            }
        catch (IllegalStateException::View)
            {
            }

        // test that escaped FinalHolder is still initializable
        hHolder = FinalHolderHolder::create();
        TS_ASSERT(NULL == hHolder->m_oh1);
        MemberHandle<Object> h(System::common(), hHolder);
        initialize(hHolder->m_oh1, ohObj);

        // test that "view-ness" is transferred by initialize()
        Object::Handle hObjTemp = NULL;
        Object::View   vObjTemp = NULL;
        hHolder = FinalHolderHolder::create();
        TS_ASSERT(NULL == hHolder->m_oh1);
        initialize(hHolder->m_oh1, ohObj);
        hObjTemp = cast<Object::Handle>(hHolder->m_oh1);
        vObjTemp = hHolder->m_oh1;
        TS_ASSERT(hObjTemp == hHolder->m_oh1);
        TS_ASSERT(hObjTemp == vObjTemp);
        TS_ASSERT(hObjTemp == ohObj);

        hHolder = FinalHolderHolder::create();
        TS_ASSERT(NULL == hHolder->m_oh1);
        initialize(hHolder->m_oh1, ohObjView);
        try
            {
            hObjTemp = cast<Object::Handle>(hHolder->m_oh1);
            TS_ASSERT(false);
            }
        catch (ConstCastException::View)
            {
            }
        vObjTemp = hHolder->m_oh1;
        TS_ASSERT(vObjTemp == hHolder->m_oh1);
        TS_ASSERT(vObjTemp == ohObjView);
        }
    };
