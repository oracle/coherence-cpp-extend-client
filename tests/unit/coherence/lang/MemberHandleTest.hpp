/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

using namespace coherence::lang;

COH_OPEN_NAMESPACE_ANON(MemberHandleTest)

class MemberHandleFlipper
    : public class_spec<MemberHandleFlipper>
    {
    friend class factory<MemberHandleFlipper>;

    protected:
        MemberHandleFlipper(Object::Handle h1, Object::Handle h2)
            : COH_NO_WARN(m_h1(self(), h1)), COH_NO_WARN(m_h2(self(), h2))
            {
            }

    public:
        MemberHandle<Object> m_h1;
        MemberHandle<Object> m_h2;
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test Suite for the MemberHandle.
*/
class MemberHandleTest : public CxxTest::TestSuite, virtual Object
    {
    public:

    void testChainedAutoFlip()
        {
        Object::Handle h1 = Object::create();
        Object::Handle h2 = Object::create();
        Object::View   v1 = h1;
        Object::View   v2 = h2;
        MemberHandleFlipper::Handle hFlip = MemberHandleFlipper::create(h1, h2);
        MemberHandleFlipper::View vFlip = hFlip;

        TS_ASSERT(h1->isImmutable() == false);
        TS_ASSERT(h2->isImmutable() == false);
        TS_ASSERT(v1->isImmutable() == false);
        TS_ASSERT(v2->isImmutable() == false);
        TS_ASSERT(hFlip->isImmutable() == false);
        TS_ASSERT(hFlip->m_h1 == h1);
        TS_ASSERT(v1 == h1);
        TS_ASSERT(hFlip->m_h2 == h2);
        TS_ASSERT(v2 == h2);
        TS_ASSERT(hFlip == vFlip);
        TS_ASSERT(v1 != v2);
        TS_ASSERT(h1 != h2);

        hFlip = NULL;

        // if auto-flip worked h1, h2 are noew the only things keeping v1, v2
        // from being immutable

        TS_ASSERT(v1->isImmutable() == false);
        h1 = NULL;
        TS_ASSERT(v1->isImmutable());

        TS_ASSERT(v2->isImmutable() == false);
        h2 = NULL;
        TS_ASSERT(v2->isImmutable());
        }

    void testExternalSynchronization()
        {
        Boolean::Handle hTrue  = Boolean::create(true);
        Boolean::Handle hFalse = Boolean::create(false);
        MemberHandle<Boolean> mhBool(self(), hTrue);

        COH_SYNCHRONIZED_MEMBER_WRITE
            {
            if (coh_synchronized_member_write.getMember(mhBool) == hTrue)
                {
                coh_synchronized_member_write.setMember(mhBool, hFalse);
                }
            }

        TS_ASSERT(mhBool == hFalse);

        COH_SYNCHRONIZED_MEMBER_WRITE
            {
            coh_synchronized_member_write.updateMember(mhBool, hFalse, hTrue);
            }

        TS_ASSERT(mhBool == hTrue);

        COH_SYNCHRONIZED_MEMBER_READ
            {
            TS_ASSERT(coh_synchronized_member_read.getMember(mhBool) == hTrue);
            }
        }

    void testExternalViewGet()
        {
        Boolean::Handle hTrue  = Boolean::create(true);
        MemberHandle<Boolean> mhBool(self(), hTrue);
        const MemberHandle<Boolean>& cmhBool(mhBool);

        COH_SYNCHRONIZED_MEMBER_READ
            {
            Boolean::View v = coh_synchronized_member_read.getMember(cmhBool);
            TS_ASSERT(v == hTrue);
            v = coh_synchronized_member_read.getMember(mhBool);
            TS_ASSERT(v == hTrue);
            }
        }

    void testWrongObjectLock()
        {
        MemberHandle<Boolean> mhBool(self(), Boolean::create(true));
        Object::Handle hWrong = Object::create();
        MemberHandle<Object> hEscape(System::common(), hWrong);
        MemberHandle<Boolean> mhWrong(*hWrong, Boolean::create(true));
        bool fCaught = false;
        try
            {
            COH_SYNCHRONIZED_MEMBER_READ
                {
                Boolean::Handle h = coh_synchronized_member_read.getMember(mhWrong);
                TS_ASSERT(false);
                }
            }
        catch (IllegalArgumentException::View)
            {
            fCaught = true;
            }
        TS_ASSERT(fCaught);
        }

    void testInterop()
        {
        MemberHandle<Boolean> mhBool(self(), Boolean::create(true));
        Boolean::View         vBool = mhBool;
        const MemberHandle<Boolean> cmhBool(self(), (Boolean::Handle) mhBool);

        TS_ASSERT(vBool == mhBool);
        TS_ASSERT(mhBool == vBool);
        TS_ASSERT(vBool == cmhBool);
        TS_ASSERT(cmhBool == vBool);
        }

    class PassMH
        : public class_spec<PassMH,
            extends<Object> >
        {
        friend class factory<PassMH>;

        public:
            PassMH(Boolean::Holder oh)
                : m_oh(self(), oh)
            {
            TS_ASSERT(instanceof<Boolean::Handle>(oh));
            TS_ASSERT(instanceof<Boolean::Handle>(m_oh));
            }

        MemberHolder<Object> m_oh;
        };

    void testPassMH()
        {
        MemberHandle<Boolean> mhBool(self(), Boolean::create(true));
        PassMH::Handle h = PassMH::create(mhBool);
        }

    void testEscapeCapture()
        {
        Boolean::Handle hFugitive = Boolean::create(true);
        System::assertAttachCount(hFugitive, 1, 0);
        TS_ASSERT(hFugitive->_isEscaped() == false);

        MemberHandle<Boolean> mhEscape(System::common(), hFugitive);
        System::assertAttachCount(hFugitive, 3, 0);

        TS_ASSERT(hFugitive->_isEscaped() == true);

        mhEscape = NULL;
        System::assertAttachCount(hFugitive, 1, 0);

        TS_ASSERT(hFugitive->_isEscaped() == false);

        Object::View vCaptured = hFugitive;
        System::assertAttachCount(hFugitive, 1, 0);
        TS_ASSERT(hFugitive->_isEscaped() == false);

        hFugitive = NULL;

        System::assertAttachCount(vCaptured, 0, 1);
        TS_ASSERT(vCaptured->_isEscaped() == false);
        }
    };
