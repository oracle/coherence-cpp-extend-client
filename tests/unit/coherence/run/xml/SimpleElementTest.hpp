/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"

#include "private/coherence/run/xml/SimpleElement.hpp"

#include <sstream>
#include <string.h>
#include <iostream>

using namespace coherence::lang;
using namespace coherence::run::xml;
using namespace std;

using coherence::util::Iterator;
using coherence::util::List;

/**
* Test Suite for SimpleElement
*/
class SimpleElementTestSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test that getName() returns the proper name
        */
        void testGetName()
            {
            XmlElement::Handle se = SimpleElement::create();
            TS_ASSERT(NULL == se->getName());

            XmlElement::Handle se2 = SimpleElement::create("test-name");
            TS_ASSERT(se2->getName()->equals("test-name"));
            }

        /**
        * Create a SimpleElement without a name, set the name and verify that the name is set correctly.
        */
        void testSetName()
            {
            XmlElement::Handle se = SimpleElement::create();
            TS_ASSERT(NULL == se->getName());

            se->setName("test-name");
            TS_ASSERT(se->getName()->equals("test-name"));

            //todo: test invalid name
            //todo: test non-mutable
            }

        /**
        * Make sure that getRoot returns the correct root element.
        */
        void testGetRoot()
            {
            XmlElement::Handle parent = SimpleElement::create();
            TS_ASSERT(parent->getRoot()->equals(parent));

            XmlElement::Handle child = SimpleElement::create();
            child->setParent(parent);
            TS_ASSERT(child->getRoot()->equals(parent));
            XmlElement::Handle child2 = child->addElement("child2");
            TS_ASSERT(child2->getRoot()->equals(parent));
            }

        /**
        * Test that getElementList returns the list of all child elements.
        */
        void testGetAllElements()
            {
            XmlElement::Handle se = SimpleElement::create();
            TS_ASSERT(se->getAllElements()->length == 0);
            se->addElement("child1");
            se->addElement("child2");
            se->addElement("child3");

            TS_ASSERT(se->getAllElements()->length == 3);
            }

        /**
        * Test that addElement adds a new child element and properly
        * sets its parent.
        *
        * *Note: The parent was not set on the child in the Java
        * implementation.
        */
        void testAddElement()
            {
            XmlElement::Handle se = SimpleElement::create();
            XmlElement::Handle child = se->addElement("child");

            TS_ASSERT(child->equals(se->getElement("child")));
            TS_ASSERT(se->equals(child->getParent()));
            }

        /**
        * Test that getElement will return an child element from the child element list.
        */
        void testGetElement()
            {
            XmlElement::Handle se = SimpleElement::create();

            XmlElement::Handle ce1 = se->addElement("child1");
            XmlElement::Handle ce2 = se->addElement("child2");
            XmlElement::Handle ce3 = se->addElement("child3");

            TS_ASSERT(ce1->equals(se->getElement("child1")));
            TS_ASSERT(ce2->equals(se->getElement("child2")));
            TS_ASSERT(ce3->equals(se->getElement("child3")));

            TS_ASSERT(NULL == se->getElement("bogus-name"));
            }

        /**
        * Test that getElement will return an child element from the child element list.
        */
        void testGetSafeElement()
            {
            XmlElement::Handle se = SimpleElement::create();

            XmlElement::Handle ce1 = se->addElement("child1");
            XmlElement::Handle ce2 = se->addElement("child2");
            XmlElement::Handle ce3 = se->addElement("child3");

            TS_ASSERT(ce1->equals(se->getSafeElement("child1")));
            TS_ASSERT(ce2->equals(se->getSafeElement("child2")));
            TS_ASSERT(ce3->equals(se->getSafeElement("child3")));
            XmlElement::View vElement = se->getSafeElement("child4");
            TS_ASSERT(NULL != se->getSafeElement("child4"));

            TS_ASSERT(NULL == se->getElement("bogus-name"));
            }

        void testFindElement()
            {
            XmlElement::Handle parent = SimpleElement::create();
            String::View s;
            // pass in NULL and ensure exception is thrown
            TS_ASSERT_THROWS(parent->findElement(s),
                             IllegalArgumentException::View);

            s = String::create("");
            // pass in "" and ensure exception is thrown
            TS_ASSERT_THROWS(parent->findElement(s),
                             IllegalArgumentException::View);

            XmlElement::Handle child = parent->addElement("child");
            XmlElement::Handle grandChild = child->addElement("grandChild");
            XmlElement::Handle grandChild2 = child->addElement("grandChild2");
            TS_ASSERT(parent->equals(child->findElement("/")));
            TS_ASSERT(parent->equals(grandChild->findElement("/")));

            TS_ASSERT(child->equals(parent->findElement("child")));
            TS_ASSERT(grandChild->equals(parent->findElement("child/grandChild")));
            TS_ASSERT(grandChild2->equals(parent->findElement("child/grandChild2")));
            //negative test
            TS_ASSERT(NULL == parent->findElement("child/grandChild3"));

            TS_ASSERT(child->equals(parent->findElement("/child")));
            TS_ASSERT(grandChild->equals(parent->findElement("/child/grandChild")));
            TS_ASSERT(grandChild2->equals(parent->findElement("/child/grandChild2")));

            TS_ASSERT(parent->equals(child->findElement("..")));
            TS_ASSERT(parent->equals(grandChild->findElement("../..")));

            TS_ASSERT(grandChild->equals(grandChild->findElement("../../child/grandChild")));
            TS_ASSERT(grandChild->equals(grandChild->findElement("/child/grandChild")));

            //pass in illegal path
            TS_ASSERT_THROWS(child->findElement("../.."),
                         IllegalArgumentException::View);
            }


        void testGetAttribute()
            {
            XmlElement::Handle se = SimpleElement::create("test-name");
            TS_ASSERT(NULL == se->getAttribute("add-attribute"));

            XmlValue::Handle e = se->addAttribute("add-attribute");
            TS_ASSERT(se->getAttribute("add-attribute")->equals(e));
            //todo: test passing in invalid name
            }

        void testSetAttribute()
            {
            XmlElement::Handle se = SimpleElement::create("test-name");
            TS_ASSERT(NULL == se->getAttribute("attribute-1"));

            XmlElement::Handle e = SimpleElement::create("attribute-1");
            se->setAttribute("attribute-1", e);
            TS_ASSERT(se->getAttribute("attribute-1")->equals(e));

            // update attribute
            e = SimpleElement::create("attribute-updated");
            se->setAttribute("attribute-1", e);
            TS_ASSERT(se->getAttribute("attribute-1")->equals(e));

            //remove by passing in null
            se->setAttribute("attribute-1", NULL);
            TS_ASSERT(NULL == se->getAttribute("attribute-1"));
            //todo: test passing in invalid name
            }

        void testAddAttribute()
            {
            XmlElement::Handle se = SimpleElement::create("test-name");
            TS_ASSERT(NULL == se->getAttribute("add-attribute"));

            XmlValue::Handle e = se->addAttribute("add-attribute");
            TS_ASSERT(NULL != e);
            TS_ASSERT(se->getAttribute("add-attribute")->equals(e));
            TS_ASSERT(e->isAttribute());
            TS_ASSERT(e->getString()->length() == 0);
            //todo: test passing in invalid name
            }

        void testSetComment()
            {
            XmlElement::Handle se = SimpleElement::create("test-name");
            TS_ASSERT(NULL == se->getComment());
            se->setComment("test comment");
            TS_ASSERT(NULL != se->getComment());
            TS_ASSERT(se->getComment()->equals("test comment"));

            // pass in illegal comment and asset exception
            TS_ASSERT_THROWS(se->setComment("--badValue"),
                             IllegalArgumentException::View);
            }

        void testGetAbsolutePath()
            {
            XmlElement::Handle parent = SimpleElement::create("parent");
            TS_ASSERT(parent->getAbsolutePath()->equals("/parent"));

            // don't provide name
            XmlElement::Handle child = SimpleElement::create();
            child->setParent(parent);

            XmlElement::Handle child2 = SimpleElement::create("child2");
            child2->setParent(child);

            TS_ASSERT(child2->getAbsolutePath()->equals("/parent/NULL/child2"));
            }

        void testCreate()
            {
            XmlElement::Handle se = SimpleElement::create(
                    String::create("test-name"), String::create("string-val"));
            TS_ASSERT(se->getString()->equals("string-val"));

            se = SimpleElement::create(String::create("test-name"), false);
            TS_ASSERT(se->getBoolean() == false);
            TS_ASSERT(se->getInt32() == 0);
            TS_ASSERT(se->getString()->equals("false"));

            int32_t i = 1;
            se = SimpleElement::create(String::create("test-name"), i);
            TS_ASSERT(se->getInt32() == 1);
            TS_ASSERT(se->getString()->equals("1"));
            TS_ASSERT(se->getBoolean());

            String::View str = String::create("500");
            se = SimpleElement::create("test-name", str);
            TS_ASSERT(se->getString()->equals("500"));
            TS_ASSERT(se->getInt32() == 500);
            TS_ASSERT(!se->getBoolean());
            TS_ASSERT( se->getBoolean(true));

            se = SimpleElement::create(String::create("test-name"), String::create("500"));
            TS_ASSERT(se->getString()->equals("500"));
            TS_ASSERT(se->getInt32() == 500);
            TS_ASSERT(!se->getBoolean());
            }

        /**
        * Test writeXml for an element with no comment, no value and no children
        */
        void testWriteXmlEmpty()
            {
            SimpleElement::Handle element = SimpleElement::create(String::create("element-name"));
            element->addAttribute("test-attribute");
            stringstream ss;
            element->writeXml(ss, false);
            TS_ASSERT(ss.str() == "<element-name test-attribute=''/>");
            }

        /**
        * Test writeXml() for an element with a value, attribute and
        * comment.
        */
        void testWriteXml()
            {
            SimpleElement::Handle element = SimpleElement::create(String::create("element"), String::create("value"));
            element->setComment("a comment");
            XmlValue::Handle attribute = element->addAttribute("foo");
            attribute->setString("foovalue");

            stringstream ss;
            element->writeXml(ss, false);
            TS_ASSERT(ss.str() == "<element foo='foovalue'><!--a comment-->value</element>");

            }

        /**
        * Test writeXml() for an element with an empty child
        */
        void testWriteXmlWithChildren()
            {
            SimpleElement::Handle element = SimpleElement::create(String::create("element"), String::create("value"));
            element->setComment("a comment");
            element->addElement( "childelement");

            stringstream ss;
            element->writeXml(ss, false);
            TS_ASSERT(ss.str() == "<element><!--a comment-->value<childelement/></element>");
            }

        /**
        * Test writeValue() properly writes the elements value.
        * writeValue() is fully tested in SimpleValueTest.
        */
        void testWriteValue()
            {
            SimpleElement::Handle element = SimpleElement::create(String::create("element"), String::create("value"));

            stringstream ss;
            element->writeValue(ss);
            TS_ASSERT(ss.str() == "value");
            }

        void testGetElementList()
            {
            SimpleElement::Handle h = SimpleElement::create(String::create("name"));
            TS_ASSERT(h->getElementList()->size() == 0);

            XmlElement::Handle hOne = h->addElement("one");
            XmlElement::Handle hTwo = h->addElement("two");

            List::Handle hList = h->getElementList();
            TS_ASSERT(hList->size() == 2);

            TS_ASSERT(hList->get(0) == hOne);
            TS_ASSERT(hList->get(1) == hTwo);
            }

        void testGetElements()
            {
            SimpleElement::Handle h = SimpleElement::create(String::create("name"));
            Iterator::Handle hIter = h->getElements("one");
            TS_ASSERT(!hIter->hasNext());

            XmlElement::Handle hOne   = h->addElement("one");
            XmlElement::Handle hTwo   = h->addElement("two");
            XmlElement::Handle hOne2  = h->addElement("one");
            XmlElement::Handle hThree = h->addElement("three");
            XmlElement::Handle hOne3  = h->addElement("one");

            hIter = h->getElements("one");
            TS_ASSERT(hIter->hasNext());
            XmlElement::View v = cast<XmlElement::View>(hIter->next());
            TS_ASSERT(v == hOne);
            TS_ASSERT(hIter->next() == hOne2);
            TS_ASSERT(hIter->next() == hOne3);
            TS_ASSERT(!hIter->hasNext());

            hIter = h->getElements("four");
            TS_ASSERT(!hIter->hasNext());
            }

        void testEnsureElement()
            {
            SimpleElement::Handle h = SimpleElement::create(String::create("name"));
            // ensure NULL path throws exception
            TS_ASSERT_THROWS(h->ensureElement(NULL),
                             IllegalArgumentException::View);

            XmlElement::Handle parent = SimpleElement::create();
            XmlElement::Handle child = parent->addElement("child");
            XmlElement::Handle grandChild = child->addElement("grandChild");
            XmlElement::Handle grandChild2 = child->addElement("grandChild2");
            TS_ASSERT(parent->equals(child->ensureElement("/")));
            TS_ASSERT(parent->equals(grandChild->ensureElement("/")));

            TS_ASSERT(child->equals(parent->ensureElement("child")));
            TS_ASSERT(grandChild->equals(parent->ensureElement("child/grandChild")));
            TS_ASSERT(grandChild2->equals(parent->ensureElement("child/grandChild2")));

            // create new leaf
            XmlElement::Handle hGrandChild3 = parent->ensureElement("child/grandChild3");
            TS_ASSERT(parent->ensureElement("child/grandChild3") == hGrandChild3);

            // create new branch
            XmlElement::Handle hNewBranch = parent->ensureElement("child2/grandChild1/greatGrandChild1");
            TS_ASSERT(parent->ensureElement("child2/grandChild1/greatGrandChild1") == hNewBranch);

            TS_ASSERT(child->equals(parent->ensureElement("/child")));
            TS_ASSERT(grandChild->equals(parent->ensureElement("/child/grandChild")));
            TS_ASSERT(grandChild2->equals(parent->ensureElement("/child/grandChild2")));

            TS_ASSERT(parent->equals(child->ensureElement("..")));
            TS_ASSERT(parent->equals(grandChild->ensureElement("../..")));

            TS_ASSERT(grandChild->equals(grandChild->ensureElement("../../child/grandChild")));
            TS_ASSERT(grandChild->equals(grandChild->ensureElement("/child/grandChild")));

            //pass in illegal path
            TS_ASSERT_THROWS(child->ensureElement("../.."),
                         IllegalArgumentException::View);

            }

        void testGetAttributeMap()
            {
            XmlElement::Handle se = SimpleElement::create(String::create("test-name"));
            HashMap::Handle hMap = se->getAttributeMap();
            TS_ASSERT(hMap->size() == 0);

            XmlValue::Handle hAttr = se->addAttribute("add-attribute");
            hMap = se->getAttributeMap();
            TS_ASSERT(hMap->size() == 1);
            TS_ASSERT(hMap->get((String::View) "add-attribute") == hAttr);
            }

        void testGetAttributeMapConst()
            {
            XmlElement::Handle se = SimpleElement::create(String::create("test-name"));
            HashMap::Handle hMap = se->getAttributeMap();
            TS_ASSERT(hMap->size() == 0);

            XmlValue::Handle hAttr = se->addAttribute("add-attribute");
            XmlElement::View v = se;
            HashMap::View vMap = v->getAttributeMap();
            TS_ASSERT(vMap->size() == 1);
            XmlValue::View vAttr = cast<XmlValue::View>(
                    vMap->get((String::View) "add-attribute"));
            TS_ASSERT(vAttr->equals(hAttr));
            // ensure views are returned
            try
                {
                cast<XmlValue::Handle>(vMap->get((String::View) "add-attribute"));
                TS_FAIL("Should not be a handle");
                }
            catch (...)
                {
                // expected
                }
            }

        void testClone()
            {
            XmlElement::Handle se = SimpleElement::create(String::create("test-name"));
            se->setString("test");
            se->setComment("This is a comment");
            se->addElement("child");
            se->addElement("child2");
            se->addAttribute("attribute");
            XmlValue::Handle hAttr = se->addAttribute("attribute2");
            hAttr->setString("value");

            XmlElement::Handle hClone = cast<XmlElement::Handle>(se->clone());

            TS_ASSERT(hClone != se);
            TS_ASSERT(hClone->equals(se));
            }
    };
