/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/net/NamedCache.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Random.hpp"
#include "coherence/util/Set.hpp"

#include "coherence/util/extractor/PofExtractor.hpp"

#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"

#include "common/AirDeal.hpp"
#include "common/AirDealComparator.hpp"
#include "common/TestClasses.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Random;
using coherence::util::Set;
using coherence::util::extractor::PofExtractor;
using coherence::util::filter::LessEqualsFilter;
using coherence::util::filter::LimitFilter;

using common::test::AirDeal;
using common::test::AirDealComparator;
using common::test::TestOrder;


/**
* LimitFilter Test Suite.
*/
class LimitFilterTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test requesting double the existing data.
        */
        void testPageSizeDouble()
            {
            cout << "testPageSizeDouble" << endl;

            NamedCache::Handle  hCache       = getNamedCache();
            LimitFilter::Handle hPagedFilter = 
                LimitFilter::create(AlwaysFilter::getInstance(), 
                                    m_cCacheSize * 2);

            // all entries, one page
            Set::Handle hAllData    = HashSet::create();
            int32_t     cIterations = 0;
            while (true)
                {
                Set::View vData = hCache->entrySet(hPagedFilter);

                if (vData->isEmpty())
                    {
                    break;
                    }
                hAllData->addAll(vData);
                cIterations++;
                hPagedFilter->nextPage();
                }
            cout << "Number of entries fetched "    << hAllData->size()
                 << ", iterations "                 << cIterations
                 << " : Number of items in Cache: " << hCache->size() << endl;
            TS_ASSERT_EQUALS(m_cCacheSize, hAllData->size());
            }

        /**
        * Test data broken in half.
        */
        void testPageSizeHalf()
            {
            cout << "testPageSizeHalf" << endl;

            NamedCache::Handle  hCache       = getNamedCache();
            LimitFilter::Handle hPagedFilter = 
                LimitFilter::create(AlwaysFilter::getInstance(), 
                                    m_cCacheSize / 2);

            // all entries, half of data per page
            Set::Handle hAllData    = HashSet::create();
            int32_t     cIterations = 0;
            while (true)
                {
                Set::View vData = hCache->entrySet(hPagedFilter);

                if (vData->isEmpty())
                    {
                    break;
                    }
                hAllData->addAll(vData);
                cIterations++;
                hPagedFilter->nextPage();
                }
            cout << "Number of entries fetched "    << hAllData->size()
                 << ", iterations "                 << cIterations
                 << " : Number of items in Cache: " << hCache->size() << endl;
            TS_ASSERT_EQUALS(m_cCacheSize, hAllData->size());
            }

        /**
        * Test one entry per page
        */
        void testPageSizeOne()
            {
            cout << "testPageSizeOne" << endl;

            NamedCache::Handle  hCache       = getNamedCache();
            LimitFilter::Handle hPagedFilter = 
                LimitFilter::create(AlwaysFilter::getInstance(), 1);

            // all entries, one per page
            Set::Handle hAllData    = HashSet::create();
            int32_t     cIterations = 0;
            while (true)
                {
                Set::View vData = hCache->entrySet(hPagedFilter);

                if (vData->isEmpty())
                    {
                    break;
                    }
                hAllData->addAll(vData);
                cIterations++;
                hPagedFilter->nextPage();
                }
            cout << "Number of entries fetched "   << hAllData->size()
                << ", iterations "                 << cIterations
                << " : Number of items in Cache: " << hCache->size() << endl;
            TS_ASSERT_EQUALS(m_cCacheSize, hAllData->size());
            }

        /**
        * Test data broken into 7 pages.
        */
        void testPageSizeSeven()
            {
            cout << "testPageSizeSeven" << endl;

            NamedCache::Handle  hCache       = getNamedCache();
            LimitFilter::Handle hPagedFilter = 
                LimitFilter::create(AlwaysFilter::getInstance(), 7);

            // all entries, in 7 pages
            Set::Handle hAllData    = HashSet::create();
            int32_t     cIterations = 0;
            while (true)
                {
                Set::View vData = hCache->entrySet(hPagedFilter);

                if (vData->isEmpty())
                    {
                    break;
                    }
                hAllData->addAll(vData);
                cIterations++;
                hPagedFilter->nextPage();
                }
            cout << "Number of entries fetched "    << hAllData->size()
                 << ", iterations "                 << cIterations
                 << " : Number of items in Cache: " << hCache->size() << endl;
            TS_ASSERT_EQUALS(m_cCacheSize, hAllData->size());
            }

        /**
        * Test data set broken into 3 pages.
        */
        void testPageSizeThird()
            {
            cout << "testPageSizeThird" << endl;

            NamedCache::Handle  hCache       = getNamedCache();
            LimitFilter::Handle hPagedFilter = 
                LimitFilter::create(AlwaysFilter::getInstance(), 
                                    m_cCacheSize / 3);

            // all entries, pages
            Set::Handle hAllData    = HashSet::create();
            int32_t     cIterations = 0;
            while (true)
                {
                Set::View vData = hCache->entrySet(hPagedFilter);

                if (vData->isEmpty())
                    {
                    break;
                    }
                hAllData->addAll(vData);
                cIterations++;
                hPagedFilter->nextPage();
                }
            cout << "Number of entries fetched "    << hAllData->size()
                 << ", iterations "                 << cIterations
                 << " : Number of items in Cache: " << hCache->size() << endl;
            TS_ASSERT_EQUALS(hAllData->size(), m_cCacheSize);
            }

        /**
        * Test setting an explicit page.
        */
        void testSetPage()
            {
            cout << "testSetPage" << endl;

            NamedCache::Handle hCache      = getNamedCache();
            int32_t            cPageSize   = m_cCacheSize / 10;
            int32_t            cTotalPages = m_cCacheSize / cPageSize +
                                             (m_cCacheSize % cPageSize > 
                                             0 ? 1 : 0);

            LimitFilter::Handle hPagedFilter = 
                LimitFilter::create(AlwaysFilter::getInstance(), cPageSize);
            Random::Handle      hRandom      = Random::create();
            hPagedFilter->setPage(hRandom->nextInt32(cTotalPages));
            // same pages have same keys
            Set::View vKeySet1 = hCache->keySet(hPagedFilter);
            Set::View vKeySet2 = hCache->keySet(hPagedFilter);
            TS_ASSERT(vKeySet1->equals(vKeySet2));
            }

        /**
        * Test moving forward and backwards through the data pages.
        */
        void testForwardAndBackward()
            {
            cout << "testForwardAndBackward" << endl;

            NamedCache::Handle hCache      = getNamedCache();
            int32_t            cPageSize   = m_cCacheSize / 10;
            int32_t            cTotalPages = m_cCacheSize / cPageSize +
                                             (m_cCacheSize % cPageSize > 
                                             0 ? 1 : 0);

            LimitFilter::Handle hPagedFilter = 
                LimitFilter::create(AlwaysFilter::getInstance(), cPageSize);
            ObjectArray::Handle hForward     =
                ObjectArray::create(cTotalPages);
            hPagedFilter->setPage(0);
            for (int32_t i = 0; i < cTotalPages; i++)
                {
                hForward[i] = hCache->keySet(hPagedFilter);
                hPagedFilter->nextPage();
                }

            ObjectArray::Handle hBackward = ObjectArray::create(cTotalPages);
            hPagedFilter->setPage(cTotalPages-1);
            for (int32_t i = cTotalPages - 1; i >= 0; i--)
                {
                hBackward[i] = hCache->keySet(hPagedFilter);

                if (i != 0)
                    {
                    hPagedFilter->previousPage();
                    }
                }

            for (int32_t i = 0; i < cTotalPages; i++)
                {
                TS_ASSERT(hForward[i]->equals(hBackward[i]));
                }
            }

        /**
        * Test LimitFilter moving through the data pages.
        */
        void testLimitFilter()
            {
            NamedCache::Handle  hCache         = ensureCleanCache("dist-cache");
            LimitFilter::Handle hLimitFilter   = 
                LimitFilter::create(AlwaysFilter::getInstance(), 10);
            Map::Handle         hMapReturn     = HashMap::create();
            bool                fEntryReturned = true;
            int32_t             cTotal         = 0,
                                cUnique        = 0,
                                cSize          = 84;
            hCache->clear();

            // add data to the cache
            for (int32_t i = 0; i < cSize; i++)
                {
                String::View vsIndex = COH_TO_STRING(i);
                hCache->put(vsIndex, vsIndex); 
                }

            while (fEntryReturned)
                {
                fEntryReturned = false;
                for (Iterator::Handle hIter = hCache->entrySet(hLimitFilter)->iterator();
                            hIter->hasNext(); )
                    {
                    Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                    ++cTotal;
                    fEntryReturned = true;
                    if (!hMapReturn->containsKey(vEntry->getKey()))
                        {
                        hMapReturn->put(vEntry->getKey(),
                                vEntry->getValue());
                        ++cUnique;
                        }
                    }
                hLimitFilter->nextPage();
                };

            TS_ASSERT(cTotal == cSize);
            TS_ASSERT(cTotal == cUnique);
            }

        /**
        * Test LimitFilter with custom comparator.
        */
        void testComparator()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-comparator-cache");
            Random::Handle     hRand  = Random::getInstance();
            for (int32_t i=0; i < 10000; i++)
                {
                AirDeal::View vDeal = AirDeal::create(i, String::create("SFO"), String::create("JFK"), hRand->nextFloat32());
                hCache->put(Integer32::create(vDeal->getOid()), vDeal);
                }
            ValueExtractor::Handle hExtractor = ReflectionExtractor::create(String::create("getOrigAirport"));
            hCache->addIndex(hExtractor, true, NULL);

            Filter::Handle hPrimaryFilter = EqualsFilter::create(hExtractor, String::create("SFO"));
            Filter::Handle hFilterLimit   = LimitFilter::create(hPrimaryFilter, 40);
            Set::View      vSetReturn     = hCache->entrySet(hFilterLimit, AirDealComparator::create());
            TS_ASSERT_EQUALS(vSetReturn->size(), (size32_t) 40);
            }

        /**
        * COH-14617 - Test LimitFilter with PofExtractor.
        */
        void testWithPofExtractor()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            Map::Handle hMap = HashMap::create(100);
            for (int32_t i = 1; i < 100; i++)
                {
                hMap->put(Integer32::create(i), TestOrder::create(
                        Integer32::create(i), COH_TO_STRING("CD" << i), i));
                }
            hCache->putAll(hMap);

            TS_ASSERT_EQUALS(hCache->entrySet(
                    LimitFilter::create(
                            LessEqualsFilter::create(PofExtractor::create(typeid(int32_t), 2),
                                    Integer32::create(42)), 10))
                    ->size(), size32_t(10));
            }

    private:
        /**
         * Return the cache used in all test methods.
         *
         * @param  cSize  the number of items to add to the cache
         *
         * @return the test cache
         */
        NamedCache::Handle getNamedCache(unsigned int cSize = s_cCACHE_SIZE)
            {
            m_cCacheSize = cSize;

            NamedCache::Handle hCache = ensureCleanCache("local-filter");
            hCache->clear();

            // add data to the cache
            for (unsigned int i = 0; i < m_cCacheSize; i++)
                {
                String::View vsIndex = COH_TO_STRING(i);
                hCache->put(vsIndex, vsIndex); 
                }
            cout << "Populated: Cache size: " << m_cCacheSize << endl << endl;
            return hCache;
            }

        /**
        * The number of entries in the cache.
        */
        unsigned int m_cCacheSize;

        /**
        * The default cache size for the LimitFilter tests.
        */
        static const unsigned int s_cCACHE_SIZE = 50;
    };
