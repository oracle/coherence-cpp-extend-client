/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_SERVICE_FACTORY_HPP
#define COH_INVOCATION_SERVICE_FACTORY_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)

using coherence::component::net::extend::AbstractPofMessageFactory;


/**
* MessageFactory for version 1 of the InvocationService Protocol.
*
* @author jh  2008.02.15
*/
class COH_EXPORT InvocationServiceFactory
    : public class_spec<InvocationServiceFactory,
        extends<AbstractPofMessageFactory> >
    {
    friend class factory<InvocationServiceFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InvocationServiceFactory instance.
        */
        InvocationServiceFactory();

    private:
        /**
        * Blocked copy constructor.
        */
        InvocationServiceFactory(const InvocationServiceFactory&);


    // ----- MessageFactory -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getVersion() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessage::Handle instantiateMessage(int32_t nType) const;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_INVOCATION_SERVICE_FACTORY_HPP
