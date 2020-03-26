/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "private/coherence/net/URI.hpp"

#include "cxxtest/TestSuite.h"

using coherence::net::URI;


/**
* Test suite for the URI class.
*/
class URITest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test URI creation from a URI string.
        */
        void testCreate()
            {
            String::View vsURI = "channel:1234#NamedCache";
            URI::View    vURI  = URI::create(vsURI);

            TS_ASSERT(vURI->getScheme()->equals(String::create("channel")));
            TS_ASSERT(vURI->getSchemeSpecificPart()->equals(String::create("1234")));
            TS_ASSERT(vURI->getFragment()->equals(String::create("NamedCache")));
            }

        /**
        * Test equals()
        */
        void testEquals()
            {
            URI::View vURI1 = URI::create("channel:1234#NamedCache");
            URI::View vURI2 = URI::create("channel", "1234", "NamedCache");
            URI::View vURI3 = URI::create("channel", "4321", "NamedCache");

            TS_ASSERT(vURI1->equals(vURI1));
            TS_ASSERT(vURI2->equals(vURI2));
            TS_ASSERT(vURI1->equals(vURI2));
            TS_ASSERT(!vURI1->equals(vURI3));
            TS_ASSERT(!vURI2->equals(vURI3));
            }

        /**
        * Test hashCode()
        */
        void testHashCode()
            {
            URI::View vURI1 = URI::create("channel:1234#NamedCache");
            URI::View vURI2 = URI::create("channel", "1234", "NamedCache");
            URI::View vURI3 = URI::create("channel", "4321", "NamedCache");

            TS_ASSERT(vURI1->hashCode() == vURI1->hashCode());
            TS_ASSERT(vURI2->hashCode() == vURI2->hashCode());
            TS_ASSERT(vURI1->hashCode() == vURI2->hashCode());
            TS_ASSERT(vURI1->hashCode() != vURI3->hashCode());
            TS_ASSERT(vURI2->hashCode() != vURI3->hashCode());
            }
    };

