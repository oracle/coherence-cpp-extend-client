/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRIMITIVE_HPP
#define COH_PRIMITIVE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/abstract_spec.hpp"
#include "coherence/lang/Comparable.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/String.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Template class which converts primitive data types into immutable managed
* objects.
*
* @author jh/mf  2007.07.05
*/
template<class T>
class COH_EXPORT Primitive
    : public abstract_spec<Primitive<T>,
        extends<Object>,
        implements<Comparable> >
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The boxed primitive type.
        */
        typedef T BoxedType;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        Primitive(const T& value = 0);

        /**
        * Copy constructor.
        */
        Primitive(const Primitive<T>& that);


    // ----- Primitive interface --------------------------------------------

    public:
        /**
        * Return the primitive value.
        *
        * @return the primitive value
        */
        virtual T getValue() const;

        /**
        * Return the primitive value
        *
        * @return The primitive value
        */
        operator T() const;

        /**
        * Return the primitive representation of a String.
        *
        * @param vs          the String to covert to a primitive type
        * @param nPrecision  the (optional) precision, if applicable, of 
        *                    the primitive type
        *
        * @return the primitive representation of the string
        *
        * @throws IllegalArgumentException if the supplied String is not
        *         convertible to the primitive type. NULL is considered an
        *         illegal argument except for the bool type, where it
        *         results in a bool(false).
        */
        static T parse(String::View vs, size_t nPrecision = 0);


    // ----- Comparable interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t compareTo(Object::View v) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * @return true
        */
        virtual bool isImmutable() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The wrapped primitive value.
        */
        T m_value;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_PRIMITIVE_HPP
