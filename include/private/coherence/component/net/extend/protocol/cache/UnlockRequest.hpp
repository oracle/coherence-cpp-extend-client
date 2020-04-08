/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNLOCK_REQUEST_HPP
#define COH_UNLOCK_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AbstractKeyRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;


/**
* NamedCache::unlock(Object::View vKey) Request message.
*
* @author jh  2008.02.29
*/
class COH_EXPORT UnlockRequest
    : public class_spec<UnlockRequest,
        extends<AbstractKeyRequest> >
    {
    friend class factory<UnlockRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new UnlockRequest instance.
        */
        UnlockRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        UnlockRequest(const UnlockRequest&);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 32;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_UNLOCK_REQUEST_HPP
