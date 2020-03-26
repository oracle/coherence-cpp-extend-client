/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/lang/MutableReference.hpp"

#include "coherence/util/extractor/TypedUpdater.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::TypedUpdater;
using coherence::util::filter::EqualsFilter;

/**
* Test Suite for  TypedUpdater.
*/
class TypedUpdaterSuite : public CxxTest::TestSuite
    {
    public:
         /**
        * Test TypedUpdater
        */
        void testTypedUpdater()
            {
            Reference::Handle hRef = MutableReference::create((Object::Handle) NULL);

            ReflectionUpdater::View vUpd =
                COH_TYPED_UPDATER(Reference, set, Object::Holder);
            TS_ASSERT(vUpd->getMethodName()->equals("set"));

            String::Handle hsValue = String::create("foo");
            vUpd->update(hRef, hsValue);
            TS_ASSERT(hRef->get()->equals(hsValue));

            try
                {
                vUpd->update(hsValue, hsValue);

                TS_ASSERT(false);
                }
            catch (ClassCastException::View e)
                {
                }
            }
    };
