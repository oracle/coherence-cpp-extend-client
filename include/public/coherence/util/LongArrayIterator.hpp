/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LONG_ARRAY_ITERATOR_HPP
#define COH_LONG_ARRAY_ITERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Muterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A LongArray specific iterator that adds a "current element" concept.
*
* @author js 2008.04.04
*/
class COH_EXPORT LongArrayIterator
    : public interface_spec<LongArrayIterator,
        implements<Muterator> >
    {
    // ----- LongArrayIterator implementation -------------------------------

    public:
        /**
        * Returns the index of the current value, which is the value returned
        * by the most recent call to the <tt>next</tt> method.
        *
        * @return the current index
        *
        * @throws IllegalStateException if the <tt>next</tt> method has
        *          not yet been called, or the <tt>remove</tt> method has
        *          already been called after the last call to the
        *          <tt>next</tt> method.
        */
        virtual int64_t getIndex() const = 0;

        /**
        * Returns the current value, which is the same value returned by the
        * most recent call to the <tt>next</tt> method, or the most recent
        * value passed to <tt>setValue</tt> if <tt>setValue</tt> were called
        * after the <tt>next</tt> method.
        *
        * @return  the current value
        *
        * @throws IllegalStateException if the <tt>next</tt> method has
        *         not yet been called, or the <tt>remove</tt> method has
        *         already been called after the last call to the
        *          <tt>next</tt> method.
        */
        virtual Object::Holder getValue() const = 0;

        /**
        * Stores a new value at the current value index, returning the value
        * that was replaced. The index of the current value is obtainable by
        * calling the <tt>getIndex</tt> method.
        *
        * @return  the replaced value
        *
        * @throws UnsupportedOperationException if the <tt>remove</tt>
        *          operation is not supported by this Iterator
        * @throws IllegalStateException if the <tt>next</tt> method has
        *          not yet been called, or the <tt>remove</tt> method has
        *          already been called after the last call to the
        *          <tt>next</tt> method.
        */
        virtual Object::Holder setValue(Object::Holder oValue) = 0;

        /**
        * Removes from the underlying collection the last element returned by
        * the iterator (optional operation).  This method can be called only
        * once per call to <tt>next</tt>.  The behavior of an iterator is
        * unspecified if the underlying collection is modified while the
        * iteration is in progress in any way other than by calling this
        * method.
        *
        * @throws UnsupportedOperationException if the <tt>remove</tt>
        *             operation is not supported by this Iterator
        * @throws IllegalStateException if the <tt>next</tt> method has
        *             not yet been called, or the <tt>remove</tt> method has
        *             already been called after the last call to the
        *             <tt>next</tt> method.
        */
        virtual void remove() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LONG_ARRAY_ITERATOR_HPP
