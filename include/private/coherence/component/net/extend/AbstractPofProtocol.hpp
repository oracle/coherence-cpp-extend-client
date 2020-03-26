/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_POF_PROTOCOL_HPP
#define COH_ABSTRACT_POF_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "coherence/util/Describable.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"



COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::net::messaging::Protocol;
using coherence::util::Describable;
using coherence::util::Map;


/**
* Abstract implementation of the Protocol interface that represents a
* collection of POFable Message types.
*
* @author jh  2008.02.14
*/
class COH_EXPORT AbstractPofProtocol
    : public abstract_spec<AbstractPofProtocol,
        extends<Describable>,
        implements<Protocol> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractPofProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractPofProtocol(const AbstractPofProtocol&);
		

    // ----- Protocol interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Protocol::MessageFactory::View getMessageFactory(
                int32_t nVersion) const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Instantiate a new MessageFactory for the given version of this
        * Protocol.
        *
        * @param nVersion  the version of the Protocol that the returned
        *                  MessageFactory will use
        *
        * @return a new MessageFactory for the given version of this
        *         Protocol
        */
        virtual AbstractPofMessageFactory::Handle instantiateMessageFactory(
                int32_t nVersion) const = 0;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A map of MessageFactory objects, keyed by Protocol version.
        */
        mutable FinalHandle<Map> f_hMapMessageFactory;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_POF_PROTOCOL_HPP
