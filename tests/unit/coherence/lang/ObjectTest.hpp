/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include <iostream>

using namespace coherence::lang;

COH_OPEN_NAMESPACE_ANON(ObjectTest)

class TestObject
        : public class_spec<TestObject>
    {
    friend class factory<TestObject>;

    public:
        static View createView()
            {
            return TestObject::create();
            }

        TestObject()
                : fInit(false)
            {
            }

    protected:
        virtual void onInit()
            {
            super::onInit();

            TS_ASSERT(fInit == false);
            Object::Handle hThis = this;
            TS_ASSERT(hThis == this);

            fInit = true;
            }

    public:

        bool fInit;
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test Suite for the Object.
*/
class ObjectTest : public CxxTest::TestSuite
    {
    public:

        void testOnInit()
            {
            TestObject::Handle hTest = TestObject::create();
            TS_ASSERT(hTest->fInit);
            System::assertAttachCount(hTest, 1, 0);

            TestObject::View vTest;
                {
                vTest = TestObject::create();
                }
            TS_ASSERT(vTest->fInit);
            System::assertAttachCount(vTest, 0, 1);

                {
                vTest = TestObject::createView();
                }
            TS_ASSERT(vTest->fInit);
            System::assertAttachCount(vTest, 0, 1);
            }

        void testImmortalityViaHandle()
            {
            Object::Handle h = Object::create();
            System::assertAttachCount(h, 1, 0);

            HeapAnalyzer::Handle hAnalyzer = System::getHeapAnalyzer();
            int64_t cImmortalsPre = hAnalyzer->getImmortalCount();

            // bring us to the edge of immortality
            for (size32_t i = LifeCycle::escaped_increment; i < LifeCycle::max_handles; ++i)
                {
                h->_attach(/*fEscaped*/ false);
                }
            System::assertAttachCount(h, LifeCycle::max_handles - 1, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);

            // validate that we can still decrement
            h->_detach(/*fEscaped*/ false);
            System::assertAttachCount(h, LifeCycle::max_handles - 2, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);

            // make us immortal
            h->_attach(/*fEscaped*/ false);
            h->_attach(/*fEscaped*/ false);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't go higher
            TS_ASSERT(h->_attach(/*fEscaped*/ false) != NULL);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't come back
            h->_detach(/*fEscaped*/ false);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can add views
                {
                Object::View v = get_pointer(h);
                System::assertAttachCount(h, LifeCycle::max_handles, 1);
                }
            // and take them away
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can add weak reference
            WeakReference::Handle wh = h->_attachWeak();
            TS_ASSERT(wh->get() == h);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate we can still escape the object
                {
                TS_ASSERT(!h->_isEscaped());
                MemberHandle<Object> mh(System::common(), h);
                TS_ASSERT(h->_isEscaped());
                System::assertAttachCount(h, LifeCycle::max_handles, 0);
                }
            // ensure nothing changes once we drop the global ref
            TS_ASSERT(h->_isEscaped());
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);
            }

        void testImmortalityViaView()
            {
            Object::Handle h = Object::create();
            Object::View   v = get_pointer(h);
            System::assertAttachCount(v, 1, 1);

            HeapAnalyzer::Handle hAnalyzer = System::getHeapAnalyzer();
            int64_t cImmortalsPre = hAnalyzer->getImmortalCount();

            // bring us to the edge of immortality
            for (size32_t i = LifeCycle::escaped_increment; i < LifeCycle::max_views; ++i)
                {
                v->_attach(/*fEscaped*/ false);
                }
            System::assertAttachCount(v, 1, LifeCycle::max_views - 1);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);


            // validate that we can still decrement
            v->_detach(/*fEscaped*/ false);
            System::assertAttachCount(v, 1, LifeCycle::max_views - 2);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);

            // make us immortal
            v->_attach(/*fEscaped*/ false);
            v->_attach(/*fEscaped*/ false);
            System::assertAttachCount(v, 1, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't go higher
            TS_ASSERT(v->_attach(/*fEscaped*/ false) != NULL);
            System::assertAttachCount(v, 1, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't come back
            v->_detach(/*fEscaped*/ false);
            System::assertAttachCount(v, 1, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can add handles
                {
                Object::Handle h1 = get_pointer(h);
                System::assertAttachCount(v, 2, LifeCycle::max_views);
                }
            // and take them away
            System::assertAttachCount(v, 1, LifeCycle::max_views);

            // validate we can still escape the object
                {
                TS_ASSERT(!v->_isEscaped());
                MemberView<Object> mv(System::common(), v);
                TS_ASSERT(v->_isEscaped());
                System::assertAttachCount(v, 1, LifeCycle::max_views);
                }
            // ensure nothing changes once we drop the global ref
            TS_ASSERT(v->_isEscaped());
            System::assertAttachCount(v, 1, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can add weak reference
            WeakReference::View wv = v->_attachWeak(); // it will be as view
            TS_ASSERT(wv->get() == v);
            // goes to 3 since we're escaped
            System::assertAttachCount(v, 3, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // drop final Handle; retest WeakView
            h = NULL;
            TS_ASSERT(wv->get() == v);
            System::assertAttachCount(v, 0, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);
            }

        void testImmortalityViaEscapedHandle()
            {
            Object::Handle h = Object::create();
            Object::Handle h1 = get_pointer(h);
            System::assertAttachCount(h1, 2, 0);

            HeapAnalyzer::Handle hAnalyzer = System::getHeapAnalyzer();
            int64_t cImmortalsPre = hAnalyzer->getImmortalCount();

            // bring us to the edge of immortality
            for (size32_t i = LifeCycle::escaped_increment;
                i < (LifeCycle::max_handles + 1) / LifeCycle::escaped_increment;
                ++i)
                {
                h->_attach(/*fEscaped*/ true);
                }
            TS_ASSERT(h->_isEscaped());
            System::assertAttachCount(h,
                    LifeCycle::max_handles - 1, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);


            // validate that we can still decrement
            h->_detach(/*fEscaped*/ true);
            System::assertAttachCount(h, LifeCycle::max_handles -
                    (LifeCycle::escaped_increment + 1), 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);

            // make us immortal
            h->_attach(/*fEscaped*/ true);
            h->_attach(/*fEscaped*/ true);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't go higher
            TS_ASSERT(h->_attach(/*fEscaped*/ true) != NULL);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't come back
            h->_detach(/*fEscaped*/ true);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can add views
                {
                MemberView<Object> v(System::common(), h);
                std::cout << __LINE__ << std::endl;
                System::assertAttachCount(h, LifeCycle::max_handles,
                        LifeCycle::escaped_increment);
                }
            // and take them away
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can add weak reference
            WeakReference::Handle wh = h->_attachWeak();
            TS_ASSERT(wh->get() == h);
            System::assertAttachCount(h, LifeCycle::max_handles, 0);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);
            }

        void testImmortalityViaEscapedView()
            {
            Object::View v = Object::create();
            Object::View v1 = get_pointer(v);
            System::assertAttachCount(v1, 0, 2);

            HeapAnalyzer::Handle hAnalyzer = System::getHeapAnalyzer();
            int64_t cImmortalsPre = hAnalyzer->getImmortalCount();

            // bring us to the edge of immortality
            for (size32_t i = LifeCycle::escaped_increment;
                i < (LifeCycle::max_views + 1) / LifeCycle::escaped_increment;
                ++i)
                {
                v->_attach(/*fEscaped*/ true);
                }
            TS_ASSERT(v->_isEscaped());
            System::assertAttachCount(v, 0,
                    LifeCycle::max_views - 1);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);

            // validate that we can still decrement
            v->_detach(/*fEscaped*/ true);
            System::assertAttachCount(v, 0, LifeCycle::max_views -
                    (LifeCycle::escaped_increment + 1));
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre);

            // make us immortal
            v->_attach(/*fEscaped*/ true);
            v->_attach(/*fEscaped*/ true);
            System::assertAttachCount(v, 0, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't go higher
            TS_ASSERT(v->_attach(/*fEscaped*/ true) != NULL);
            System::assertAttachCount(v, 0, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can't come back
            v->_detach(/*fEscaped*/ true);
            System::assertAttachCount(v, 0, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);

            // validate that we can add weak reference
            WeakReference::View wv = v->_attachWeak();
            TS_ASSERT(wv->get() == v);
            System::assertAttachCount(v, 0, LifeCycle::max_views);
            TS_ASSERT(hAnalyzer->getImmortalCount() == cImmortalsPre + 1);
            }

        void testSizeOf()
            {
            Object::View        v  = TestObject::create();
            ObjectArray::Handle ha = ObjectArray::create(10);

            size64_t cbV     = v->sizeOf();
            size64_t cbVDeep = v->sizeOf(/*fDeep*/ true);
            TS_ASSERT(cbV == cbVDeep);

            size64_t cbha1 = ha->sizeOf(/*fDeep*/ true);
            ha[0] = v;
            size64_t cbha2 = ha->sizeOf(/*fDeep*/ true);
            TS_ASSERT(cbha1 < cbha2);

            for (size32_t i = 0, c = ha->length; i < c; ++i)
                {
                ha[i] = v;
                }
            size64_t cbha3 = ha->sizeOf(/*fDeep*/ true);

            TS_ASSERT(cbha2 == cbha3); // all of ha refers to v so cost shouldn't increase

            for (size32_t i = 0, c = ha->length; i < c; ++i)
                {
                ha[i] = ha; // create cycles
                }
            size64_t cbha4 = ha->sizeOf(/*fDeep*/ true);

            TS_ASSERT(cbha4 == cbha1); // cycle should have no cost

            // test that creating a weak ref to an object has a cost
            WeakView<Object> vw(*ha, v);
            TS_ASSERT(cbV < v->sizeOf(true));
            cbV = v->sizeOf(true);

            // test that syncing on an non-escaped object has no cost
            COH_SYNCHRONIZED (v)
                {
                }
            TS_ASSERT(cbV == v->sizeOf(true));

            // test that escaping an object has no cost
            MemberView<Object> vm(System::common(), v);
            TS_ASSERT(cbV == v->sizeOf(true));
            cbV = v->sizeOf(true);

            // test that syncing on an escaped object has a cost
            COH_SYNCHRONIZED (v)
                {
                }
            TS_ASSERT(cbV < v->sizeOf(true));
            cbV = v->sizeOf(true);
            }
    };
