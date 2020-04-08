/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net.ns"
#include "coherence/run/xml.ns"

#include <sstream>
#include <iomanip>
#include <iostream>

using namespace coherence::lang;
using namespace coherence::net;
using namespace coherence::run::xml;

class Stringable
    : public class_spec<Stringable>
    {
    friend class factory<Stringable>;

    public:

    virtual TypedHandle<const String> toString() const
        {
        return String::create("I'm a Stringable");
        }
    };

class Streamable
    : public class_spec<Streamable>
    {
    friend class factory<Streamable>;

    public:

    virtual void toStream(std::ostream& out) const
        {
        out << "I'm a Streamable";
        }
    };

/**
* Test Suite for ensuring compatibility with the C++ standard library
*/
class StandardLibTest : public CxxTest::TestSuite
    {
    public:
        /**
         * Test that we can output an object to a stream via toString.
         */
        void testToString()
            {
            Stringable::Handle h = Stringable::create();
            std::stringstream ss;
            ss << h;
            std::cout << h << std::endl; // ensure it doesn't crash
            TS_ASSERT(ss.str().compare("I'm a Stringable") == 0);
            }

        /**
         * Test that we can output an object to a stream via deprecated toStream.
         */
        void testToStream()
            {
            Streamable::Handle h = Streamable::create();
            std::stringstream ss;
            ss << h;
            std::cout << h << std::endl; // ensure it doesn't crash
            TS_ASSERT(ss.str().compare("I'm a Streamable") == 0);
            }

        /**
         * Test that we can read XML from a stream.
         */
        void testLoadXml()
            {
            std::stringstream ss;
            ss << "<coherence></coherence>";
            TS_ASSERT(CacheFactory::loadXml(ss)->getName()->equals("coherence"));
            }

        /**
         * Test that we pass a std::type_info across the lib boundard and interpret it
         */
        void testTypeInfo()
            {
            TS_ASSERT(Class::getTypeName(typeid(Stringable))->equals("Stringable"));
            }

        /**
         * Test that we can catch a managed exception as a std::exception
         */
        void testCatchStdEx()
            {
            try
                {
                Object::Handle h = NULL;
                h->toString(); // trigger NPE
                TS_ASSERT(false);
                }
            catch (const std::exception &e)
                {
                TS_ASSERT(String::create(e.what())->startsWith("coherence::lang::NullPointerException"));
                }
            }
    };
