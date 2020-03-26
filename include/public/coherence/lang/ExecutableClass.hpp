/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EXECUTABLE_CLASS_HPP
#define COH_EXECUTABLE_CLASS_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Class.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

/**
* Interface specialization for Classes which are executable.
*
* An executable class includes a method compatible with the following
* signature:
* @code
* public: static void main(ObjectArray::View vasArg);
* @endcode
* The class does not implement this interface, it however must be registered
* with the SystemClassLoader via COH_REGISTER_EXECUTABLE_CLASS(class).
*
* As of Coherence 3.7.1 it is no longer necessary to register executable classes
* via this interface. As an alternative the "main" method with the above
* signature can be registered via any Class implementation and will be
* resolved via reflection.
*
* @author mf 2008.10.02
*/
class ExecutableClass
    : public interface_spec<ExecutableClass>
    {
    // ----- ExecutableClass interface --------------------------------------

    public:
        /**
        * Execute the class.
        *
        * @param vasArg  the executable argument String array
        */
        virtual void execute(ObjectArray::View vasArg) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EXECUTABLE_CLASS_HPP
