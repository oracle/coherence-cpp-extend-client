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
* Test Suite for the Array object.
*/
class ArrayTest : public CxxTest::TestSuite
    {
    public:
        void testSubArrayHandle()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(10);
            for (size32_t i = 0, c = hab->length; i < c; ++i)
                {
                hab[i] = (octet_t) i;
                }

            Array<octet_t>::Handle habSub = hab->subArray(1,3);
            for (size32_t i = 0, c = habSub->length; i < c; ++i)
                {
                TS_ASSERT(habSub[i] == (octet_t) i + 1);
                }

            hab[1] = 42;
            TS_ASSERT(habSub[0] == 42);
            habSub[0] = 1;
            TS_ASSERT(hab[1] == 1);

            try
                {
                habSub[habSub->length] = 1;
                TS_ASSERT(false);
                }
            catch (IndexOutOfBoundsException::View)
                {
                }
            }

        void testSubArrayView()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(10);
            for (size32_t i = 0, c = hab->length; i < c; ++i)
                {
                hab[i] = (octet_t) i;
                }

            Array<octet_t>::View vabSub = ((Array<octet_t>::View) hab)->subArray(1,3);
            for (size32_t i = 0, c = vabSub->length; i < c; ++i)
                {
                TS_ASSERT(vabSub[i] == (octet_t) i + 1);
                }

            hab[1] = 42;
            TS_ASSERT(vabSub[0] == 42);

            TS_ASSERT(vabSub->isImmutable() == false);
            hab = NULL;
            TS_ASSERT(vabSub->isImmutable());
            }

        void testRegionMatches()
            {
            Array<int32_t>::Handle han = Array<int32_t>::create(10);
            for (int32_t i = 0; i < 10; ++i)
                {
                han[i] = i;
                }

            TS_ASSERT(han->regionMatches(0, han));
            TS_ASSERT(han->regionMatches(0, han, 0, 10));
            TS_ASSERT(han->regionMatches(1, han, 1, 9));
            TS_ASSERT(!han->regionMatches(0, han, 1, 9));
            TS_ASSERT_THROWS(han->regionMatches(10, han, 0,  10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(han->regionMatches(0,  han, 11, 10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(han->regionMatches(0,  han, 1,  10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(han->regionMatches(0,  han, 0,  11), IndexOutOfBoundsException::View);

            // Test octet_t's optimized regionMatches (uses memcmp)
            Array<octet_t>::Handle hab = Array<octet_t>::create(10);
            for (size32_t i = 0; i < 10; ++i)
                {
                hab[i] = i;
                }

            TS_ASSERT(hab->regionMatches(0, hab));
            TS_ASSERT(hab->regionMatches(0, hab, 0, 10));
            TS_ASSERT(hab->regionMatches(1, hab, 1, 9));
            TS_ASSERT(!hab->regionMatches(0, hab, 1, 9));
            TS_ASSERT_THROWS(hab->regionMatches(10, hab, 0,  10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hab->regionMatches(0,  hab, 11, 10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hab->regionMatches(0,  hab, 1,  10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hab->regionMatches(0,  hab, 0,  11), IndexOutOfBoundsException::View);
            }

        void testEmptyImmutability()
            {
            Array<int32_t>::Handle han = Array<int32_t>::create(0);
            TS_ASSERT(han->isImmutable());

            ObjectArray::Handle hao = ObjectArray::create(0);
            TS_ASSERT(hao->isImmutable());
            }

        void testClone()
            {
            Array<int32_t>::Handle han = Array<int32_t>::create(10);
            for (int32_t i = 0; i < 10; ++i)
                {
                han[i] = i;
                }
            Array<int32_t>::Handle hanc = cast<Array<int32_t>::Handle>(han->clone());
            TS_ASSERT(hanc != han);
            TS_ASSERT(hanc->equals(han));
            }

        void testHolderAssign()
            {
            Array<int32_t>::Handle han = Array<int32_t>::create(10);
            Array<int32_t>::Holder ohan = han;
            Object::Holder oh = han;
            }

        void testEscape()
            {
            ObjectArray::Handle ha = ObjectArray::create(10);
            for (size32_t i = 0, c = ha->length; i < c; ++i)
                {
                ha[i] = Integer32::create((int32_t) i);
                }

            TS_ASSERT(!ha->_isEscaped());
            TS_ASSERT(!ha[0]->_isEscaped());
            TS_ASSERT(&get_guardian(ha[0]) == get_pointer(ha));
            TS_ASSERT(!ha[9]->_isEscaped());
            TS_ASSERT(&get_guardian(ha[9]) == get_pointer(ha));

            // escape the array
            MemberHandle<Object> h(System::common(), ha);

            TS_ASSERT(ha[0]->_isEscaped());
            TS_ASSERT(ha->_isEscaped());
            TS_ASSERT(&get_guardian(ha[0]) != get_pointer(ha));
            TS_ASSERT(ha[9]->_isEscaped());
            TS_ASSERT(&get_guardian(ha[9]) != get_pointer(ha));

            // recapture array
            h = NULL;

            TS_ASSERT(!ha->_isEscaped());
            TS_ASSERT(!ha[0]->_isEscaped());
            TS_ASSERT(&get_guardian(ha[0]) == get_pointer(ha));
            TS_ASSERT(!ha[9]->_isEscaped());
            TS_ASSERT(&get_guardian(ha[9]) == get_pointer(ha));
            }

        void testArrayHolder()
            {
            ObjectArray::Handle ha = ObjectArray::create(10);
            ObjectArray::Holder oha = ha;

            ha[0] = Object::create();
            ha[9] = Object::create();

            TS_ASSERT(oha[0] != NULL);
            TS_ASSERT(oha[1] == NULL);
            TS_ASSERT(oha[9] != NULL);
            ObjectArray::View   va = oha;

            TS_ASSERT(oha[0] == va[0]);
            TS_ASSERT(oha[9] == va[9]);

            oha = va;

            TS_ASSERT(oha[0] == va[0]);
            TS_ASSERT(oha[9] == va[9]);

            va = oha;

            TS_ASSERT(oha[0] == va[0]);
            TS_ASSERT(oha[9] == va[9]);
            }

        void testRawHashHelper()
            {
            std::string sHello("hello world");
            std::string sHello2("hello world");
            std::string sBye   ("bye   world");
            TS_ASSERT(Array<char>::hashCode(sHello.data(), (size32_t) sHello.size()) ==
                      Array<char>::hashCode(sHello2.data(), (size32_t) sHello2.size()));

            TS_ASSERT(Array<char>::hashCode(sHello.data(), (size32_t) sHello.size()) !=
                      Array<char>::hashCode(sBye.data(), (size32_t) sBye.size()));
            }

        };
