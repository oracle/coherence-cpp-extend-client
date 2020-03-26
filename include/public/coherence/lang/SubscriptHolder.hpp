/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUBSCRIPT_HOLDER_HPP
#define COH_SUBSCRIPT_HOLDER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* A TypedHolder implementation which supports automatic delegation of the
* subscript operator.
*
* @author mf  2009.03.10
*/
template<class T, class ET, class IT = size32_t>
class SubscriptHolder
    : public TypedHolder<T>
    {
    // ----- typedefs -------------------------------------------------------

    typedef ET ElementType;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create an empty SubscriptHolder.
        */
        SubscriptHolder()
            : TypedHolder<T>()
            {
            }

        /**
        * Create a new SubscriptHolder from the TypedHandle with a type
        * conversion.
        */
        template<class DT> SubscriptHolder<T, ET, IT>(const TypedHandle<DT>& that)
            : TypedHolder<T>(that)
            {
            }

        /**
        * Create a new SubscriptHolder from the TypedHolder with a type
        * conversion.
        */
        template<class DT> SubscriptHolder<T, ET, IT>(const TypedHolder<DT>& that)
            : TypedHolder<T>(that)
            {
            }

        /**
        * Create a new SubscriptHolder from the raw pointer.
        */
        SubscriptHolder(T* o)
            : TypedHolder<T>(o)
            {
            }

        /**
        * The copy constructor.
        */
        SubscriptHolder<T, ET, IT>(const SubscriptHolder<T, ET, IT>& that)
            : TypedHolder<T>(that)
            {
            }

        /**
        * The subscript operator.
        */
        ElementType& operator[](IT i) const
            {
            const T* cpo = TypedHolder<T>::m_cpo;
            if (NULL == cpo)
                {
                coh_throw_npe(typeid(T));
                }
            return (*cpo)[i];
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SUBSCRIPT_HOLDER_HPP
