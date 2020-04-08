/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_BARREN_CLASS_HPP
#define COH_TYPED_BARREN_CLASS_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/AbstractTypedClass.hpp"
#include "coherence/lang/Class.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/UnsupportedOperationException.hpp"


#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* TypedBarrenClass provides a templated extension of Class but does not
* provide a means to instantiate new instances of the class.
*
* This is suitable for use with interfaces, abstract classes, and classes
* which do not have a zero-argument create method.
*
* @author mf/jh  2008.05.02
*/
template<class T> class TypedBarrenClass
    : public class_spec<TypedBarrenClass<T>,
        extends<AbstractTypedClass<T> > >
    {
    friend class factory<TypedBarrenClass<T> >;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Class type.
        */
        typedef T Type;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Create a new TypedBarrenClass.
         */
        TypedBarrenClass<T>()
            {
            }

    private:
        /**
        * Blocked copy constructor
        */
        TypedBarrenClass<T>(const TypedBarrenClass<T>&);


    // ----- TypedBarrenClass interface -------------------------------------

    public:
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


    // ----- Object interface -----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("TypedBarrenClass<" << Class::getName() << ">");
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TYPED_BARREN_CLASS_HPP
