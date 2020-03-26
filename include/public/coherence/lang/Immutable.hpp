/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IMMUTABLE_HPP
#define COH_IMMUTABLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"

#include <sstream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A Handle implementation which upon assignment ensures that it will
* reference an immutable Object. If the passed Object is immutable
* (as defined by Object::isImmutable()) then the handle simply holds a const
* pointer to the Object. If the passed Object is not immutable, then a clone
* attempt is made, and held via a const pointer.  If the supplied Object is
* not immutable and not cloneable then assignment will fail with a
* CloneNotSupport exception.
*
* @author mf  2007.07.05
*/
template<class T>
class Immutable
    : public TypedHandle<const T>
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a @c NULL handle.
        */
        Immutable()
            : TypedHandle<const T>()
            {
            }

        /**
        * Construct an Immutable handle to the given Object.
        *
        * @param po the pointer to the Object to reference
        */
        Immutable(const T* po)
            : TypedHandle<const T>(NULL)
            {
            set(po, NULL);
            }

        /**
        * Construct an Immutable handle from another Handle
        *
        * @param that another handle
        */
        template<class O> Immutable<T>(const TypedHandle<O>& that)
            : TypedHandle<const T>(that)
            {
            }

        /**
        * Construct an Immutable handle from another Handle
        *
        * @param that  another handle
        */
        Immutable<T>(const Immutable<T>& that)
            : TypedHandle<const T>(that)
            {
            }

    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Set this handle to point to immutable version of the given object,
        * or unset the handle.
        *
        * If the given pointer points to the immutable object, then this
        * handle will point it. If the given pointer points to the mutable
        * object, then this handle will point to the clone of that object.
        * If the given pointer is NULL, then this handle will point to no
        * object.
        *
        * @param cpo    pointer to the target object
        * @param pThat  pointer to the source ChainedHandleElement
        */
        Immutable<T>& set(const T* cpo, const ChainedHandleElement* pThat)
            {
            if (cpo == NULL || cpo->isImmutable())
                {
                if (pThat != NULL)
                    {
                    TypedHandle<const T>::set(cpo, *pThat);
                    }
                else
                    {
                    TypedHandle<const T>::operator=(cpo);
                    }
                }
            else
                {
                Object::View vClone;
                    {
                    // With the Sun compiler the temporary handle created
                    // here will live for the duration of the surrounding
                    // scope, we must pop the scope to ensure that the
                    // temporary handle is destroyed/detached, so the
                    // immutability check can pass
                    vClone = cpo->clone();
                    }
                if (NULL == vClone || !vClone->isImmutable())
                    {
                    COH_THROW_STREAM(IllegalArgumentException,
                        "Object of type '" << typeid(*cpo).name() <<
                        "' cannot be made immutable through cloning.");
                    }
                TypedHandle<const T>::operator=(cast<typename T::View>(vClone));
                }
            return *this;
            }
    };

/**
* Return an immutable version of the supplied Object. If the supplied view
* does not refer to an immutable Object, then a clone of the original Object
* is returned.
*
* @param v  view to the Object to return an immutable copy of
*
* @return a view to an immutable version of the supplied Object, or the
*         supplied Object if it was already immutable
*/
template<class T> typename T::View immutable_view(const typename T::View& v)
    {
    return Immutable<T>(v);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_IMMUTABLE_HPP
