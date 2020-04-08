/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CHARACTER16_HPP
#define COH_CHARACTER16_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable 16-bit Unicode character value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Character16
    : public cloneable_spec<Character16,
        extends<Primitive<wchar16_t> >,
        implements<Comparable> >
    {
    friend class factory<Character16>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Create a new Character16 instance.
        *
        * @param ch  the value of the new Character16
        */
        Character16(wchar16_t ch);

        /**
        * Copy constructor.
        */
        Character16(const Character16&);


    // ----- Character16 interface ------------------------------------------

    public:
        /**
        * @return true if this is an ASCII (ISO-8859-1) Character16
        */
        virtual bool isASCII() const;


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Factory method to produce Character16 objects with an optimization
        * that uses cached objects for all relatively-low values.
        *
        * @param ch  a wchar16_t
        *
        * @return a Character16 whose value is the passed wchar16_t
        */
        static Handle valueOf(wchar16_t ch);



    // ----- constants ------------------------------------------------------

    public:
        /**
        * A constant for the minimum representable value.
        */
        static const wchar16_t min_value = 0x0;

        /**
        * A constant for the maximum representable value.
        */
        static const wchar16_t max_value = 0xFFFF;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CHARACTER16_HPP
