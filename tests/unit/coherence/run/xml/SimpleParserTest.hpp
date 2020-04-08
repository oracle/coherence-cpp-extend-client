/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"
#include "private/coherence/run/xml/SimpleParser.hpp"
#include "private/coherence/run/xml/SimpleDocument.hpp"
#include <fstream>
#include <stdlib.h>

using namespace coherence::lang;
using namespace coherence::run::xml;
using coherence::io::IOException;
using namespace std;


COH_PRAGMA_PUSH // Windows generates warnings related to use of env variables
/**
* Test Suite for SimpleParser
*/
class SimpleParserTestSuite : public CxxTest::TestSuite
    {
    public:

        void testParseXml()
            {
            SimpleParser::Handle parser = SimpleParser::create();
            SimpleParser::Handle parser2 = SimpleParser::create();

            stringstream xmlScriptPath;
            xmlScriptPath << getenv ("DEV_ROOT") << "/tests/unit/resource/tangosol-coherence-2.xml";

            const string& temp = xmlScriptPath.str();
            fstream filestr( temp.c_str(), fstream::in );
            XmlDocument::Handle doc = parser->parseXml(filestr);
            filestr.close();

            String::View xmlString = doc->getXml();
            XmlDocument::Handle doc2 = parser2->parseXml(xmlString);
            String::View xmlString2 = doc2->getXml();
            TS_ASSERT(xmlString2->equals(xmlString));
            }



        void testParseXmlString()
            {
            SimpleParser::Handle parser = SimpleParser::create();
            SimpleParser::Handle parser2 = SimpleParser::create();

            stringstream xmlScriptPath;
            xmlScriptPath << getenv ("DEV_ROOT") << "/tests/unit/resource/tangosol-coherence-2.xml";

            const string& temp = xmlScriptPath.str();
            fstream filestr( temp.c_str(), fstream::in );
            stringstream ss;
            char line[256];
            while (!filestr.eof())
                {
                filestr.read(line, 255);
                line[filestr.gcount()] = 0x00;
                ss <<  line;
                }
            XmlDocument::Handle doc = parser->parseXml(ss.str());
            filestr.close();

            String::View xmlString = doc->getXml();
            XmlDocument::Handle doc2 = parser2->parseXml(xmlString);
            String::View xmlString2 = doc2->getXml();
            TS_ASSERT(xmlString2->equals(xmlString));
            }


        void testParseXmlWithError()
            {
            SimpleParser::Handle parser = SimpleParser::create();
            try
                {
                XmlDocument::Handle doc = parser->parseXml("Invalid XML<element>value</element>");
                TS_ASSERT(false);
                }
                catch (RuntimeException::View)
                    {
                    //Expected...
                    }
            }

        void testParseXmlWithBadStream()
            {
            SimpleParser::Handle parser = SimpleParser::create();
            try
                {
                fstream filestr( "!~!!~bogus.xml", fstream::in );
                XmlDocument::Handle doc = parser->parseXml(filestr);

                TS_ASSERT(false);
                }
                catch (IOException::View)
                    {
                    //Expected...
                    }
            }
    };

COH_PRAGMA_POP
