/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/BoxExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::BoxExtractor;
using coherence::util::filter::EqualsFilter;

class Foo
    {
    public:
    Foo(int32_t nFoo = 0)
        : m_nFoo(nFoo)
        {}

    int32_t getBar() const
        {
        return m_nFoo;
        }

    private:
    int32_t m_nFoo;
    };

bool operator==(const Foo& fooA, const Foo& fooB)
    {
    return fooA.getBar() == fooB.getBar();
    }

size_t hash_value(const Foo& foo)
    {
    return (size_t) foo.getBar();
    }

template<typename Char, typename Traits> std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
        const Foo& foo)
    {
    out << foo.getBar();
    return out;
    }

/**
* Test Suite for  BoxExtractor.
*/
class BoxExtractorSuite : public CxxTest::TestSuite
    {
    public:
         /**
        * Test BoxExtractor
        */
        void testBoxExtractor()
            {
            EqualsFilter::Handle hFilter = EqualsFilter::create(
                    COH_BOX_EXTRACTOR(Integer32::View, Float32, getInt32Value),
                    Integer32::create(Float32::create(42.0F)->getInt32Value()));
            TS_ASSERT(hFilter->evaluate(Float32::create(42.0F)));
            TS_ASSERT(!hFilter->evaluate(Float32::create(43.0F)));
            try
                {
                hFilter->evaluate(Integer32::create(123));

                TS_ASSERT(false);
                }
            catch (ClassCastException::View e)
                {
                }
            }

        void testManagedBoxExtractor()
            {
            EqualsFilter::Handle hFilter = EqualsFilter::create(
                    COH_MANAGED_BOX_EXTRACTOR(Integer32::View, Foo, getBar),
                    Integer32::create(123));
            TS_ASSERT(hFilter->evaluate(Managed<Foo>::create(Foo(123))));
            TS_ASSERT(!hFilter->evaluate(Managed<Foo>::create(Foo(456))));
            try
                {
                hFilter->evaluate(Integer32::create(123));

                TS_ASSERT(false);
                }
            catch (ClassCastException::View e)
                {
                }
            }
    };
