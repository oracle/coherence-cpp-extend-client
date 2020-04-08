/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPARABLE_HPP
#define COH_COMPARABLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/interface_spec.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* This interface imposes a total ordering on the objects of each class that
* implements it. This ordering is referred to as the class's <i>natural
* ordering</i>, and the class's <tt>compareTo</tt> method is referred to as
* its <i>natural comparison method</i>.
*
* @author jh  2008.02.27
*/
class COH_EXPORT Comparable
    : public interface_spec<Comparable>
    {
    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * Compare this object with the specified object for order. Return a
        * negative integer, zero, or a positive integer if this object is
        * less than, equal to, or greater than the specified object.
        *
        * It is strongly recommended, but <i>not</i> strictly required, that
        * <tt>(x->compareTo(y) == 0) == (x->equals(y))</tt>.
        *
        * @param v  the Object to be compared
        *
        * @return a negative integer, zero, or a positive integer if this
        *         object is less than, equal to, or greater than the
        *         specified object
        *
        * @throws ClassCastException if the specified Object's type prevents it
        *         from being compared to this Object
        */
        virtual int32_t compareTo(Object::View v) const = 0;

        /**
        * Standard C++ less-than operator delegate.
        *
        * @param v  the Object to be compared
        *
        * @return true if this object is less than the specified object
        */
        virtual bool operator<(Object::View v) const
            {
            return compareTo(v) < 0;
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_COMPARABLE_HPP
