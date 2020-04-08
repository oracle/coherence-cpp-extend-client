/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/util/Binary.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"

#include <sstream>
#include <iostream>

using namespace coherence::lang;
using namespace coherence::net;
using namespace coherence::run::xml;
using namespace std;

using coherence::util::Binary;

/**
 * Test Suite for XmlHelper
 */
class XmlHelperTestSuite : public CxxTest::TestSuite
    {
        public:

            /**
             * Test encodeAttribute()
             */
            void testEncodeAttribute()
                {
                // >
                TS_ASSERT(XmlHelper::encodeAttribute(">1", '\'')->equals("&gt;1"));
                // <
                TS_ASSERT(XmlHelper::encodeAttribute("1<a", '\'')->equals("1&lt;a"));
                // &
                TS_ASSERT(XmlHelper::encodeAttribute("&a&", '\'')->equals("&amp;a&amp;"));
                // '
                TS_ASSERT(XmlHelper::encodeAttribute("'hello'", '\'')->equals("&apos;hello&apos;"));
                // ' with " as quoting char
                TS_ASSERT(XmlHelper::encodeAttribute("'hello'", '"')->equals("'hello'"));
                // "
                TS_ASSERT(XmlHelper::encodeAttribute("\"hello\"", '"')->equals("&quot;hello&quot;"));
                // " with ' as quoting char
                TS_ASSERT(XmlHelper::encodeAttribute("\"hello\"", '\'')->equals("\"hello\""));
                // no encoding needed
                TS_ASSERT(XmlHelper::encodeAttribute("abcdefghijklmnopqrstuvwxyz", '\'')->equals("abcdefghijklmnopqrstuvwxyz"));
                TS_ASSERT(XmlHelper::encodeAttribute("ABCDEFGHIJKLMNOPQRSTUVWXYZ", '\'')->equals("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
                TS_ASSERT(XmlHelper::encodeAttribute("0123456789", '\'')->equals("0123456789"));
                TS_ASSERT(XmlHelper::encodeAttribute("~`!@#$%^*()-_+={[}]|\\:;,.?/", '\'')->equals("~`!@#$%^*()-_+={[}]|\\:;,.?/"));
                //control chars
                stringstream ss;
                ss << (char)0x01 << (char)0x02 << (char)0x03 << (char)0x04 << (char)0x05 << (char)0x06;
                TS_ASSERT(XmlHelper::encodeAttribute(ss.str(), '\'')->equals(
                    "&#x01;&#x02;&#x03;&#x04;&#x05;&#x06;"));

                ss.str("");
                ss << (char)0x07 << (char)0x08 << (char)0x09 << (char)0x0A << (char)0x0B << (char)0x0C;
                TS_ASSERT(XmlHelper::encodeAttribute(ss.str(), '\'')->equals(
                    "&#x07;&#x08;&#x09;&#x0A;&#x0B;&#x0C;"));

                ss.str("");
                ss << (char)0x0D << (char)0x0E << (char)0x0F << (char)0x10 << (char)0x11 << (char)0x12;
                TS_ASSERT(XmlHelper::encodeAttribute(ss.str(), '\'')->equals(
                    "&#x0D;&#x0E;&#x0F;&#x10;&#x11;&#x12;"));

                ss.str("");
                ss << (char)0x13 << (char)0x14 << (char)0x15 << (char)0x16 << (char)0x17 << (char)0x18;
                TS_ASSERT(XmlHelper::encodeAttribute(ss.str(), '\'')->equals(
                    "&#x13;&#x14;&#x15;&#x16;&#x17;&#x18;"));

                ss.str("");
                ss << (char)0x19 << (char)0x1A << (char)0x1B << (char)0x1C << (char)0x1D << (char)0x1E << (char)0x1F;
                TS_ASSERT(XmlHelper::encodeAttribute(ss.str(), '\'')->equals(
                    "&#x19;&#x1A;&#x1B;&#x1C;&#x1D;&#x1E;&#x1F;"));
                }

            /**
             * Test encodeAttribute passing in an invalid quote char.
             */
            void testEncodeAttributeInvalidQuote()
                {
                TS_ASSERT_THROWS(XmlHelper::encodeAttribute("test", '-'),
                                IllegalArgumentException::View);
                }

            /**
             * Test encodeAttribute for an empty String
             */
            void testEncodeAttributeEmptyString()
                {
                TS_ASSERT(XmlHelper::encodeAttribute("", '\'')->equals(""));
                //TODO: Passing in null causes NPE
                //assertEquals("", XmlHelper.encodeAttribute(null, '\''));

                }

            /**
             * Test quote()
             */
            void testQuote()
                {
                // >
                TS_ASSERT(XmlHelper::quote(">1")->equals("'&gt;1'"));
                // <
                TS_ASSERT(XmlHelper::quote("1z0904560<b34rty123")->equals("'1z0904560&lt;b34rty123'"));
                // &
                TS_ASSERT(XmlHelper::quote("&a&")->equals("'&amp;a&amp;'"));
                // '
                TS_ASSERT(XmlHelper::quote("'hello'")->equals("'&apos;hello&apos;'"));
                // "
                TS_ASSERT(XmlHelper::quote("\"hello\"")->equals("'\"hello\"'"));
                // no encoding needed
                TS_ASSERT(XmlHelper::quote("abcdefghijklmnopqrstuvwxyz")->equals("'abcdefghijklmnopqrstuvwxyz'"));
                TS_ASSERT(XmlHelper::quote("ABCDEFGHIJKLMNOPQRSTUVWXYZ")->equals("'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"));
                TS_ASSERT(XmlHelper::quote("0123456789")->equals("'0123456789'"));
                TS_ASSERT(XmlHelper::quote("~`!@#$%^*()-_+={[}]|\\:;,.?/")->equals("'~`!@#$%^*()-_+={[}]|\\:;,.?/'"));

                // control chars 0x00 - 0x1F
                //TODO: Encoding of control chars is broken(same as encodeAttribute).  For example: 0x10 is encoded as &#x00
                //TODO  Also, there are several code paths(all but last if statement in default) that are unreachable
                }

            /**
             * Test encodeContent() using CDATA
             */
            void testEncodeContentUsingCDATA()
                {
                TS_ASSERT(XmlHelper::encodeContent("", true)->equals(""));
                //no encoding necessary
                TS_ASSERT(XmlHelper::encodeContent("test123", true)->equals("test123"));
                TS_ASSERT(XmlHelper::encodeContent("[test123]", true)->equals("[test123]"));

                // begin/end whitespace
                TS_ASSERT(XmlHelper::encodeContent(" test ", true)->equals("<![CDATA[ test ]]>"));
                // <>
                TS_ASSERT(XmlHelper::encodeContent("<test>", true)->equals("<![CDATA[<test>]]>"));
                // &
                TS_ASSERT(XmlHelper::encodeContent("&test", true)->equals("<![CDATA[&test]]>"));
                // ]]> excludes use of CDATA
                TS_ASSERT(XmlHelper::encodeContent("  ]]>  ", true)->equals("&#x20;&#x20;]]&gt;&#x20;&#x20;"));
                // ]>
                TS_ASSERT(XmlHelper::encodeContent("  ]>  ", true)->equals("<![CDATA[  ]>  ]]>"));
                }

            /**
             * Test encodeContent() not using CDATA
             */
            void testEncodeContentNotUsingCDATA()
                {
                stringstream ss;
                // no encoding necessary
                TS_ASSERT(XmlHelper::encodeContent("test123", false)->equals("test123"));
                TS_ASSERT(XmlHelper::encodeContent("[test123]", false)->equals("[test123]"));
                TS_ASSERT(XmlHelper::encodeContent(" test123 ", false)->equals("&#x20;test123&#x20;"));
                // &
                TS_ASSERT(XmlHelper::encodeContent("&", false)->equals("&amp;"));
                TS_ASSERT(XmlHelper::encodeContent(" & ", false)->equals("&#x20;&amp;&#x20;"));
                // <...>
                //TODO: ensure correctness.  > is not encoded unless used used in context ]]>
                TS_ASSERT(XmlHelper::encodeContent("<test>", false)->equals("&lt;test>"));
                // 0x09
                char c = 0x09;
                ss << " " << c << " ";
                TS_ASSERT(XmlHelper::encodeContent(ss.str(), false)->equals("&#x20;&#x09;&#x20;"));
                //reset the stringstream
                ss.str("");
                // 0x0A
                c = 0x0A;
                ss << " " << c << " ";
                TS_ASSERT(XmlHelper::encodeContent(ss.str(), false)->equals("&#x20;&#x0A;&#x20;"));
                //reset the stringstream
                ss.str("");
                // 0x0D
                c = 0x0D;
                ss << " " << c << " ";
                TS_ASSERT(XmlHelper::encodeContent(ss.str(), false)->equals("&#x20;&#x0D;&#x20;"));
                // ]]>
                TS_ASSERT(XmlHelper::encodeContent("  ]]>  ", false)->equals("&#x20;&#x20;]]&gt;&#x20;&#x20;"));
                }

            /**
             * Test isEncodingValid()
             */
            void testIsEncodingValid()
                {
                TS_ASSERT(XmlHelper::isEncodingValid("Aa1-_."));
                TS_ASSERT(XmlHelper::isEncodingValid("a1-_."));

                //negative cases
                //empty string
                TS_ASSERT( !XmlHelper::isEncodingValid(""));
                // non aplha first char
                TS_ASSERT( !XmlHelper::isEncodingValid("1A"));
                TS_ASSERT( !XmlHelper::isEncodingValid("-A"));
                }

            /**
             * Test decodeAttribute()
             */
            void testDecodeAttribute()
                {
                // &
                TS_ASSERT(XmlHelper::decodeAttribute("1&amp;ab")->equals("1&ab"));
                // '
                TS_ASSERT(XmlHelper::decodeAttribute("&apos;hello&apos;")->equals("'hello'"));
                // ""
                TS_ASSERT(XmlHelper::decodeAttribute("&quot;hello&quot;")->equals("\"hello\""));
                // < >
                TS_ASSERT(XmlHelper::decodeAttribute("&lt;test&gt;")->equals("<test>"));
                // ASCII 65 : A
                TS_ASSERT(XmlHelper::decodeAttribute("&#65;")->equals("A"));
                // ASCII 0x41 : A
                TS_ASSERT(XmlHelper::decodeAttribute("&#x41;")->equals("A"));
                }

            /**
             * Test isPublicIdentifierValid()
             */
            void testIsPublicIdentifierValid()
                {
                TS_ASSERT(XmlHelper::isPublicIdentifierValid(" \r\n"));
                TS_ASSERT(XmlHelper::isPublicIdentifierValid("ABCDEFGHIJKLOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));
                TS_ASSERT(XmlHelper::isPublicIdentifierValid("1234567890"));
                TS_ASSERT(XmlHelper::isPublicIdentifierValid("-'()+,./:=?;!*#@$_%"));

                //negative tests
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("["));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("]"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("{"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("}"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("|"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("~"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("^"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("&"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("\\"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("\""));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid("<"));
                TS_ASSERT(!XmlHelper::isPublicIdentifierValid(">"));
                }

            //TODO: Method ALWAYS returns true
            void testIsSystemIdentifierValid()
                {
                TS_ASSERT(XmlHelper::isSystemIdentifierValid("BlahBlah"));
                TS_ASSERT(XmlHelper::isSystemIdentifierValid(""));
                }

            /**
             * Test isNameValid()
             */
            void testIsNameValid()
                {
                TS_ASSERT(XmlHelper::isNameValid("ABCDEFGHIJKLOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));
                TS_ASSERT(XmlHelper::isNameValid("_a"));
                TS_ASSERT(XmlHelper::isNameValid(":a"));
                TS_ASSERT(XmlHelper::isNameValid("A.-_:"));

                //Negative Tests

                // empty string
                TS_ASSERT(!XmlHelper::isNameValid(""));
                // invalid first char
                TS_ASSERT(!XmlHelper::isNameValid("!a"));
                //invalid chars
                TS_ASSERT(!XmlHelper::isNameValid("'()+,/=?;!*#@$%[]{}|~^&\\\"<>"));
                }

            /**
             * Test decodeContent()
             */
            void testdecodeContent()
                {
                // &
                TS_ASSERT(XmlHelper::decodeContent("1&amp;ab")->equals("1&ab"));
                // '
                TS_ASSERT(XmlHelper::decodeContent("&apos;hello&apos;")->equals("'hello'"));
                // ""
                TS_ASSERT(XmlHelper::decodeContent("&quot;hello&quot;")->equals("\"hello\""));
                // < >
                TS_ASSERT(XmlHelper::decodeContent("&lt;test&gt;")->equals("<test>"));

                }

            /**
             * Test trim()
             */
            void testTrim()
                {
                TS_ASSERT(XmlHelper::trim("   asd123\r")->equals("asd123"));
                TS_ASSERT(XmlHelper::trim("   \t\r\n  123\t\r\n ")->equals("123"));
                TS_ASSERT(XmlHelper::trim("   \t\r\n  1\t2    3\t\r\n ")->equals("1\t2    3"));
                }

        /**
        * Test encodeUri
        */
        void testEncodeUri()
            {
            //empty string
            TS_ASSERT(XmlHelper::encodeUri("")->equals(""));
            // no escaping
            TS_ASSERT(XmlHelper::encodeUri("testEncodeUri")->equals("testEncodeUri"));
            // {<>}
            TS_ASSERT(XmlHelper::encodeUri("{xxx<testEncodeUri>123}")->equals("%7bxxx%3ctestEncodeUri%3e123%7d"));
            // "|'
            TS_ASSERT(XmlHelper::encodeUri("val=\"one|'two'\"")->equals("val=%22one%7c%27two%27%22"));
            // ^\% `
            TS_ASSERT(XmlHelper::encodeUri("^32 others\\-`%12")->equals("%5e32%20others%5c-%60%2512"));
            }

        void testBreakLines()
            {
            String::View testLine = "**-* *** *** ***\n* ***** ***** **\n** *** ** ***\n***************";
            String::View expected = "**-* ***\n*** ***\n* *****\n***** **\n** *** **\n***\n**********\n*****";

            TS_ASSERT(Object::equals(expected, XmlHelper::breakLines(testLine, 10)));

            // test indentation
            testLine = "**        **\n    *****";
            expected = "**\n  **\n  ***\n  **";

            TS_ASSERT(Object::equals(expected, XmlHelper::breakLines(testLine, 5, 2)));
            }

        void testToUpperCase()
            {
            String::View vs = String::create("abcDEF123~abc");
            String::View vsExpected = String::create("ABCDEF123~ABC");

            TS_ASSERT(Object::equals(vsExpected, XmlHelper::toUpperCase(vs)));
            }

        void testParseInitParams()
            {
            XmlElement::View vXmlDoc = NULL;
            try {
                std::stringstream ss;
                ss  << "      <init-params>"
                    << "        <init-param>" 
                    << "          <param-type>string</param-type>"
                    << "          <param-value>test</param-value>"
                    << "        </init-param>"
                    << "        <init-param>"
                    << "          <param-type>int</param-type>"
                    << "          <param-value>10</param-value>"
                    << "        </init-param>"
                    << "        <init-param>"
                    << "          <param-type>boolean</param-type>"
                    << "          <param-value>true</param-value>"
                    << "        </init-param>"
                    << "        <init-param>"
                    << "          <param-type>unknown</param-type>"
                    << "          <param-value>20</param-value>"
                    << "        </init-param>"
                    << "      </init-params>";

                vXmlDoc = CacheFactory::loadXml(ss);
                TS_ASSERT(vXmlDoc != NULL);
                }
            catch (Exception::View ve)
                {
                TS_FAIL(ve->getDescription());
                }
            ObjectArray::View vao = XmlHelper::parseInitParams(vXmlDoc);
            TS_ASSERT(vao->length == 4);

            TS_ASSERT((cast<String::View>(vao[0]))->equals("test"));

            TS_ASSERT((cast<Integer32::View>(vao[1]))->getInt32Value() == 10);

            TS_ASSERT((cast<Boolean::View>(vao[2]))->getValue() == true);

            String::View vsTestStr = String::create("<init-param><param-type>unknown</param-type><param-value>20</param-value></init-param>");

            XmlElement::View vXmlEle = cast<XmlElement::View>(vao[3]);
            std::stringstream ssParam;
            vXmlEle->writeXml(ssParam);
            String::View vsUnknownStr = String::create(ssParam.str());

            TS_ASSERT(vsTestStr->equals(vsUnknownStr));
            }

        void testTransformInitParams()
            {
            XmlElement::Handle hParent = SimpleElement::create("parent");
            XmlElement::Handle hElement = SimpleElement::create("test");
            XmlElement::Handle hChild = hElement->addElement("init-param");
            hChild->addElement("param-name")->setString("test-name");
            hChild->addElement("param-value")->setString("test-value");

            XmlHelper::transformInitParams(hParent, hElement);
            XmlElement::Handle hResult = hParent->getElement("test-name");
            TS_ASSERT(hResult != NULL);
            TS_ASSERT(hResult->getString()->equals("test-value"));
            }

        void testReplaceSystemProperties()
            {
            XmlElement::Handle hElement = SimpleElement::create("test");
            String::View vs = "system-property";
            hElement->setString("originalValue");

            XmlElement::Handle hChild = hElement->addElement("test-child");
            hChild->setString("originalValueChild");

            hElement->addAttribute(vs)->setString("prop-value");
            hChild->addAttribute(vs)->setString("prop-value");
            System::setProperty("prop-value", "systemPropertyOverrideValue");

            TS_ASSERT(hElement->getString()->equals("originalValue"));
            TS_ASSERT(hChild->getString()->equals("originalValueChild"));

            XmlHelper::replaceSystemProperties(hElement, vs);

            // attribute should be removed
            TS_ASSERT(NULL == hElement->getAttribute(vs));
            TS_ASSERT(NULL == hChild->getAttribute(vs));

            // check that system property prop-value has been replaced
            TS_ASSERT(hElement->getString()->equals("systemPropertyOverrideValue"));
            TS_ASSERT(hChild->getString()->equals("systemPropertyOverrideValue"));

            System::clearProperty("prop-value");
            }

        void testProcessValueMacro()
            {
            XmlElement::Handle hElement = SimpleElement::create("test");
            XmlElement::Handle hChild   = hElement->addElement("test-child");

            hElement->setString("originalValue${inline.macro.value -default}");
            hChild->setString("${inline.macro.value default-}originalValueChild");

            String::View vs = "system-property";
            hElement->addAttribute(vs)->setString("prop-value");
            hChild->addAttribute(vs)->setString("prop-value");

            // test macro defaulting; macro system property is not set.
            System::clearProperty("prop-value");
            System::clearProperty("inline.macro.value");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(hElement->getString()->equals("originalValue-default"));
            TS_ASSERT(hChild->getString()->equals("default-originalValueChild"));

            // test scenario where macro system property value set
            System::setProperty("inline.macro.value", "MacroReplacementValue");
            hElement->setString("originalValue${inline.macro.value -default}");
            hChild->setString("${inline.macro.value default-}OriginalValueChild");
            hElement->addAttribute(vs)->setString("prop-value");
            hChild->addAttribute(vs)->setString("prop-value");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(NULL == hElement->getAttribute(vs));
            TS_ASSERT(NULL == hChild->getAttribute(vs));
            TS_ASSERT(hElement->getString()->equals("originalValueMacroReplacementValue"));
            TS_ASSERT(hChild->getString()->equals("MacroReplacementValueOriginalValueChild"));
            System::clearProperty("inline.macro.value");

            // test scenario where macro system property is set via camel case environment variable
            System::setProperty("InlineMacroValue", "MacroReplacementValue");
            hElement->setString("originalValue${inline.macro.value -default}");
            hChild->setString("${inline.macro.value default-}OriginalValueChild");
            hElement->addAttribute(vs)->setString("prop-value");
            hChild->addAttribute(vs)->setString("prop-value");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(NULL == hElement->getAttribute(vs));
            TS_ASSERT(NULL == hChild->getAttribute(vs));
            TS_ASSERT(hElement->getString()->equals("originalValueMacroReplacementValue"));
            TS_ASSERT(hChild->getString()->equals("MacroReplacementValueOriginalValueChild"));
            System::clearProperty("InlineMacroValue");

            // test inline macro replaced by system property
            hElement->addAttribute(vs)->setString("prop-value");
            hChild->addAttribute(vs)->setString("prop-value");
            System::setProperty("prop-value", "systemPropertyOverrideValue");
            hElement->setString("originalValue${inline.macro.value -default}");
            hChild->setString("${inline.macro.value default-}originalValueChild");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(hElement->getString()->equals("systemPropertyOverrideValue"));
            TS_ASSERT(hChild->getString()->equals("systemPropertyOverrideValue"));
            System::clearProperty("prop-value");

            // test macro replacement within system property value
            hElement->addAttribute(vs)->setString("prop-value");
            hChild->addAttribute(vs)->setString("prop-value");
            System::setProperty("prop-value", "systemPropertyOverride${inline.macro.value default}");
            System::setProperty("inline.macro.value", "MacroProcessed");
            hElement->setString("originalValue${inline.macro.value -default}");
            hChild->setString("${inline.macro.value default-}originalValueChild");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(hElement->getString()->equals("systemPropertyOverrideMacroProcessed"));
            TS_ASSERT(hChild->getString()->equals("systemPropertyOverrideMacroProcessed"));
            System::clearProperty("prop-value");
            System::clearProperty("inline.macro.value");

            // test malformed macro does not result in a processing failure
            hElement->setString("originalValue${inline.macro.value -default");
            hChild->setString("${inline.macro.value default-OriginalValueChild");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(hElement->getString()->equals("originalValue${inline.macro.value -default"));
            TS_ASSERT(hChild->getString()->equals("${inline.macro.value default-OriginalValueChild"));

            // test multiple different macro processing
            hElement->setString("prefix-${macro1 default1}-${macro2 default2}-post");
            hChild->setString("prefix-${macro1 default1}-${macro2 default2}-post");
            System::setProperty("macro1", "macro1Replacement");
            System::setProperty("macro2", "macro2Replacement");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(hElement->getString()->equals("prefix-macro1Replacement-macro2Replacement-post"));
            TS_ASSERT(hChild->getString()->equals("prefix-macro1Replacement-macro2Replacement-post"));
            System::clearProperty("macro1");
            System::clearProperty("macro2");

            // test multiple of same macro processing
            hElement->setString("prefix-${macro1 default1}-${macro1 default2}-post");
            hChild->setString("prefix-${macro1 default1}-${macro1 default2}-post");
            System::setProperty("macro1", "macro1Replacement");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(hElement->getString()->equals("prefix-macro1Replacement-macro1Replacement-post"));
            TS_ASSERT(hChild->getString()->equals("prefix-macro1Replacement-macro1Replacement-post"));
            System::clearProperty("macro1");

            // test multiple of same macro processing
            hElement->setString("prefix-${macro1 default1}-${macro1 default2}-post");
            hChild->setString("prefix-${macro1 default1}-${macro1 default2}-post");
            System::clearProperty("macro1");
            XmlHelper::replaceSystemProperties(hElement, vs);
            TS_ASSERT(hElement->getString()->equals("prefix-default1-default2-post"));
            TS_ASSERT(hChild->getString()->equals("prefix-default1-default2-post"));
            System::clearProperty("macro1");
            }

        void testReplaceElement()
            {
            XmlElement::Handle hElement = SimpleElement::create("test");
            XmlElement::Handle hReplace = SimpleElement::create("replace");
            hReplace->setString("replaced");

            XmlElement::Handle hChild1 = hElement->addElement("replace");
            XmlElement::Handle hChild2 = hElement->addElement("dont-replace");
            hChild2->setString("not-replaced");
            XmlElement::Handle hChild3 = hElement->addElement("replace");

            XmlHelper::replaceElement(hElement, hReplace);

            Iterator::Handle hIter = hElement->getElements("replace");
            XmlElement::View vChangedChild =
                    cast<XmlElement::View>(hIter->next());
            TS_ASSERT(vChangedChild->getString()->equals("replaced"));
            vChangedChild = cast<XmlElement::View>(hIter->next());

            //TODO: Look at why ony first element encountered is modified

            //TS_ASSERT(vChangedChild->getString()->equals("replaced"));
            //TS_ASSERT(!hIter->hasNext());

            TS_ASSERT(hElement->getElement(
                    "dont-replace")->getString()->equals("not-replaced"));
            }

        void testIsEmpty()
            {
            XmlElement::Handle hElement = SimpleElement::create("test");
            TS_ASSERT(XmlHelper::isEmpty(hElement));
            hElement->setString("test");
            TS_ASSERT(! XmlHelper::isEmpty(hElement));

            hElement = SimpleElement::create("test");
            hElement->addAttribute("test-attr");
            TS_ASSERT(! XmlHelper::isEmpty(hElement));

            hElement = SimpleElement::create("test");
            hElement->addElement("test-element");
            TS_ASSERT(! XmlHelper::isEmpty(hElement));
            }

        void testConvert()
            {
            String::View vs = String::create("123");
            Integer32::View vIConverted = cast<Integer32::View>(XmlHelper::convert(vs, XmlValue::type_int));
            TS_ASSERT(vIConverted->getValue() == 123);
            
            size32_t size = 8;
            Array<octet_t>::Handle hab  = Array<octet_t>::create(size);
            for (size32_t i = 0; i < size; ++i)
                {
                hab[i] = (octet_t) 'a';
                }
            Binary::View vBin = Binary::create(hab, 0);
            String::View vsConverted = cast<String::View>(XmlHelper::convert(vBin, XmlValue::type_string));
            TS_ASSERT(vsConverted->equals("aaaaaaaa"));
            }
    };
