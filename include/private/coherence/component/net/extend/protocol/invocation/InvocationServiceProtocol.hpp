/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_SERVICE_PROTOCOL_HPP
#define COH_INVOCATION_SERVICE_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"
#include "private/coherence/component/net/extend/AbstractPofProtocol.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)

using coherence::component::net::extend::AbstractPofMessageFactory;
using coherence::component::net::extend::AbstractPofProtocol;


/**
* The InvocationService Protocol is used to execute Invocable tasks within a
* remote Coherence cluster.
*
* @author jh  2008.02.15
*/
class COH_EXPORT InvocationServiceProtocol
    : public class_spec<InvocationServiceProtocol,
        extends<AbstractPofProtocol> >
    {
    friend class factory<InvocationServiceProtocol>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InvocationServiceProtocol instance.
        */
        InvocationServiceProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        InvocationServiceProtocol(const InvocationServiceProtocol&);


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

#endif // COH_INVOCATION_SERVICE_PROTOCOL_HPP
