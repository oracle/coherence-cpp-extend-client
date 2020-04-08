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
* Test Suite for the WeakReferences.
*/
class WeakReferenceTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test Object Count
        */
        void testObjectCount()
            {
            int64_t cObj = System::getHeapAnalyzer()->getObjectCount();
            Boolean::Handle fTest = Boolean::create(true);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 1);
            }

        /**
        * Test Ensure Heap
        */
        void testEnsureHeap()
            {
            COH_ENSURE_HEAP
                {
                Boolean::Handle fTest = Boolean::create(true);
                }
            }

        /**
        * Test Ensure Heap, with a leak
        */
        void testEnsureLeak()
            {
            Boolean::Handle fTest;
            try
                {
                COH_ENSURE_HEAP
                    {
                    fTest = Boolean::create(true);
                    }
                TS_ASSERT(false);
                }
            catch (IllegalStateException::View v)
                {
                // expected
                }
            }

        /**
        * Test Handle Count
        */
        void testHandleCount()
            {
            Boolean::Handle fTest = Boolean::create(true);
            System::assertAttachCount(fTest, 1, 0);
            }

        /**
        * Test View Count
        */
        void testViewCount()
            {
            Boolean::View vTest;
                {
                // stupid solaris holds the temporary for the entire
                // scope, making the next assertion confusing
                vTest = Boolean::create(true);
                }
            System::assertAttachCount(vTest, 0, 1);
            }

        /**
        * Test Handle/View Count
        */
        void testHandleViewCount()
            {
            Boolean::Handle fTest = Boolean::create(true);
            Boolean::View   vTest = fTest;
            System::assertAttachCount(fTest, 1, 0);
            System::assertAttachCount(vTest, 1, 0);
            }


        /**
        * Test retrieving a handle for a live object.
        */
        void testGetLiveHandle()
            {
            Boolean::Handle fTest = Boolean::create(true);
            WeakReference::Handle whfTest = fTest->_attachWeak();
            TS_ASSERT(fTest == cast<Boolean::Handle>(whfTest->get()));
            System::assertAttachCount(fTest, 2, 0);
            }

        /**
        * Test retrieving a handle for a dead object.
        */
        void testGetDeadHandle()
            {
            int64_t cObj = System::getHeapAnalyzer()->getObjectCount();
            Boolean::Handle fTest = Boolean::create(true);
            WeakReference::Handle whfTest = fTest->_attachWeak();
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            System::assertAttachCount(fTest, 2, 0);
            fTest = NULL;
            TS_ASSERT(NULL == cast<Boolean::Handle>(whfTest->get()));
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 1);
            }

        /**
        * Test retrieving a view from a live object view.
        */
        void testGetLiveView()
            {
            int64_t cObj = System::getHeapAnalyzer()->getObjectCount();
            Boolean::View fTest;
                {
                fTest = Boolean::create(true);
                }
            System::assertAttachCount(fTest, 0, 1);
            WeakReference::View wvfTest = fTest->_attachWeak();
            System::assertAttachCount(fTest, 0, 2);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            TS_ASSERT(fTest == cast<Boolean::View>(wvfTest->get()));
            }

        /**
        * Test retrieving a view from a dead object view.
        */
        void testGetDeadView()
            {
            int64_t cObj = System::getHeapAnalyzer()->getObjectCount();
            Boolean::View fTest;
                {
                fTest = Boolean::create(true);
                }
            System::assertAttachCount(fTest, 0, 1);
            WeakReference::View wvfTest = fTest->_attachWeak();
            System::assertAttachCount(fTest, 0, 2);
            fTest = NULL;
            TS_ASSERT(NULL == cast<Boolean::View>(wvfTest->get()));
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 1);
            }
        /**
        * Test retrieving a view from a live object handle.
        */
        void testViewFromHandle()
            {
            int64_t cObj = System::getHeapAnalyzer()->getObjectCount();
            Boolean::Handle fTest = Boolean::create(true);
            Boolean::View   vfTest = fTest;
            WeakReference::View wvfTest = fTest->_attachWeak();
            System::assertAttachCount(fTest, 2, 0);
            TS_ASSERT(vfTest == cast<Boolean::View>(wvfTest->get()));
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            }

        /**
        * Test a weakly reference Object becoming immutable
        */
        void testImmutable()
            {
            int64_t cObj = System::getHeapAnalyzer()->getObjectCount();
            Boolean::Handle fTest = Boolean::create(true);
            Boolean::View   vfTest = fTest;
            WeakReference::Handle whfTest = fTest->_attachWeak();
            System::assertAttachCount(vfTest, 2, 0);
            fTest = NULL;
            System::assertAttachCount(vfTest, 0, 2);
            TS_ASSERT(NULL == cast<Boolean::Handle>(whfTest->get(), false));
            WeakReference::View wvfTest = whfTest;
            TS_ASSERT(vfTest == cast<Boolean::View>(wvfTest->get()));
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            }

        void testEscape()
            {
            HeapAnalyzer::ensureHeap(); // prime
            HeapAnalyzer::Snapshot::View vStart = HeapAnalyzer::ensureHeap();
            Boolean::Handle h = Boolean::create(true);
            System::assertAttachCount(h, 1, 0);
            WeakHandle<Boolean> wh(System::common(), h);
            WeakReference::Handle wr = cast<WeakReference::Handle>(WeakReference::valueOf(h));

            System::assertAttachCount(wr, 5, 0);
            TS_ASSERT(wr->_isEscaped() == true);
            Boolean::View v = h;
            System::assertAttachCount(h, 3, 0);
            h = NULL;
            System::assertAttachCount(v, 0, 3);
            TS_ASSERT(v->_isEscaped() == true);
            v = NULL;
            HeapAnalyzer::ensureHeap(vStart, 1);

            System::assertAttachCount(wr, 3, 0);
            wh = NULL;
            wr = NULL;
            HeapAnalyzer::ensureHeap(vStart, 0);
            }
    };

