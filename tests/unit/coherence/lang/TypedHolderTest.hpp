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
* Test Suite for the Object::Holder.
*/
class TypedHolderTest : public CxxTest::TestSuite
    {
    public:

        // used to ensure normal Handles and Views can do type conversion
        // to Object::Holder
        void helperInHolder(Object::Holder)
            {
            }

        // used to test that Object::Holders as a return type
        Object::Holder helperOutHolder()
            {
            return Object::Holder();
            }

        void testAssignObjectView()
            {
            Object::View v  = Boolean::create(true);
            helperInHolder(v);
            Object::View v2 = helperOutHolder();
            Object::Holder oh(v);
            Object::View v3 = oh;
            TS_ASSERT(v3 == v);
            }

        void testHolderCast()
            {
            String::Handle hs = "hello world";
            Object::Holder oh = hs;
            String::Holder ohs = cast<String::Holder>(oh);
            TS_ASSERT(hs == ohs);
            String::Handle hs2 = cast<String::Handle>(oh);
            TS_ASSERT(hs2 == hs);
            }

        void testSimpleAssignObjectHandle()
            {
            Object::Handle h = Boolean::create(true);
            Object::Holder oh(h);
            TS_ASSERT(Object::View(oh) != NULL);

            Object::Handle h3 = cast<Object::Handle>(oh);
            TS_ASSERT(h3 == h);
            }

        void testAssignObjectHandle()
            {
            Object::Handle h  = Boolean::create(true);
            helperInHolder(h);
            Object::Handle h2 = cast<Object::Handle>(helperOutHolder());
            Object::Holder oh(h);
            Object::Handle h3 = cast<Object::Handle>(oh);
            TS_ASSERT(h3 == h);
            }

        void testAssignTypedView()
            {
            Boolean::View vBool  = Boolean::create(true);
            helperInHolder(vBool);
            Boolean::View vBool2 = cast<Boolean::View>(helperOutHolder());
            Object::Holder  ohBool = vBool;
            Boolean::View vBool3 = cast<Boolean::View>(ohBool);
            TS_ASSERT(vBool3 == vBool);
            }

        void testAssignTypedHandle()
            {
            Boolean::Handle hBool  = Boolean::create(true);
            helperInHolder(hBool);
            Boolean::Handle hBool2 = cast<Boolean::Handle>(helperOutHolder());
            Object::Holder  ohBool = hBool;
            Boolean::Handle hBool3 = cast<Boolean::Handle>(ohBool);
            TS_ASSERT(hBool3 == hBool);
            }

        void testDissallowUpgradeView()
            {
            Object::View v = Boolean::create(true);
            Object::Holder  oh = v;
            Object::View v2 = oh;
            TS_ASSERT(v2 == v);
            TS_ASSERT(oh != NULL);
            TS_ASSERT(oh == v);
            TS_ASSERT(cast<Object::Handle>(oh, false) == NULL);
            TS_ASSERT(instanceof<Object::View>(oh));
            TS_ASSERT(instanceof<Boolean::View>(oh));
            TS_ASSERT(!instanceof<Object::Handle>(oh));
            TS_ASSERT(!instanceof<Boolean::Handle>(oh));
            }

        void testDowngradeHandle()
            {
            Object::Handle h = Boolean::create(true);
            Object::Holder oh = h;
            Object::View v = oh;
            Object::View v1 = h;
            TS_ASSERT(v1 == v);
            }

        void testAssignNULL()
            {
            helperInHolder((Object::View) NULL);
            helperInHolder((Object::Handle) NULL);
            }

        void testDerivedAssignement()
            {
            Boolean::Handle hBool = Boolean::create(true);
            Boolean::View   vBool = Boolean::create(true);
            Integer32::View vInt  = Integer32::create(123);

            Object::Holder oh;

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
            Boolean::Holder      thBool = hBool;
            Object::Holder       oh;

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

        void testBadCast()
            {
            Object::Holder ho = Integer32::create(123);
            try
                {
                cast<String::Handle>(ho);
                TS_ASSERT(false);
                }
            catch (const std::bad_cast&)
                {
                //std::cout << e.what() << std::endl;
                }
            }

        void testBadCastBase()
            {
            Object::Holder ho = Integer32::create(123);
            try
                {
                cast<String::Handle>(ho);
                TS_ASSERT(false);
                }
            catch (const std::exception&)
                {
                //std::cout << e.what() << std::endl;
                }
            }

        void testClassCast()
            {
            Object::Holder ho = Integer32::create(123);
            try
                {
                cast<String::Handle>(ho);
                TS_ASSERT(false);
                }
            catch (ClassCastException::View)
                {
                //std::cout << e << std::endl;
                }
            }


        void testClassCastBase()
            {
            Object::Holder ho = Integer32::create(123);
            try
                {
                cast<String::Handle>(ho);
                TS_ASSERT(false);
                }
            catch (Exception::View)
                {
                //std::cout << e << std::endl;
                }
            }

        void testConstClassCast()
            {
            Object::Holder ho = (Object::View) String::create("hello world");
            try
                {
                String::Handle hs = cast<String::Handle>(ho);
                TS_ASSERT(hs == NULL);
                TS_ASSERT(false);
                }
            catch (ClassCastException::View)
                {
                //std::cout << e << std::endl;
                }
            }

        void testPointerInit()
            {
            Boolean::Handle hB = Boolean::create(true);
            Boolean* pB = get_pointer(hB);
            Boolean::Holder ohB(pB);
            const Boolean* cpB = pB;
            Boolean::Holder ohB2((Boolean::View) cpB);
            ohB2 = NULL;
            }

        void testConstPointerInit()
            {
            Boolean::Handle hB = Boolean::create(true);
            const Boolean* cpB = get_pointer(hB);
            Boolean::Holder ohB(cpB);
            System::assertAttachCount(hB, 1, 1);
            }

        void testNullInit()
            {
            Boolean::Holder ohB(NULL);
            }
    };
