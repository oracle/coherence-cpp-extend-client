/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/lang/WeakHolder.hpp"

using namespace coherence::lang;

/**
* Test Suite for the WeakHolder.
*/
class WeakHolderTest : public CxxTest::TestSuite, public Object
    {
    public:

        void testAssignObjectView()
            {
            Object::View v  = Boolean::create(true);
            WeakHolder<Object> wh(self());
            Object::Holder oh = wh;
            oh = wh;
            wh = v;
            Object::View v2 = wh;
            TS_ASSERT(v2 == v);
            }

        void testSimpleAssignObjectHandle()
            {
            Object::Handle h = Boolean::create(true);
            WeakHolder<Object> wh(self());
            wh = h;
            TS_ASSERT(Object::View(wh) != NULL);
            Object::Handle h3 = cast<Object::Handle>(wh);
            TS_ASSERT(h3 != NULL);
            TS_ASSERT(h3 == h);
            }

        void testAssignObjectHandle()
            {
            Object::Handle h  = Boolean::create(true);
            WeakHolder<Object> wh(self());
            wh = h;
            Object::Handle h2 = cast<Object::Handle>(wh);
            TS_ASSERT(h2 == h);
            }

        void testAssignTypedView()
            {
            Boolean::View vBool  = Boolean::create(true);
            WeakHolder<Object>  whObj(self());
            whObj = vBool;
            Boolean::View vBool2 = cast<Boolean::View>(whObj);
            TS_ASSERT(vBool2 == vBool);

            WeakHolder<Boolean> whBool(self());
            whBool = vBool;
            TS_ASSERT(whBool == vBool);

            vBool = whBool;
            TS_ASSERT(vBool == whBool);
            }

        void testAssignTypedHandle()
            {
            Boolean::Handle hBool  = Boolean::create(true);
            WeakHolder<Object>  whBool(self());
            whBool = hBool;
            Boolean::Handle hBool2 = cast<Boolean::Handle>(whBool);
            TS_ASSERT(hBool2 == hBool);
            }

        void testDissallowUpgradeView()
            {
            Object::View v = Boolean::create(true);
            WeakHolder<Object>  wh(self());
            wh = v;
            Object::View v2 = wh;
            TS_ASSERT(v2 == v);
            TS_ASSERT(wh != NULL);
            TS_ASSERT(wh == v);

            try
                {
                cast<Object::Handle>(wh);
                TS_ASSERT(false);
                }
            catch (ConstCastException::View)
                {
                }
            }

        void testDowngradeHandle()
            {
            Object::Handle h = Boolean::create(true);
            WeakHolder<Object> wh(self());
            wh = h;
            Object::View v = wh;
            Object::View v1 = h;
            TS_ASSERT(v1 == v);

            WeakHolder<Boolean> whb(self());
            whb = cast<Boolean::Handle>(h);
            TS_ASSERT(whb == h);
            TS_ASSERT(h == whb);
            TS_ASSERT(wh == whb);

            Boolean::Holder thb = whb;
            whb = thb;
            }

        void testAssignNULL()
            {
            WeakHolder<Object> wh(self());
            wh = (Object::View) NULL;
            TS_ASSERT(NULL == wh);
            wh = (Object::Handle) NULL;
            TS_ASSERT(wh == NULL);
            clear_handle(wh);
            TS_ASSERT(wh == NULL);
            wh = NULL;
            TS_ASSERT(NULL == wh);
            }

        void testDerivedAssignement()
            {
            Boolean::Handle hBool = Boolean::create(true);
            Boolean::View   vBool = Boolean::create(true);
            Integer32::View vInt  = Integer32::create(123);

            WeakHolder<Object> oh(self());

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
            WeakHolder<Boolean> thBool(self(), hBool);
            WeakHolder<Object>  oh(self());

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
            WeakHolder<Boolean> whb(self(), Boolean::create(true));
            Boolean::Holder thb = whb;
            WeakHolder<Object> who(self(), thb);

            TS_ASSERT(whb == thb);
            TS_ASSERT(whb == who);
            TS_ASSERT(who == thb);
            }

        void testCollect()
            {
            Boolean::Handle hb = Boolean::create(true);
            WeakHolder<Boolean> whb(self(), hb);

            TS_ASSERT(hb == whb);
            TS_ASSERT(whb->getValue() == true);
            hb = NULL;
            TS_ASSERT(whb == NULL);
            }
    };
