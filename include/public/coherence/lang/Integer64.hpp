/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INTEGER64_HPP
#define COH_INTEGER64_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Number.hpp"
#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 64-bit integer value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Integer64
    : public cloneable_spec<Integer64,
        extends<Primitive<int64_t> >,
        implements<Number, Comparable> >
    {
    friend class factory<Integer64>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Integer64 instance.
        *
        * @param l the value of the new Integer64
        */
        Integer64(int64_t l);

        /**
        * Copy constructor.
        */
        Integer64(const Integer64& that);


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


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Factory method to produce Integer64 objects with an optimization
        * that uses cached objects for all relatively-low numbers.
        *
        * @param l  an int64_t
        *
        * @return an Integer64 whose value is the passed int64_t
        */
        static Handle valueOf(int64_t l);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * A constant for the minimum representable value.
        */
        static const int64_t min_value = COH_INT64(0x80000000U, 0x0);

        /**
        * A constant for the maximum representable value.
        */
        static const int64_t max_value = COH_INT64(0x7FFFFFFFU, 0xFFFFFFFFU);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INTEGER64_HPP
