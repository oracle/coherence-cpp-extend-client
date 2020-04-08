/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/Binary.hpp"
#include "coherence/util/BinaryWriteBuffer.hpp"
#include "coherence/util/SerializationHelper.hpp"

using namespace coherence::lang;

using coherence::io::IOException;
using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::io::pof::SystemPofContext;
using coherence::util::Binary;
using coherence::util::BinaryWriteBuffer;
using coherence::util::SerializationHelper;


/**
* Test Suite for SerializationHelper.
*/

class SerializationHelperTest : public CxxTest::TestSuite
    {

    public:
        // ----- test methods -----------------------------------------------

        /**
        * Test toBinary() and fromBinary()
        */
        void testToAndFromBinary()
            {
            toAndFromBinaryTester(mock::TestStringSerializer::create());
            }

        /**
        * Test toBinary() and fromBinary() with SystemPofContext
        */
        void testToAndFromBinaryWithSystemPofContext()
            {
            toAndFromBinaryTester(SystemPofContext::getInstance());
            }

        /**
        * Test decorateBinary(), isIntDecorated() and extractIntDecoration()
        */
        void testDecoration()
            {
            Serializer::View vSerializer = mock::TestStringSerializer::create();
            String::View     vsOrig      = "hello";
            Binary::View     vBin        = SerializationHelper::toBinary(vsOrig, vSerializer);

            TS_ASSERT(!SerializationHelper::isIntDecorated(vBin));
            vBin = SerializationHelper::decorateBinary(vBin, 6);
            TS_ASSERT(SerializationHelper::isIntDecorated(vBin));
            TS_ASSERT(SerializationHelper::extractIntDecoration(vBin) == 6);

            // verify adding a decoration does not impact the deserialized Object
            String::View vsDest = cast<String::View>(SerializationHelper::fromBinary(vBin, vSerializer));
            TS_ASSERT(vsOrig->equals(vsDest));

            // handle empty Binary appropriately
            TS_ASSERT(!SerializationHelper::isIntDecorated(Binary::getEmptyBinary()));
            TS_ASSERT_THROWS(SerializationHelper::extractIntDecoration(Binary::getEmptyBinary()),
                    IllegalArgumentException::View);
            }

        /**
        * Test statistics gathering and reset
        */
        void testStats()
            {
            SerializationStatsHelper::runTest();
            }


        // ----- helper methods ---------------------------------------------

        /**
        * Test toBinary() and fromBinary() with SystemPofContext
        */
        static void toAndFromBinaryTester(Serializer::View vSerializer)
            {
            String::View     vsOrig      = "hello";
            Binary::View     vBin        = SerializationHelper::toBinary(vsOrig, vSerializer);
            String::View     vsDest      = cast<String::View>(SerializationHelper::fromBinary(vBin, vSerializer));
            TS_ASSERT(vsOrig->equals(vsDest));

            WriteBuffer::Handle hBuf = BinaryWriteBuffer::create(64);
            vSerializer->serialize(hBuf->getBufferOutput(), vsOrig);
            // throws IOException as type (fmt_ext) information is missing
            TS_ASSERT_THROWS(SerializationHelper::fromBinary(hBuf->toBinary(), vSerializer), IOException::View);
            }


        // ----- inner class: SerializationStatsHelper ----------------------

        /**
        * Test statistics gathering and reset helper class
        */
        class SerializationStatsHelper
                : public abstract_spec<SerializationStatsHelper,
                extends<SerializationHelper> >
            {
            // ----- constructors -----------------------------------------

            private:
                /**
                * This constructor is blocked (private) as SerializationStatsHelper
                * provides only a static interface and no instances may be created.
                */
                SerializationStatsHelper();


            // ----- SerializationStatsHelper interface -------------------

            public:
                static void runTest()
                    {
                    Stats::Handle hStats = Stats::create();

                    // populate stats, verifying stats are not reset
                    hStats->update(5100);
                    hStats->update(5000);
                    hStats->update(4900);
                    hStats->update(4800);
                    hStats->update(4700);
                    TS_ASSERT_EQUALS(hStats->instantiateBuffer()->getCapacity(), (size32_t) 5100 + 4);

                    // update with large value; verify stats reset
                    hStats->update(1024*1024*4);
                    TS_ASSERT_EQUALS(hStats->instantiateBuffer()->getCapacity(), (size32_t) 1024*1024*4);

                    // update with small value; verify stats reset
                    hStats->update(1000);
                    TS_ASSERT_EQUALS(hStats->instantiateBuffer()->getCapacity(), (size32_t) 1000 + 8);
                    }
            };
    };
