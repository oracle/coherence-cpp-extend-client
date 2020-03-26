/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIZE32_HPP
#define COH_SIZE32_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 32-bit unsigned value.
*
* @author mf  2011.02.24
*/
class COH_EXPORT Size32
    : public cloneable_spec<Size32,
        extends<Primitive<size32_t> >,
        implements<Comparable> >
    {
    friend class factory<Size32>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Size32 instance.
        *
        * @param n  the value of the new Size32
        *
        * @return the new Size32
        */
        Size32(size32_t n);

        /**
        * Copy constructor.
        */
        Size32(const Size32& that);


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Factory method to produce Size32 objects with an optimization
        * that uses cached objects for all relatively-low values.
        *
        * @param n   an size32_t
        *
        * @return an Size32 whose value is the passed size32_t
        */
        static Handle valueOf(size32_t n);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * A constant for the minimum representable value.
        */
        static const size32_t min_value = 0x00000000;

        /**
        * A constant for the maximum representable value.
        */
        static const size32_t max_value = 0xFFFFFFFF;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIZE32_HPP
