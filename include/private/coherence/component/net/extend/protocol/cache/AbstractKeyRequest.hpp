/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_KEY_REQUEST_HPP
#define COH_ABSTRACT_KEY_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Base class for all NamedCache Protocol Request messages that include a key.
*
* @author jh  2008.02.18
*/
class COH_EXPORT AbstractKeyRequest
    : public abstract_spec<AbstractKeyRequest,
        extends<NamedCacheRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractKeyRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractKeyRequest(const AbstractKeyRequest&);
		

    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the key associated with this Request.
        *
        * @return the key associated with this Request
        */
        virtual Object::View getKey() const;

        /**
        * Associate the given key with this Request.
        *
        * @param vKey  the key to associate with this Request
        */
        virtual void setKey(Object::View vKey);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The key associated with this Request.
        */
        FinalView<Object> f_vKey;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_ABSTRACT_KEY_REQUEST_HPP
