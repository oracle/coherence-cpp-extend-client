/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BOX_HANDLE_HPP
#define COH_BOX_HANDLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A TypedHandle implementation which supports auto-boxing.
*
* To support boxing a managed class defines an inner-type named "BoxedType",
* and includes a public static "create" method which takes that type. The
* BoxedType defines the type which the BoxHandle can use to create a managed
* object from. Alternatively the BoxHandle declaration can identify the
* "BoxedType" via a secondary template parameter.
*
* Managed classes which wish to be auto-boxing may use this handle type
* when defining their Handle and View definitions. See String as an
* example.
*
* When used with integral types a natural ambiguity is introduced when
* setting or comparing a BoxHandle to NULL, as it also an integral type. In
* such cases one may use the is_null(h) and clear_handle(h) helper functions
* to ensure the correct behavior.
*
* BoxHandle can be configured as strict, or non-strict. In strict mode
* (the default), derefencing a NULL BoxHandle will result in a
* NullPointerException. In non-strict mode, dereferencing a BoxHandle will
* result in the BoxedType's default value being returned.
*
* @author mf  2007.07.05
*/
template<class T, class B = typename T::BoxedType, bool fStrict = true>
class BoxHandle
    : public TypedHandle<T>
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /*
        * The boxed type.
        */
        typedef B BoxedType;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create an empty BoxHandle.
        */
        BoxHandle()
            : TypedHandle<T>()
            {
            }

        /**
        * Create a new BoxHandle from the supplied BoxedType
        */
        BoxHandle(const B& b)
            : TypedHandle<T>(T::create(b))
            {
            }

        /**
        * Create a new BoxHandle from the TypedHandle with a type conversion.
        */
        template<class O> BoxHandle(const TypedHandle<O>& that)
            : TypedHandle<T>(that)
            {
            }

        /**
        * Create a new BoxHandle from the raw pointer.
        */
        BoxHandle(T* o)
            : TypedHandle<T>(o)
            {
            }

        /**
        * The copy constructor.
        */
        BoxHandle(const BoxHandle& h)
            : TypedHandle<T>(h)
            {
            }

    // ----- operators ------------------------------------------------------

    public:
        /**
        * The "boxing" operator.
        */
        BoxHandle& operator=(const BoxedType& b)
            {
            TypedHandle<T>::operator=(T::create(b));
            return *this;
            }

        /**
        * The "unboxing" operator.
        *
        * @return a copy of the referenced Object
        */
        operator BoxedType() const
            {
            const T* pT = TypedHandle<T>::get();
            if (NULL == pT)
                {
                if (fStrict)
                    {
                    coh_throw_npe(typeid(T));
                    }
                return BoxedType();
                }
            return (BoxedType) *pT;
            }
    };

template<class T, class B, bool S>
bool operator==(const BoxHandle<T, B, S>& h1, const BoxHandle<T, B, S>& h2)
    {
    return operator==(Object::Holder(h1), Object::Holder(h2));
    }

template<class T, class B, bool S>
bool operator==(const BoxHandle<T, B, S>& h, const B& b)
    {
    const T* pT = get_pointer(h);
    if (S && pT == NULL)
        {
        coh_throw_npe(typeid(T));
        }
    return NULL == pT ? B() == b : ((B) *pT) == b;
    }

template<class T, class B, bool S>
bool operator==(const B& b, const BoxHandle<T, B, S>& h)
    {
    return operator==(h, b);
    }

template<class T, class B, bool S>
bool operator!=(const BoxHandle<T, B, S>& h1, const BoxHandle<T, B, S>& h2)
    {
    return !operator==(h1, h2);
    }

template<class T, class B, bool S>
bool operator!=(const BoxHandle<T, B, S>& h, const B& b)
    {
    return !operator==(h, b);
    }

template<class T, class B, bool S>
bool operator!=(const B& b, const BoxHandle<T, B, S>& h)
    {
    return !operator==(h, b);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_BOX_HANDLE_HPP
