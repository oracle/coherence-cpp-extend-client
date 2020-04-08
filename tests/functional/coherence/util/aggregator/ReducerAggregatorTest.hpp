/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Hashtable.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/aggregator/ReducerAggregator.hpp"
#include "coherence/util/extractor/MultiExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"

#include "common/Book.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::Hashtable;
using coherence::util::InvocableMap;
using coherence::util::List;
using coherence::util::Map;
using coherence::util::aggregator::ReducerAggregator;
using coherence::util::extractor::MultiExtractor;
using coherence::util::extractor::ReflectionExtractor;
using coherence::util::filter::AlwaysFilter;
using common::test::Book;

using namespace common::test;

/**
* Test Suite for the ReducerAggregator.
*/
class ReducerAggregatorTest : public CxxTest::TestSuite
  {
  public:
        /**
        * Test remote ReducerAggregator
        */
        void testRemote()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-test");
   
            Hashtable::Handle hHt = Hashtable::create();
            hHt->put(String::create("1"),
                    Book::create("Aleksandar Seovic",    "Oracle Coherence 3.5",       "ISBN 978-1-847196-12-5", 382, 2010));
            hHt->put(String::create("2"),
                    Book::create("Rob Weldman",          "LDAP Programming with Java", "ISBN 0201657589N",       692, 2000));
            hHt->put(String::create("3"),
                    Book::create("H.X. Mel",             "Cryptography Decrypted",     "ISBN 0-201-61647-5",     352, 2001));
            hHt->put(String::create("4"),
                    Book::create("SSL and TLS",          "Eric Rescorla",              "ISBN 0-201-61598-3",     499, 2001));
            hHt->put(String::create("5"),
                    Book::create("Enterprise JavaBeans", "Tom Valesky",                "ISBN 0-201-60446-9",     323, 1999));
            hCache->putAll(hHt);

            // Remote execution, java method names 

            ObjectArray::Handle ha = ObjectArray::create(2);
            ha[0]                  = ReflectionExtractor::create("getTitle");
            ha[1]                  = ReflectionExtractor::create("getYear");

            MultiExtractor::View      vExtract = MultiExtractor::create(ha);
            ReducerAggregator::Handle hAgent   = ReducerAggregator::create(
                    (ValueExtractor::View)vExtract);

            Map::View vM = cast<Map::View>(hCache->aggregate(
                    (Filter::View)AlwaysFilter::getInstance(), 
                    (InvocableMap::EntryAggregator::Handle)hAgent));

            TS_ASSERT(vM->size() == hCache->size());

            List::View vList = cast<List::View>(vM->get(String::create("1")));

            TS_ASSERT(2 == vList->size());
            TS_ASSERT((cast<String::View>(vList->get(0)))->equals("Oracle Coherence 3.5"));
            TS_ASSERT((cast<Integer32::View>(vList->get(1)))->getInt32Value() == 2010);

            vList = cast<List::View>(vM->get(String::create("2")));

            TS_ASSERT(2 == vList->size());
            TS_ASSERT((cast<String::View>(vList->get(0)))->equals("LDAP Programming with Java"));
            TS_ASSERT((cast<Integer32::View>(vList->get(1)))->getInt32Value() == 2000);
            }
  };
