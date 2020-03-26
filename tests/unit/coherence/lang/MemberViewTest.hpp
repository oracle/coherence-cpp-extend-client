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
* Test Suite for the MemberView smart pointer.
*/
class MemberViewSuite : public CxxTest::TestSuite, Object
    {
    public:
        typedef MemberView<Boolean> BooleanMember;

        void testInitialization()
            {
            BooleanMember mvfTest(self(), Boolean::valueOf(true));
            TS_ASSERT(mvfTest == Boolean::valueOf(true));

            Boolean::View vfTest = mvfTest;
            BooleanMember mv2(self(), vfTest);
            mv2 = vfTest;
            TS_ASSERT(mv2 == vfTest);

            BooleanMember mv3(self(), mv2);
            TS_ASSERT(mv2 == mv3);
            }

        void testAssignment()
            {
            BooleanMember mvfTest(self());
            mvfTest = Boolean::valueOf(true);
            TS_ASSERT(mvfTest == Boolean::valueOf(true));

            Boolean::View vfTest;
            vfTest = mvfTest;
            BooleanMember mv2(self());
            mv2 = vfTest;
            TS_ASSERT(mv2 == vfTest);

            BooleanMember mv3(self());
            mv3 = mv2;
            TS_ASSERT(mv2 == mv3);
            }

        void testDereference()
            {
            BooleanMember mvfTest(self(), Boolean::valueOf(true));
            TS_ASSERT(NULL != (Boolean::View) mvfTest);
            TS_ASSERT(mvfTest->getValue() == true);
            }

        void testReferenceEquality()
            {
            BooleanMember mvfTest(self(), Boolean::valueOf(true));
            TS_ASSERT(mvfTest == Boolean::valueOf(true));
            TS_ASSERT(mvfTest != Boolean::create(true));
            MemberHandle<Boolean> mhfTest(self(), Boolean::valueOf(true));
            TS_ASSERT(mvfTest == mhfTest);
            }

        void testAttachCount()
            {
            BooleanMember mvfTest(self()); // 1 view
                {
                mvfTest = Boolean::create(true);
                }
            System::assertAttachCount(mvfTest, 0, 1);
            }

        void testEscapedAttachCount()
            {
            BooleanMember mvfTest(System::common());
                {
                mvfTest = Boolean::create(true); // 2 escaped view
                }
            System::assertAttachCount(mvfTest, 0, 3); // +1 view from assert call
            }

        void testNullOut()
            {
            Boolean::View vfTest;
            BooleanMember mvfTest(self());
                {
                vfTest  = Boolean::create(true);
                mvfTest = vfTest;
                }
            System::assertAttachCount(vfTest, 0, 1);
            mvfTest = NULL;
            System::assertAttachCount(vfTest, 0, 1);
            }

        void testEscapedNullOut()
            {
            Boolean::View vfTest;
            BooleanMember mvfTest(System::common());
                {
                vfTest  = Boolean::create(true); // 2 escaped view
                mvfTest = vfTest;
                }
            System::assertAttachCount(vfTest, 0, 3); // +1 view from assert call
            mvfTest = NULL;
            System::assertAttachCount(vfTest, 0, 1);
            }

        void testDerivedAssignement()
            {
            Boolean::Handle hBool = Boolean::create(true);
            Boolean::View   vBool = Boolean::create(true);
            Integer32::View vInt  = Integer32::create(123);

            MemberView<Object> oh(self());

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
            MemberView<Boolean> mvBool(self(), hBool);
            MemberView<Object>  mv(self());
            MemberHandle<Boolean> mhBool(self(), hBool);


            TS_ASSERT(mvBool == mhBool);
            TS_ASSERT(mhBool == mvBool);

            TS_ASSERT(NULL != mvBool);
            TS_ASSERT(mvBool != NULL);

            TS_ASSERT(mvBool == hBool);
            TS_ASSERT(mvBool == vBool);
            TS_ASSERT(vBool  == mvBool);
            TS_ASSERT(mvBool == hBool);
            TS_ASSERT(hBool  == cast<Boolean::View>(mvBool));
            TS_ASSERT(mvBool == mvBool);
            TS_ASSERT(mvBool != mv);
            TS_ASSERT(mv     != mvBool);

            mvBool = mhBool;
            mv     = mhBool;
            vBool = mvBool;
            clear_handle(mv);
            mvBool  = NULL;
            mv    = mvBool;
            v     = mvBool;
            v     = NULL;
            mv    = NULL;
            }

        void testTypedHolderCompatibility()
            {
            MemberView<Boolean> mvb(self(), Boolean::create(true));
            Boolean::Holder thb = mvb;
            MemberView<Object> mvo(self(), thb);

            TS_ASSERT(mvb == thb);
            TS_ASSERT(mvb == mvo);
            TS_ASSERT(mvo == thb);
            }

        void testMemberHolderCompatibility()
            {
            MemberView<Boolean> mvb(self(), Boolean::create(true));
            MemberHolder<Boolean> mhb(self());
            MemberView<Object> mvo(self(), mhb);

            //mhb = mvb; // broken
            mvb = mhb;

            TS_ASSERT(mvb == mhb);
            TS_ASSERT(mvb == mvo);
            TS_ASSERT(mvo == mhb);
            }

        void testExternalSync()
            {
            Boolean::View vTrue  = Boolean::create(true);
            Boolean::View vFalse = Boolean::create(false);
            MemberView<Boolean> mvBool(self(), vTrue);
            COH_SYNCHRONIZED_MEMBER_WRITE
                {
                TS_ASSERT(vTrue == coh_synchronized_member_write.getMember(mvBool));
                coh_synchronized_member_write.setMember(mvBool, vFalse);
                }
            TS_ASSERT(vFalse == mvBool);
            }
    };
