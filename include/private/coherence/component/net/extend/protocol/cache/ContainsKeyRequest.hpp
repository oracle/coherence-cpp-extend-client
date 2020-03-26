/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONTAINS_KEY_REQUEST_HPP
#define COH_CONTAINS_KEY_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AbstractKeyRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;


/**
* Map::containsKey(Object::View vKey) Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT ContainsKeyRequest
    : public class_spec<ContainsKeyRequest,
        extends<AbstractKeyRequest> >
    {
    friend class factory<ContainsKeyRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ContainsKeyRequest instance.
        */
        ContainsKeyRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        ContainsKeyRequest(const ContainsKeyRequest&);
		

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
        static const int32_t type_id = 2;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_CONTAINS_KEY_REQUEST_HPP
