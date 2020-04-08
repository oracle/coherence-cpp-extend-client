/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"
#include "coherence/util.ns"
#include "coherence/io/pof.ns"
#include "coherence/net/cache.ns"
#include <iostream>

#include "ContactInfo.hpp"

using namespace coherence::lang;
using namespace coherence::util;
using namespace coherence::io::pof;
using namespace coherence::net::cache;

template<class T> void test(bool fDynamic, const char* clazz, Object::Handle h)
    {
        size32_t cIters = 1000000;

        int64_t ldtStart = System::currentTimeMillis();
        int64_t lIgnore = 0;
        for (size32_t i = 0; i < cIters; ++i)
            {
            lIgnore += (fDynamic ? NULL != dynamic_cast<typename T::ValueType*>(get_pointer(h)) : instanceof<T>(h)) ? lIgnore -1 : 0;
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << Class::getClassName(h) << " to " << clazz << (fDynamic ? " dynamic_cast" : " instanceof") << " took avg of " << (cMillis*1000000/cIters) << " ns " << (lIgnore ? "positive" : "negative") << std::endl; 
    }

class Boo
    {
    public: Boo() {}
    virtual void foo() {}
    };

class Foo
    {
    public: Foo() {}
    virtual void foo() {}
    };

class Bar : public Foo
    {
    public: Bar() {}
    virtual void foo() {}
    };

class Car : public Bar
    {
    public: Car() {}
    virtual void foo() {}
    };

class ManagedCar : public class_spec<ManagedCar>, public Car
    {
    friend class factory<ManagedCar>;
    protected: ManagedCar() {}
    };

template<class T> void test2(const char* clazz, Foo* p)
    {
        size32_t cIters = 1000000;

        int64_t ldtStart = System::currentTimeMillis();
        int64_t lIgnore = 0;
        for (size32_t i = 0; i < cIters; ++i)
            {
            lIgnore += NULL != dynamic_cast<T*>(p) ? lIgnore -1 : 0;
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " raw dynamic took avg of " << (cMillis*1000000/cIters) << " ns " << (lIgnore ? "positive" : "negative") << std::endl; 
    }

void test3(const char* clazz, Foo* p)
    {
        size32_t cIters = 1000000;

        int64_t ldtStart = System::currentTimeMillis();
        for (size32_t i = 0; i < cIters; ++i)
            {
            p->foo();
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " virtual call took avg of " << (cMillis*1000000/cIters) << " ns"  << std::endl; 
    }

/*template<class T> bool operator!=(const TypedHandle<T>& th, Object* p)
    {
    return get_pointer(th) != p;
    }

template<class T> bool operator!=(Object* p, const TypedHandle<T>& th)
    {
    return get_pointer(th) != p;
    }*/

void test4(const char* clazz, Object::Handle th)
    {
        size32_t cIters = 1000000;
        coh_class_id id = COH_CLASS_ID(ManagedCar);
        int64_t ldtStart = System::currentTimeMillis();
        for (size32_t i = 0; i < cIters; ++i)
            {
            if (th != NULL)
		{
        (assignment<ManagedCar, Object>::allowed ||
         th->_cast(id));
		}
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " mod call took avg of " << (cMillis*1000000/cIters) << " ns"  << std::endl; 
}

void test5(const char* clazz, Object::Handle th)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        for (size32_t i = 0; i < cIters; ++i)
            {
            instanceof<ManagedCar::Handle>(th);
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " instanceof call took avg of " << (cMillis*1000000/cIters) << " ns"  << std::endl; 
    }

void test6(const char* clazz, Object::Handle th)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        for (size32_t i = 0; i < cIters; ++i)
            {
            th->_cast(COH_CLASS_ID(ManagedCar));
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " _cast call took avg of " << (cMillis*1000000/cIters) << " ns"  << std::endl; 
    }

template<class T1, class T2> void test7(const char* clazz, T1 th1, T2 th2)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        int64_t iIgnore = 0;
        for (size32_t i = 0; i < cIters; ++i)
            {
            if (th1 == th2) iIgnore++;
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " ref equality took avg of " << (cMillis*1000000/cIters) << " ns " << iIgnore  << std::endl; 
    }

template<class T1, class T2> void test8(const char* clazz, T1 th1)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        int64_t iIgnore = 0;
        for (size32_t i = 0; i < cIters; ++i)
            {
            T2 th2(th1);
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " ref assignment took avg of " << (cMillis*1000000/cIters) << " ns " << iIgnore  << std::endl; 
    }

template<class T> void test9a(T h)
    {
    }

template<class T1, class T2> void test9(const char* clazz, T1 th1)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        int64_t iIgnore = 0;
        for (size32_t i = 0; i < cIters; ++i)
            {
            test9a<T2>(th1);
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " ref call took avg of " << (cMillis*1000000/cIters) << " ns " << iIgnore  << std::endl; 
    }

template<class T1> void test10(const char* clazz)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        int64_t iIgnore = 0;
        for (size32_t i = 0; i < cIters; ++i)
            {
            T1 foo;
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " creation took avg of " << (cMillis*1000000/cIters) << " ns " << iIgnore  << std::endl; 
    }

template<class T1, class T2> void test11(const char* clazz, T2 th2)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        int64_t iIgnore = 0;
        for (size32_t i = 0; i < cIters; ++i)
            {
            T1 th1 = th2; 
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " assignment took avg of " << (cMillis*1000000/cIters) << " ns " << iIgnore  << std::endl; 
}

template<class T1, class T2> void test12(const char* clazz, T2 th2)
    {
        size32_t cIters = 1000000;
        int64_t ldtStart = System::currentTimeMillis();
        int64_t iIgnore = 0;
        T1 th1;
        for (size32_t i = 0; i < cIters; ++i)
            {
            th1 = th2; 
            }

        int64_t cMillis = System::currentTimeMillis() - ldtStart;
        std::cout << clazz << " re-assignment took avg of " << (cMillis*1000000/cIters) << " ns " << iIgnore  << std::endl; 
}

int main(int argc, char** argv)
    {
    try
        {
        test<Hashtable::Handle>(false, "Hashtable", Hashtable::create());
        test<Hashtable::Handle>(true, "Hashtable", Hashtable::create());
        test<AbstractMap::Handle>(false, "AbstractMap", Hashtable::create());
        test<AbstractMap::Handle>(true, "AbstractMap", Hashtable::create());
        test<Map::Handle>(false, "Map", Hashtable::create());
        test<Map::Handle>(true, "Map", Hashtable::create());
        test<PofIntrinsic::Handle>(false, "PofIntrinsic", Hashtable::create());
        test<PofIntrinsic::Handle>(true, "PofIntrinsic", Hashtable::create());
        test<PofIntrinsic::Handle>(false, "PofIntrinsic", LinkedList::create());
        test<PofIntrinsic::Handle>(true, "PofIntrinsic", LinkedList::create());
        test<Map::Handle>(false, "Map", LinkedList::create());
        test<Map::Handle>(true, "Map", LinkedList::create());

        test<KeyAssociation::Handle>(false, "KeyAssociation", Managed<ContactInfo>::create());
        test<KeyAssociation::Handle>(true, "KeyAssocation", Managed<ContactInfo>::create());
        

        test2<Foo>("Foo to Foo", new Foo());
        test2<Bar>("Foo to Bar", new Foo());
        test2<Bar>("Bar to Bar", new Bar());
        test2<Car>("Car to Car", new Car());
        test2<Boo>("Car to Boo", new Car());

        ManagedCar::Handle hCar = ManagedCar::create();
        test3("Foo", new Foo());
        test3("Bar", new Bar());
        test3("Car", new Car());
        test3("Car", get_pointer(hCar));
        
        test<ManagedCar::Handle>(false, "ManagedCar", ManagedCar::create());
        test4("ManagedCar", hCar);
        test5("ManagedCar", hCar);
        test6("ManagedCar", hCar);

        test7<Object::Handle, ManagedCar::Handle>("Object Car", hCar, hCar);
        test8<ManagedCar::Handle, Object::Handle>("Car Object", hCar);
        test8<ManagedCar::Handle, ManagedCar::Handle>("Car Car", hCar);
        test9<ManagedCar::Handle, Object::View>("Car by Object::View call", hCar);
        test9<ManagedCar::Handle, Object::Handle>("Car by Object::Handle call", hCar);
        test9<ManagedCar::Handle, ManagedCar::Handle>("Car by Car::Handle call", hCar);
        test9<ManagedCar::Handle, ManagedCar::View>("Car by Car::View call", hCar);
        test9<ManagedCar::Handle, const ManagedCar::Handle&>("Car by const Car::Handle& call", hCar);

        test10<Foo>("Foo");
        test10<TypedHandle<Object> >("TypedHandle<Object>");
        test10<ManagedCar::Handle>("Car::Handle");

        test11<Object::Handle, Object::Handle>("Object to Object", Object::create());
        test11<Object::Handle, ManagedCar::Handle>("Car to Object", ManagedCar::create());
        test11<ManagedCar::Handle, ManagedCar::Handle>("Car to Car", ManagedCar::create());
        test12<Object::Handle, Object::Handle>("Object to Object", Object::create());
        test12<Object::Handle, ManagedCar::Handle>("Car to Object", ManagedCar::create());
        test12<ManagedCar::Handle, ManagedCar::Handle>("Car to Car", ManagedCar::create());
        test12<ManagedCar::View, ManagedCar::View>("Car::View to Car::View", ManagedCar::create());
        }
    catch (Exception::View &e)
        {
        std::cerr << e << std::endl;
        }
    std::cout << "done" << std::endl;
    return 0;
    }
