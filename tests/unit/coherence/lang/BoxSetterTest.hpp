/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/lang/TypedMethod.hpp"

#include <string>

using namespace coherence::lang;


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

    std::string getName() const
        {
        return m_sName;
        }

    void setName(std::string sName)
        {
        m_sName = sName;
        }

    private:
    int32_t m_nFoo;
    std::string m_sName;
    };

bool operator==(const BoxUpdateValue& fooA, const BoxUpdateValue& fooB)
    {
    return fooA.getBar() == fooB.getBar() && fooA.getName() == fooB.getName();
    }

size_t hash_value(const BoxUpdateValue& foo)
    {
    return (size_t) foo.getBar();
    }

template<typename Char, typename Traits> std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out,
        const BoxUpdateValue& foo)
    {
    out << foo.getBar() << " " << foo.getName();
    return out;
    }

class BoxSetterManagedValue
    : public class_spec<BoxSetterManagedValue>
    {
    friend class factory<BoxSetterManagedValue>;

    protected:
        BoxSetterManagedValue(int32_t nValue = 0)
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
* Test Suite for  BoxSetter.
*/
class BoxSetterSuite : public CxxTest::TestSuite
    {
    public:
       void testManagedBoxSetter()
            {
            BoxUpdateValue bar(123);
            Managed<BoxUpdateValue>::Handle hFoo = Managed<BoxUpdateValue>::create(bar);

            Class::View vClass = TypedClass<Managed<BoxUpdateValue> >::create()
                    ->declare(COH_MANAGED_PROPERTY(BoxUpdateValue, Bar, Integer32::View))
                    ->declare(COH_MANAGED_PROPERTY(BoxUpdateValue, Name, String::View));

            ObjectArray::Handle haClassParam = ObjectArray::create(1);
            haClassParam[0] = SystemClassLoader::getInstance()->loadByType(typeid(Integer32));
            Method::View vMethodSet = vClass->getDeclaredMethod("setBar", haClassParam);

            ObjectArray::Handle hanValue = ObjectArray::create(1);
            hanValue[0] = Integer32::create(456);
            vMethodSet->invoke(hFoo, hanValue);

            TS_ASSERT(hFoo->getBar() == 456);

            Method::View vMethodGet = vClass->getDeclaredMethod("getBar");
            TS_ASSERT(cast<Integer32::View>(vMethodGet->invoke(hFoo))->getValue() == 456);

            hanValue[0] = String::create("mark");
            Method::View vMethodSetName = vClass->getDeclaredMethod("setName", hanValue);
            vMethodSetName->invoke(hFoo, hanValue);
            TS_ASSERT(vClass->getDeclaredMethod("getName")->invoke(hFoo)->equals(hanValue[0]));
            }

        void testBoxSetter()
            {
            BoxSetterManagedValue::Handle hFoo = BoxSetterManagedValue::create(123);

            Class::View vClass = TypedClass<BoxSetterManagedValue>::create()
                    ->declare(COH_BOX_PROPERTY(BoxSetterManagedValue, Value, Integer32::View));

            ObjectArray::Handle haClassParam = ObjectArray::create(1);
            haClassParam[0] = SystemClassLoader::getInstance()->loadByType(typeid(Integer32));
            Method::View vMethodSet = vClass->getDeclaredMethod("setValue", haClassParam);

            ObjectArray::Handle hanValue = ObjectArray::create(1);
            hanValue[0] = Integer32::create(456);
            vMethodSet->invoke(hFoo, hanValue);

            TS_ASSERT(hFoo->getValue() == 456);
            }
    };
