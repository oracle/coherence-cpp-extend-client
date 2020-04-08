/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_EXECUTABLE_CLASS_HPP
#define COH_TYPED_EXECUTABLE_CLASS_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/ExecutableClass.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/TypedClass.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)


/**
* TypedExecutableClass provides a templated extension of ExecutableClass.
*
* @author mf  2008.10.02
*/
template<class T, class C> class TypedExecutableClass
    : public class_spec<TypedExecutableClass<T, C>,
        extends<C>,
        implements<ExecutableClass> >
    {
    friend class factory<TypedExecutableClass<T, C> >;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Class type.
        */
        typedef T Type;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TypedExecutableClass.
        *
        * @return a new TypedExecutableClass
        */
        TypedExecutableClass<T, C>()
            {
            }

    private:
        /**
        * Blocked copy constructor
        */
        TypedExecutableClass<T, C>(const TypedExecutableClass<T, C>&);


    // ----- ExecutableClass interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void execute(ObjectArray::View vasArg) const
            {
            T::main(vasArg);
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("TypedExecutableClass<" << Class::getName() << ">");
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TYPED_EXECUTABLE_CLASS_HPP
