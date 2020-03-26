/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OCTET_HPP
#define COH_OCTET_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 8-bit unsigned value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Octet
    : public cloneable_spec<Octet,
        extends<Primitive<octet_t> >,
        implements<Comparable> >
    {
    friend class factory<Octet>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Octet instance.
        *
        * @param b  the value of the new Octet
        *
        * @return the new Octet
        */
        Octet(octet_t b);

        /**
        * Copy constructor.
        */
        Octet(const Octet& that);


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Factory method to produce Octet objects with an optimization
        * that uses cached objects for all relatively-low values.
        *
        * @param b   an octet_t
        *
        * @return an Octet whose value is the passed octet_t
        */
        static Handle valueOf(octet_t b);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * A constant for the minimum representable value.
        */
        static const octet_t min_value = 0x0;

        /**
        * A constant for the maximum representable value.
        */
        static const octet_t max_value = 0xFF;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OCTET_HPP
