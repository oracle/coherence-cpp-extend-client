/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_SERVICE_REQUEST_HPP
#define COH_CACHE_SERVICE_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Base class for all CacheService Protocol Request messages.
*
* @author jh  2008.02.18
*/
class COH_EXPORT CacheServiceRequest
    : public abstract_spec<CacheServiceRequest,
        extends<AbstractPofRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        CacheServiceRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        CacheServiceRequest(const CacheServiceRequest&);


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
        * Return the name of the target NamedCache.
        *
        * @return the name of the target NamedCache
        */
        virtual String::View getCacheName() const;

        /**
        * Configure the name of the target NamedCache.
        *
        * @param vsName  the name of the target NamedCache
        */
        virtual void setCacheName(String::View vsName);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The name of the target NamedCache.
        */
        FinalView<String> f_vsCacheName;
    };

COH_CLOSE_NAMESPACE7

#endif // COH_CACHE_SERVICE_REQUEST_HPP
