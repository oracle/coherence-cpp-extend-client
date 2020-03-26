/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAME_MANGLER_HPP
#define COH_NAME_MANGLER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

/**
* NameMangler implementations provide the ability to transform a
* string to the string convention employed by the mangler implementation.
* An example of this would be to convert a non-camel case string to a camel
* case string.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT NameMangler
    : public interface_spec<NameMangler>
    {
    public:
        /**
        * Convert the given string to a new string using a convention
        * determined by the implementer.
        *
        * @param vsName  original string
        *
        * @return mangled string
        */
        virtual String::View mangle(String::View vsName) const = 0;
    };

COH_CLOSE_NAMESPACE5

#endif /* COH_NAME_MANGLER_HPP */
