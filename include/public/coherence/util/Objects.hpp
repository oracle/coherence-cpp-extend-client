/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OBJECTS_HPP
#define COH_OBJECTS_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
 * This class consists of static NULL-safe or NULL-tolerant
 * utility methods for operating on Objects.
 *
 * @author phf  2015.07.10
 *
 * @since 12.2.1
 */
class COH_EXPORT Objects
    : public abstract_spec<Objects>
    {
    // ----- Objects interface ----------------------------------------------

    public:
        using Object::toString;

        /**
         * Returns the result of calling toString on the first
         * argument if the first argument is not NULL and returns
         * the second argument otherwise.
         *
         * @param v              an Object
         * @param vsNullDefault  string to return if the first argument is
         *                       NULL
         *
         * @return the result of calling toString on the first
         * argument if it is not NULL and the second argument
         * otherwise.
         *
         * @see Object#toString(Object)
         */
        static String::View toString(Object::View v, String::View vsNullDefault);

        /**
         * Returns 0 if the arguments are identical and 
         * vC->compare(vA, vB) otherwise.
         * Consequently, if both arguments are NULL, 0
         * is returned.
         *
         * Note that if one of the arguments is NULL, a 
         * NullPointerException may or may not be thrown depending on
         * what ordering policy, if any, the Comparator
         * chooses to have for NULL values.
         *
         * @param vA  an Object
         * @param vB  an Object to be compared with vA
         * @param vC  the Comparator to compare the first two arguments
         *
         * @return 0 if the arguments are identical and
         *           c->compare(vA, vB) otherwise.
         *
         * @see Comparable
         * @see Comparator
         */
        static int32_t compare(Object::View vA, Object::View vB, Comparator::View vC);

        /**
         * Checks that the specified object reference is not NULL. This
         * method is designed primarily for doing parameter validation in methods
         * and constructors, as demonstrated below:
         * <blockquote><pre>
         * protected Foo(Bar::View vBar) {
         *     this.vBar = Objects::requireNonNull(vBar);
         * }
         * </pre></blockquote>
         *
         * @param oh the Object reference to check for nullity
         *
         * @return oh if not NULL
         *
         * @throws NullPointerException if oh is NULL
         */
        static Object::Holder requireNonNull(Object::Holder oh);

        /**
         * Checks that the specified Object reference is not NULL and
         * throws a customized NullPointerException if it is. This method
         * is designed primarily for doing parameter validation in methods and
         * constructors with multiple parameters, as demonstrated below:
         * <blockquote><pre>
         * protected Foo(Bar::View vBar, Baz::View vBaz) {
         *     this.vBar = Objects::requireNonNull(vBar, "vBar must not be null");
         *     this.vBaz = Objects::requireNonNull(vBaz, "vBaz must not be null");
         * }
         * </pre></blockquote>
         *
         * @param oh         the Object reference to check for nullity
         * @param vsMessage  detail message to be used in the event that a
         *                   NullPointerException is thrown
         *
         * @return  oh if not NULL
         *
         * @throws NullPointerException if oh is NULL
         */
        static Object::Holder requireNonNull(Object::Holder oh, String::View vsMessage);

        /**
         * Returns true if the provided reference is NULL otherwise
         * returns false.
         *
         * @param v  an Object to be checked against NULL
         *
         * @return true if the provided reference is NULL otherwise false
         */
        static bool isNull(Object::View v);

        /**
         * Returns true if the provided reference is non-NULL
         * otherwise returns false.
         *
         * @param v  an Object to be checked against NULL
         *
         * @return true if the provided reference is non-NULL otherwise false
         */
        static bool nonNull(Object::View v);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OBJECTS_HPP
