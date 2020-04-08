/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GET_ALL_REQUEST_HPP
#define COH_GET_ALL_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AbstractKeySetRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;


/**
* CacheMap::getAll(Collection::View vColKeys) Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT GetAllRequest
    : public class_spec<GetAllRequest,
        extends<AbstractKeySetRequest> >
    {
    friend class factory<GetAllRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new GetAllRequest instance.
        */
        GetAllRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        GetAllRequest(const GetAllRequest&);


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
        static const int32_t type_id = 21;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_GET_ALL_REQUEST_HPP
