/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYSTEM_CLASS_LOADER_HPP
#define COH_SYSTEM_CLASS_LOADER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/ClassLoader.hpp"
#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/System.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* SystemClassLoader is a ClassLoader implementation based on registration
* only.  That is it does not "load" any classes, it only knows about those
* which have been registered with it.
*
* @author mf  2008.04.03
*/
class COH_EXPORT SystemClassLoader
    : public class_spec<SystemClassLoader,
        extends<Object>,
        implements<ClassLoader> >
    {
    friend class factory<SystemClassLoader>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        using this_spec::create;

    private:
        /**
        * @internal
        */
        SystemClassLoader();


    // ----- SystemClassLoader interface ------------------------------------

    public:
        /**
        * Register a class with the SystemClassLoader.
        *
        * @param vClass the Class to register
        *
        * @return the registered class
        *
        * @throws IllegalArgumentException if a Class of the same name has
        *         already been registered
        */
        virtual Class::View registerClass(Class::View vClass);


    // ----- ClassLoader interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Class::View loadByName(String::View vsName) const;

        /**
        * {@inheritDoc}
        */
        virtual Class::View loadByType(const std::type_info& info) const;

        /**
        * @return NULL
        */
        virtual ClassLoader::Handle getParent() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        TypedHandle<const String> toString() const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Return the SystemClassLoader singleton.
        *
        * @return the SystemClassLoader
        */
        static SystemClassLoader::Handle getInstance();

        /**
        * Executable entrypoint for the SystemClassLoader class.
        *
        * Print the loaded classes.
        *
        * @param vasArg  a list of libraries to load, and print their
        *                registered classes.
        */
        static void main(ObjectArray::View vasArg);
    };

COH_CLOSE_NAMESPACE2


// ----- helper macros ------------------------------------------------------

#include "coherence/lang/TypedClass.hpp"
#include "coherence/lang/TypedExecutableClass.hpp"


/**
* Register a Class with the system class loader.
*
* @param CLASS the Class object to register
*/
#define COH_REGISTER_CLASS(CLASS) \
    COH_STATIC_INIT(coherence::lang::SystemClassLoader::getInstance()-> \
            registerClass(CLASS))

/**
 * Register a Class with the system class loader, and make it locally
 * referenceable within this file by the specified name.
 */
#define COH_REGISTER_NAMED_CLASS(NAME, CLASS) \
        static coherence::lang::FinalHandle<coherence::lang::Class> s_coh_static_class_##NAME \
            (coherence::lang::System::common(), CLASS); \
        COH_REGISTER_CLASS(s_coh_static_class_##NAME)

/**
* Register a TypedClass with the system class loader.
*
* @param TYPE the class type to register, i.e. coherence::lang::Object
*/
#define COH_REGISTER_TYPED_CLASS(TYPE) \
    COH_REGISTER_NAMED_CLASS(TYPE, coherence::lang::TypedClass<TYPE >::create())

/**
* Register a TypedExecutableClass with the system class loader.
*
* @param TYPE the class type to register, i.e. coherence::lang::Object
*/
#define COH_REGISTER_EXECUTABLE_CLASS(TYPE) \
    COH_REGISTER_NAMED_CLASS(TYPE, (coherence::lang::TypedExecutableClass<TYPE, \
            coherence::lang::TypedClass<TYPE > >::create()))

/**
 * Register a method on a locally registered class.
 *
 * This macro is only available in the context of the file which registered
 * the specified class.  The class must have been registered either by its
 * type or explicitly via COH_REGISTER_NAMED_CLASS.
 *
 * @param NAME    the type of the locally defined class
 * @param METHOD  the method to add
 */
#define COH_REGISTER_METHOD(NAME, METHOD) \
    COH_STATIC_INIT(s_coh_static_class_##NAME->declare(METHOD))

#endif // COH_SYSTEM_CLASS_LOADER_HPP
