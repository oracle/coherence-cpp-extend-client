/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/InKeySetFilter.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Set.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::InKeySetFilter;
using coherence::util::Converter;
using coherence::util::HashSet;
using coherence::util::Map;
using coherence::util::HashMap;
using coherence::util::Set;


/**
* Test Suite for the InKeySetFilter.
*/
class InKeySetFilterSuite : public CxxTest::TestSuite
    {

    //  ----- inner class: Add1Converter -------------------------------------

    /*
    * Convert a String by appending '1' to the String
    */
    class Add1Converter
        : public class_spec<Add1Converter,
              extends<Object>,
              implements<Converter> >
        {
        friend class factory<Add1Converter>;

        // ----- constructors -------------------------------------------

        protected:
            /**
            * Default constructor.
            */
            Add1Converter()
                {
                }

        // ----- Converter interface ------------------------------------

        public:
            /**
             * {@inheritDoc}
             */
            virtual Object::Holder convert(Object::Holder oh) const
                {
                return COH_TO_STRING(oh << '1');
                }
        };

    public:
        /**
        * Test InKeySetFilter
        */
        void testInKeySetFilter()
            {
            HashSet::Handle hHS = HashSet::create();
            hHS->add(String::create("Noah"));
            hHS->add(String::create("Mark"));
            hHS->add(String::create("Jason"));
            InKeySetFilter::Handle hFilter = InKeySetFilter::create(
                    AlwaysFilter::getInstance(), hHS);

            HashMap::Handle hMap = HashMap::create();
            hMap->put(String::create("Noah"),Integer32::create(42));
            Set::View         vSet = hMap->entrySet();
            Iterator::Handle  iter = vSet->iterator();
            Map::Entry::View vME = cast<Map::Entry::View>(iter->next());
            TS_ASSERT(hFilter->evaluateEntry(vME));

            hMap = HashMap::create();
            hMap->put(String::create("David"),Integer32::create(42));
            vSet = hMap->entrySet();
            iter = vSet->iterator();
            vME  = cast<Map::Entry::View>(iter->next());
            TS_ASSERT(!hFilter->evaluateEntry(vME));

            // COH-13107
            hFilter->ensureConverted(Add1Converter::create());
            hMap = HashMap::create();
            hMap->put(String::create("Noah1"), Integer32::create(42));
            vSet = hMap->entrySet();
            iter = vSet->iterator();
            vME  = cast<Map::Entry::View>(iter->next());
            TS_ASSERT(hFilter->evaluateEntry(vME));

            // verify that subsequent ensureConverted() calls do nothing
            hFilter->ensureConverted(Add1Converter::create());
            TS_ASSERT(hFilter->evaluateEntry(vME));
            }
    };
