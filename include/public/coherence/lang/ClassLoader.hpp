/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_LOADER_HPP
#define COH_CLASS_LOADER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/interface_spec.hpp"
#include "coherence/lang/Class.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/String.hpp"

#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* ClassLoader provides a mechanism for obtaining a Class object.  Classes
* may be manually registered with a loader, or the loader may search out
* classes using custom search and load mechanisms.
*
* Not all classes are required to have a corresponding Class representation.
*
* The simplest way to enable a Class to be looked up by name, is to register
* it with the system ClassLoader, which can be obtained by calling
* ClassLoader::getSystemClassLoader().  For classes which include a no
* parameter static create method, a utility macro can be used to generate
* and register a Class automatically.  For example, given the class Foo:
*
* COH_REGISTER_TYPED_CLASS(Foo)
*
* Will register a Class for Foo. This registration would normally occur in
* Foo.cpp, outside of any method.
*
* @author mf  2008.04.02
*/
class COH_EXPORT ClassLoader
    : public interface_spec<ClassLoader>
    {
    // ----- ClassLoader interface ------------------------------------------

    public:
        /**
        * Loads the class for the specified name.
        *
        * @param vsName  the fully qualified name of the desired class
        *
        * @return the resulting {@link Class} object
        *
        * @throws ClassNotFoundException if the class was not found
        */
        virtual Class::View loadByName(String::View vsName) const = 0;

        /**
        * Loads the class for the specified RTTI typeid.
        *
        * @param info  the class's type_info
        *
        * @return the resulting {@link Class} object
        *
        * @throws ClassNotFoundException if the class was not found
        */
        virtual Class::View loadByType(const std::type_info& info) const = 0;

        /**
        * Returns the parent class loader for delegation.
        *
        * @return the parent class loader for delegation; NULL indicates the
        *         system ClassLoader has been reached
        */
        virtual ClassLoader::Handle getParent() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CLASS_LOADER_HPP
