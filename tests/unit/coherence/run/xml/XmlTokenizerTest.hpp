/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "private/coherence/run/xml/XmlTokenizer.hpp"
#include "private/coherence/run/xml/XmlScript.hpp"

#include <stdio.h>

using namespace coherence::run::xml;
using namespace std;

/**
 * Test Suite for the XmlTokenizer object.
 */
class XmlTokenizerSuite : public CxxTest::TestSuite
  {
public:

    /**
     * Test setScript
     */
    void testSetScript()
      {
        XmlScript::Handle script =
            XmlScript::create("<element>value</element>");

        ErrorList::Handle errList = ErrorList::create();

        XmlTokenizer::Handle tokenizer = XmlTokenizer::create();
        TS_ASSERT(!tokenizer->hasMoreTokens());

        tokenizer->setScript(script, errList);
        // Do a simple check to see if the correct script and error list are set.
        int32_t tokenCount = 0;
        while (tokenizer->hasMoreTokens() )
          {
            tokenizer->nextToken();
            tokenCount++;
          }
        //There should be 7 tokens in the script
        TS_ASSERT(tokenCount==7);

        try
          {
            tokenizer->setScript(script, errList);
            //Shouldn't be able to reset the script for a tokenizer
            TS_ASSERT(false);
          }
        catch (IllegalStateException::View)
          {
            //expected
          }
      }

    /**
     * Test hasMoreTokens
     */
    void testHasMoreTokens()
      {
        XmlScript::Handle script = XmlScript::create("<element>");
        ErrorList::Handle errList = ErrorList::create();

        XmlTokenizer::Handle tokenizer = XmlTokenizer::create();

        // There should be 3 tokens for the above script.
        tokenizer->setScript(script, errList);
        TS_ASSERT(tokenizer->hasMoreTokens() );
        tokenizer->nextToken();
        TS_ASSERT(tokenizer->hasMoreTokens() );
        tokenizer->nextToken();
        TS_ASSERT(tokenizer->hasMoreTokens() );
        tokenizer->nextToken();
        TS_ASSERT( !tokenizer->hasMoreTokens() );
      }

    /**
     * Test nextToken
     */
    void testNextToken()
      {
        //TODO : come up with a script that covers all token types.
        XmlScript::Handle script = XmlScript::create("<element/>");
        ErrorList::Handle errList = ErrorList::create();

        XmlTokenizer::Handle tokenizer = XmlTokenizer::create();
        // There should be 3 tokens for the above script.
        // Make sure that nextToken returns the correct token
        tokenizer->setScript(script, errList);
        Token::View token = tokenizer->nextToken();
        TS_ASSERT(token->getText()->equals("<") );
        token = tokenizer->nextToken();
        TS_ASSERT(token->getText()->equals("element") );
        token = tokenizer->nextToken();
        TS_ASSERT(token->getText()->equals("/>") );
      }

    /**
     * Test putBackToken
     */
    void testPutBackToken()
      {
        //TODO : come up with a script that covers all token types.
        XmlScript::Handle script = XmlScript::create("<element/>");
        ErrorList::Handle errList = ErrorList::create();

        XmlTokenizer::Handle tokenizer = XmlTokenizer::create();
        // There should be 3 tokens for the above script.
        // Make sure that nextToken returns the correct token
        tokenizer->setScript(script, errList);
        Token::View token1 = tokenizer->nextToken();
        TS_ASSERT(token1->getText()->equals("<") );
        Token::View token2 = tokenizer->nextToken();
        TS_ASSERT(token2->getText()->equals("element") );
        tokenizer->putBackToken(token2);
        token2 = tokenizer->nextToken();
        TS_ASSERT(token2->getText()->equals("element") );
        tokenizer->putBackToken(token2);
        tokenizer->putBackToken(token1);
        token1 = tokenizer->nextToken();
        TS_ASSERT(token1->getText()->equals("<") );
        token2 = tokenizer->nextToken();
        TS_ASSERT(token2->getText()->equals("element") );
        Token::View token3 = tokenizer->nextToken();
        TS_ASSERT(token3->getText()->equals("/>") );
      }

  };

