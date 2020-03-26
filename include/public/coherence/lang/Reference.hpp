/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REFERENCE_HPP
#define COH_REFERENCE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/interface_spec.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Reference provides an additional level of indirection to object instances.
*
* It is not a general use class, but rather it is a building block for such
* things as WeakReference.
*
* @author mf  2008.04.10
*/
class COH_EXPORT Reference
    : public interface_spec<Reference>
    {
    // ----- Reference interface --------------------------------------------

    public:
        /**
        * Set the referenced Object.
        *
        * @param oh  the Object to reference
        */
        virtual void set(Object::Holder oh) = 0;

        /**
        * Return a Holder to the referenced Object.
        *
        * @return a Holder to the referenced Object
        */
        virtual Object::Holder get() const = 0;

        /**
        * Return a Holder to the referenced Object.
        *
        * @return a Holder to the referenced Object
        */
        virtual Object::Holder get() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_REFERENCE_HPP
