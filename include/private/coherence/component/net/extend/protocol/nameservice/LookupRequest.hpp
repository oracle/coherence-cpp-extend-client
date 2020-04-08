/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOOKUP_REQUEST_HPP
#define COH_LOOKUP_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/nameservice/NameServiceRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

/**
* NameService::lookup(String::View vsName) Request message.
*
* @author Wei Lin  2012.06.15
*
* @since Coherence 12.1.2
*/
class COH_EXPORT LookupRequest
    : public class_spec<LookupRequest,
        extends<NameServiceRequest> >
    {
    friend class factory<LookupRequest>;
    
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new LookupRequest instance.
        */
        LookupRequest();

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
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);

    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the lookup name.
        *
        * @return the lookup name
        */
        virtual String::View getLookupName() const;

        /**
        * Configure the lookup name.
        *
        * @param vsLookupName  the lookup name
        */
        virtual void setLookupName(String::View vsLookupName);

    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 1;

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Invocable task to execute.
        */
        MemberView<String> m_vsLookupName;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_LOOKUP_REQUEST_HPP
