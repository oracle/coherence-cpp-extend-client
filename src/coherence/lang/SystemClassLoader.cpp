/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/SystemClassLoader.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/ClassNotFoundException.hpp"
#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/SafeHashMap.hpp"

#include "private/coherence/lang/ClassInfo.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <ostream>
#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::logging::Logger;
using coherence::util::HashMap;
using coherence::util::Map;
using coherence::util::SafeHashMap;

extern Class::View coh_exchangeCustomHeapAnalyzerClass(String::View vsName,
        Class::View vClass);

COH_EXPORT const Class* coh_loadClassByType(const std::type_info& ti)
    {
    return get_pointer(SystemClassLoader::getInstance()->loadByType(ti));
    }

COH_REGISTER_CLASS((TypedExecutableClass<SystemClassLoader,
        TypedBarrenClass<SystemClassLoader> >::create()));

// ----- file local helpers -------------------------------------------------

COH_OPEN_NAMESPACE_ANON(SystemClassLoader)

/**
* Return the singleton class map, keys are demangled class names, values
* are Class::Views.
*
* @return the singleton class map
*/
Map::Handle getClassMap()
    {
    static FinalHandle<Map> hMap(System::common(),
            SafeHashMap::create());
    return hMap;
    }
COH_STATIC_INIT(getClassMap());

/**
* TypedPrimitiveClass can be used to declare Classes for primitive types.
*/
template<class T> class TypedPrimitiveClass
    : public class_spec<TypedPrimitiveClass<T>,
        extends<Class> >
    {
    friend class factory<TypedPrimitiveClass<T> >;

    // ----- typedefs -----------------------------------------------

    public:
        /**
        * Class type.
        */
        typedef T Type;

    // ----- constructors -------------------------------------------

    protected:
        /**
        * Initialize an AbstractTypedClass.
        */
        TypedPrimitiveClass<T>()
            : class_spec<TypedPrimitiveClass<T>, extends<Class> >(typeid(T))
            {
            }

    private:
        /**
        * Blocked copy constructor
        */
        TypedPrimitiveClass<T>(const TypedPrimitiveClass<T>&);


    // ----- TypedPrimitiveClass interface -----------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual const std::type_info& getTypeInfo() const
            {
            return typeid(T);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t getSize() const
            {
            return (size32_t) sizeof(T);
            }

        /**
        * {@inheritDoc}
        *
        * @throws UnsupportedOperationException
        */
        virtual Object::Handle newInstance(ObjectArray::View /*vaParam*/) const
            {
            if (true)
                {
                COH_THROW (UnsupportedOperationException::create(Class::getName()));
                }
            return NULL;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isInstance(Object::View /*v*/) const
            {
            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isAssignableFrom(Class::View /*vClass*/) const
            {
            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual Class::View getSuperclass() const
            {
            return NULL;
            }

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::View getInterfaces() const
            {
            return ObjectArray::create(0);
            }

    // ----- Object interface -----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("TypedPrimitiveClass<" << Class::getName() << ">");
            }
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

SystemClassLoader::SystemClassLoader()
    {
    }


// ----- SystemClassLoader interface ----------------------------------------

Class::View SystemClassLoader::registerClass(Class::View vClass)
    {
    COH_ENSURE_PARAM(vClass);

    String::View vsName = vClass->getName();
    if (NULL == vsName)
        {
        COH_THROW (IllegalArgumentException::create("Class name cannot be NULL"));
        }

    Map::Handle hMap = getClassMap();
    COH_SYNCHRONIZED (hMap)
        {
        if (hMap->containsKey(vsName))
            {
            COH_LOG ("Class \"" << vsName <<
                    "\" already defined within SystemClassLoader", 1);
            COH_THROW_STREAM (IllegalArgumentException, "Class \"" << vsName <<
                    "\" already defined within SystemClassLoader");
            }

        // cache the shallow size for quick access
        ClassInfo::findByName(vsName)->setSize(vClass->getSize());
        hMap->put(vsName, vClass);
        }
    COH_LOG("Registered \"" << vClass << "\" with SystemClassLoader", 7);

    // inform HeapAnalyzer bootstraping of new class registration
    coh_exchangeCustomHeapAnalyzerClass(vsName, vClass);
    return vClass;
    }


// ----- ClassLoader interface ----------------------------------------------

Class::View SystemClassLoader::loadByName(String::View vsName) const
    {
    Class::View vClass = cast<Class::View>(getClassMap()->get(vsName));
    if (NULL == vClass)
        {
        // it is possible this failed because we've yet to run static
        // initializers, load and retry
        System::loadLibrary(NULL);

        vClass = cast<Class::View>(getClassMap()->get(vsName));
        if (NULL == vClass)
            {
            COH_THROW (ClassNotFoundException::create(vsName));
            }
        }
    return vClass;
    }

Class::View SystemClassLoader::loadByType(const std::type_info& info) const
    {
    return loadByName(Class::getTypeName(info));
    }

ClassLoader::Handle SystemClassLoader::getParent() const
    {
    return NULL;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> SystemClassLoader::toString() const
    {
    return COH_TO_STRING("SystemClassLoader: registered classes " << getClassMap());
    }


// ----- static methods -----------------------------------------------------

SystemClassLoader::Handle SystemClassLoader::getInstance()
    {
    static FinalHandle<SystemClassLoader> hLoader(System::common(),
            create());
    return hLoader;
    }
COH_STATIC_INIT(SystemClassLoader::getInstance());

void SystemClassLoader::main(ObjectArray::View vasArg)
    {
    if (vasArg->length == 0)
        {
        std::cout << getInstance() << std::endl;
        }
    else
        {
        Map::View   vMapLoaded = getClassMap();
        Map::Handle hMapStart  = HashMap::create();
        hMapStart->putAll(vMapLoaded);

        for (size32_t i = 0, c = vasArg->length; i < c; ++i)
            {
            System::loadLibrary(cast<String::View>(vasArg[i]));

            Map::Handle hMapEnd = HashMap::create();
            hMapEnd->putAll(vMapLoaded);
            hMapEnd->entrySet()->removeAll(hMapStart->entrySet());

            if (i > 0)
                {
                std::cout << std::endl;
                }

            std::cout << "SystemClassLoader: " << vasArg[i]
                << " registered classes\n"
                << hMapEnd << std::endl;
            }
        }
    }

// ----- primitive type registration ----------------------------------------

COH_REGISTER_CLASS(TypedPrimitiveClass<bool>::create());
COH_REGISTER_CLASS(TypedPrimitiveClass<octet_t>::create());
COH_REGISTER_CLASS(TypedPrimitiveClass<wchar16_t>::create());
COH_REGISTER_CLASS(TypedPrimitiveClass<int16_t>::create());
COH_REGISTER_CLASS(TypedPrimitiveClass<int32_t>::create());
COH_REGISTER_CLASS(TypedPrimitiveClass<int64_t>::create());
COH_REGISTER_CLASS(TypedPrimitiveClass<float32_t>::create());
COH_REGISTER_CLASS(TypedPrimitiveClass<float64_t>::create());

COH_CLOSE_NAMESPACE2
