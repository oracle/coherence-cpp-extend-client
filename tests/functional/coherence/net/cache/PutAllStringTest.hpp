/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/util/Hashtable.hpp"

#include "common/TestUtils.hpp"

#include <fstream>

using namespace coherence::lang;

using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::Hashtable;

/**
* putAll() performance test suite.
*/
class PutAllStringTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test performance on a local Hashtable,put, and a Distributed cache.putAll.
        * This uses a set of standard data, loaded from a file, and outputs the
        * results to a standard out.  (Result is not checked into p4).
        *
        * This is a test of putAll(); first the data is loaded from
        * from the data file into a Hashtable. Then the entries of that Hashtable
        * are loaded into another Hashtable, one at a time, and each put() time
        * is captured and stored. (Note: the data file has >40000 entries, which
        * is too much data for some machines.  This test actually uses 20000+
        * entries.
        *
        * The cache is populated from the second Hashtable, using cache.putAll().
        * The cache is cleared, then populated 10 times, and the time of each
        * population is captured and stored.
        *
        * Ending statistics report the average time for the first local put, and
        * for the remote putAll.
        */
        void testPutAllPerformance()
            {
            // COH-7674 Temporarily disable the test while investigating build failures
#ifdef NEVER
            // open the data file, read stuff in and construct a HashSet from it.
            // This HashSet will be used to load the Hashtable that will be used in the
            // putAll
            Hashtable::Handle hLoadedData = Hashtable::create();

	        String::View baseKey
                    = String::create("/tests/functional/net/cache/PutAllStringTest/key_");
            std::stringstream inputFile;
            inputFile << getenv("DEV_ROOT")
                    << "/prj/tests/functional/defaultconfig/PutAllStringTest.txt";

	        std::ifstream inputStream(inputFile.str().c_str(), std::fstream::in);
	        std::string cs_value;

	        int i = 0;
	        std::stringstream strStream;

	        while(getline(inputStream, cs_value))
	            {
		        strStream << baseKey << i;
                String::Handle sKey   = String::create(strStream.str());
                String::Handle sValue = String::create(cs_value);
		        strStream.str("");

		        hLoadedData->put(sKey, sValue);
		        ++i;
	            }
            int32_t cNumberOfEntries = i;

            /**
            * construct the hash map that will be used to putAll.
            */
            Hashtable::Handle      hPutAllHashtable = Hashtable::create();
            Array<int64_t>::Handle hPutTime         = Array<int64_t>::create(cNumberOfEntries);
            Set::View              vSet             = hLoadedData->keySet();
            Iterator::Handle       hIterator        = vSet->iterator();

            int64_t ldtStartTime;
            int64_t ldtEndTime;
            int32_t cEntryCount = 0;

            /**
            * The data load can cause problems on a single machine, lower it down to a point
            * where the test doesn't fail.  (Failure is a lack of response from the
            * remote cache; connect is closed.).
            */
            while (hIterator->hasNext() && cEntryCount < cMaxUsed)
                {
                Object::View key = hIterator->next();
                ldtStartTime = System::currentTimeMillis();
                hPutAllHashtable->put(key, key);
                ldtEndTime = System::currentTimeMillis();
                hPutTime[cEntryCount] = ldtEndTime - ldtStartTime;
                cEntryCount++;
                }

            /**
            * do the putall.  We want to do this at least 10 times, and clear the cache each
            * time so there's nothing in it when we putall.
            * average the length of time it took over each putall.  Do not include
            * the time spent in cache.clear().
            */

            Array<int64_t>::Handle hPutAllTime = Array<int64_t>::create(cNumberOfIterations);

            NamedCache::Handle hCache = CacheFactory::getCache("dist-test");
            for (i = 0; i < cNumberOfIterations; i++)
                {
                 hCache->clear();

                 // start time
                 ldtStartTime = System::currentTimeMillis();

                 hCache->putAll(hPutAllHashtable);

                 // end time
                 ldtEndTime = System::currentTimeMillis();

                 // store duration
                 hPutAllTime[i] = ldtEndTime - ldtStartTime;
                 }
            hCache->clear();
            hCache->release();


            // write the benchmark to standard out.
            int64_t total = 0;
            for (i = 0; i < cNumberOfEntries; i++)
                {
                total += hPutTime[i];
                }

            int64_t avgPutTime = total/cEntryCount;

            total = 0;
            for (i = 0; i < cNumberOfIterations; i++)
                {
                total += hPutAllTime[i];
                }

            int64_t avgPutAllTime = total/cNumberOfIterations;

            std::cout << std::endl;
            std::cout << "-------------------------------------------------------------" << std::endl;
            std::cout << "PutAllStringTest BenchMark Start" << std::endl;
            std::cout << "  Loading Hashtable: " << std::endl;
            std::cout << "    Number of data entries loaded: " << cNumberOfEntries << std::endl;
            std::cout << "    Number of data entries used:   " << cEntryCount << std::endl;
            std::cout << "    Average time to put one entry (millisec): " << avgPutTime << std::endl;
            std::cout << "  Cache.putAll: " << std::endl;
            std::cout << "    Number of putAll iterations:   " << cNumberOfIterations << std::endl;
            std::cout << "    Average time to putAll one data set (millisec): " << avgPutAllTime << std::endl;
            std::cout << "PutAllStringTest BenchMark End" << std::endl;
            std::cout << "-------------------------------------------------------------" << std::endl << std::endl;
#endif /* NEVER */
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }

    /**
    *  Data constants
    */

    // number of data entries actually used in test
    static const int32_t cMaxUsed            = 1000;

    // number of times to do the putAll
    static const int16_t cNumberOfIterations = 10;
    };
