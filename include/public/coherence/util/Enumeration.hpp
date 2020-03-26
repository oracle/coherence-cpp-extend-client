/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ENUMERATION_HPP
#define COH_ENUMERATION_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An object that implements the Enumeration interface generates a series of
* elements, one at a time. Successive calls to the nextElement method return
* successive elements of the series.
*
* @author tb  2010.10.16
*/
class COH_EXPORT Enumeration
    : public interface_spec<Enumeration>
    {
    // ----- Enumeration interface ---------------------------------------------

    public:
        /**
        * Tests if this enumeration contains more elements.
        *
        * @return true if and only if this enumeration object contains at
        *         least one more element to provide; false otherwise
        */
        virtual bool hasMoreElements() const = 0;

        /**
        * Returns the next element of this enumeration if this enumeration
        * object has at least one more element to provide.
        *
        * @return the next element of this enumeration
        *
        * @throws coherence::lang::NoSuchElementException if no more elements
        *         exist.
        */
        virtual Object::Holder nextElement() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ENUMERATION_HPP
