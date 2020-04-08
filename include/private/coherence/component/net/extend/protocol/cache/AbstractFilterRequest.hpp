/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_FILTER_REQUEST_HPP
#define COH_ABSTRACT_FILTER_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Filter.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::Filter;


/**
* Base class for all NamedCache Protocol Request messages that include a
* coherence::util::Filter.
*
* @author jh  2008.02.29
*/
class COH_EXPORT AbstractFilterRequest
    : public abstract_spec<AbstractFilterRequest,
        extends<NamedCacheRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractFilterRequest();


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
        * Return the Filter associated with this Request.
        *
        * @return the Filter associated with this Request
        */
        virtual Filter::View getFilter() const;

        /**
        * Associate the given Filter with this Request.
        *
        * @param vFilter  the Filter to associate with this Request
        */
        virtual void setFilter(Filter::View vFilter);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Filter associated with this Request.
        */
        FinalView<Filter> f_vFilter;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_ABSTRACT_FILTER_REQUEST_HPP
