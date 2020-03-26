/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/lang.ns"

#include "coherence/lang.ns"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"


using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::LessEqualsFilter;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;



/**
* Test Suite for the LessEqualsFilter.
*/
class LessEqualsFilterSuite : public CxxTest::TestSuite
  {
  public:
        /**
        * Test LessEqualsFilter
        * 
        * also test QueryMap API [addIndex, removeIndex, and
        * entrySet(Filter::View, Comparator::View)]
        *   
        */
      void testLessEqualsFilter()
          {
          NamedCache::Handle hCache = ensureCleanCache("dist-filter");

          IdentityExtractor::Handle hExtractor = IdentityExtractor::create();

          hCache->addIndex(hExtractor, true, SafeComparator::getInstance());

          hCache->put(String::create("String-able"),
                  String::create("able"));
          hCache->put(String::create("String-baker"),
                  String::create("baker"));
          hCache->put(String::create("String-charley"),
                  String::create("charley"));
          hCache->put(String::create("String-walley"),
                  String::create("walley"));
          hCache->put(String::create("String-zebra"),
                  String::create("zebra"));

          LessEqualsFilter::View hFilter = LessEqualsFilter::create(
                  hExtractor,
                  String::create("mary"));

          Set::View vFilterEntries = hCache->entrySet(hFilter,
                  SafeComparator::getInstance());

          hCache->removeIndex(hExtractor);

          if (vFilterEntries->size() == 3)
              {
              Iterator::Handle hIter = vFilterEntries->iterator();
              TS_ASSERT(cast<Map::Entry::View>(hIter->next())->getValue()
                          ->equals(String::create("able")));
              TS_ASSERT(cast<Map::Entry::View>(hIter->next())->getValue()
                           ->equals(String::create("baker")));
              TS_ASSERT(cast<Map::Entry::View>(hIter->next())->getValue()
                           ->equals(String::create("charley")));
              }
          else
              {
              TS_ASSERT(false);
              }
          }
  };
