/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_STRATEGY_HPP
#define COH_INVOCATION_STRATEGY_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

/**
* An InvocationStrategy provides an abstraction of the underlying
* mechanisms used to retrieve and set a property's value.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT InvocationStrategy
    : public interface_spec<InvocationStrategy>
    {
    public:
        /**
        * Returns the value of the property.
        *
        * @param vhContainer  container of this and all other properties
        *
        * @return property value
        */
        virtual Object::Holder get(Object::View vhContainer) const = 0;

        /**
        * Sets the parameter value to the property.
        *
        * @param ohContainer  container of this and all other properties
        * @param oValue       new value to assign to the property
        */
        virtual void set(Object::Handle ohContainer, Object::Holder oValue) const = 0;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_INVOCATION_STRATEGY_HPP
