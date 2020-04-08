/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/util/aggregator/Float64Average.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"

#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::io::OctetArrayReadBuffer;
using coherence::io::OctetArrayWriteBuffer;
using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::Float64Average;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;

using namespace common::test;

/**
* Test Suite for the Float64Average.
*/
class Float64AverageTest : public CxxTest::TestSuite
  {
  public:
        /**
        * Test Float64AverageTest
        */
        void testFloat64AverageTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    Float32::create(22.0F)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    Float32::create(42.0F)));
            Float64Average::Handle hAgg = Float64Average::create(
                    (ValueExtractor::View) IdentityExtractor::getInstance());
            Object::View vObj = hAgg->aggregate(hSet);
            TS_ASSERT(Float64::create(32.0)->equals(vObj));

            hSet->clear();
            OctetArrayWriteBuffer::Handle hBuff =
                    OctetArrayWriteBuffer::create(12);
            WriteBuffer::BufferOutput::Handle hOut = hBuff->getBufferOutput();
            hOut->writeInt32(2);
            hOut->writeFloat64(32.0);
            hSet->add(hBuff->toOctetArray());

            hBuff = OctetArrayWriteBuffer::create(12);
            hOut = hBuff->getBufferOutput();
            hOut->writeInt32(4);
            hOut->writeFloat64(64.0);
            hSet->add(hBuff->toOctetArray());

            TS_ASSERT(Float64::create(16.0)->equals(
                        hAgg->aggregateResults(hSet)));

            }

        /**
        * Test Float64AverageStringConstructor
        */
        void testFloat64AverageStringConstructor()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    SalariedPerson::create(String::create("Noah"), 
                        (int32_t) 23, (float64_t) 1000.0)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    SalariedPerson::create(String::create("David"),
                        (int32_t) 42, (float64_t) 200000.0)));
            Float64Average::Handle hAgg = Float64Average::create(
                    (String::View) String::create("getSalary"));
            Object::View vObj = hAgg->aggregate(hSet);
            TS_ASSERT(Float64::create(100500.0)->equals(vObj));
            }
  };
