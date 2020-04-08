/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/BoxUpdater.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::BoxUpdater;
using coherence::util::extractor::ReflectionUpdater;
using coherence::util::filter::EqualsFilter;

class BoxUpdateValue
    {
    public:
        BoxUpdateValue(int32_t nFoo = 0)
        : m_nFoo(nFoo)
        {}

    int32_t getBar() const
        {
        return m_nFoo;
        }

    void setBar(int32_t n)
        {
        m_nFoo = n;
        }

    private:
    int32_t m_nFoo;
    };

bool operator==(const BoxUpdateValue& fooA, const BoxUpdateValue& fooB)
    {
    return fooA.getBar() == fooB.getBar();
    }

size_t hash_value(const BoxUpdateValue& foo)
    {
    return (size_t) foo.getBar();
    }

template <typename Char, typename Traits> std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
        const BoxUpdateValue& foo)
    {
    out << foo.getBar();
    return out;
    }

class BoxUpdaterManagedValue
    : public class_spec<BoxUpdaterManagedValue>
    {
    friend class factory<BoxUpdaterManagedValue>;

    protected:
        BoxUpdaterManagedValue(int32_t nValue = 0)
            : m_nValue(nValue)
        {}

    public:
    int32_t getValue() const
        {
        return m_nValue;
        }

    void setValue(int32_t n)
        {
        m_nValue = n;
        }

    int32_t m_nValue;
    };

/**
* Test Suite for  BoxUpdater.
*/
class BoxUpdaterSuite : public CxxTest::TestSuite
    {
    public:
        void testManagedBoxUpdater()
            {
            BoxUpdateValue bar(123);
            Managed<BoxUpdateValue>::Handle hFoo = Managed<BoxUpdateValue>::create(bar);

            ReflectionUpdater::View vUpd =
                COH_MANAGED_BOX_UPDATER(BoxUpdateValue, setBar, Integer32::View);
            TS_ASSERT(vUpd->getMethodName()->equals("setBar"));

            Integer32::Handle hnValue = Integer32::create(456);
            vUpd->update(hFoo, hnValue);
            TS_ASSERT(hFoo->getBar() == 456);

             try
                {
                vUpd->update(hnValue, hnValue);

                TS_ASSERT(false);
                }
            catch (ClassCastException::View e)
                {
                }
            }

        void testBoxUpdater()
            {
            BoxUpdaterManagedValue::Handle hFoo = BoxUpdaterManagedValue::create(123);

            ReflectionUpdater::View vUpd =
                COH_BOX_UPDATER(BoxUpdaterManagedValue, setValue, Integer32::View);
            TS_ASSERT(vUpd->getMethodName()->equals("setValue"));

            Integer32::Handle hnValue = Integer32::create(456);
            vUpd->update(hFoo, hnValue);
            TS_ASSERT(hFoo->getValue() == 456);

             try
                {
                vUpd->update(hnValue, hnValue);

                TS_ASSERT(false);
                }
            catch (ClassCastException::View e)
                {
                }
            }
    };
