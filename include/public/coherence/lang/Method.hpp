/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_METHOD_HPP
#define COH_METHOD_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/abstract_spec.hpp"
#include "coherence/lang/AnnotatedElement.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/FinalView.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)

// ---- forward declarations ------------------------------------------------

class Class;


/**
 * Method represents a method within a managed class.
 *
 * To be of general use these Methods must also be registered with the
 * corresponding Class object during Class registration. Once registered the
 * Method can be found at runtime as follows:
 *
 * @code
 * Method::View vMethodName = SystemClassLoader::getInstance()
 *     ->loadByName("Person")->getMethod("getName");
 * @endcode
 *
 * And then finally applied:
 *
 * @code
 * Object::View vo = getPersonFromSomewhere();
 * std::cout << vMethodName->invoke(vo) << std::endl;
 * @endcode
 *
 * @author mf  2011.02.24
 *
 * @since Coherence 3.7.1
 *
 * @see TypedMethod for registration helpers and examples
 */
class COH_EXPORT Method
    : public abstract_spec<Method,
          extends<AnnotatedElement> >
    {
    public:
        /**
         * Forward declaration of Class::View.
         */
        typedef TypedHandle<const Class> ClassView;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Construct a Method of the specified name.
         */
        Method(String::View vsName);


    // ----- Method interface -----------------------------------------------

    public:
        /**
         * Return the method name.
         */
        virtual String::View getName() const;

        /**
         * Return the method modifiers.
         *
         * @return the method modifiers
         */
        virtual int32_t getModifiers() const = 0;

        /**
         * Return an array of Class::Views representing the method parameter
         * types.
         *
         * @return the parameter types
         */
        virtual ObjectArray::View getParameterTypes() const = 0;

        /**
         * Return the Class::View representing the method's return type, or
         * NULL for void.
         *
         * @return the return type
         */
        virtual ClassView getReturnType() const = 0;

        /**
         * Return the Class::View representing the class in which the method
         * is declared.
         *
         * @return the declaring class
         */
        virtual ClassView getDeclaringClass() const = 0;

        /**
         * Execute the method.
         *
         * @param oh      the object on which to invoke the method
         * @param vaArgs  the parameters, or NULL if the method takes none.
         *
         * @return the method return value, or NULL if the method has a void return
         */
        virtual Object::Holder invoke(Object::Holder oh, ObjectArray::View vaArgs = NULL) const = 0;

        /**
         * Add the specified Annotation to the element.
         *
         * @param vAnnontation  the annotation
         *
         * @return this object
         */
        virtual Method::Handle annotate(Annotation::View vAnnontation);


    // ----- Object interface -----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual TypedHandle<const String> toString() const;

        /**
         * {@inheritDoc}
         */
        virtual bool equals(Object::View vThat) const;

        /**
         * {@inheritDoc}
         */
        virtual size32_t hashCode() const;


    // ----- enum: Modifier -------------------------------------------------

    public:
        /**
         * Modifier defines the various reflectable Method modifiers.
         */
        typedef enum
            {
            /**
             * modifier_instance indicates that the method is an instance method.
             */
            modifier_instance = 1,

            /**
             * modifier_static indicates that the method is static.
             */
            modifier_static = 2,

            /**
             * modifier_mutable indicates that the method is non-const.
             */
            modifier_mutable = 4,

            /**
             * modifier_const indicates that the method is const.
             */
            modifier_const = 8
            } Modifier;


    // ----- data members ---------------------------------------------------

    protected:
        /**
         * The Method name.
         */
        FinalView<String> f_vsName;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_METHOD_HPP
