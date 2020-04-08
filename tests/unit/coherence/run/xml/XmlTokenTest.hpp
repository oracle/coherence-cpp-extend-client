/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "private/coherence/run/xml/XmlToken.hpp"

#include <stdio.h>

using namespace coherence::lang;
using namespace coherence::run::xml;
using namespace std;

/**
* Test Suite for the XmlToken object.
*/
class XmlTokenSuite : public CxxTest::TestSuite
    {
public:

    /**
    * Test getCategory
    */
    void testGetCategory()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( token->getCategory()== XmlToken::cat_name);

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( token->getCategory()== XmlToken::cat_name);

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"), 0, 1, 2);
        TS_ASSERT( token->getCategory()== XmlToken::cat_literal);
        }

    /**
    * Test getSubCategory
    */
    void testGetSubCategory()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( token->getSubCategory()== XmlToken::subcat_lit_pi);

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( token->getSubCategory()== XmlToken::subcat_lit_pi);

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::name, String::create("Value"), String::create("Text"), 0, 1, 2);
        TS_ASSERT( token->getSubCategory()== XmlToken::subcat_lit_comment);
        }


    /**
    * Test getId
    */
    void testGetId()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( token->getID()== XmlToken::name);

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( token->getID()== XmlToken::name);

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::literal, String::create("Value"), String::create("Text"), 0, 1, 2);
        TS_ASSERT( token->getID()== XmlToken::literal);
        }


    /**
    * Test getValue
    */
    void testGetValue()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( String::create("Value")->equals(token->getValue()));

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( String::create("Value")->equals(token->getValue()));

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::literal, String::create("Another Value"), String::create("Text"), 0, 1, 2);
        TS_ASSERT( String::create("Another Value")->equals(token->getValue()) );
        }


    /**
    * Test getText
    */
    void testGetText()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( String::create("Text")->equals(token->getText()));

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( String::create("Text")->equals(token->getText()));

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::literal, String::create("Another Value"), String::create("More Text"), 0, 1, 2);
        TS_ASSERT( String::create("More Text")->equals(token->getText()) );
        }

    /**
    * Test getLine
    */
    void testGetLine()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( token->getLine()== 0);

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( token->getLine()== 0);

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::literal, String::create("Another Value"), String::create("More Text"), 99, 1, 2);
        TS_ASSERT( token->getLine()== 99);
        }

    /**
    * Test getOffset
    */
    void testGetOffset()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( token->getOffset()== 0);

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( token->getOffset()== 1);

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::literal, String::create("Another Value"), String::create("More Text"), 0, 99, 2);
        TS_ASSERT( token->getOffset()== 99);
        }


    /**
    * Test getLength
    */
    void testGetLength()
        {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_name, XmlToken::subcat_lit_pi, XmlToken::name, String::create("Value"), String::create("Text"));
        TS_ASSERT( token->getLength()== 0);

        token = XmlToken::create(token, 0, 1, 2);
        TS_ASSERT( token->getLength()== 2);

        token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::literal, String::create("Another Value"), String::create("More Text"), 0, 1, 99);
        TS_ASSERT( token->getLength()== 99);
        }

    /**
    * Test adjust
    */
    void testAdjust() {
        XmlToken::Handle token = XmlToken::create(XmlToken::cat_literal, XmlToken::subcat_lit_comment, XmlToken::literal, String::create("Another Value"), String::create("More Text"), 0, 1, 99);
        token->adjust( 10, 10 );

        TS_ASSERT( token->getLine()== 10);
        TS_ASSERT( token->getOffset()== 11);

        token->adjust( -2, -2 );

        TS_ASSERT( token->getLine()== 8);
        TS_ASSERT( token->getOffset()== 9);

        token->adjust( -10, -10 );

        TS_ASSERT( token->getLine()== 0);
        TS_ASSERT( token->getOffset()== 0);
    }

};



