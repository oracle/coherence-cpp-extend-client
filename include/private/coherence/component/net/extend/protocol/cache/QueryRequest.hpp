/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_QUERY_REQUEST_HPP
#define COH_QUERY_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AbstractFilterRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* QueryMap::entrySet(Filter::View vFilter) and keySet(Filter::View vFilter)
* Request message.
*
* @author jh  2008.02.29
*/
class COH_EXPORT QueryRequest
    : public class_spec<QueryRequest,
        extends<AbstractFilterRequest> >
    {
    friend class factory<QueryRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new QueryRequest instance.
        */
        QueryRequest();


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


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


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Factory method: create a new Response instance.
        *
        * @param  the MessageFactory used to create the new Response object
        *
        * @return a new Response object
        */
        virtual AbstractPofResponse::Handle instantiateResponse(
                Protocol::MessageFactory::View vFactory) const;

        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Determine what kind of response is required.
        *
        * @return  true if a key Set is sent back, false if an entry Set is
        *          sent back
        */
        virtual bool isKeysOnly() const;

        /**
        * Configure the type of response.
        *
        * @param fKeysOnly  if true, a key Set is sent back, otherwise an
        *                   entry Set is sent back
        */
        virtual void setKeysOnly(bool fKeysOnly);

        /**
        * Getter for property Cookie.
        * 
        * @return the opaque cookie used to support streaming.
        */
        virtual Binary::View getCookie() const;
        
        /**
        * Setter for property Cookie.
        * 
        * @param vBin  the opaque cookie used to support streaming.
        */
        virtual void setCookie(Binary::View vBin);

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
        static const int32_t type_id = 41;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Specifies what kind of response is required: if true, a key Set is
        * sent back, otherwise an entry Set.
        */
        bool m_fKeysOnly;
        
        /**
        * The opaque cookie used to support streaming.
        */
        FinalView<Binary> f_vCookie;
        
        /**
        * The opaque cookie used by a LimitFilter request.
        */
        FinalView<Object> f_vFilterCookie;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_QUERY_REQUEST_HPP
