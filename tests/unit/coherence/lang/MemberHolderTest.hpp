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
* Test Suite for the MemberHolder.
*/
class MemberHolderTest : public CxxTest::TestSuite, public Object
    {
    public:

        void testAssignObjectView()
            {
            Object::View v  = Boolean::create(true);
            MemberHolder<Object> mh(self());
            Object::Holder oh = mh;
            oh = mh;
            mh = v;
            Object::View v2 = mh;
            TS_ASSERT(v2 == v);
            }

        void testSimpleAssignObjectHandle()
            {
            Object::Handle h = Boolean::create(true);
            MemberHolder<Object> mh(self());
            mh = h;
            TS_ASSERT(Object::View(mh) != NULL);
            Object::Handle h3 = cast<Object::Handle>(mh);
            TS_ASSERT(h3 != NULL);
            TS_ASSERT(h3 == h);
            }

        void testAssignObjectHandle()
            {
            Object::Handle h  = Boolean::create(true);
            MemberHolder<Object> mh(self());
            mh = h;
            Object::Handle h2 = cast<Object::Handle>(mh);
            TS_ASSERT(h2 == h);
            }

        void testAssignTypedView()
            {
            Boolean::View vBool  = Boolean::create(true);
            MemberHolder<Object>  mhObj(self());
            mhObj = vBool;
            Boolean::View vBool2 = cast<Boolean::View>(mhObj);
            TS_ASSERT(vBool2 == vBool);

            MemberHolder<Boolean> mhBool(self());
            mhBool = vBool;
            TS_ASSERT(mhBool == vBool);

            vBool = mhBool;
            TS_ASSERT(vBool == mhBool);
            }

        void testAssignTypedHandle()
            {
            Boolean::Handle hBool  = Boolean::create(true);
            MemberHolder<Object>  mhBool(self());
            mhBool = hBool;
            Boolean::Handle hBool2 = cast<Boolean::Handle>(mhBool);
            TS_ASSERT(hBool2 == hBool);
            }

        void testDissallowUpgradeView()
            {
            Object::View v = Boolean::create(true);
            MemberHolder<Object>  mh(self());
            mh = v;
            Object::View v2 = mh;
            TS_ASSERT(v2 == v);
            TS_ASSERT(mh != NULL);
            TS_ASSERT(mh == v);

            try
                {
                cast<Object::Handle>(mh);
                TS_ASSERT(false);
                }
            catch (ConstCastException::View)
                {
                }
            }

        void testDowngradeHandle()
            {
            Object::Handle h = Boolean::create(true);
            MemberHolder<Object> mh(self());
            mh = h;
            Object::View v = mh;
            Object::View v1 = h;
            TS_ASSERT(v1 == v);

            MemberHolder<Boolean> mhb(self());
            mhb = cast<Boolean::Handle>(h);
            TS_ASSERT(mhb == h);
            TS_ASSERT(h == mhb);
            TS_ASSERT(mh == mhb);

            Boolean::Holder thb = mhb;
            mhb = thb;
            }

        void testAssignNULL()
            {
            MemberHolder<Object> mh(self());
            mh = (Object::View) NULL;
            TS_ASSERT(NULL == mh);
            mh = (Object::Handle) NULL;
            TS_ASSERT(mh == NULL);
            clear_handle(mh);
            TS_ASSERT(mh == NULL);
            mh = NULL;
            TS_ASSERT(NULL == mh);
            }

        void testDerivedAssignement()
            {
            Boolean::Handle hBool = Boolean::create(true);
            Boolean::View   vBool = Boolean::create(true);
            Integer32::View vInt  = Integer32::create(123);

            MemberHolder<Object> oh(self());

            oh = hBool;
            TS_ASSERT(oh == hBool);
            TS_ASSERT(hBool == oh);

            oh = vInt;
            TS_ASSERT(oh == vInt);
            TS_ASSERT(vInt == oh);
            }

        void testReverseAssignment()
            {
            Object::View v;
            Boolean::Handle      hBool  = Boolean::create(true);
            Boolean::View        vBool  = hBool;
            MemberHolder<Boolean> thBool(self(), hBool);
            MemberHolder<Object>  oh(self());

            TS_ASSERT(NULL != thBool);
            TS_ASSERT(thBool != NULL);

            TS_ASSERT(thBool == hBool);
            TS_ASSERT(thBool == vBool);
            TS_ASSERT(vBool  == thBool);
            TS_ASSERT(thBool == hBool);
            TS_ASSERT(hBool  == cast<Boolean::Handle>(thBool));
            TS_ASSERT(thBool == thBool);
            TS_ASSERT(thBool != oh);
            TS_ASSERT(oh     != thBool);

            vBool = thBool;
            hBool = cast<Boolean::Handle>(thBool);
            clear_handle(oh);
            thBool  = NULL;
            oh    = thBool;
            v     = thBool;
            v     = NULL;
            oh    = NULL;
            }

        void testTypedHolderCompatibility()
            {
            MemberHolder<Boolean> mhb(self(), Boolean::create(true));
            Boolean::Holder thb = mhb;
            MemberHolder<Object> mho(self(), thb);

            TS_ASSERT(mhb == thb);
            TS_ASSERT(mhb == mho);
            TS_ASSERT(mho == thb);
            }
    };
