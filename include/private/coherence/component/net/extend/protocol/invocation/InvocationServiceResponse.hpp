/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_SERVICE_RESPONSE_HPP
#define COH_INVOCATION_SERVICE_RESPONSE_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)

using coherence::component::net::extend::AbstractPofResponse;


/**
* Generic Response class used for basic InvocationService Protocol Response
* messages.
*
* @author jh  2008.02.15
*/
class COH_EXPORT InvocationServiceResponse
    : public class_spec<InvocationServiceResponse,
        extends<AbstractPofResponse> >
    {
    friend class factory<InvocationServiceResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InvocationServiceResponse instance.
        */
        InvocationServiceResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        InvocationServiceResponse(const InvocationServiceResponse&);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;

        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 0;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_INVOCATION_SERVICE_RESPONSE_HPP
