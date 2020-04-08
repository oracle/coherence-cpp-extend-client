/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIZE64_HPP
#define COH_SIZE64_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 64-bit unsigned value.
*
* @author mf  2011.02.24
*/
class COH_EXPORT Size64
    : public cloneable_spec<Size64,
        extends<Primitive<size64_t> >,
        implements<Comparable> >
    {
    friend class factory<Size64>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Size64 instance.
        *
        * @param n  the value of the new Size64
        *
        * @return the new Size64
        */
        Size64(size64_t n);

        /**
        * Copy constructor.
        */
        Size64(const Size64& that);


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Factory method to produce Size64 objects with an optimization
        * that uses cached objects for all relatively-low values.
        *
        * @param n   an size64_t
        *
        * @return an Size64 whose value is the passed size64_t
        */
        static Handle valueOf(size64_t n);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * A constant for the minimum representable value.
        */
        static const size64_t min_value = 0x0;

        /**
        * A constant for the maximum representable value.
        */
        static const size64_t max_value = size64_t(0xFFFFFFFF) << 32 | size64_t(0xFFFFFFFF);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIZE64_HPP
