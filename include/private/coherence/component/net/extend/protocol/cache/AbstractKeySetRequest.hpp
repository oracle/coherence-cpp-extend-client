/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_KEY_SET_REQUEST_HPP
#define COH_ABSTRACT_KEY_SET_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Collection.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::Collection;


/**
* Base class for all NamedCache Protocol Request messages that include a
* Collection of keys.
*
* @author jh  2008.02.18
*/
class COH_EXPORT AbstractKeySetRequest
    : public abstract_spec<AbstractKeySetRequest,
        extends<NamedCacheRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractKeySetRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractKeySetRequest(const AbstractKeySetRequest&);


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
        * Return the Collection of keys associated with this Request.
        *
        * @return the Collection of keys associated with this Request
        */
        virtual Collection::View getKeySet() const;

        /**
        * Associate the given Collection of keys with this Request.
        *
        * @param vCol  the Collection of keys to associate with this Request
        */
        virtual void setKeySet(Collection::View vCol);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Collection of keys associated with this Request.
        */
        FinalView<Collection> f_vCol;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_ABSTRACT_KEY_SET_REQUEST_HPP
