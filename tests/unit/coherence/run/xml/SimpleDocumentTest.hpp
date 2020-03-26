/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "private/coherence/run/xml/SimpleDocument.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"

using namespace coherence::lang;
using namespace coherence::run::xml;
using namespace std;

/**
* Test Suite for SimpleDocument
*/
class SimpleDocumentTestSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Set the name of the doc in the create call and assert it was
        * properly set.
        */
        void testGetName()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            TS_ASSERT(NULL == doc->getName());

            doc = SimpleDocument::create("test-name");
            TS_ASSERT(doc->getName()->equals("test-name"));

            doc = SimpleDocument::create("test-name2", "test-dtd-uri", "test-dtd-name");
            TS_ASSERT(doc->getName()->equals("test-name2"));
            }

        /**
        * Test getDtdUri after construction of SimpleTest
        */
        void testGetDtdUri()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            TS_ASSERT(NULL == doc->getDtdUri());

            doc = SimpleDocument::create("test-name");
            TS_ASSERT(NULL == doc->getDtdUri());

            doc = SimpleDocument::create("test-name", "test-dtd-uri", "test-dtd-name");
            TS_ASSERT(doc->getDtdUri()->equals("test-dtd-uri"));
            }


        /**
        * Test setting and getting dtdUri
        */
        void testSetDtdUri()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            doc->setDtdUri("test-dtd-uri");
            TS_ASSERT(doc->getDtdUri()->equals("test-dtd-uri"));
            }

        /**
        * Test setDtdUri with a null value
        */
        void testSetDtdUriWithNullValue()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            String::View s;
            doc->setDtdUri(s);
            TS_ASSERT(NULL == doc->getDtdUri());
            }

        /**
        * Test setDtdUri when doc is not mutable
        */
        void XXXtestSetDtdUriNotMutable()
            {
            //todo
            }

        /**
        * Test getDtdName after construction of doc
        */
        void testGetDtdName()
            {
            SimpleDocument::Handle doc = SimpleDocument::create("name", "test-dtdUri", "test-dtd-name");
            TS_ASSERT(doc->getDtdName()->equals("test-dtd-name"));

            doc = SimpleDocument::create("name");
            TS_ASSERT(NULL == doc->getDtdName());

            doc = SimpleDocument::create();
            TS_ASSERT(NULL == doc->getDtdName());
            }

        /**
        * Test setting/getting dtdName
        */
        void testSetDtdName()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            doc->setDtdName("test-dtd-name");
            TS_ASSERT(doc->getDtdName()->equals("test-dtd-name"));
            }

        /**
        * Test setDtdName when doc is not mutauble
        */
        void XXXtestSetDtdNameNotMutable()
            {
            //todo
            }

        /**
        * Test setDtdName with a zero length String results in a null value being set.
        */
        void testSetDtdNameWithZeroLengthName()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            doc->setDtdName("");
            TS_ASSERT(NULL == doc->getDtdName());
            }

        /**
        * Test setDtdName with an invalid name
        */
        void XXXtestSetDtdNameWithInvalidName()
            {
            //todo
            }

        /**
        * Test setDtdname with a NULL value
        */
        void testSetDtdNameWithNullValue()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            String::View s;
            doc->setDtdName(s);
            TS_ASSERT(NULL == doc->getDtdName());
            }

        /**
        * Set and get encoding
        */
        void testSetEncoding()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            TS_ASSERT(NULL == doc->getEncoding());

            doc->setEncoding("test-encoding");
            TS_ASSERT(doc->getEncoding()->equals("test-encoding"));
            }

        /**
        * Test setEncoding passing in an invalid encoding.
        */
        void XXXtestSetEncodingWithInvalidEncoding()
            {
            //todo
            }

        /**
        * Test setEncoding with non-mutable doc
        */
        void XXXtestSetEncodingNotMutable()
            {
            //todo
            }

        /**
        * Test setEncoding passing in a zero length String.
        */
        void testSetEncodingWithZeroLengthValue()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            doc->setEncoding("");
            TS_ASSERT(NULL == doc->getEncoding());
            }

        /**
        * Test that setting a null encoding value is allowed
        */
        void testSetEncodingWithNullValue()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            String::View s;
            doc->setEncoding(s);
            TS_ASSERT(NULL == doc->getEncoding());
            }

        /**
        * Test setDocumentComment
        */
        void testSetDocumentComment()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            TS_ASSERT(NULL == doc->getDocumentComment());

            doc->setDocumentComment("test-comment");
            TS_ASSERT(doc->getDocumentComment()->equals("test-comment"));
            }

        /**
        * Test setDocumentComment passing in an invalid comment
        */
        void testSetDocumentCommentWithInvalidComment()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            TS_ASSERT_THROWS(doc->setDocumentComment("Bad--Comment"),
                             IllegalArgumentException::View);
            }

        /**
        * Test setDocumentComment on a non-mutable doc
        */
        void XXXtestSetDocumentCommentNotMutable()
            {
            //todo
            }

        /**
        * Test setDocumentComment passing in a zero length String
        */
        void testSetDocumentCommentWithZeroLengthValue()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            doc->setDocumentComment("test-comment");
            doc->setDocumentComment("");
            TS_ASSERT(NULL == doc->getDocumentComment());
            }

        /**
        * Test setDocumentComment passing in NULL
        */
        void testSetDocumentCommentWithNullValue()
            {
            SimpleDocument::Handle doc = SimpleDocument::create();
            doc->setDocumentComment("test-comment");
            String::View s;
            doc->setDocumentComment(s);
            TS_ASSERT(NULL == doc->getDocumentComment());
            }

