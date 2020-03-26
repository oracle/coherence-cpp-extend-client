/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashMap.hpp"

#include "coherence/stl/boxing_map.hpp"

using namespace coherence::lang;
using namespace coherence::util;
using namespace coherence::stl;

class Address
    {
    public:
        Address()
            : m_nZip(0)
            {
            }

        Address(const Address& that)
            : m_sCity(that.m_sCity), m_sState(that.m_sState), m_nZip(that.m_nZip)
            {
            }

        Address(std::string sCity, std::string sState, int nZip)
            : m_sCity(sCity), m_sState(sState), m_nZip(nZip)
            {
            }

        bool operator==(const Address& that) const
            {
            return m_nZip   == that.m_nZip &&
                   m_sState == that.m_sState &&
                   m_sCity  == that.m_sCity;
            }

    public:
        std::string m_sCity;
        std::string m_sState;
        int         m_nZip;
    };

template <typename Char, typename Traits> std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
        const Address& addr)
    {
    out << addr.m_sCity << ",  " << addr.m_sState << " " << addr.m_nZip;
    return out;
    }

size_t hash_value(const Address& addr)
    {
    return addr.m_nZip;
    }

/**
* Test Suite for Boxing_map.
*/
class BoxingMapTest : public CxxTest::TestSuite
    {
    public:
        void testPutGet()
            {
            boxing_map<String, Integer32> msi(HashMap::create());
            std::string sKey = "hello";

            msi[std::string("hello")] = 123;
            TS_ASSERT(msi[std::string("hello")] == 123);
            msi[sKey] = msi[sKey] + 1;
            TS_ASSERT(msi[sKey] == 124);

            boxing_map<Integer32, String> mis(HashMap::create());
            mis[123] = sKey;

            TS_ASSERT(mis[123] == sKey);

            TS_ASSERT(is_null(mis[234]));
            TS_ASSERT(mis[234] != sKey);

            TS_ASSERT(mis[999] == NULL); // test non-strict unbox
            }

        void testIteration()
            {
            typedef boxing_map<String, Integer32> mymap;
            mymap map(HashMap::create());

            TS_ASSERT(map.empty());
            TS_ASSERT_EQUALS(0UL, map.size());
            TS_ASSERT_EQUALS(map.begin(), map.end());

            map[std::string("one")] = 1;

            TS_ASSERT(!map.empty());
            TS_ASSERT_EQUALS(1UL, map.size());

            // COH-9732 - verify that map.begin() points to the singleton entry
            TS_ASSERT_DIFFERS(map.begin(), map.end());
            TS_ASSERT_EQUALS(std::string("one"), map.begin()->first);
            TS_ASSERT_EQUALS(1, map.begin()->second);

            map[std::string("two")]   = 2;
            map[std::string("three")] = 3;
            map[std::string("four")]  = 4;

            int c= 0;
            for (mymap::const_iterator i = map.begin(), e = map.end(); i != e; ++i)
                {
                int32_t nVal = i->second;
                switch (nVal)
                    {
                    case 1:
                        TS_ASSERT(i->first == std::string("one"));
                        break;

                    case 2:
                        TS_ASSERT(i->first == std::string("two"));
                        break;

                    case 3:
                        TS_ASSERT(i->first == std::string("three"));
                        break;

                    case 4:
                        TS_ASSERT(i->first == std::string("four"));
                        break;

                    default:
                        TS_ASSERT(false);
                    }
                ++c;
                }
            TS_ASSERT (c == 4);
            }

        void testFindIterator()
            {
            typedef boxing_map<String, Integer32> mymap;
            mymap map(HashMap::create());
            map[std::string("one")]   = 1;
            map[std::string("two")]   = 2;
            map[std::string("three")] = 3;
            map[std::string("four")]  = 4;

            map.find(std::string("two"))->second = 6;

            TS_ASSERT(map[std::string("two")] == 6);
            }

        void testFindAdvance()
            {
            typedef boxing_map<String, Integer32> mymap;
            mymap map(HashMap::create());
            map[std::string("one")]   = 1;
            map[std::string("two")]   = 2;
            map[std::string("three")] = 3;
            map[std::string("four")]  = 4;

            Iterator::Handle hIter = map.delegate()->keySet()->iterator();
            hIter->next();
            String::View vsNext = cast<String::View>(hIter->next());
            mymap::iterator i = map.find(vsNext);

            int c = 0;
            for (mymap::iterator e = map.end(); i != e; ++i)
                {
                ++c;
                }
            TS_ASSERT(c == 3);
            }

        void testInteropCopyIn()
            {
            typedef std::map<std::string, int32_t> theirmap;
            typedef boxing_map<String, Integer32>  mymap;

            theirmap tmap;
            mymap mmap(HashMap::create());

            tmap["one"]   = 1;
            tmap["two"]   = 2;
            tmap["three"] = 3;
            tmap["four"]  = 4;

            copy_map(tmap, mmap);

            int c= 0;
            for (mymap::const_iterator i = mmap.begin(), e = mmap.end(); i != e; ++i)
                {
                int32_t nValue = i->second;
                switch (nValue)
                    {
                    case 1:
                        TS_ASSERT(i->first == std::string("one"));
                        break;

                    case 2:
                        TS_ASSERT(i->first == std::string("two"));
                        break;

                    case 3:
                        TS_ASSERT(i->first == std::string("three"));
                        break;

                    case 4:
                        TS_ASSERT(i->first == std::string("four"));
                        break;

                    default:
                        TS_ASSERT(false);
                    }
                ++c;
                }
            TS_ASSERT (c == 4);
            }

        void testInteropCopyOut()
            {
            typedef std::map<std::string, int32_t> theirmap;
            typedef boxing_map<String, Integer32>  mymap;

            theirmap tmap;
            mymap mmap(HashMap::create());

            mmap[std::string("one")]   = 1;
            mmap[std::string("two")]   = 2;
            mmap[std::string("three")] = 3;
            mmap[std::string("four")]  = 4;

            // tmap.insert(mmap.begin(), mmap.end()); // doens't work on solaris
            coherence::stl::copy_map(mmap, tmap);

            int c= 0;
            for (theirmap::const_iterator i = tmap.begin(), e = tmap.end(); i != e; ++i)
                {
                int32_t nValue = i->second;
                switch (nValue)
                    {
                    case 1:
                        TS_ASSERT(i->first == std::string("one"));
                        break;

                    case 2:
                        TS_ASSERT(i->first == std::string("two"));
                        break;

                    case 3:
                        TS_ASSERT(i->first == std::string("three"));
                        break;

                    case 4:
                        TS_ASSERT(i->first == std::string("four"));
                        break;

                    default:
                        TS_ASSERT(false);
                    }
                ++c;
                }
            TS_ASSERT (c == 4);
            }

        void testManagedData()
            {
            typedef boxing_map<Integer32, Managed<Address> > addr_cache;
            typedef std::map<int, Address> addr_map;

            addr_cache cache;
            addr_map   map;

            cache = HashMap::create();

            cache[2421]  = Address("Burlington", "MA", 2421);
            cache[1803]  = Address("Hoboken", "NJ", 1803);
            cache[90041] = Address("Los Angeles", "CA", 90041);

            // map.insert(cache.begin(), cache.end()); // doesn't work on solaris
            coherence::stl::copy_map(cache, map);

            TS_ASSERT(map[2421].m_nZip = 2421);
            TS_ASSERT(map[1803].m_nZip = 1803);
            TS_ASSERT(map[90041].m_nZip = 90041);
            }

        void testViewDelegate()
            {
            typedef boxing_map<String, Integer32>  mymap;
            Map::Handle hMap = HashMap::create();
            Map::View   vMap = hMap;
            const mymap map(vMap);

            hMap->put(String::create("one"),   Integer32::valueOf(1));
            hMap->put(String::create("two"),   Integer32::valueOf(2));
            hMap->put(String::create("three"), Integer32::valueOf(3));
            hMap->put(String::create("four"),  Integer32::valueOf(4));

            int c= 0;
             for (mymap::const_iterator i = map.begin(), e = map.end(); i != e; ++i)
                 {
                 int32_t nValue = i->second;
                 switch (nValue)
                     {
                     case 1:
                         TS_ASSERT(i->first == std::string("one"));
                         break;

                     case 2:
                         TS_ASSERT(i->first == std::string("two"));
                         break;

                     case 3:
                         TS_ASSERT(i->first == std::string("three"));
                         break;

                     case 4:
                         TS_ASSERT(i->first == std::string("four"));
                         break;

                     default:
                         TS_ASSERT(false);
                     }
                 ++c;
                 }
             TS_ASSERT (c == 4);
            }
    };
