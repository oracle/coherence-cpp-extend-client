/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCABLE_HPP
#define COH_INVOCABLE_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,net)

class InvocationService;


/**
* The Invocable object is a cluster-portable object that can be invoked on
* any set of remote members and each can optionally register a return value
* for the invocation.
*
* When an Invocable object is received for execution, the order of execution
* is:
* <ul>
* <li>the Invocable object is deserialized by the InvocationService;
* <li>the InvocationService provides a reference to itself to the Invocable
*     object by calling the init method;
* <li>the InvocationService invokes the Invocable object by calling the run
*     method;
* <li>if the InvocationService is responsible for returning a value from the
*     invocation, it obtains the value by calling the getResult method.
* </ul>
*
* @author jh  2007.12.21
*/
class COH_EXPORT Invocable
    : public interface_spec<Invocable,
        implements<Runnable> >
    {
    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * InvocationService Handle definition.
        */
        typedef TypedHandle<InvocationService> InvocationServiceHandle;

        /**
        * InvocationService View definition.
        */
        typedef TypedHandle<const InvocationService> InvocationServiceView;


    // ----- Invocable interface --------------------------------------------

    public:
        /**
        * Called by the InvocationService exactly once on this Invocable
        * object as part of its initialization.
        * <p>
        * <b>Note:</b> implementations of the Invocable interface that store
        * the service reference must do so only in a transient field.
        *
        * @param hService  the containing InvocationService
        */
        virtual void init(InvocationServiceHandle hService) = 0;

        /**
        * Determine the result from the invocation of this object. This
        * method is called by the InvocationService after the run() method
        * returns.
        *
        * @return the invocation result, if any
        */
        virtual Object::Holder getResult() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INVOCABLE_HPP
