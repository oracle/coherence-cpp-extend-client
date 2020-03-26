/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BOOLEAN_HPP
#define COH_BOOLEAN_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/cloneable_spec.hpp"
#include "coherence/lang/Primitive.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An immutable boolean value.
*
* @author jh  2008.04.01
*/
class COH_EXPORT Boolean
    : public cloneable_spec<Boolean,
        extends<Primitive<bool> > >
    {
    friend class factory<Boolean>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * @internal
        */
        Boolean(bool f);

        /**
        * Copy constructor.
        */
        Boolean(const Boolean& that);


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Get an instance of a Boolean that represents true or false.
        *
        * @param f  the instance of Boolean to return
        *
        * @return the instance of boolean that represents true or false
        *         based on f
        */
        static Handle valueOf(bool f);

        using super::toString;

        /**
         * Return "true" or "false" for a given boolean value.
         *
         * @param f  the boolean value
         *
         * @return "true" or "false" based on f
         *
         * @since 12.2.1.3
         */
        static String::View toString(bool f);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_BOOLEAN_HPP
