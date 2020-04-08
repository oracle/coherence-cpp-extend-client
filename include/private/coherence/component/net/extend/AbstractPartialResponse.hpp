/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_PARTIAL_RESPONSE_HPP
#define COH_ABSTRACT_PARTIAL_RESPONSE_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::AbstractPofResponse;


/**
* Abstract Response implementation that carries a partial result.
*
* @author tb  2008.08.12
*/
class COH_EXPORT AbstractPartialResponse
    : public abstract_spec<AbstractPartialResponse,
        extends<AbstractPofResponse> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractPartialResponse();

        
    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


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
        * Getter for property Cookie.<p>
        * 
        * If non-null, this PartialResponse contains a partial result. The 
        * receiver of a PartialResponse can accumulate or iterate the entire 
        * result by sending additional Request(s) until this property is 
        * NULL.
        * 
        * @return the opaque cookie used to support streaming
        */
        virtual Binary::View getCookie() const;
        
        /**
        * Setter for property Cookie.<p>
        * 
        * If non-null, this PartialResponse contains a partial result. The 
        * receiver of a PartialResponse can accumulate or iterate the entire 
        * result by sending additional Request(s) until this property is 
        * NULL.
        * 
        * @param vBin  opaque cookie used to support streaming
        */
        virtual void setCookie(Binary::View vBin);
       
        
    // ----- data members ---------------------------------------------------

    protected:
    /**
    * Property Cookie
    *
    * Opaque cookie used to support streaming.
    * 
    * If non-null, this PartialResponse contains a partial result. The 
    * receiver of a PartialResponse can accumulate or iterate the entire 
    * result by sending additional Request(s) until this property is NULL.
    */
    MemberView<Binary> m_vCookie;

    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_PARTIAL_RESPONSE_HPP
