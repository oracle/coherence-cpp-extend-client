/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "private/coherence/util/Date.hpp"

#include <sstream>

using namespace coherence::util;
using namespace std;


class DateTest : public CxxTest::TestSuite
    {
    public:

    void testAccessors()
        {
        int32_t nYear    = 2008;
        int32_t nMonth   = 8;
        int32_t nDay     = 14;
        int32_t nHours   = 8;
        int32_t nMinutes = 35;
        int32_t nSeconds = 20;
        int32_t nMillis  = 650;

        Date::View vDate = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(vDate->getYear() == nYear);
        TS_ASSERT(vDate->getMonth() == nMonth);
        TS_ASSERT(vDate->getDay() == nDay);
        TS_ASSERT(vDate->getHours() == nHours);
        TS_ASSERT(vDate->getMinutes() == nMinutes);
        TS_ASSERT(vDate->getSeconds() == nSeconds);
        TS_ASSERT(vDate->getMillis() == nMillis);
        }

    void testCreaeteFromMillis()
        {
        // simply ensure that Date can be createed with millis
        Date::View vDate = Date::create(System::currentTimeMillis());

        TS_ASSERT(vDate->getYear() != 0);
        TS_ASSERT(vDate->getMonth() != 0);
        TS_ASSERT(vDate->getDay() != 0);
        // no other meaningful assertions
        }

    void testCreateFromCurrentTime()
        {
        // simply ensure that Date can be createed from the current time
        Date::View vDate = Date::create();

        TS_ASSERT(vDate->getYear() != 0);
        TS_ASSERT(vDate->getMonth() != 0);
        TS_ASSERT(vDate->getDay() != 0);
        // no other meaningful assertions
        }

    void testToString()
        {
        int32_t nYear    = 2008;
        int32_t nMonth   = 9;
        int32_t nDay     = 1;
        int32_t nHours   = 0;
        int32_t nMinutes = 2;
        int32_t nSeconds = 6;
        int32_t nMillis  = 8;

        Date::View vDate = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(vDate->toString()->equals("2008-09-01 00:02:06.008"));
        }

    void testHashCode()
        {
        int32_t nYear    = 2008;
        int32_t nMonth   = 8;
        int32_t nDay     = 14;
        int32_t nHours   = 8;
        int32_t nMinutes = 35;
        int32_t nSeconds = 20;
        int32_t nMillis  = 650;

        Date::View vDate = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        Date::View vDate2 = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(vDate->hashCode() == vDate2->hashCode());
        }

    void testEquals()
        {
        int32_t nYear    = 2008;
        int32_t nMonth   = 8;
        int32_t nDay     = 14;
        int32_t nHours   = 8;
        int32_t nMinutes = 35;
        int32_t nSeconds = 20;
        int32_t nMillis  = 650;

        Date::View vDate = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        Date::View vDate2 = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(vDate->equals(vDate));
        TS_ASSERT(!vDate->equals(String::create("not")));
        TS_ASSERT(vDate->equals(vDate2) && vDate2->equals(vDate));

        vDate2 = Date::create(9, nMonth, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(!vDate->equals(vDate2) && !vDate2->equals(vDate));

        vDate2 = Date::create(nYear, 12, nDay, nHours, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(!vDate->equals(vDate2) && !vDate2->equals(vDate));

        vDate2 = Date::create(nYear, nMonth, 1, nHours, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(!vDate->equals(vDate2) && !vDate2->equals(vDate));

        vDate2 = Date::create(nYear, nMonth, nDay, 22, nMinutes,
                nSeconds, nMillis);

        TS_ASSERT(!vDate->equals(vDate2) && !vDate2->equals(vDate));

        vDate2 = Date::create(nYear, nMonth, nDay, nHours, 55,
                nSeconds, nMillis);

        TS_ASSERT(!vDate->equals(vDate2) && !vDate2->equals(vDate));

        vDate2 = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                61, nMillis);

        TS_ASSERT(!vDate->equals(vDate2) && !vDate2->equals(vDate));

        vDate2 = Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, 400);

        TS_ASSERT(!vDate->equals(vDate2) && !vDate2->equals(vDate));
        }

    void testIllegalArgs()
        {
        int32_t nYear    = 2008;
        int32_t nMonth   = 8;
        int32_t nDay     = 14;
        int32_t nHours   = 8;
        int32_t nMinutes = 35;
        int32_t nSeconds = 20;
        int32_t nMillis  = 650;


        TS_ASSERT_THROWS(Date::create(nYear, 13, nDay, nHours, nMinutes,
                nSeconds, nMillis), IllegalArgumentException::View);

        TS_ASSERT_THROWS(Date::create(nYear, nMonth, 32, nHours, nMinutes,
                nSeconds, nMillis), IllegalArgumentException::View);

        TS_ASSERT_THROWS(Date::create(nYear, nMonth, nDay, 24, nMinutes,
                nSeconds, nMillis), IllegalArgumentException::View);

        TS_ASSERT_THROWS(Date::create(nYear, nMonth, nDay, nHours, 61,
                nSeconds, nMillis), IllegalArgumentException::View);

        TS_ASSERT_THROWS(Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                65, nMillis), IllegalArgumentException::View);

        TS_ASSERT_THROWS(Date::create(nYear, nMonth, nDay, nHours, nMinutes,
                nSeconds, 1000), IllegalArgumentException::View);
        }
    };


