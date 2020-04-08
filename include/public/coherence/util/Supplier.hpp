/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUPPLIER_HPP
#define COH_SUPPLIER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)

/**
 * A Supplier provides the ability to provide a value in a deferred
 * fashion.
 *
 * @author rl 2019.04.29
 * @since 12.2.1.4
 */ 
class COH_EXPORT Supplier
    : public interface_spec<Supplier>
    {
    // ----- Supplier interface ---------------------------------------------

    public:
        /**
         * Return the value.
         * 
         * @return the value
         */ 
        virtual Object::Holder get() const = 0;

        /**
         * Return the value.
         * 
         * @return the value
         */ 
        virtual Object::Holder get() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SUPPLIER_HPP
