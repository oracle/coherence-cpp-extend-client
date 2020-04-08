/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUBSCRIPT_HANDLE_HPP
#define COH_SUBSCRIPT_HANDLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A TypedHandle implementation which supports automatic delegation of the
* subscript operator.
*
* @author mf  2008.05.20
*/
template<class T, class ET, class IT = size32_t>
class SubscriptHandle
    : public TypedHandle<T>
    {
    // ----- typedefs -------------------------------------------------------

    typedef ET ElementType;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create an empty SubscriptHandle.
        */
        SubscriptHandle()
            : TypedHandle<T>()
            {
            }

        /**
        * Create a new SubscriptHandle from the TypedHandle with a type
        * conversion.
        */
        template<class DT> SubscriptHandle<T, ET, IT>(const TypedHandle<DT>& that)
            : TypedHandle<T>(that)
            {
            }

        /**
        * Create a new SubscriptHandle from the TypedHolder with a type
        * conversion.
        */
        template<class DT> SubscriptHandle<T, ET, IT>(const TypedHolder<DT>& that)
            : TypedHandle<T>(that)
            {
            }

        /**
        * Create a new SubscriptHandle from the raw pointer.
        */
        SubscriptHandle(T* o)
            : TypedHandle<T>(o)
            {
            }

        /**
        * The copy constructor.
        */
        SubscriptHandle<T, ET, IT>(const SubscriptHandle<T, ET, IT>& that)
            : TypedHandle<T>(that)
            {
            }

        /**
        * The subscript operator.
        */
        ElementType& operator[](IT i) const
            {
            T* po = TypedHandle<T>::m_o;
            if (NULL == po)
                {
                coh_throw_npe(typeid(T));
                }
            return (*po)[i];
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SUBSCRIPT_HANDLE_HPP
