/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAME_SERVICE_FACTORY_HPP
#define COH_NAME_SERVICE_FACTORY_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

using coherence::component::net::extend::AbstractPofMessageFactory;

/**
* MessageFactory for version 1 of the NameService Protocol.
*
* @author Wei Lin  2012.06.15
*
* @since Coherence 12.1.2
*/
class COH_EXPORT NameServiceFactory
    : public class_spec<NameServiceFactory,
        extends<AbstractPofMessageFactory> >
    {
    friend class factory<NameServiceFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NameServiceFactory instance.
        */
        NameServiceFactory();

    private:
        /**
        * Blocked copy constructor.
        */
        NameServiceFactory(const NameServiceFactory&);


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

#endif // COH_NAME_SERVICE_FACTORY_HPP
