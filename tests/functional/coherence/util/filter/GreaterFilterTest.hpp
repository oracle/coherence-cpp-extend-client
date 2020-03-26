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
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"


using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::GreaterFilter;
using coherence::util::HashSet;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;



/**
* Test Suite for the GreaterFilter.
*/
class GreaterFilterSuite : public CxxTest::TestSuite
  {
  public:
        /**
        * Test GreaterFilter
        * 
        * also test QueryMap API keySet
        *   
        */
      void testGreaterFilter()
          {
          NamedCache::Handle hCache = ensureCleanCache("dist-filter");

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

          HashSet::Handle hSet = HashSet::create();
          hSet->add(String::create("String-walley"));
          hSet->add(String::create("String-zebra"));      

          GreaterFilter::View hFilter = GreaterFilter::create(
                  IdentityExtractor::getInstance(),
                  String::create("mary"));

          Set::View vFilterEntries = hCache->keySet(hFilter);
          if (vFilterEntries->size() == 2)
              {
              Iterator::Handle hIter = vFilterEntries->iterator();
              TS_ASSERT(hSet->remove(hIter->next()));
              TS_ASSERT(hSet->remove(hIter->next()));
              TS_ASSERT(hSet->size() == 0);
              }
          else
              {
              TS_ASSERT(false);
              }
          }
  };
