/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_ITERATOR_HPP
#define COH_SIMPLE_ITERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Provide a generic implementation of an array iterator.
*
* @author tb  2010.10.19
*/
class COH_EXPORT SimpleIterator
    : public class_spec<SimpleIterator,
        extends<Object>,
        implements<Iterator> >
    {
    friend class factory<SimpleIterator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct the simple iterator based on an array of objects.
        *
        * @param ohaItem  array of objects to iterate
        */
        SimpleIterator(ObjectArray::Holder ohaItem);

        /**
        * Construct the simple iterator based on an array of objects.
        *
        * @param ohaItem   array of objects to iterate
        * @param ofStart   the first object position
        * @param cItems    the number of objects to iterate
        * @param fForward  true to iterate forwards, false to iterate from the
        *                  end backwards to the beginning
        * @param fCopy     pass true to make a copy of the array or false if
        *                  the array's contents will not change
        */
        SimpleIterator(ObjectArray::Holder ohaItem, size32_t ofStart,
                size32_t cItems, bool fForward = true, bool fCopy = false);


    // ----- Iterator interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Array of items to iterate.
        */
        FinalHolder<ObjectArray> f_ohaItem;

        /**
        * Iterator position:  next item to return.
        */
        size32_t m_iItem;

        /**
        * Iterator end position (beyond last).
        */
        size32_t m_ofLimit;

        /**
        * Iterator direction.
        */
        bool m_fForward;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_ITERATOR_HPP
