/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "private/coherence/io/pof/reflect/internal/NameManglers.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;
using namespace std;

using annotations_test::Poffed;

using coherence::io::pof::reflect::internal::NameMangler;
using coherence::io::pof::reflect::internal::NameManglers;

/**
* Test suite for NameManglers building.
*/
class ManglersTest : public CxxTest::TestSuite
    {
    public:
        typedef NameManglers::MethodMangler MethodMangler;

        /**
         * Test field mangler.
         */
        void testFieldMangler()
            {
            NameManglers::FieldMangler::View vMangler        = NameManglers::FieldMangler::create();
            String::View                     vsEclipseNaming = vMangler->mangle("fFoo");
            String::View                     vsCppNaming     = vMangler->mangle("m_Foo");

            TS_ASSERT(vsEclipseNaming->equals("foo"));
            TS_ASSERT(vsCppNaming    ->equals("foo"));
            }

        /**
         * Test method mangler.
         */
        void testMethodMangler()
            {
            NameManglers::MethodMangler::View  vMangler        = NameManglers::MethodMangler::create();
            String::View                       vsEclipseNaming = vMangler->mangle("getFoo");
            String::View                       vsCppNaming     = vMangler->mangle("setFoo");

            TS_ASSERT(vsEclipseNaming->equals("foo"));
            TS_ASSERT(vsCppNaming    ->equals("foo"));
            }
    };
