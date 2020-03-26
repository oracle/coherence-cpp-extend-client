/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_HPP
#define COH_CLASS_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/abstract_spec.hpp"
#include "coherence/lang/AnnotatedElement.hpp"
#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/Method.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/String.hpp"


#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A Class represents a managed object implementation.
*
* Not all managed classes may have a corresponding Class representation.
* Classes are referred to via their demangled RTTI name, for example
* coherence::lang::Object.
*
* Classes can be loaded by name via a ClassLoader.
*
* @see ClassLoader
*
* @author mf  2008.04.03
*/
class COH_EXPORT Class
    : public abstract_spec<Class,
          extends<AnnotatedElement> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a class based on a C++ type_info.
        *
        * @param info the type_info for the class represented by this Class.
        */
        Class(const std::type_info& info);

    private:
        /**
        * Blocked copy constructor
        */
        Class(const Class&);


    // ----- Class interface ------------------------------------------------

    public:
        /**
        * Return the name of the class.
        *
        * @return the class name
        */
        virtual String::View getName() const;

        /**
        * Returns the simple name of the underlying class as given in the
        * source code.
        *
        * If a simple name cannot be determined, the full name from
        * getName() will be returned.
        *
        * @return the simple name of the underlying class
        *
        * @since 12.2.1.3
        */
        virtual String::View getSimpleName() const;

        /**
        * Create a new instance of the corresponding type.
        *
        * @param vaParam the object's initialization parameters
        *
        * @return a new instance of the corresponding type
        */
        virtual Object::Handle newInstance(ObjectArray::View vaParam = NULL) const;

        /**
        * Return the typeinfo for the corresponding type.
        *
        * @return the typeinfo for the corresponding type
        */
        virtual const std::type_info& getTypeInfo() const = 0;

        /**
        * Return the shallow size for an instance of the class represented
        * by this Class.
        *
        * @return the shallow size of an instance of the represented class.
        */
        virtual size32_t getSize() const = 0;

        /**
        * Return whether the passed in Object is an instance of class represented
        * by this Class.
        *
        * @param v  the object to test
        *
        * @return whether the passed in Object is an instance of class represented
        * by this Class.
        *
        * @since Coherence 3.7
        */
        virtual bool isInstance(Object::View v) const = 0;

        /**
         * Return true if the class represented by this object is the same
         * Class or a super class or super interface of the specified class.
         *
         * @param vClass  the class to test
         *
         * @return true if the specified class "extends" the class represented
         *         by this object.
         */
        virtual bool isAssignableFrom(Class::View vClass) const = 0;

        /**
         * Register a method with this class.
         *
         * @param vMethod  the method to register
         *
         * @since Coherence 3.7.1
         */
        virtual Class::Handle declare(Method::View vMethod);

        /**
         * Return the declared method of the specified name and parameters.
         *
         * The method is supplied with an array of either parameters or
         * parameter types expressed as Class::View objects.  If non-Class
         * objects are provided then this method will search for a Method
         * which can accept those parameters directly.  If Class objects are
         * provided then this method will search for a Method which takes
         * the specified type, or a superclass.
         *
         * @param vsName    the method name
         * @param vaParams  the method parameters or types
         * @param nMod      the modifiers to match
         * @param fThrow    false to return NULL rather then throw on failure
         *
         * @return the Method
         *
         * @throw NoSuchMethodException if the method is not found and fThrow == true
         *
         * @since Coherence 3.7.1
         */
        virtual Method::View getDeclaredMethod(String::View vsName,
                ObjectArray::View vaParams = NULL, int32_t nMod = 0,
                bool fThrow = true) const;

        /**
         * Return an array containing all the Methods declared on this Class.
         *
         * @return the declared methods
         *
         * @since Coherence 3.7.1
         */
        virtual ObjectArray::View getDeclaredMethods() const;

        /**
         * Return the method of the specified name and parameters which is
         * either declared or inherited by this class.
         *
         * The method is supplied with an array of either parameters or
         * parameter types expressed as Class::View objects.  If non-Class
         * objects are provided then this method will search for a Method
         * which can accept those parameters directly.  If Class objects are
         * provided then this method will search for a Method which takes
         * the specified type, or a superclass.
         *
         * @param vsName    the method name
         * @param vaParams  the method parameters or types
         * @param nMod      the modifiers to match
         * @param fThrow    false to return NULL rather then throw on failure
         *
         * @return the Method
         *
         * @throw NoSuchMethodException if the method is not found and fThrow == true
         *
         * @since Coherence 3.7.1
         */
        virtual Method::View getMethod(String::View vsName,
                ObjectArray::View vaParams = NULL, int32_t nMod = 0,
                bool fThrow = true) const;

        /**
         * Return an array containing all the Methods declared on and inherited
         * by this Class.
         *
         * @return the declared methods
         *
         * @since Coherence 3.7.1
         */
        virtual ObjectArray::View getMethods() const;

        /**
         * Return the superclass of this class, or NULL if this class an
         * interface or top level class.
         *
         * @return the superclass
         *
         * @throws ClassNotFoundException if the superclass is not registered
         *
         * @since Coherence 3.7.1
         */
        virtual Class::View getSuperclass() const = 0;

        /**
         * Return an array of Class::View objects representing the interfaces
         * which the Class directly implements.
         *
         * Any interfaces not registered with the ClassLoader will be left
         * out of the array.
         *
         * @return the array of Class::Views representing the interfaces
         *
         * @since Coherence 3.7.1
         */
        virtual ObjectArray::View getInterfaces() const = 0;

        /**
         * Add the specified Annotation to the element.
         *
         * @param vAnnontation  the annotation
         *
         * @return this object
         */
        virtual Class::Handle annotate(Annotation::View vAnnontation);


    // ----- AnnotatedElement interface -------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual AnnotatedElement::View getSuperelement() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View that) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;


    // ----- static helpers -------------------------------------------------

    public:
        /**
        * Return the class name of the supplied Object.
        *
        * @param v  the object instance for which to obtain the class name
        *
        * @return the class name of the supplied Object
        */
        static String::View getClassName(Object::View v);

        /**
        * Returns the simple name of the underlying class of the
        * supplied Object as given in the code.
        *
        * If a simple name cannot be determined, the full name from
        * getClassName() will be returned.
        *
        * @param v  the object instance for which to obtain the name
        *
        * @return the simple name of the underlying class
        *
        * @since 12.2.1.3
        */
        static String::View getSimpleClassName(Object::View v);

        /**
         * Returns the std::type_info for the supplied Object.
         *
         * @param v  the object instance for which to obtain the type info
         *
         * @return the std::type_info for the supplied Object
         *
         * @since 12.2.1
         */
        static const std::type_info& getTypeInfo(Object::View v);

        /**
        * Return the type name of the supplied type id.
        *
        * @param info  the type id for which to obtain the type name
        *
        * @return the type name of the supplied type
        */
        static String::View getTypeName(const std::type_info& info);

        /**
        * Returns the simple name of the underlying class of the
        * supplied type as given in the code.
        *
        * If a simple name cannot be determined, the full name from
        * getTypeInfo() will be returned.
        *
        * @param info  the type id for which to obtain the name
        *
        * @return the simple name of the underlying class
        *
        * @since 12.2.1.3
        */
        static String::View getSimpleTypeName(const std::type_info& info);

        /**
        * Compare two Objects for type equality.
        *
        * @param v1  the first Object
        * @param v2  the second Object
        *
        * @return  <tt>true</tt> iff v1 and v2 are the same type
        *
        * @since 12.2.1
        */
        static bool typeEquals(Object::View v1, Object::View v2);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The class name.
        */
        FinalView<String> f_vsName;

        /**
        * Reference to a Map of method names to a List of Method objects.
        */
        MemberHandle<Object> m_hMapMethods;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CLASS_HPP
