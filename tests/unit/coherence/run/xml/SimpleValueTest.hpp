/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/run/xml/SimpleValue.hpp"

#include <sstream>
#include <stdio.h>
#include <string.h>

using namespace coherence::lang;
using namespace coherence::run::xml;
using namespace std;


/**
* Test Suite for the SimpleValue object.
*/
class SimpleValueSuite : public CxxTest::TestSuite
    {
    public:

        /**
        * Test setBooleanean & getBooleanean
        */
        void testSetGetBooleanean()
        {
        SimpleValue::Handle value = SimpleValue::create();

        TS_ASSERT(!value->getBoolean());

        value->setBoolean(true);
        TS_ASSERT(value->getBoolean());

        value->setBoolean(false);
        TS_ASSERT(!value->getBoolean());

        value->setString("1");
        TS_ASSERT(value->getBoolean());

        value->setString("99");
        TS_ASSERT(!value->getBoolean());
        TS_ASSERT(value->getBoolean(true));
        TS_ASSERT(!value->getBoolean(false));

        value->setString("0");
        TS_ASSERT(!value->getBoolean());

        value->setString("t");
        TS_ASSERT(value->getBoolean());

        value->setString("T");
        TS_ASSERT(value->getBoolean());

        value->setString("true");
        TS_ASSERT(value->getBoolean());

        value->setString("True");
        TS_ASSERT(value->getBoolean());

        value->setString("TRUE");
        TS_ASSERT(value->getBoolean());

        value->setString("y");
        TS_ASSERT(value->getBoolean());

        value->setString("Y");
        TS_ASSERT(value->getBoolean());

        value->setString("yes");
        TS_ASSERT(value->getBoolean());

        value->setString("YES");
        TS_ASSERT(value->getBoolean());

        value->setString("f");
        TS_ASSERT(!value->getBoolean());

        value->setString("F");
        TS_ASSERT(!value->getBoolean());

        value->setString("false");
        TS_ASSERT(!value->getBoolean());

        value->setString("fAlse");
        TS_ASSERT(!value->getBoolean());

        value->setString("FALSE");
        TS_ASSERT(!value->getBoolean());

        value->setString("n");
        TS_ASSERT(!value->getBoolean());

        value->setString("N");
        TS_ASSERT(!value->getBoolean());

        value->setString("no");
        TS_ASSERT(!value->getBoolean());

        value->setString("NO");
        TS_ASSERT(!value->getBoolean());

        value->setString("monkey");
        TS_ASSERT(!value->getBoolean());

        value = SimpleValue::create( true );
        TS_ASSERT(value->getBoolean());

        value = SimpleValue::create(true, true);
        TS_ASSERT(value->getBoolean());

        value = SimpleValue::create( true, false);
        TS_ASSERT(value->getBoolean());
        }

        /**
        * Test setInt32 & getInt32
        */
        void testSetgetInt32()
        {
        int32_t i = 99;
        SimpleValue::Handle value = SimpleValue::create();

        TS_ASSERT(value->getInt32()==0);

        value->setInt32(i);
        TS_ASSERT(value->getInt32()==99);

        value->setString("100");
        TS_ASSERT(value->getInt32()==100);

        value->setString("-100");
        TS_ASSERT(value->getInt32()==-100);

        value->setString("foo");
        TS_ASSERT(value->getInt32()==0);

        value->setBoolean(true);
        TS_ASSERT(value->getInt32()==1);

        value->setBoolean(false);
        TS_ASSERT(value->getInt32()==0);


        value = SimpleValue::create(i);
        TS_ASSERT(value->getInt32()==99);

        value = SimpleValue::create(i, true);
        TS_ASSERT(value->getInt32()==99);

        value = SimpleValue::create(i, false);
        TS_ASSERT(value->getInt32()==99);
        }

        /**
        * Test setString & getString
        */
        void testSetGetString()
        {
        SimpleValue::Handle value = SimpleValue::create();

        TS_ASSERT(value->getString()->equals(""));

        value->setString("foo");
        TS_ASSERT(value->getString()->equals("foo"));

        value->setString("bar");
        TS_ASSERT(value->getString()->equals("bar"));

        value->setBoolean(false);
        TS_ASSERT(value->getString()->equals("false"));

        value->setBoolean(true);
        TS_ASSERT(value->getString()->equals("true"));


        value->setInt32(99);
        TS_ASSERT(value->getString()->equals("99"));

        value->setInt32(-99);
        TS_ASSERT(value->getString()->equals("-99"));

        value->setInt32(0);
        TS_ASSERT(value->getString()->equals("0"));


        value = SimpleValue::create(String::create("monkey"));
        TS_ASSERT(value->getString()->equals("monkey"));

        value = SimpleValue::create(String::create("runner"), true);
        TS_ASSERT(value->getString()->equals("runner"));
        }



        /**
        * Test setParent/getParent
        */
        void testSetParent()
        {
        SimpleValue::Handle value = SimpleValue::create();
        TS_ASSERT(value->getParent() == NULL);
        // try to set a NULL parent
        TS_ASSERT_THROWS(value->setParent(NULL),
                         IllegalArgumentException::View);

        XmlElement::Handle parent = SimpleElement::create();
        value->setParent(parent);
        TS_ASSERT(NULL != value->getParent());
        TS_ASSERT(value->getParent()->equals(parent));

        // try to reset parent
        TS_ASSERT_THROWS(value->setParent(SimpleElement::create()),
                         IllegalStateException::View);
        }


        /**
        * Test getValue
        */
        void testGetValue()
        {
        SimpleValue::Handle value = SimpleValue::create();
        TS_ASSERT(value->getValue()==NULL);

        String::Handle stringValue = String::create("foo");
        value->setString(stringValue);
        TS_ASSERT(NULL != value->getValue());
        TS_ASSERT(value->getValue()->equals(stringValue));

        Integer32::Handle intValue = Integer32::create(99);
        value->setInt32(99);
        TS_ASSERT(NULL != value->getValue());
        TS_ASSERT(value->getValue()->equals(intValue));

        Boolean::Handle boolTrueValue = Boolean::create(true);

        value->setBoolean(true);
        TS_ASSERT(NULL != value->getValue());

        TS_ASSERT(boolTrueValue->equals(value->getValue()));
        TS_ASSERT(value->getValue()->equals(boolTrueValue));

        Boolean::Handle boolFalseValue = Boolean::create(false);
        value->setBoolean(false);
        TS_ASSERT(NULL != value->getValue());
        TS_ASSERT(value->getValue()->equals(boolFalseValue));
        }


        /**
        * Test isEmpty
        */
        void testIsEmpty()
        {
        SimpleValue::Handle value = SimpleValue::create();
        TS_ASSERT(value->isEmpty());

        value->setBoolean(true);
        TS_ASSERT(!value->isEmpty());

        value = SimpleValue::create();
        TS_ASSERT(value->isEmpty());

        value->setInt32(99);
        TS_ASSERT(!value->isEmpty());

        value = SimpleValue::create();
        TS_ASSERT(value->isEmpty());

        value->setString("foo");
        TS_ASSERT(!value->isEmpty());
        }


        /**
        * Test isAttribute
        */
        void testIsAttribute()
        {
        SimpleValue::Handle value = SimpleValue::create();
        TS_ASSERT(!value->isAttribute());

        value = SimpleValue::create(String::create("foo"));
        TS_ASSERT(!value->isAttribute());

        value = SimpleValue::create(String::create("foo"), false);
        TS_ASSERT(!value->isAttribute());

        value = SimpleValue::create(String::create("foo"), true);
        TS_ASSERT(value->isAttribute());

        int32_t i = 99;
        value = SimpleValue::create(i, false);
        TS_ASSERT(!value->isAttribute());

        value = SimpleValue::create(i, true);
        TS_ASSERT(value->isAttribute());

        value = SimpleValue::create(true, false);
        TS_ASSERT(!value->isAttribute());

        value = SimpleValue::create(true, true);
        TS_ASSERT(value->isAttribute());

        }

        /**
        * Test isContent
        */
        void testIsContent()
        {
        SimpleValue::Handle value = SimpleValue::create();
        TS_ASSERT(value->isContent());

        value = SimpleValue::create(String::create("foo"));
        TS_ASSERT(value->isContent());

        value = SimpleValue::create(String::create("foo"), false);
        TS_ASSERT(value->isContent());

        value = SimpleValue::create(String::create("foo"), true);
        TS_ASSERT(!value->isContent());

        int32_t i = 99;
        value = SimpleValue::create(i, false);
        TS_ASSERT(value->isContent());

        value = SimpleValue::create(i, true);
        TS_ASSERT(!value->isContent());

        value = SimpleValue::create(true, false);
        TS_ASSERT(value->isContent());

        value = SimpleValue::create(true, true);
        TS_ASSERT(!value->isContent());

        }

    /**
    * Test writeValue for different value types
    */
    void testWriteValue()
        {
        stringstream ss;
        //string type val should be encoded
        SimpleValue::Handle value = SimpleValue::create(String::create("test value "));

        value->writeValue(ss);
        TS_ASSERT(ss.str() == "<![CDATA[test value ]]>");
        // reset ss
        ss.str("");

        //attribute should be quoted
        value = SimpleValue::create(String::create("test"), true);
        value->writeValue(ss);
        TS_ASSERT(ss.str() == "'test'");
        // reset ss
        ss.str("");

        //int val
        int32_t i = 500;
        value = SimpleValue::create(i);
        value->writeValue(ss);
        TS_ASSERT(ss.str() == "500");
        // reset ss
        ss.str("");

        //boolean val
        value = SimpleValue::create(true);
        value->writeValue(ss);
        TS_ASSERT(ss.str() == "true");
        }

        void testEquals()
            {
            SimpleValue::Handle h1 = SimpleValue::create();
            SimpleValue::Handle h2 = SimpleValue::create();
            TS_ASSERT(h1->equals(h2));
            TS_ASSERT(h2->equals(h1));
            TS_ASSERT(h1->equals(h1));

            // empty value is same as ""
            h1->setString("");
            TS_ASSERT(h1->equals(h2));
            TS_ASSERT(h2->equals(h1));

            h1 = SimpleValue::create(String::create("test-string"));
            h2 = SimpleValue::create(String::create("test-string"));
            TS_ASSERT(h1->equals(h2));
            TS_ASSERT(h2->equals(h1));

            // attribute flag is not considered in equals
            h2 = SimpleValue::create(String::create("test-string"), true);
            TS_ASSERT(h1->equals(h2));
            TS_ASSERT(h2->equals(h1));

            h2 = SimpleValue::create(String::create("test-string2"));
            TS_ASSERT(! h1->equals(h2));
            TS_ASSERT(! h2->equals(h1));

            String::View vs1 = String::create("test");
            String::View vs2 = String::create("test");
            h1 = SimpleValue::create(vs1);
            h2 = SimpleValue::create(vs2);

            TS_ASSERT(h1->equals(h2));
            TS_ASSERT(h2->equals(h1));

            vs2 = String::create("test2");
            h2->setString(vs2);
            TS_ASSERT(! h1->equals(h2));
            TS_ASSERT(! h2->equals(h1));

            int32_t n = 100;
            h1 = SimpleValue::create(n);
            h2 = SimpleValue::create(n);

            TS_ASSERT(h1->equals(h2));
            TS_ASSERT(h2->equals(h1));

            n = 101;
            h2->setInt32(n);
            TS_ASSERT(! h1->equals(h2));
            TS_ASSERT(! h2->equals(h1));

            h1 = SimpleValue::create(false);
            h2 = SimpleValue::create(false);

            TS_ASSERT(h1->equals(h2));
            TS_ASSERT(h2->equals(h1));

            h2->setBoolean(true);
            TS_ASSERT(! h1->equals(h2));
            TS_ASSERT(! h2->equals(h1));
            }

        void testHashCode()
            {
            SimpleValue::Handle h1 = SimpleValue::create();
            SimpleValue::Handle h2 = SimpleValue::create();
            TS_ASSERT(h1->hashCode() == h2->hashCode());

            h1 = SimpleValue::create(String::create("test"), false);
            h2 = SimpleValue::create(String::create("test"), false);
            TS_ASSERT(h1->hashCode() == h2->hashCode());

            h1 = SimpleValue::create(false, true);
            h2 = SimpleValue::create(false, true);
            TS_ASSERT(h1->hashCode() == h2->hashCode());

            int32_t n = 500;
            h1 = SimpleValue::create(n, true);
            h2 = SimpleValue::create(n, true);
            TS_ASSERT(h1->hashCode() == h2->hashCode());
            }

        void testClone()
            {
            // NULL parent
            SimpleValue::Handle h = SimpleValue::create(String::create("test"), false);
            SimpleElement::Handle hParent = SimpleElement::create(String::create("parent"));
            SimpleValue::Handle hClone = cast<SimpleValue::Handle>(h->clone());
            TS_ASSERT(hClone->equals(h));
            TS_ASSERT(hClone != h);

            // set parent
            h->setParent(hParent);
            hClone = cast<SimpleValue::Handle>(h->clone());
            TS_ASSERT(hClone->equals(h));
            TS_ASSERT(hClone != h);
            }

        void testToString()
            {
            SimpleValue::View v = SimpleValue::create(true, true);
            TS_ASSERT(v->toString()->equals("'true'"));

            v = SimpleValue::create(String::create("test"), false);
            TS_ASSERT(v->toString()->equals("test"));
            }
    };

