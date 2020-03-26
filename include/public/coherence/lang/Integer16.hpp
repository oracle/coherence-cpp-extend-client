/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INTEGER16_HPP
#define COH_INTEGER16_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Number.hpp"
#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 16-bit integer value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Integer16
    : public cloneable_spec<Integer16,
        extends<Primitive<int16_t> >,
        implements<Number, Comparable> >
    {
    friend class factory<Integer16>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Integer16 instance.
        *
        * @param n the value of the new Integer16
        */
        Integer16(int16_t n);

        /**
        * Copy constructor.
        */
        Integer16(const Integer16& that);


    // ----- Number interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int16_t getInt16Value() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getInt32Value() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getInt64Value() const;

        /**
        * {@inheritDoc}
        */
        virtual float32_t getFloat32Value() const;

        /**
        * {@inheritDoc}
        */
        virtual float64_t getFloat64Value() const;


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Factory method to produce Integer16 objects with an optimization
        * that uses cached objects for all relatively-low numbers.
        *
        * @param n  an int16_t
        *
        * @return an Integer16 whose value is the passed int16_t
        */
        static Handle valueOf(int16_t n);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * A constant for the minimum representable value.
        */
        static const int16_t min_value = (int16_t) 0x8000;

        /**
        * A constant for the maximum representable value.
        */
        static const int16_t max_value = 0x7FFF;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INTEGER16_HPP
