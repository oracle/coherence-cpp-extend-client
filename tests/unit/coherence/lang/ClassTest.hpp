/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/HashSet.hpp"

#include "common/TestPerson.hpp"

#include<typeinfo>

using namespace coherence::lang;
using namespace coherence::util;

using namespace common::test;

namespace class_test
    {
    class RegisteredClass
        : public class_spec<RegisteredClass>
            {
            friend class factory<RegisteredClass>;
            };

    COH_REGISTER_TYPED_CLASS(RegisteredClass);
    }


/**
* Test Suite for the Class object.
*/
class ClassTest : public CxxTest::TestSuite
    {
    // ----- inner classes: A, B, C ------------------------------------------

    class A
        : public abstract_spec<A>
        {
        };

    class B
        : public class_spec<B, extends<A> >
        {
        friend class factory<B>;
        };

    class C
        : public class_spec<C, extends<B> >
        {
        friend class factory<C>;
        };


    public:
        void testNewInstance()
            {
            Class::View vClass = SystemClassLoader::getInstance()->
                    loadByName("class_test::RegisteredClass");
            class_test::RegisteredClass::Handle hSet =
                cast<class_test::RegisteredClass::Handle>(vClass->newInstance());

            TS_ASSERT(hSet != NULL);
            }

        void testIsAssignableFrom()
            {
            ClassLoader::View vLoader = SystemClassLoader::getInstance();
            TS_ASSERT(vLoader->loadByType(typeid(Object))->isAssignableFrom(vLoader->loadByType(typeid(PersonInterface))));
            TS_ASSERT(vLoader->loadByType(typeid(Object))->isAssignableFrom(vLoader->loadByType(typeid(TestPerson))));
            TS_ASSERT(vLoader->loadByType(typeid(Object))->isAssignableFrom(vLoader->loadByType(typeid(MarriedPerson))));
            TS_ASSERT(vLoader->loadByType(typeid(TestPerson))->isAssignableFrom(vLoader->loadByType(typeid(MarriedPerson))));
            TS_ASSERT(vLoader->loadByType(typeid(PersonInterface))->isAssignableFrom(vLoader->loadByType(typeid(MarriedPerson))));
            TS_ASSERT(!vLoader->loadByType(typeid(MarriedPerson))->isAssignableFrom(vLoader->loadByType(typeid(TestPerson))));
            }

        void testGetMethod()
            {
            ClassLoader::View vLoader = SystemClassLoader::getInstance();
            Method::View vMethod = vLoader->loadByType(typeid(MarriedPerson))->getMethod("getAge");
            TS_ASSERT(vMethod->getName()->equals("getAge"));

            try
                {
                vLoader->loadByType(typeid(MarriedPerson))->getMethod("forceFailure");
                TS_ASSERT(false);
                }
            catch (NoSuchMethodException::View vEx)
                {
                }
            }

        void testStaticReflection()
            {
            ClassLoader::View vLoader = SystemClassLoader::getInstance();

            MarriedPerson::Handle hPerson1 = MarriedPerson::create("Mark", 36);
            MarriedPerson::Handle hPerson2 = MarriedPerson::create("Otti", 36, hPerson1);
            hPerson1->setSpouse(hPerson2);

            ObjectArray::Handle haParam = ObjectArray::create(2);
            haParam[0] = hPerson1;
            haParam[1] = hPerson2;

            Method::View vMethod = vLoader->loadByType(typeid(MarriedPerson))->getMethod("areMarried", haParam, Method::modifier_static);

            TS_ASSERT(cast<Boolean::View>(vMethod->invoke(NULL, haParam))->getValue());

            haParam[0] = String::create("Mark");
            haParam[1] = Integer32::create(36);
            vMethod = vLoader->loadByType(typeid(TestPerson))->getMethod("create", haParam, Method::modifier_static);
            TS_ASSERT(cast<TestPerson::Handle>(vMethod->invoke(NULL, haParam))->getName()->equals("Mark"));

            ObjectArray::Handle haParam3 = ObjectArray::create(3);
            haParam3[0] = String::create("Mark");
            haParam3[1] = Integer32::create(36);
            haParam3[2] = hPerson2;

            TS_ASSERT(cast<MarriedPerson::Handle>(vLoader->loadByType(typeid(MarriedPerson))->newInstance(haParam3))->getSpouse() == hPerson2);
            }

        void testAnnotation()
            {
            ClassLoader::View vLoader = SystemClassLoader::getInstance();
            Class::View       vClass  = vLoader->loadByType(typeid(MarriedPerson));
            Class::View       vType   = vLoader->loadByType(typeid(Human));

            TS_ASSERT(vClass->isAnnotationPresent(vType));
            TS_ASSERT(!vClass->isAnnotationPresent(vLoader->loadByType(typeid(Inherited))));

            Human::View vHuman = cast<Human::View>(vClass->getAnnotation(vType));

            TS_ASSERT(vHuman != NULL);
            }

        void testGetTypeInfo()
            {
            String::View    vsFoo   = "foo";
            String::View    vsBar   = "bar";
            String::Handle  hsBaz   = "baz";
            const Object&   fooRef  = *vsFoo;
            Integer32::View vInt    = Integer32::create(1);

            TS_ASSERT(Class::getTypeInfo(vsFoo) == typeid(fooRef));
            TS_ASSERT(Class::getTypeInfo(vsBar) == typeid(fooRef));
            TS_ASSERT(Class::getTypeInfo(hsBaz) == typeid(fooRef));
            TS_ASSERT(Class::getTypeInfo(vsFoo) != Class::getTypeInfo(vInt));

            // inheritance
            A::View vA1 = B::create();
            A::View vA2 = C::create();
            TS_ASSERT(Class::getTypeInfo(vA1) != Class::getTypeInfo(vA2));
            }

        void testTypeEquals()
            {
            String::View    vsFoo   = "foo";
            String::View    vsBar   = "bar";
            String::Handle  hsBaz   = "baz";
            Integer32::View vInt    = Integer32::create(1);

            TS_ASSERT( Class::typeEquals(vsFoo,  vsFoo));
            TS_ASSERT( Class::typeEquals(vsFoo,  vsBar));
            TS_ASSERT( Class::typeEquals(vsFoo,  hsBaz));
            TS_ASSERT(!Class::typeEquals(vsFoo,  vInt));

            // inheritance
            A::View vA1 = B::create();
            A::View vA2 = C::create();
            TS_ASSERT(!Class::typeEquals(vA1, vA2));
            }

        void testGetSimpleNames()
            {
            Boolean::View vBool  = Boolean::valueOf(true);
            Class::View   vClass = SystemClassLoader::getInstance()->loadByType(Class::getTypeInfo(vBool));

            TS_ASSERT(vClass->getSimpleName()->equals("Boolean"));
            TS_ASSERT(Class::getSimpleClassName(vBool)->equals("Boolean"));
            TS_ASSERT(Class::getSimpleTypeName(Class::getTypeInfo(vBool))->equals("Boolean"));
            }
    };
