/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/HashMap.hpp"

using namespace coherence::lang;
using namespace coherence::util;
using namespace coherence::io::pof;

/**
* Test Suite for the Object::Holder.
*/
class TypedHandleTest : public CxxTest::TestSuite
    {
    public:
        void testNULLAssign()
            {
            Boolean::Handle h;
            Boolean::Handle h1(NULL);
            Boolean::Handle h2;
            h2 = NULL;

            TS_ASSERT(h == h1);
            TS_ASSERT(h == h2);
            TS_ASSERT(NULL == h);
            TS_ASSERT(h == NULL);
            }

        void testNULLViewAssign()
            {
            Boolean::View v;
            Boolean::View v1(NULL);
            Boolean::View v2;
            v2 = NULL;

            TS_ASSERT(v == v1);
            TS_ASSERT(v == v2);
            TS_ASSERT(NULL == v);
            TS_ASSERT(v == NULL);
            }

        void testNULLAccess()
            {
            Boolean::View v(NULL);

            try
                {
                v->getValue();
                TS_ASSERT(false);
                }
            catch (NullPointerException::View)
                {
                }
            }

        void testDirectAssign()
            {
            Boolean::Handle hBool = Boolean::create(true);
            Boolean::Handle h2(hBool);
            Boolean::Handle h3 = hBool;

            TS_ASSERT(hBool == h2);
            TS_ASSERT(hBool == h3);
            }

        void testDirectViewAssign()
            {
            Boolean::View vBool = Boolean::create(true);
            Boolean::View v2(vBool);
            Boolean::View v3 = vBool;

            TS_ASSERT(vBool == v2);
            TS_ASSERT(vBool == v3);
            }

        void testDerivedAssign()
            {
            Boolean::Handle hb = Boolean::create(true);
            Object::Handle h = hb;

            TS_ASSERT(hb == h);
            TS_ASSERT(h == hb);
            TS_ASSERT(NULL != h);
            }

        void testDerivedViewAssign()
            {
            Boolean::View vb = Boolean::create(true);
            Object::View v = vb;

            TS_ASSERT(vb == v);
            TS_ASSERT(v == vb);
            TS_ASSERT(NULL != v);
            }

        void testLessThan()
            {
            Boolean::Handle hb = Boolean::create(true);
            Boolean::Handle hb2 = Boolean::create(true);

            TS_ASSERT(hb != hb2);
            if (hb < hb2)
                {
                TS_ASSERT((hb2 < hb) == false);
                }
            else
                {
                TS_ASSERT(hb2 < hb);
                }
            }

        void testLessThanView()
            {
            Boolean::View vb = Boolean::create(true);
            Boolean::View vb2 = Boolean::create(true);

            TS_ASSERT(vb != vb2);
            if (vb < vb2)
                {
                TS_ASSERT((vb2 < vb) == false);
                }
            else
                {
                TS_ASSERT(vb2 < vb);
                }
            }

        void testHandleView()
            {
            Boolean::View vTrue = Boolean::create(true);
            Boolean::Handle hTrue = Boolean::create(true);

            TS_ASSERT(vTrue != hTrue);
            Object::equals(vTrue, hTrue);

            vTrue = hTrue;
            TS_ASSERT(vTrue == hTrue);
            TS_ASSERT(hTrue == vTrue);
            }

        //#define BREAK_BUILD
        void testNoBuild()
            {
            #ifdef BREAK_BUILD
            // these checks should not build

            // down assign without cast
            Boolean::View vb = Object::create();

            // assign view to handle
            Boolean::Handle vb2;
            Boolean::Handle hb = vb;
            #endif
            }

        void testCast()
            {
            Object::Handle h1   = String::create("hello world");
            Object::Handle h2   = Boolean::create(true);

            String::Handle hs1  = cast<String::Handle>(h1);
            TS_ASSERT(hs1 == h1);

            String::View   vs1  = cast<String::View>(h1);
            TS_ASSERT(vs1 == h1);

            Boolean::Handle hf1 = cast<Boolean::Handle>(h2);
            TS_ASSERT(hf1 == h2);

            Boolean::View vf1 = cast<Boolean::View>(h2);
            TS_ASSERT(vf1 == h2);
            }

        void testCastTest()
            {
            Object::Handle h1   = String::create("hello world");
            Object::Handle h2   = Boolean::create(true);

            String::Handle hs1  = cast<String::Handle>(h2, false);
            TS_ASSERT(hs1 == NULL);

            String::View   vs1  = cast<String::View>(h2, false);
            TS_ASSERT(vs1 == NULL);

            Boolean::Handle hf1 = cast<Boolean::Handle>(h1, false);
            TS_ASSERT(hf1 == NULL);

            Boolean::View vf1 = cast<Boolean::View>(h1, false);
            TS_ASSERT(vf1 == NULL);
            }

        void testInstanceof()
            {
            Object::Handle h1   = String::create("hello world");
            Object::Handle h2   = Boolean::create(true);

            TS_ASSERT(instanceof<String::Handle>(h1));
            TS_ASSERT(instanceof<String::View>(h1));
            TS_ASSERT(!instanceof<Boolean::Handle>(h1));
            TS_ASSERT(!instanceof<Boolean::View>(h1));

            TS_ASSERT(!instanceof<String::Handle>(h2));
            TS_ASSERT(!instanceof<String::View>(h2));
            TS_ASSERT(instanceof<Boolean::Handle>(h2));
            TS_ASSERT(instanceof<Boolean::View>(h2));
            Boolean::Handle hf; // NULL
            TS_ASSERT(!instanceof<Boolean::Handle>(hf));

            h1 = HashMap::create();
            TS_ASSERT(instanceof<HashMap::Handle>(h1));
            TS_ASSERT(instanceof<AbstractMap::Handle>(h1));
            TS_ASSERT(instanceof<Object::Handle>(h1));
            TS_ASSERT(instanceof<Map::Handle>(h1));
            TS_ASSERT(instanceof<PofIntrinsic::Handle>(h1));
            TS_ASSERT(!instanceof<String::Handle>(h1));
            TS_ASSERT(!instanceof<Collection::Handle>(h1));
            }

        void testBadCast()
            {
            Object::Handle ho = Integer32::create(123);
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
            Object::Handle ho = Integer32::create(123);
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
            Object::Handle ho = Integer32::create(123);
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
            Object::View vo = Integer32::create(123);
            try
                {
                cast<String::View>(vo);
                TS_ASSERT(false);
                }
            catch (Exception::View)
                {
                //std::cout << e << std::endl;
                }
            }
};
