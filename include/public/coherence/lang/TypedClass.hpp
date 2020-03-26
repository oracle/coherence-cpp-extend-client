/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_CLASS_HPP
#define COH_TYPED_CLASS_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/AbstractTypedClass.hpp"
#include "coherence/lang/Class.hpp"
#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/String.hpp"


#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* TypedClass extends AbstractTypedClass supporting classes which
* include a no-argument create method.
*
* @author mf  2008.04.03
*/
template<class T> class TypedClass
    : public class_spec<TypedClass<T>,
        extends<AbstractTypedClass<T> > >
    {
    friend class factory<TypedClass<T> >;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Class type.
        */
        typedef T Type;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TypedClass.
        */
        TypedClass<T>()
            {
            }

    private:
        /**
        * Blocked copy constructor
        */
        TypedClass<T>(const TypedClass<T>&);


    // ----- TypedClass interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Handle newInstance(ObjectArray::View vaParam) const
            {
            if (vaParam == NULL || vaParam->length == 0)
                {
                return T::create();
                }
            return AbstractTypedClass<T>::newInstance(vaParam);
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("TypedClass<" << Class::getName() << ">");
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TYPED_CLASS_HPP
