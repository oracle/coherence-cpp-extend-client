/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAME_SERVICE_PROTOCOL_HPP
#define COH_NAME_SERVICE_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"
#include "private/coherence/component/net/extend/AbstractPofProtocol.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

using coherence::component::net::extend::AbstractPofMessageFactory;
using coherence::component::net::extend::AbstractPofProtocol;

/**
* The NameService Protocol is used to access a remote NameService running 
* within a Coherence cluster.
*
* @author Wei Lin  2012.06.15
*
* @since Coherence 12.1.2
*/
class COH_EXPORT NameServiceProtocol
    : public class_spec<NameServiceProtocol,
        extends<AbstractPofProtocol> >
    {
    friend class factory<NameServiceProtocol>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NameServiceProtocol instance.
        */
        NameServiceProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        NameServiceProtocol(const NameServiceProtocol&);

    // ----- Protocol interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getName() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getCurrentVersion() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getSupportedVersion() const;

    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessageFactory::Handle instantiateMessageFactory(
                int32_t nVersion) const;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * Return the singleton InvocationServiceProtocol instance.
        *
        * @return the singleton
        */
        static Handle getInstance();
    };

COH_CLOSE_NAMESPACE6

#endif // COH_NAME_SERVICE_PROTOCOL_HPP
