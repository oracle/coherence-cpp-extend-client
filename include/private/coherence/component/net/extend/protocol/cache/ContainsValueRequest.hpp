/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONTAINS_VALUE_REQUEST_HPP
#define COH_CONTAINS_VALUE_REQUEST_HPP

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
* Map::containsValue(Object::View vValue) Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT ContainsValueRequest
    : public class_spec<ContainsValueRequest,
        extends<NamedCacheRequest> >
    {
    friend class factory<ContainsValueRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ContainsValueRequest instance.
        */
        ContainsValueRequest();


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
        * Return the entry value.
        *
        * @return the entry value
        */
        virtual Object::View getValue() const;

        /**
        * Configure the entry value.
        *
        * @param vValue  the value
        */
        virtual void setValue(Object::View vValue);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 3;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The entry value.
        */
        mutable FinalView<Object> f_vValue;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_CONTAINS_VALUE_REQUEST_HPP
