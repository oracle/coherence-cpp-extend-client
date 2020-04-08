/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_INVOCATION_SERVICE_HPP
#define COH_SAFE_INVOCATION_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/Invocable.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/component/util/SafeService.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::net::Invocable;
using coherence::net::InvocationService;
using coherence::util::Map;
using coherence::util::Set;


/**
* Safe wrapper for RemoteInvocationService.
*
* @author tb  2008.02.28
*/
class COH_EXPORT SafeInvocationService
    : public class_spec<SafeInvocationService,
        extends<SafeService>,
        implements<InvocationService> >
    {
    friend class factory<SafeInvocationService>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a SafeInvocationService.
        */
        SafeInvocationService();

    private:
        /**
        * Blocked copy constructor.
        */
        SafeInvocationService(const SafeInvocationService&);


    // ----- InvocationService interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Map::View query(Invocable::Handle hTask,
                Set::View vSetMembers);


    // ----- SafeInvocationService interface --------------------------------

    public:
        /**
        * Ensures that the returned InvocationService is running before
        * returning it. If the InvocationService is not running and has not
        * been explicitly stopped, the InvocationService is restarted.
        *
        * @param fDrain  if true, the calling thread will be blocked until
        *                the wrapped CacheService event dispatcher queue is
        *                empty and all outstanding tasks have been executed
        *
        * @return the running wrapped InvocationService
        */
        virtual InvocationService::Handle ensureRunningInvocationService(
                bool fDrain);

        /**
        * Ensures that the returned InvocationService is running before
        * returning it. If the InvocationService is not running and has not
        * been explicitly stopped, the InvocationService is restarted.
        *
        * @param fDrain  if true, the calling thread will be blocked until
        *                the wrapped CacheService event dispatcher queue is
        *                empty and all outstanding tasks have been executed
        *
        * @return the running wrapped InvocationService
        */
        virtual InvocationService::View ensureRunningInvocationService(
                bool fDrain) const;


    // ----- property getters/setters ---------------------------------------

    public:
        /**
        * Getter for the invocationService property.
        *
        * @return the wrapped InvocationService implementation
        */
        virtual InvocationService::View getRunningInvocationService() const;


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Ensures that the returned InvocationService is running before
        * returning it. If the InvocationService is not running and has not
        * been explicitly stopped, the InvocationService is restarted.
        *
        * @param fDrain  if true, the calling thread will be blocked until
        *                the wrapped CacheService event dispatcher queue is
        *                empty and all outstanding tasks have been executed
        *
        * @return the running wrapped InvocationService
        */
        virtual InvocationService::Handle
                ensureRunningInvocationServiceInternal(bool fDrain) const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SAFE_INVOCATION_SERVICE_HPP
