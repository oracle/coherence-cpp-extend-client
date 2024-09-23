/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"

#include "coherence/tests/SimplePerson.hpp"

#include "coherence/util/extractor/UniversalUpdater.hpp"

#include "coherence/util/processor/UpdaterProcessor.hpp"

#include "common/TestClasses.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;

using coherence::util::extractor::UniversalUpdater;
using coherence::util::processor::UpdaterProcessor;
using coherence::tests::SimplePerson;

/**
 * Test Suite for the UniversalUpdater.
 */
class UniversalUpdaterTests : public CxxTest::TestSuite
  {
  public:
    /**
     * Update SimplePerson LastName.
     */
    void testUniversalUpdater()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-filter");

        ArrayList::Handle hasChildrenIds = ArrayList::create(1);
        hasChildrenIds->add(String::create("456-78-9123"));

        String::View           vsKey      = "p1";
        String::View           vsLastName = "Van Halen";
        SimplePerson::View     vPerson    = SimplePerson::create(String::create("123-45-6789"),
            String::create("Eddie"), vsLastName, 1955,
            String::create("987-65-4321"), hasChildrenIds);
        UniversalUpdater::View vUpdater   = UniversalUpdater::create(String::create("LastName"));

        hCache->put(vsKey, vPerson);
        vPerson = cast<SimplePerson::View>(hCache->get(vsKey));
        TS_ASSERT(vPerson->getLastName()->equals(vsLastName));

        // update the last name
        vsLastName = "Van Helen";
        hCache->invoke(vsKey, UpdaterProcessor::create(vUpdater, vsLastName));
        vPerson = cast<SimplePerson::View>(hCache->get(vsKey));
        TS_ASSERT(vPerson->getLastName()->equals(vsLastName));
        }
  };
