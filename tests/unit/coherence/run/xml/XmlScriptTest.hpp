/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "private/coherence/run/xml/XmlScript.hpp"


#include <stdio.h>

using namespace coherence::run::xml;

/**
 * Test Suite for the XmlScript object.
 * 
 * Test that we can create and use XmlScript objects.  More complete
 * tests are done in the SimpleScript test suite.
 */
class XmlScriptSuite : public CxxTest::TestSuite
  {
public:
    
    /**
     * Test the no-arg constructor
     */
    void testNoArgCtor()
      {
        XmlScript::Handle script = XmlScript::create();
        TS_ASSERT(!script->hasMoreChars());
        script->setScript("<e>");
        TS_ASSERT(script->hasMoreChars());
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->hasMoreChars());
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->hasMoreChars());
        TS_ASSERT(script->nextChar()=='>');
        TS_ASSERT(!script->hasMoreChars());
      }


    /**
     * Test the constructor that takes a single String arg
     */
    void testStringCtor()
      {
        XmlScript::Handle script = XmlScript::create("<e>");
        TS_ASSERT(script->hasMoreChars());
        TS_ASSERT(script->nextChar()=='<');
        TS_ASSERT(script->hasMoreChars());
        TS_ASSERT(script->nextChar()=='e');
        TS_ASSERT(script->hasMoreChars());
        TS_ASSERT(script->nextChar()=='>');
        TS_ASSERT(!script->hasMoreChars());
      }
      
      };

