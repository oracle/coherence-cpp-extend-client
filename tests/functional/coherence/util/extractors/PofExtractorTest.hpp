/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/extractor/PofExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/ValueChangeEventFilter.hpp"
#include "coherence/tests/CustomKeyClass.hpp"
#include "common/TestClasses.hpp"
#include "common/TestSyncListener.hpp"

using namespace coherence::lang;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::io::pof::RawDate;
using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::ValueExtractor;
using coherence::util::extractor::PofExtractor;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::LessEqualsFilter;
using coherence::util::filter::ValueChangeEventFilter;
using coherence::tests::CustomKeyClass;
using common::test::TestOrder;
using common::test::TestPart;


/**
* Test Suite for the PofExtractor.
*/
class PofExtractorTest : public CxxTest::TestSuite
  {
  public:
    /**
    * Test PofExtractor with Filter
    */
    void testPofExtractorWithFilter()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-filter");

        for (int32_t i = 1; i < 100; i++)
            {
            hCache->put(Integer32::create(i), TestOrder::create(
                    Integer32::create(i), COH_TO_STRING("CD" << i), i));
            }

        ValueExtractor::Handle hExtractor = PofExtractor::create(typeid(void), 0);
        LessEqualsFilter::Handle hFilter = LessEqualsFilter::create(hExtractor, Integer32::create(70));
        TS_ASSERT(hCache->entrySet(hFilter)->size() == 70);

        hExtractor = PofExtractor::create(typeid(int32_t), 2);
        hFilter = LessEqualsFilter::create(hExtractor, Integer32::create(42));
        TS_ASSERT(hCache->entrySet(hFilter)->size() == 42);

        hExtractor = PofExtractor::create(typeid(String), 1);
        EqualsFilter::Handle hFilterEquals = EqualsFilter::create(hExtractor, String::create("CD42"));
        TS_ASSERT(hCache->entrySet(hFilterEquals)->size() == 1);

        hExtractor = PofExtractor::create(typeid(TestPart), 3);
        hFilterEquals = EqualsFilter::create(hExtractor, TestPart::create(40, String::create("widget")));
        TS_ASSERT(hCache->entrySet(hFilterEquals)->size() == 1);
        }

  public:
    /**
    * Test PofExtractor with ValueChangeEventFilter
    */
    void testPofExtractorWithValueChangeEventFilter1()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-filter1");

        CustomKeyClass::Handle key1 = CustomKeyClass::create(String::create("Customer1"));
        CustomKeyClass::Handle key2 = CustomKeyClass::create(String::create("Customer2"));

        hCache->put(String::create("key1"), key1);
        hCache->put(String::create("key2"), key2);

        TestSyncListener::Handle       listener = TestSyncListener::create();
        ValueChangeEventFilter::Handle filter   = ValueChangeEventFilter::create(PofExtractor::create(typeid(String), 0));

        hCache->addFilterListener(listener, filter, false);

        TS_ASSERT(listener->getEvent() == NULL);

        hCache->put(String::create("key1"), key1);
        TS_ASSERT(listener->getEvent() == NULL);

        hCache->put(String::create("key1"), CustomKeyClass::create(String::create("Customer12")));
        TS_ASSERT(listener->getEvent() != NULL);
        }

  public:
    /**
    * Test PofExtractor with ValueChangeEventFilter
    */
    void testPofExtractorWithValueChangeEventFilter2()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-filter1");

        ExampleAddress::Handle address1 = ExampleAddress::create(
                   String::create("Street11"), 
                   String::create("Street12"),
                   String::create("City1"), 
                   String::create("State1"), 
                   String::create("Zip1"),
                   String::create("Country1"));
        ExampleAddress::Handle address2 = ExampleAddress::create(
                   String::create("Street21"), 
                   String::create("Street22"), 
                   String::create("City2"), 
                   String::create("State2"), 
                   String::create("Zip2"),
                   String::create("Country2"));

        hCache->put(String::create("key1"), address1);
        hCache->put(String::create("key2"), address2);

        TestSyncListener::Handle       listener = TestSyncListener::create();
        ValueChangeEventFilter::Handle filter   = ValueChangeEventFilter::create(PofExtractor::create(typeid(String), 0));

        hCache->addFilterListener(listener, filter, false);

        TS_ASSERT(listener->getEvent() == NULL);

        ExampleAddress::Handle address1a = ExampleAddress::create(
                    String::create("Street11"), 
                    String::create("Street12a"),
                    String::create("City1a"), 
                    String::create("State1a"), 
                    String::create("Zip1a"),
                    String::create("Country1a"));
        hCache->put(String::create("key1"), address1a);
        TS_ASSERT(listener->getEvent() == NULL);

        ExampleAddress::Handle address1aa = ExampleAddress::create(
                    String::create("Street1a"), 
                    String::create("Street12"), 
                    String::create("City1"), 
                    String::create("State1"), 
                    String::create("Zip1"),
                    String::create("Country1"));
        hCache->put(String::create("key1"), address1aa);
        TS_ASSERT(listener->getEvent() != NULL);
        }
  };
