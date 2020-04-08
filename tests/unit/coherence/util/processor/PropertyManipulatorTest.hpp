/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/processor/PropertyManipulator.hpp"
#include "coherence/util/extractor/ChainedExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"
#include "coherence/util/extractor/CompositeUpdater.hpp"


using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::extractor;

/**
* Test Suite for the PropertyManipulator object.
*/
class PropertyManipulatorSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test getExtractor
        */
        void testGetExtractor()
            {

            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo");


            ValueExtractor::View vExtractor = hPropertyManipulator->getExtractor();

            ReflectionExtractor::View vReflectionExtractor = cast<ReflectionExtractor::View>(vExtractor);

            TS_ASSERT(NULL!=vReflectionExtractor);

            if(NULL!=vReflectionExtractor)
                {
                TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("getfoo")));
                }
            }


        /**
        * Test getExtractor
        */
        void testGetExtractorWithIs()
            {

            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo", true);


            ValueExtractor::View vExtractor = hPropertyManipulator->getExtractor();

            ReflectionExtractor::View vReflectionExtractor = cast<ReflectionExtractor::View>(vExtractor);

            TS_ASSERT(NULL!=vReflectionExtractor);

            if(NULL!=vReflectionExtractor)
                {
                TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("isfoo")));
                }
            }


        /**
        * Test getExtractor
        */
        void testGetExtractorWithCompositeName()
            {

            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("too.foo.bar");


            ValueExtractor::View vExtractor = hPropertyManipulator->getExtractor();

            ChainedExtractor::View vChainedExtractor = cast<ChainedExtractor::View>(vExtractor);

            TS_ASSERT(NULL!=vChainedExtractor);

            if(NULL!=vChainedExtractor)
                {
                ObjectArray::View vaExtractors = vChainedExtractor->getExtractors();

                ReflectionExtractor::View vReflectionExtractor = cast<ReflectionExtractor::View>(vaExtractors[2]);

                TS_ASSERT(NULL!=vReflectionExtractor);

                if(NULL!=vReflectionExtractor)
                    {
                    TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("getbar")));
                    }

                vReflectionExtractor = cast<ReflectionExtractor::View>(vaExtractors[1]);

                TS_ASSERT(NULL!=vReflectionExtractor);

                if(NULL!=vReflectionExtractor)
                    {
                    TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("getfoo")));
                    }

                vReflectionExtractor = cast<ReflectionExtractor::View>(vaExtractors[0]);

                TS_ASSERT(NULL!=vReflectionExtractor);

                if(NULL!=vReflectionExtractor)
                    {
                    TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("gettoo")));
                    }
                }
            }


        /**
        * Test getUpdater
        */
        void testGetUpdater()
            {

            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo");


            ValueUpdater::View vUpdater = hPropertyManipulator->getUpdater();

            ReflectionUpdater::View vReflectionUpdater = cast<ReflectionUpdater::View>(vUpdater);

            TS_ASSERT(NULL!=vReflectionUpdater);

            if(NULL!=vReflectionUpdater)
                {
                TS_ASSERT(vReflectionUpdater->getMethodName()->equals(String::create("setfoo")));
                }
            }

        /**
        * Test getUpdater
        */
        void testGetUpdaterWithCompositeName()
            {

            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo.bar");


            ValueUpdater::View vUpdater = hPropertyManipulator->getUpdater();

            CompositeUpdater::View vCompositeUpdater = cast<CompositeUpdater::View>(vUpdater);

            TS_ASSERT(NULL!=vCompositeUpdater);

            if(NULL!=vCompositeUpdater)
                {
                vUpdater = vCompositeUpdater->getUpdater();

                ReflectionUpdater::View vReflectionUpdater = cast<ReflectionUpdater::View>(vUpdater);

                TS_ASSERT(NULL!=vReflectionUpdater);

                if(NULL!=vReflectionUpdater)
                    {
                    TS_ASSERT(vReflectionUpdater->getMethodName()->equals(String::create("setbar")));
                    }

                ValueExtractor::View vExtractor = vCompositeUpdater->getExtractor();

                ReflectionExtractor::View vReflectionExtractor = cast<ReflectionExtractor::View>(vExtractor);

                TS_ASSERT(NULL!=vReflectionExtractor);

                if(NULL!=vReflectionExtractor)
                    {
                    TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("getfoo")));
                    }
                }
            }


        /**
        * Test getUpdater
        */
        void testGetUpdaterWithCompositeNameChained()
            {

            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("too.foo.bar");


            ValueUpdater::View vUpdater = hPropertyManipulator->getUpdater();

            CompositeUpdater::View vCompositeUpdater = cast<CompositeUpdater::View>(vUpdater);

            TS_ASSERT(NULL!=vCompositeUpdater);

            if(NULL!=vCompositeUpdater)
                {
                vUpdater = vCompositeUpdater->getUpdater();

                ReflectionUpdater::View vReflectionUpdater = cast<ReflectionUpdater::View>(vUpdater);

                TS_ASSERT(NULL!=vReflectionUpdater);

                if(NULL!=vReflectionUpdater)
                    {
                    TS_ASSERT(vReflectionUpdater->getMethodName()->equals(String::create("setbar")));
                    }

                ValueExtractor::View vExtractor = vCompositeUpdater->getExtractor();

                ChainedExtractor::View vChainedExtractor = cast<ChainedExtractor::View>(vExtractor);

                TS_ASSERT(NULL!=vChainedExtractor);

                if(NULL!=vChainedExtractor)
                    {
                    ObjectArray::View vaExtractors = vChainedExtractor->getExtractors();

                    ReflectionExtractor::View vReflectionExtractor =  cast<ReflectionExtractor::View>(vaExtractors[1]);

                    TS_ASSERT(NULL!=vReflectionExtractor);

                    if(NULL!=vReflectionExtractor)
                        {
                        TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("getfoo")));
                        }

                    vReflectionExtractor = cast<ReflectionExtractor::View>(vaExtractors[0]);

                    TS_ASSERT(NULL!=vReflectionExtractor);

                    if(NULL!=vReflectionExtractor)
                        {
                        TS_ASSERT(vReflectionExtractor->getMethodName()->equals(String::create("gettoo")));
                        }
                    }
                }
            }

        /**
        * Test readExternal
        */
        void testReadExternal()
            {
            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo", true);
            //TODO : need a PofReader mock.
            }

        /**
        * Test writeExternal
        */
        void testWriteExternal()
            {
            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo", true);
            //TODO : need a PofWriter mock.
            }

        /**
        * Test equals
        */
        void testEquals()
            {
            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo");

            TS_ASSERT(hPropertyManipulator->equals(hPropertyManipulator));

            PropertyManipulator::Handle hPropertyManipulator2 =
                    PropertyManipulator::create("foo");

            TS_ASSERT(hPropertyManipulator->equals(hPropertyManipulator2));
            TS_ASSERT(hPropertyManipulator2->equals(hPropertyManipulator));

            PropertyManipulator::Handle hPropertyManipulator3 =
                    PropertyManipulator::create("foo", false);

            TS_ASSERT(hPropertyManipulator->equals(hPropertyManipulator3));
            TS_ASSERT(hPropertyManipulator3->equals(hPropertyManipulator));

            PropertyManipulator::Handle hPropertyManipulator4 =
                    PropertyManipulator::create("foo", true);

            TS_ASSERT(!hPropertyManipulator->equals(hPropertyManipulator4));
            TS_ASSERT(!hPropertyManipulator4->equals(hPropertyManipulator));

            PropertyManipulator::Handle hPropertyManipulator5 =
                    PropertyManipulator::create("bar");

            TS_ASSERT(!hPropertyManipulator->equals(hPropertyManipulator5));
            TS_ASSERT(!hPropertyManipulator5->equals(hPropertyManipulator));

            }


        /**
        * Test hashCode
        */
        void testHashCode()
            {
            PropertyManipulator::Handle hPropertyManipulator =
                    PropertyManipulator::create("foo");

            PropertyManipulator::Handle hPropertyManipulator2 =
                    PropertyManipulator::create("foo");

            TS_ASSERT(hPropertyManipulator->hashCode() == hPropertyManipulator2->hashCode());

            PropertyManipulator::Handle hPropertyManipulator3 =
                    PropertyManipulator::create("foo", true);

            PropertyManipulator::Handle hPropertyManipulator4 =
                    PropertyManipulator::create("foo", true);

            TS_ASSERT(hPropertyManipulator3->hashCode() == hPropertyManipulator4->hashCode());

            PropertyManipulator::Handle hPropertyManipulator5 =
                    PropertyManipulator::create("foo", false);

            TS_ASSERT(hPropertyManipulator->hashCode() == hPropertyManipulator5->hashCode());

            }

        };

