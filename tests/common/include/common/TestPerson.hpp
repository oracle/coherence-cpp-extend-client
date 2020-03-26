/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_PERSON_HPP
#define COH_TEST_PERSON_HPP

#include "coherence/lang.ns"

#include "coherence/lang/annotation/Inherited.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::lang::annotation::Inherited;

class Human
    : public class_spec<Human,
          extends<Object>,
          implements<Annotation> >
    {
    friend class factory<Human>;

    public:
        virtual Class::View annotationType() const
            {
            return SystemClassLoader::getInstance()->loadByType(typeid(Human));
            }
    };
COH_REGISTER_CLASS(TypedClass<Human>::create()
        ->annotate(Inherited::getInstance()));

class PersonInterface
    : public interface_spec<PersonInterface>
    {
    public:
        virtual String::View getName() const = 0;
        virtual void setName(String::View vsName) = 0;
        virtual int32_t getAge() const = 0;
        virtual void setAge(int32_t nAge) = 0;
    };
COH_REGISTER_CLASS(TypedBarrenClass<PersonInterface>::create()
        ->declare(COH_PROPERTY    (PersonInterface, Name, String::View))
        ->declare(COH_BOX_PROPERTY(PersonInterface, Age,  Integer32::View)));

class TestPerson
    : public class_spec<TestPerson,
          extends<Object>,
          implements<PersonInterface> >
    {
    friend class factory<TestPerson>;

    protected:
        TestPerson(String::View vsName = String::null_string, int32_t nAge = 0)
            : m_vsName(self(), vsName), m_nAge(nAge)
            {
            }

    public:
        virtual String::View getName() const
            {
            return m_vsName;
            }
        virtual void setName(String::View vsName)
            {
            m_vsName = vsName;
            }
        virtual int32_t getAge() const
            {
            return m_nAge;
            }
        virtual void setAge(int32_t nAge)
            {
            m_nAge = nAge;
            }

    MemberView<String> m_vsName;
    int32_t m_nAge;
    };
COH_REGISTER_CLASS(TypedClass<TestPerson>::create()
        ->annotate(Human::create())
        ->declare(COH_CONSTRUCTOR2(TestPerson, String::View, BoxHandle<const Integer32>)));

class MarriedPerson
    : public class_spec<MarriedPerson,
          extends<TestPerson> >
    {
    friend class factory<MarriedPerson>;

    protected:
        MarriedPerson(String::View vsName = String::null_string, int32_t nAge = 0,
                MarriedPerson::Handle hSpouse = NULL);

    public:
    virtual MarriedPerson::Handle getSpouse() const;
    virtual void setSpouse(MarriedPerson::Handle hSpouse);
    static bool areMarried(MarriedPerson::View vPerson1, MarriedPerson::View vPerson2);

    mutable MemberHandle<MarriedPerson> m_hSpouse;
    };
COH_REGISTER_TYPED_CLASS(MarriedPerson);

MarriedPerson::MarriedPerson(String::View vsName, int32_t nAge, MarriedPerson::Handle hSpouse)
    : super(vsName, nAge), m_hSpouse(self(), hSpouse, true)
    {
    }
COH_REGISTER_METHOD(MarriedPerson, COH_CONSTRUCTOR3(MarriedPerson, String::View, BoxHandle<const Integer32>, MarriedPerson::Handle));

MarriedPerson::Handle MarriedPerson::getSpouse() const
    {
    return m_hSpouse;
    }
COH_REGISTER_METHOD(MarriedPerson, COH_METHOD0(MarriedPerson::Handle, MarriedPerson::View, getSpouse));

void MarriedPerson::setSpouse(MarriedPerson::Handle hSpouse)
    {
    m_hSpouse = hSpouse;
    }
COH_REGISTER_METHOD(MarriedPerson, COH_METHOD1(void, MarriedPerson::Handle, setSpouse, MarriedPerson::Handle));

// demonstrate adding an annotation while registering a method
bool MarriedPerson::areMarried(MarriedPerson::View vPerson1, MarriedPerson::View vPerson2)
    {
    return Object::equals(vPerson1->getSpouse(), vPerson2) && Object::equals(vPerson2->getSpouse(), vPerson1);
    }
COH_REGISTER_METHOD(MarriedPerson, COH_STATIC_METHOD2(BoxHandle<const Boolean> , MarriedPerson, areMarried, MarriedPerson::View, MarriedPerson::View)
        ->annotate(Inherited::getInstance()));

class SalariedPerson
    : public class_spec<SalariedPerson,
          extends<TestPerson> >
    {
    friend class factory<SalariedPerson>;

    protected:
        SalariedPerson(String::View vsName = String::null_string, int32_t nAge = 0, float64_t flSalary = 0);

    public:
        virtual float64_t getSalary() const;
        virtual void      setSalary(float64_t flSalary);

    float64_t m_flSalary;
    };
COH_REGISTER_CLASS(TypedClass<SalariedPerson>::create()
    ->declare(COH_CONSTRUCTOR3(SalariedPerson, String::View, BoxHandle<const Integer32>, BoxHandle<const Float64>)) 
    ->declare(COH_BOX_PROPERTY(SalariedPerson, Salary, Float64::View)));

SalariedPerson::SalariedPerson(String::View vsName, int32_t nAge, float64_t flSalary)
    : super(vsName, nAge), m_flSalary(flSalary)
    {
    }

float64_t SalariedPerson::getSalary() const
    {
    return m_flSalary;
    }

void SalariedPerson::setSalary(float64_t flSalary)
    {
    m_flSalary = flSalary;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_PERSON_HPP