//TODO:  Need to access protected functions
//  void testWriteXml()
//      {
//      SimpleDocument::Handle doc = SimpleDocument::create();
//      doc->setDtdUri("test-dtd-uri");
//      doc->setDtdName("test-dtd-name");
//      doc->setEncoding("test-encoding");
//      doc->setDocumentComment("test-document-comment");
//
//      stringstream ssControl;
//      stringstream ssActual;
//      writeXmlControl(ssControl, false, doc);
//      doc->writeXml(ssActual, false);
//      String::View vsControl = ssControl.str().c_str();
//      TS_ASSERT(vsControl->equals(ssActual.str().c_str()));
//      }
//
//
//
//  void writeXmlControl(ostream& out, bool fPretty, SimpleDocument::Handle doc)
//      {
//      out << "<?xml version='1.0'";
//      if (doc->getEncoding())
//          {
//          out << " encoding=" << XmlHelper::quote(doc->getEncoding());
//          }
//      out << "?>";
//      if (fPretty)
//          {
//          out << endl;
//          }
//      if (doc->getDtdUri())
//          {
//          out << "<!DOCTYPE " << doc->getName() << ' ';
//          if (doc->getDtdName())
//              {
//              out << "PUBLIC";
//              if (fPretty)
//                  {
//                  out << endl;
//                  }
//              out << ' '
//                  << XmlHelper::quote(XmlHelper::encodeUri(doc->getDtdUri()))
//                  << '>';
//              if (fPretty)
//                  {
//                  out << endl;
//                  }
//              else
//                  {
//                  out << ' ';
//                  }
//              }
//          }
//      if (doc->getDocumentComment())
//          {
//          out << "<!--";
//          }
//      if (fPretty)
//          {
//          out << endl
//              << XmlHelper::breakLines(doc->getDocumentComment(), 78);
//          }
//      else
//          {
//          out << doc->getDocumentComment();
//          }
//      out << "-->";
//      if (fPretty)
//          {
//          out << endl;
//          }
//      else
//          {
//          out << ' ';
//          }
//      doc->writeEmptyTag(out, fPretty);
//      if (fPretty)
//          {
//          out << endl;
//          }
//      }

    };
