/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONSOLE_HPP
#define COH_CONSOLE_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,net)

/**
* Coherence command line console.
*
* Note this class is not registered as an ExecutableClass, it is accessed via
* the CacheFactory.
*
* @author mf  2008.10.09
*/
class Console
    {
    private:
        /**
        * Blocked constructor.
        */
        Console();

    public:
        /**
        * Invoke the Coherence C++ command line tool.
        */
        static void main(ObjectArray::View vasArg);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONSOLE_HPP
