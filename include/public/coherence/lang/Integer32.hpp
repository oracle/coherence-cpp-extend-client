/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INTEGER32_HPP
#define COH_INTEGER32_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Number.hpp"
#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 32-bit integer value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Integer32
    : public cloneable_spec<Integer32,
        extends<Primitive<int32_t> >,
        implements<Number, Comparable> >
    {
    friend class factory<Integer32>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Integer32 instance.
        *
        * @param n the value of the new Integer32
        */
        Integer32(int32_t n);

        /**
        * Copy constructor.
        */
        Integer32(const Integer32& that);


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
        * Factory method to produce Integer32 objects with an optimization
        * that uses cached objects for all relatively-low numbers.
        *
        * @param n  an int32_t
        *
        * @return an Integer32 whose value is the passed int32_t
        */
        static Handle valueOf(int32_t n);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * A constant for the minimum representable value.
        */
        static const int32_t min_value = 0x80000000;

        /**
        * A constant for the maximum representable value.
        */
        static const int32_t max_value = 0x7FFFFFFF;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INTEGER32_HPP
