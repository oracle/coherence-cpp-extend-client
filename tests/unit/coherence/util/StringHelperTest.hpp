/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"
#include "private/coherence/util/StringHelper.hpp"

using namespace coherence::util;
using namespace coherence::lang;

/**
* Test Suite for the Boolean object.
*/
class StringHelperTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test that we can replace the beginning of a string
        */
        void testReplaceBeginning()
            {
            String::View vsStr = "This is my first string";
            String::View vsNew = StringHelper::replace(vsStr, "This", "That");

            TS_ASSERT(vsNew->equals("That is my first string"));
            }

        /**
        * Test that we can replace the middle of a string
        */
        void testReplaceMiddle()
            {
            String::View vsStr = "This is my first string";
            String::View vsNew = StringHelper::replace(vsStr, "my ", "");

            TS_ASSERT(vsNew->equals("This is first string"));
            }

        /**
        * Test that we can replace the end of a string
        */
        void testReplaceEnd()
            {
            String::View vsStr = "This is my first string";
            String::View vsNew = StringHelper::replace(vsStr, " string", "");

            TS_ASSERT(vsNew->equals("This is my first"));
            }

        /**
        * Test that we can replace the end of a string
        */
        void testReplaceMany()
            {
            String::View vsStr = "ab ba cd ef ba gh ba";
            String::View vsNew = StringHelper::replace(vsStr, "ba", "xx");

            TS_ASSERT(vsNew->equals("ab xx cd ef xx gh xx"));
            }

        /**
        * Test split function.
        * String = "!this,is a test,, ok " and delims = " ,!".
        * Should produce the following tokens:
        * ""
        * "this"
        * "is"
        * "a"
        * "test"
        * ""
        * ""
        * "ok"
        */
        void testSplit()
            {
            String::View vsStr = "!this,is a test,, ok ";
            String::View vsDelims = " ,!";
            ObjectArray::Handle haTokens =
                    StringHelper::split(vsStr, vsDelims);
            // check number of tokens
            TS_ASSERT(haTokens->length == 8);
            // check each token
            TS_ASSERT(cast<String::View>(haTokens[0])->equals(""));
            TS_ASSERT(cast<String::View>(haTokens[1])->equals("this"));
            TS_ASSERT(cast<String::View>(haTokens[2])->equals("is"));
            TS_ASSERT(cast<String::View>(haTokens[3])->equals("a"));
            TS_ASSERT(cast<String::View>(haTokens[4])->equals("test"));
            TS_ASSERT(cast<String::View>(haTokens[5])->equals(""));
            TS_ASSERT(cast<String::View>(haTokens[6])->equals(""));
            TS_ASSERT(cast<String::View>(haTokens[7])->equals("ok"));
            }

        /**
        * Test split() passing in an empty src string.
        */
        void testSplitEmptyString()
            {
            ObjectArray::Handle haTokens = StringHelper::split("", ",");
            TS_ASSERT(haTokens->length == 0);
            }

        /**
        * Test split() passing in no delimiters. One token consisting of
        * the original string should be returned.
        */
        void testSplitNoDelimiters()
            {
            ObjectArray::Handle haTokens = StringHelper::split(
                    "this is my original string", "");
            // check number of tokens
            TS_ASSERT(haTokens->length == 1);
            // check each token
            TS_ASSERT(cast<String::View>(haTokens[0])->equals(
                    "this is my original string"));
            }

        /**
        * Test split() throws an IllegalArgumentException if delims is
        * NULL.
        */
        void testSplitNullDelims()
            {
            TS_ASSERT_THROWS(StringHelper::split("blah", NULL),
                    IllegalArgumentException::View);

            }

        void testParseTimeNano()
            {
            TS_ASSERT(1 == StringHelper::parseTime("1000000NS"));
            TS_ASSERT(1 == StringHelper::parseTime("1000000", StringHelper::unit_ns));
            }

        void testParseTimeMicro()
            {
            TS_ASSERT(1 == StringHelper::parseTime("1000US"));
            TS_ASSERT(1 == StringHelper::parseTime("1000", StringHelper::unit_us));
            }

        void testParseTimeMillis()
            {
            TS_ASSERT(1 == StringHelper::parseTime("1MS"));
            TS_ASSERT(1 == StringHelper::parseTime("1", StringHelper::unit_ms));
            }

        void testParseTimeSeconds()
            {
            TS_ASSERT(1000 == StringHelper::parseTime("1S"));
            TS_ASSERT(1000 == StringHelper::parseTime("1", StringHelper::unit_s));
            }

        void testParseTimeMinutes()
            {
            TS_ASSERT(60000 == StringHelper::parseTime("1M"));
            TS_ASSERT(60000 == StringHelper::parseTime("1", StringHelper::unit_m));
            }

        void testParseTimeHours()
            {
            TS_ASSERT(3600000 == StringHelper::parseTime("1H"));
            TS_ASSERT(3600000 == StringHelper::parseTime("1", StringHelper::unit_h));
            }

        void testParseTimeDays()
            {
            TS_ASSERT(86400000 == StringHelper::parseTime("1D"));
            TS_ASSERT(86400000 == StringHelper::parseTime("1", StringHelper::unit_d));
            }

        void testCamel()
            {
            TS_ASSERT(StringHelper::camel("tangosol.coherence", ".")->equals("TangosolCoherence"));
            }

        void testCompare()
            {
            TS_ASSERT(StringHelper::compare("", "b") < 0);
            TS_ASSERT(StringHelper::compare("b", "") > 0);
            TS_ASSERT(StringHelper::compare("", "") == 0);
            TS_ASSERT(StringHelper::compare("a", "b") < 0);
            TS_ASSERT(StringHelper::compare("b", "a") > 0);
            TS_ASSERT(StringHelper::compare("a", "a") == 0);
            TS_ASSERT(StringHelper::compare("abc", "abcd") < 0);
            TS_ASSERT(StringHelper::compare("abcd", "abc") > 0);
            TS_ASSERT(StringHelper::compare("abc", "abc") == 0);
            }

        void testToMemorySizeString()
            {
            int64_t lTest = 1024;
            TS_ASSERT(StringHelper::toMemorySizeString(lTest, true)->equals("1KB"));
            lTest = lTest*1024;
            TS_ASSERT(StringHelper::toMemorySizeString(lTest, true)->equals("1MB"));
            lTest = lTest*1024;
            TS_ASSERT(StringHelper::toMemorySizeString(lTest, true)->equals("1GB"));
            lTest = lTest*1024;
            TS_ASSERT(StringHelper::toMemorySizeString(lTest, true)->equals("1TB"));
            }

        void testParseMemorySize()
            {
            int64_t lTest   = 1024;
            int64_t lResult = StringHelper::parseMemorySize("1K");
            TS_ASSERT(lTest == lResult);

            lTest   = lTest*1024;
            lResult = StringHelper::parseMemorySize("1M");
            TS_ASSERT(lTest == lResult);

            lTest   = lTest*1024;
            lResult = StringHelper::parseMemorySize("1G");
            TS_ASSERT(lTest == lResult);
            }
    };
