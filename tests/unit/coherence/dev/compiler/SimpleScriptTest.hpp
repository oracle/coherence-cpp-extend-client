/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/io/EOFException.hpp"

#include "private/coherence/dev/compiler/SimpleScript.hpp"

#include <stdio.h>

using namespace coherence::lang;
using namespace coherence::dev::compiler;
using coherence::io::EOFException;

/**
 * Test Suite for the SimpleScript object.
 */
class SimpleScriptSuite : public CxxTest::TestSuite
  {
public:

    /**
     * Test setScript
     */
    void testSetScript()
      {
        SimpleScript::Handle script = SimpleScript::create();
        TS_ASSERT(!script->hasMoreChars());
        script->setScript("<element>value</element>");
        TS_ASSERT(script->hasMoreChars());

        try
            {
            script->setScript("<element>another value</element>");
            TS_FAIL("Expected an IllegalStateException.");
            }
        catch (IllegalStateException::View)
            {
            //expected
            }

        script = SimpleScript::create();
        try
            {
            script->setScript("\x080");
            TS_FAIL("Expected an IllegalArgumentException.");
            }
        catch (IllegalArgumentException::View)
            {
            //expected
            }
      }

    /**
     * Test that creation with a non-Ascii script throws exception
     */
    void nonAsciiScript()
      {
      try
          {
          SimpleScript::Handle script = SimpleScript::create("\x080");
          TS_FAIL("Expected an IllegalArgumentException.");
          }
      catch (IllegalArgumentException::View)
          {
          //expected
          }
      }
    
    /**
     * Test hasMoreChars
     */
    void testHasMoreChars()
      {
        SimpleScript::Handle script = SimpleScript::create("<e>");
        TS_ASSERT(script->hasMoreChars());
        script->nextChar();
        TS_ASSERT(script->hasMoreChars());
        script->nextChar();
        TS_ASSERT(script->hasMoreChars());
        script->nextChar();
        TS_ASSERT(!script->hasMoreChars());
      }

    /**
     * Test nextChar
     */
    void testNextChar()
      {
        SimpleScript::Handle script = SimpleScript::create("<e>");
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='>');

        script->nextChar();
        try
            {
            script->nextChar();
            TS_FAIL("Expected an EOFException.");
            }
        catch (EOFException::View)
            {
            //expected
            }
      }

    
    /**
     * Test putBackChar
     */
    void testPutBackChar()
      {
        SimpleScript::Handle script = SimpleScript::create("<e>");
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->nextChar()=='e');
        script->putBackChar();
        TS_ASSERT(script->nextChar()=='e');
        script->putBackChar();
        script->putBackChar();
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='>');

        script->putBackChar();
        script->putBackChar();
        script->putBackChar();
        try
            {
            script->putBackChar();
            TS_FAIL("Expected an IllegalStateException.");
            }
        catch (IllegalStateException::View)
            {
            //expected
            }
      }

    /**
     * Test savePosition & restorePosition
     */
    void testSaveRestorePosition()
      {
        SimpleScript::Handle script = SimpleScript::create("<element>");
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='l');

        ParsePosition::Handle pos = script->savePosition();

        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='m');

        script->restorePosition(pos);

        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='m');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='n');
        TS_ASSERT(script->nextChar()=='t');
        TS_ASSERT(script->nextChar()=='>');

        script->restorePosition(pos);

        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='m');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='n');
        ParsePosition::Handle pos2 = script->savePosition();
        script->putBackChar();
        script->putBackChar();
        script->putBackChar();
        script->putBackChar();
        TS_ASSERT(script->nextChar()=='e');

        //restore forward
        script->restorePosition(pos2);

        TS_ASSERT(script->nextChar()=='t');
        TS_ASSERT(script->nextChar()=='>');
      }

    /**
     * Test getLine
     */
    void testGetLine()
      {
        SimpleScript::Handle script = SimpleScript::create("0\n1\n2\n3\n");
        TS_ASSERT(script->getLine()==0);
        script->nextChar();
        script->nextChar();
        TS_ASSERT(script->getLine()==1);
        ParsePosition::Handle pos = script->savePosition();
        script->nextChar();
        script->nextChar();
        TS_ASSERT(script->getLine()==2);
        script->nextChar();
        script->nextChar();
        TS_ASSERT(script->getLine()==3);
        script->putBackChar();
        TS_ASSERT(script->getLine()==2);
        script->restorePosition(pos);
        TS_ASSERT(script->getLine()==1);
        script->nextChar();
        script->nextChar();
        TS_ASSERT(script->getLine()==2);
        script->nextChar();
        script->nextChar();
        TS_ASSERT(script->getLine()==3);
      }

    /**
     * Test getOffset
     */
    void testGetOffset()
      {
        SimpleScript::Handle script = SimpleScript::create("01234\nabcde\n");
        TS_ASSERT(script->getOffset()==0);
        script->nextChar();
        TS_ASSERT(script->getOffset()==1);
        script->nextChar();
        TS_ASSERT(script->getOffset()==2);
        script->nextChar();
        TS_ASSERT(script->getOffset()==3);
        script->nextChar();
        TS_ASSERT(script->getOffset()==4);
        script->nextChar();
        script->nextChar();
        TS_ASSERT(script->getOffset()==0);
        script->putBackChar();
        script->putBackChar();

        TS_ASSERT(script->getOffset()==4);
        script->nextChar();
        script->nextChar();
        TS_ASSERT(script->getOffset()==0);
      }
    /**
     * Test subScript
     */
    void testSubScript()
      {
        SimpleScript::Handle script = SimpleScript::create("<element></element>");
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='l');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='m');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='n');
        TS_ASSERT(script->nextChar()=='t');
        TS_ASSERT(script->nextChar()=='>');
        ParsePosition::Handle pos = script->savePosition();
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->nextChar()=='/');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='l');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='m');
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->nextChar()=='n');
        TS_ASSERT(script->nextChar()=='t');
        TS_ASSERT(script->nextChar()=='>');
        Script::Handle subScript = script->subScript(pos);
        TS_ASSERT(subScript->getLine()==0);
        TS_ASSERT(subScript->getOffset()==0);
        TS_ASSERT(subScript->nextChar()=='<');
        TS_ASSERT(subScript->nextChar()=='/');
        TS_ASSERT(subScript->nextChar()=='e');
        ParsePosition::Handle pos2 = subScript->savePosition();
        TS_ASSERT(subScript->nextChar()=='l');
        TS_ASSERT(subScript->nextChar()=='e');
        TS_ASSERT(subScript->nextChar()=='m');
        Script::Handle subScript2 = subScript->subScript(pos2);
        TS_ASSERT(subScript->nextChar()=='e');
        TS_ASSERT(subScript->nextChar()=='n');
        TS_ASSERT(subScript->nextChar()=='t');
        TS_ASSERT(subScript->nextChar()=='>');
        TS_ASSERT(!subScript->hasMoreChars());

        TS_ASSERT(subScript2->nextChar()=='l');
        TS_ASSERT(subScript2->nextChar()=='e');
        TS_ASSERT(subScript2->nextChar()=='m');
        TS_ASSERT(!subScript2->hasMoreChars());
        }


    /**
     * Test getText
     */
    void testGetText()
      {
        SimpleScript::Handle script = SimpleScript::create("<element></element>");
        String::View text = script->getText();
        TS_ASSERT(text->equals("<element></element>"));
      }


  };

