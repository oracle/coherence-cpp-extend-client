/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PARTIAL_RESPONSE_HPP
#define COH_PARTIAL_RESPONSE_HPP

#include "coherence/util/Filter.hpp"

#include "private/coherence/component/net/extend/AbstractPartialResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPartialResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::Filter;


/**
* Generic Response component used for partial NamedCache Protocol Responses.
*
* @author tb 2008.08.14
*/
class COH_EXPORT PartialResponse
    : public class_spec<PartialResponse,
        extends<AbstractPartialResponse> >
    {
    friend class factory<PartialResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PartialResponse instance.
        */
        PartialResponse();


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
        

    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Getter for property Filter.
        * 
        * @return the filter used by QueryRequests that carry a LimitFilter.
        */
        virtual Filter::View getFilter() const;
        
        /**
        * Setter for property Filter.
        * 
        * @param vFilter  the filter to be returned.
        */
        virtual void setFilter(Filter::View vFilter);

        /**
        * Getter for property FilterCookie.
        * 
        * @return the opaque cookie used by a LimitFilter request.
        */
        virtual Object::View getFilterCookie() const;
        
        /**
        * Setter for property FilterCookie.
        * 
        * @param vCookie  the opaque cookie used by a LimitFilter request.
        */
        virtual void setFilterCookie(Object::View vCookie);
        

    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 1000;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The filter to be returned.  This is only used for QueryRequests
        * that carry a LimitFilter.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The opaque cookie used by a LimitFilter request.
        */
        FinalView<Object> f_vFilterCookie;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_PARTIAL_RESPONSE_HPP
