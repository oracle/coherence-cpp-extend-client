/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAME_SERVICE_RESPONSE_HPP
#define COH_NAME_SERVICE_RESPONSE_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

using coherence::component::net::extend::AbstractPofResponse;

/**
* Generic Response class used for basic NameService Protocol Response
* messages.
*
* @author Wei Lin  2012.06.15
*
* @since Coherence 12.1.2
*/
class COH_EXPORT NameServiceResponse
    : public class_spec<NameServiceResponse,
        extends<AbstractPofResponse> >
    {
    friend class factory<NameServiceResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NameServiceResponse instance.
        */
        NameServiceResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        NameServiceResponse(const NameServiceResponse&);

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

#endif // COH_NAME_SERVICE_RESPONSE_HPP
