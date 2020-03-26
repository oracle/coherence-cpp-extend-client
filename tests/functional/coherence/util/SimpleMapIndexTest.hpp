/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "private/coherence/net/cache/LocalQueryCache.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/filter/ContainsAnyFilter.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

using namespace coherence::lang;

using coherence::net::cache::LocalQueryCache;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::Collections;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::ContainsAnyFilter;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Map;
using coherence::util::Set;


/**
* Test Suite for SimpleMapIndexTest
*/
class SimpleMapIndexTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test indexed value as collection
        */
        void testCollection()
            {
            LocalQueryCache::Handle hCache = LocalQueryCache::create();

            hCache->clear();
            hCache->addIndex(IdentityExtractor::getInstance(), false, NULL);
            DoPutALL::Handle hDoPutAll = DoPutALL::create(hCache, "collection");

            Thread::Handle hThread = Thread::create(hDoPutAll);
            hThread->start();

            Thread::sleep(2222);

            ContainsAnyFilter::Handle hFilter = ContainsAnyFilter::create(IdentityExtractor::getInstance(), 
                    Collections::singleton(String::create("test")));
            for (int i = 0; i < 50000; i++)
                {
                Set::View hResult = hCache->entrySet(hFilter);
                TS_ASSERT_EQUALS(hResult->size(), hCache->size());
                }

            hDoPutAll->stop();
            hThread->join();
            }

        void testArray()
            {
            LocalQueryCache::Handle hCache = LocalQueryCache::create();

            hCache->clear();
            hCache->addIndex(IdentityExtractor::getInstance(), false, NULL);

            DoPutALL::Handle hDoPutAll = DoPutALL::create(hCache, "array");

            Thread::Handle hThread = Thread::create(hDoPutAll);
            hThread->start();

            Thread::sleep(2222);

            ContainsAnyFilter::Handle hFilter = ContainsAnyFilter::create(IdentityExtractor::getInstance(),
                    Collections::singleton(String::create("test")));

            for (int i = 0; i < 50000; i++)
                {
                Set::View hResult = hCache->entrySet(hFilter);
                TS_ASSERT_EQUALS(hResult->size(), hCache->size());
                }

            hDoPutAll->stop();
            hThread->join();
            }


    class DoPutALL
        : public class_spec<DoPutALL,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<DoPutALL>;

        protected:
            /**
            * Constructor
            */
            DoPutALL(LocalQueryCache::Handle hCache, String::View vsName)
                : m_hCache(self(), hCache),
                  m_vsName(self(), vsName),
                  m_fStop(false)
            {
            }

        // ----- Runnable interface ---------------------------------------------

        public:
            void run()
                {
                LocalQueryCache::Handle hCache = m_hCache;
                String::View            vsName = m_vsName;

                while (!isStopRequested())
                    {
                    if (vsName->equals("collection"))
                        {
                        doCollection(hCache, vsName);
                        }
                    else
                        {
                        doArray(hCache, vsName);
                        }
                    }
                }

        // ----- DoPutAll methods ------------------------------------------------

        void doCollection(LocalQueryCache::Handle hCache, String::View vsName)
            {
            Collection::Handle values = ArrayList::create();
            for (int i = 0; i <= 8; i++)
                {
                // duplicates will cause Missing inverse index messages
                String::View val1 = "test";
                String::View val2 = COH_TO_STRING("test" << ((rand()%10) + 1));
                values->add(val1);
                values->add(val2);

                Map::Handle hMap = HashMap::create();
                hMap->put(vsName, values);

                hCache->putAll(hMap);
                }
            }


        void doArray(LocalQueryCache::Handle hCache, String::View vsName)
            {
            ObjectArray::Handle hAvalues = ObjectArray::create(10);
            for  (int i = 0; i <= 9; i++)
                {
                hAvalues[i] = String::create("null"); 
                }

            for (int i = 0; i <= 8; i++)
                {
                // duplicates will cause Missing inverse index messages
                hAvalues[i]     = String::create("test");
                hAvalues[i + 1] = COH_TO_STRING(hAvalues[i] << ((rand()%10) + 1));

                Map::Handle hMap = HashMap::create();
                hMap->put(vsName, hAvalues);

                hCache->putAll(hMap);
                }
            }

        void stop()
            {
            m_fStop = true;
            }

        bool isStopRequested()
            {
            return m_fStop;
            }


        // ----- data members ---------------------------------------------------

        private:
	    MemberHandle<LocalQueryCache> m_hCache;
            MemberView<String>            m_vsName;
            bool                          m_fStop;            
        };
    };
