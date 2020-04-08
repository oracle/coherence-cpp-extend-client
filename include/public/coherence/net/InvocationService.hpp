/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_SERVICE_HPP
#define COH_INVOCATION_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/Invocable.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::util::Map;
using coherence::util::Set;


/**
* The InvocationService is a Service for delivering executable objects to the
* cluster member to which the client is connected. The executable objects
* must implement the Invocable interface, which extends the Runnable
* interface. Using this interface, application code can cause execution of an
* Invocable object to occur within the cluster.
*
* @author jh  2007.12.21
*/
class COH_EXPORT InvocationService
    : public interface_spec<InvocationService,
        implements<Service> >
    {
    // ----- InvocationService interface ------------------------------------

    public:
        /**
        * Synchronously invoke the specified task on each of the specified
        * members. This method will not return until the specified members
        * have completed their processing, failed in their processing, or
        * died trying.
        * <p>
        * Currently, the client implementation of this interface only
        * supports invocation on the cluster member to which the client is
        * connected. Therefore, the specified Set of Member objects must
        * be NULL.
        *
        * @param hTask        the Invocable object to distribute to the
        *                     specified members in order to be invoked on
        *                     those members
        * @param vSetMembers  must be NULL (future use)
        *
        * @return a Map of result objects keyed by Member object
        */
        virtual Map::View query(Invocable::Handle hTask, Set::View vSetMembers) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INVOCATION_SERVICE_HPP
