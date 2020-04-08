/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_STACK_TRACE_ELEMENT_HPP
#define COH_STACK_TRACE_ELEMENT_HPP

#include "coherence/lang.ns"

#include "coherence/lang/interface_spec.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)


/**
* StackTraceElement interface represents a frame within a stack trace.
*
* The information included in the trace is not of a guaranteed format and may
* differ between platforms and debug/release libraries.  It is recommended
* that applications only print the element, and not attempt to interpret the
* output.
*
* This interface is also a place holder and may grow over time to include
* certain pre-parsed information, such as file and line numbers, though the
* availability of this information would still not be guaranteed.
*
* @author mf  2008.04.01
*/
class COH_EXPORT StackTraceElement
    : public interface_spec<StackTraceElement>
    {
    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_STACK_TRACE_ELEMENT_HPP
