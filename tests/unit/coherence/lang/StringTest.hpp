/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

using namespace coherence::lang;


/**
* Test Suite for the String class.
*/
class StringTest : public CxxTest::TestSuite
    {
    public:
        void testReferenceAssignment()
            {
            String::Handle hs = "hello world";
            String::Handle hs2 = hs;
            String::View   vs  = hs;
            Object::View   v   = hs;
            String::View   vs2 = cast<String::View>(v);
            TS_ASSERT(hs == hs2);
            TS_ASSERT(hs == vs);
            TS_ASSERT(hs == v);
            TS_ASSERT(hs == vs2);
            }

        void testBoxing()
            {
            String::Handle hs = "hello world";
            TS_ASSERT(hs->equals("hello world"));
            TS_ASSERT(!hs->equals("something else"));

            hs = "goodbye world";
            TS_ASSERT(hs->equals("goodbye world"));
            }

        void testNullBox()
            {
            String::Handle hn = NULL;
            TS_ASSERT(NULL == hn);
            hn = NULL;
            TS_ASSERT(NULL == hn);
            TS_ASSERT(hn == NULL);
            }

        void testBoxWide()
            {
            wchar_t awch[5];
            for (size32_t i = 0; i < 4; ++i)
                {
                awch[i] = wchar_t(i+1);
                }
            awch[4] = 0;
            String::Handle hsw = String::Handle(awch);
            TS_ASSERT(hsw->equals(awch));
            TS_ASSERT(!hsw->equals("foo"));
            awch[0] = 73;
            TS_ASSERT(!hsw->equals(awch));
            hsw = String::create(awch);
            TS_ASSERT(hsw->equals(awch));
            }

        void testBoxStl()
            {
            std::string sts = "hello world";
            String::Handle hs = sts;
            TS_ASSERT(hs->equalsStd(sts));
            std::string sts1 = hs;
            TS_ASSERT(sts == sts1);
            }

        void testBoxStlWide()
            {
            wchar_t awch[5];
            for (size32_t i = 0; i < 4; ++i)
                {
                awch[i] = wchar_t(i+60);
                }
            awch[4] = 0;

            std::wstring stws(awch);
            String::Handle hs = stws;
            TS_ASSERT(hs->equalsStd(stws));
            std::wstring stws1 = hs;
            TS_ASSERT(stws == stws1);
            }

        void testArrayEquals()
            {
            Array<octet_t>::Handle hach = Array<octet_t>::create(4);
            hach[0] = 'a';
            hach[1] = 'b';
            hach[2] = 'c';

            String::Handle hs = "abc";
            TS_ASSERT(hs->getOctets()->equals(hach));
            TS_ASSERT(hach->equals(hs->getOctets()));
            TS_ASSERT(hach->equals(hs)); // odd but ok
            TS_ASSERT(hs->equals(hach)); // symetric with above
            }

        void testArrayClone()
            {
            String::Handle hs = "abc";
            Array<octet_t>::View vach = cast<Array<octet_t>::View>(hs->getOctets()->clone());
            TS_ASSERT(NULL != vach);
            TS_ASSERT(hs->getOctets() != vach);
            TS_ASSERT(hs->getOctets()->equals(vach));
            }

        void testCharArrayCreate()
            {
            Array<char>::Handle hach = Array<char>::create(3);
            hach[0] = 'a';
            hach[1] = 'b';
            hach[2] = 'c';
            String::Handle hs = String::create((Array<char>::View) hach);
            TS_ASSERT(hs->equals("abc"));
            }

        void testWCharArrayCreate()
            {
            Array<wchar_t>::Handle hach = Array<wchar_t>::create(3);
            hach[0] = 'a';
            hach[1] = 'b';
            hach[2] = 'c';
            String::Handle hs = String::create((Array<wchar_t>::View) hach);
            TS_ASSERT(hs->equals("abc"));
            }

        void testChar16ArrayCreate()
            {
            Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(3);
            hach[0] = 'a';
            hach[1] = 'b';
            hach[2] = 'c';
            String::Handle hs = String::create((Array<wchar16_t>::View) hach);
            TS_ASSERT(hs->equals("abc"));
            }

        void fill(Array<wchar16_t>::Handle hac, wchar16_t ch = 0x0032)
            {
            for (int i = 0, cch = hac->length; i < cch; ++i)
                {
                hac[i] = ch;
                }
            }

        void replace(Array<wchar16_t>::Handle hac, String::View vs,
                size32_t iBegin = 0)
            {
            size32_t    cch = vs->length();
            const char* ach = vs->getCString();

            for (size32_t i = iBegin; i < cch; ++i)
                {
                hac[i] = String::next(ach);
                }
            }

        Array<wchar16_t>::Handle toCharArray(String::View vs)
            {
            Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(vs->length());
            replace(hach, vs);
            return hach;
            }

        void testUnicodeArrayCreate()
            {
            size32_t i1 = 11;
            size32_t i2 = 18;
            String::View vs = "the amount=?123 & ? pence";
            Array<wchar16_t>::Handle hach = toCharArray(vs);
            hach[i1] = 0x00a3;
            hach[i2] = 0x00BC;
            String::Handle hs = String::create((Array<wchar16_t>::View) hach);
            Array<octet_t>::View vab = hs->getOctets();
            TS_ASSERT(vab->length == 28);

            size32_t indx  = hs->indexOf(0xA3);
            size32_t indx2 = hs->indexOf(0xBC);
            TS_ASSERT(indx == i1);
            TS_ASSERT(indx2 == i2);
            TS_ASSERT(hs->substring(indx+1,indx2)->equals("123 & "));
            indx2 = hs->lastIndexOf(0xBC, 20);
            TS_ASSERT(indx2 == i2);
            indx2 = hs->lastIndexOf(0xBC);
            TS_ASSERT(indx2 == i2);

            vs = "? x ? a set s such that x ? s";
            hach = toCharArray(vs);
            hach[0]  = 0x2200;
            hach[4]  = 0x2203;
            hach[26] = 0x2208;
            hs = String::create((Array<wchar16_t>::View) hach);

            String::View vsb = hs->substring(0,vs->indexOf('s',9)+1);
            String::View vse = hs->substring(vs->lastIndexOf('x'));
            TS_ASSERT(vsb->length() == 13);
            TS_ASSERT(vse->length() == 5);
            TS_ASSERT(hs->startsWith(vsb));
            TS_ASSERT(hs->endsWith(vse));
            // check to see if we have 3 tripple octet chars
            TS_ASSERT(hs->getOctets()->length == (hs->length() + 6 + 1));
            }

        /**
        * Test that we can find something at the beginning of the string
        */
        void testStartIndex()
            {
            String::View vsStr = "This is my first string";
            size32_t x = vsStr->indexOf("This");
            TS_ASSERT(x == 0);

            vsStr = "short";
            x = vsStr->indexOf("longlonglong");
            TS_ASSERT(x == String::npos);
            }

        /**
        * Test that StringHelper works;
        */
        void testCharIndex()
            {
            String::Handle hString = String::create("hello.dave");
            size32_t cI = hString->indexOf('.');
            TS_ASSERT(cI == 5);

            cI = hString->indexOf(':');
            TS_ASSERT(cI == String::npos);


            hString = String::create("hello.dave.you.good");
            cI = hString->indexOf('.',6);
            TS_ASSERT(cI == 10);
            }

        /**
        * Test that we can find an index of a character searching from the end;
        */
        void testLastIndex()
            {
            String::Handle hString = String::create("hello.dave.what.up");
            size32_t cI = hString->lastIndexOf('.');
            TS_ASSERT(cI == 15);

            cI = hString->lastIndexOf(':');
            TS_ASSERT(cI == String::npos);

            hString = String::create("hello.dave.you.good");
            cI = hString->lastIndexOf('.', 9);
            TS_ASSERT(cI == 5);

            hString = String::create("hello,dave.you.good");
            cI = hString->lastIndexOf('.', 9);
            TS_ASSERT(cI == String::npos);

            hString = String::create("hello.dave.you.good");
            cI = hString->lastIndexOf(".good");
            TS_ASSERT(cI == 14);


            hString = String::create("hello.dave.hello.mark");
            cI = hString->lastIndexOf("hello");
            TS_ASSERT(cI == 11);

            cI = hString->lastIndexOf("hello",9);
            TS_ASSERT(cI == 0);

            cI = hString->lastIndexOf("helilo",9);
            TS_ASSERT(cI == String::npos);
            }

        /**
        * Test that we can find something at the end of the string
        */
        void testEndIndex()
            {
            String::View vsStr = "This is my first string";
            size32_t x = vsStr->indexOf("string");
            TS_ASSERT(x == 17);
            }

        /**
        * Test that we can find something in the middle of the string
        */
        void testMidIndex()
            {
            String::View vsStr = "This is my first string";
            size32_t x = vsStr->indexOf("my");
            TS_ASSERT(x == 8);
            }

        /**
        * Test that a non-found string returns -1
        */
        void testNoIndex()
            {
            String::View vsStr = "This is my first string";
            size32_t x = vsStr->indexOf("foo");
            TS_ASSERT(x == String::npos);
            }

        /**
        * Test that we can take the substring of a string
        */
        void testSubString()
            {
            String::View vsStr = "This is my first string";
            String::View vsSub = vsStr->substring(vsStr->indexOf("first"));
            TS_ASSERT(vsSub->equals("first string"));
            }

        /**
        * Test that we can take the substring of a string with an end idx
        */
        void testSubStringEndIdx()
            {
            String::View vsStr = "This is my first string";
            String::View vsSub =
                vsStr->substring(
                        vsStr->indexOf("first"),
                        vsStr->indexOf("string") - 1);

            TS_ASSERT(vsSub->equals("first"));
            }

        void testStartsWith()
            {
            String::View vsStr = "This is my first string";
            bool f = vsStr->startsWith("This");
            TS_ASSERT(f);

            f = vsStr->startsWith("is");
            TS_ASSERT(!f);
            }

        void testEndsWith()
            {
            String::View vsStr = "This is my last string";
            bool f = vsStr->endsWith("This");
            TS_ASSERT(!f);

            f = vsStr->endsWith("string");
            TS_ASSERT(f);
            }

        void testRegonMatches()
            {
            String::View vsStr = "This is beyond my last string";
            bool f = vsStr->regionMatches(8,"beyond");
            TS_ASSERT(f);

            f = vsStr->regionMatches(8,"the great beyond", 10, 6);
            TS_ASSERT(f);

            f = vsStr->regionMatches(8,"the great beyond", 4, 6);
            TS_ASSERT(!f);
            }

        void testTrim()
            {
            String::View vsStr = "   This is fun    ";
            String::View vsAnswer = vsStr->trim();
            TS_ASSERT(vsAnswer->equals(String::create("This is fun")));

            vsAnswer = vsStr->trim();
            TS_ASSERT(vsAnswer->equals(String::create("This is fun")));

            vsStr = "   This is fun";
            vsAnswer = vsStr->trim();
            TS_ASSERT(vsAnswer->equals(String::create("This is fun")));

            vsStr = "This is fun";
            vsAnswer = vsStr->trim();
            TS_ASSERT(vsAnswer == vsStr);

            vsStr = "";
            vsAnswer = vsStr->trim();
            TS_ASSERT(vsAnswer == vsStr);
            }

        void testSizeOf()
            {
            String::View vsSmall = "abc";
            String::View vsBig   = "bigger string";
            String::View vsBig2  = "equally big..";

            TS_ASSERT(vsSmall->sizeOf() < vsBig->sizeOf());
            TS_ASSERT(vsBig->sizeOf() == vsBig2->sizeOf());
            }

        void testLength()
            {
            String::View vsEmpty = "";
            TS_ASSERT(vsEmpty->isEmpty());
            TS_ASSERT(vsEmpty->length() == 0);

            String::View vsOneChar = "a";
            TS_ASSERT(!vsOneChar->isEmpty());
            TS_ASSERT(vsOneChar->length() == 1);
            }

        void testValueOf()
            {
            TS_ASSERT(String::valueOf(NULL)->equals("NULL"));
            TS_ASSERT(String::valueOf(String::create("not-null"))->equals("not-null"));
            TS_ASSERT(String::valueOf(Integer32::create(1))->equals("1"));
            }

        void testCloneEquality()
            {
            String::View vsOrig  = "original";
            String::View vsOther = "other";
            String::View vsClone = cast<String::View>(vsOrig->clone());

            TS_ASSERT(vsOrig != vsClone);
            TS_ASSERT(vsOrig->equals(vsClone));
            TS_ASSERT(vsClone->equals(vsOrig));
            TS_ASSERT(!vsOther->equals(vsClone));
            TS_ASSERT(!vsClone->equals(vsOther));
            TS_ASSERT(vsOrig->equals(vsOrig));
            TS_ASSERT(vsClone->equals(vsClone));
            }

        void testUnicodeBMP()
            {
            size32_t ccp = 0x10000 - (0xE000 - 0xD800);
            Array<wchar16_t>::Handle hacp = Array<wchar16_t>::create(ccp);

            for (wchar16_t cp = 0; cp < 0xD800; ++cp)
                {
                hacp[cp] = cp;
                }
            // skip non-BMP character range
            for (wchar16_t cp = 0xE000; cp != 0x0; ++cp)
                {
                hacp[cp - (0xE000 - 0xD800)] = cp;
                }

            // test that all BMP characters are "legal"
            // also tests conversion to UTF-8
            String::Handle hUTF16 = String::create((Array<wchar16_t>::View) hacp);

            // test construction from a UTF-8 String of all the BMP characters
            Array<octet_t>::View vaOctet = hUTF16->getOctets();
            String::Handle hUTF8 = String::create(vaOctet, 0, vaOctet->length - 1);

            TS_ASSERT(hUTF16->length() == ccp);
            TS_ASSERT(hUTF8 ->length() == ccp);

            TS_ASSERT(hUTF16->equals(hUTF8));
            TS_ASSERT(hUTF8 ->equals(hUTF16));

            const char* autf8 = hUTF8->getCString();
            size32_t i = 0;
            for (i = 0; i < ccp && String::next(autf8) == hacp[i]; ++i)
                ;
            TS_ASSERT(i == ccp);
            }

        void testStdStringConversion()
            {
            // Test fix for COH-3658, Bug 9962771
            char car[5];
            car[0] = (char) 0xc2; // c2 a2 is cent character
            car[1] = (char) 0xa2;
            car[2] = (char) 0xc2;
            car[3] = (char) 0xa2;
            car[4] =        '\0';
            std::string  data(car);
            String::View vString = String::create(data);
            std::string  conv    = vString;
            TS_ASSERT(data == conv);

            // Test fix for COH-3709
            car[0] = (char) 0x01;
            car[1] = (char) 0xDD;
            car[2] = (char) 0x01;
            car[3] = (char) 0xDD;
            std::string data2(car, 4);
            try
                {
                vString = String::create(data2);
                TS_FAIL("Expected an IllegalArgumentException.");
                }
            catch (IllegalArgumentException::View)
                {
                // expected
                }
            }

        void testUTF8Validation()
            {
            // test Euro character U+20AC; good 3 byte
            char euro[4];
            euro[0] = (char) 0xe2;
            euro[1] = (char) 0x82;
            euro[2] = (char) 0xac;
            euro[3] =        '\0';
            std::string  data(euro);
            String::View vString = String::create(data);
            std::string  conv    = vString;
            TS_ASSERT_EQUALS(data, conv);

            // test bad 3 byte, bad second byte
            char three2[4];
            three2[0] = (char) 0xe2;
            three2[1] = (char) 0x22; // bad - should have upper bits '10'
            three2[2] = (char) 0xa9;
            three2[3] =        '\0';
            std::string data2(three2);
            TS_ASSERT_THROWS(String::create(data2), IllegalArgumentException::View);

            // test bad 3 byte, bad third byte
            char three3[4];
            three3[0] = (char) 0xe2;
            three3[1] = (char) 0x82;
            three3[2] = (char) 0x29; // bad - should have upper bits '10'
            three3[3] =        '\0';
            std::string data3(three3);
            TS_ASSERT_THROWS(String::create(data3), IllegalArgumentException::View);

            // test good 2 byte
            char two[3];
            two[0] = (char) 0xc2; // c2 a2 is cent character
            two[1] = (char) 0xa2;
            two[2] =        '\0';
            std::string  data4(two);
            String::View vString2 = String::create(data4);
            std::string  conv4    = vString2;
            TS_ASSERT_EQUALS(data4, conv4);

            // test bad 2 byte
            char two2[3];
            two2[0] = (char) 0xc2;
            two2[1] = (char) 0x22; // bad -should have upper bits '10'
            two2[2] =        '\0';
            std::string  data5(two2);
            TS_ASSERT_THROWS(String::create(data5), IllegalArgumentException::View);

            // test good 4 byte single character: 0xf0938080
            char four[5];
            four[0] = (char) 0xf0;
            four[1] = (char) 0x93;
            four[2] = (char) 0x80;
            four[3] = (char) 0x80;
            four[4] =        '\0';
            std::string  data6(four);
            String::View vString3 = String::create(data6);
            std::string  conv5    = vString3;
            TS_ASSERT_EQUALS(data6, conv5);
            TS_ASSERT_EQUALS(size32_t(1), vString3->length());

            // test good 4 byte multiple characters: 0xf09f8ebf, 0xf09f8f80, 0xf09f8e89, 0xf09f9294
            char four2[17];
            // 0xf09f8ebf
            four2[0] = (char) 0xf0;
            four2[1] = (char) 0x9f;
            four2[2] = (char) 0x8e;
            four2[3] = (char) 0xbf;
            // 0xf09f8f80
            four2[4] = (char) 0xf0;
            four2[5] = (char) 0x9f;
            four2[6] = (char) 0x8f;
            four2[7] = (char) 0x80;
            // 0xf09f8e89
            four2[8]  = (char) 0xf0;
            four2[9]  = (char) 0x9f;
            four2[10] = (char) 0x8e;
            four2[11] = (char) 0x89;
            // 0xf09f9294
            four2[12] = (char) 0xf0;
            four2[13] = (char) 0x9f;
            four2[14] = (char) 0x92;
            four2[15] = (char) 0x94;
            four2[16] =        '\0';
            std::string data7(four2);
            vString3 = String::create(data7);
            conv5    = vString3;
            TS_ASSERT_EQUALS(data7, conv5);
            TS_ASSERT_EQUALS(size32_t(4), vString3->length());

            // test bad 4 byte, bad second byte
            char four3[5];
            four3[0] = (char) 0xf0;
            four3[1] = (char) 0x22; // bad - should have upper bits '10'
            four3[2] = (char) 0x8e;
            four3[3] = (char) 0xbf;
            four3[4] =        '\0';
            std::string data8(four3);
            TS_ASSERT_THROWS(String::create(data8), IllegalArgumentException::View);

            // test bad 4 byte, bad third byte
            char four4[5];
            four4[0] = (char) 0xf0;
            four4[1] = (char) 0x9f;
            four4[2] = (char) 0x22; // bad - should have upper bits '10'
            four4[3] = (char) 0xbf;
            four4[4] =        '\0';
            std::string data9(four4);
            TS_ASSERT_THROWS(String::create(data9), IllegalArgumentException::View);

            // test bad 4 byte, bad fourth byte
            char four5[5];
            four5[0] = (char) 0xf0;
            four5[1] = (char) 0x9f;
            four5[2] = (char) 0x8e;
            four5[3] = (char) 0x29; // bad - should have upper bits '10'
            four5[4] =        '\0';
            std::string data10(four5);
            TS_ASSERT_THROWS(String::create(data10), IllegalArgumentException::View);

            // test bad 4 byte, incomplete - just one byte
            char four6[2];
            four6[0] = (char) 0xf0;
            four6[1] =        '\0';
            std::string data11(four6);
            TS_ASSERT_THROWS(String::create(data11), IllegalArgumentException::View);

            // test bad 4 byte, incomplete - just two bytes
            char four7[3];
            four7[0] = (char) 0xf0;
            four7[1] = (char) 0x93;
            four7[2] =        '\0';
            std::string data12(four7);
            TS_ASSERT_THROWS(String::create(data12), IllegalArgumentException::View);

            // test bad 4 byte, incomplete - just three bytes
            char four8[4];
            four8[0] = (char) 0xf0;
            four8[1] = (char) 0x93;
            four8[2] = (char) 0x80;
            four8[3] =        '\0';
            std::string data13(four8);
            TS_ASSERT_THROWS(String::create(data13), IllegalArgumentException::View);
            }
    };
