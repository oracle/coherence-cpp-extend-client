/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PUT_REQUEST_HPP
#define COH_PUT_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AbstractKeyRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Map::put(Object::View vKey, Object::Holder ohValue, int64_t cMillis)
* Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT PutRequest
    : public class_spec<PutRequest,
        extends<AbstractKeyRequest> >
    {
    friend class factory<PutRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PutRequest instance.
        */
        PutRequest();


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
        * Return the entry expiration value.
        *
        * @return the entry expiration value
        */
        virtual int64_t getExpiryDelay() const;

        /**
        * Configure the entry expiration value.
        *
        * @param cMillis  the entry expiration in milliseconds
        */
        virtual void setExpiryDelay(int64_t cMillis);

        /**
        * Return true if this PutRequest should return the old value back to
        * the caller; otherwise the return value will be ignored.
        *
        * @return the return required flag
        */
        virtual bool isReturnRequired() const;

        /**
        * Configure the return required flag.
        *
        * @param fRequired  the new value of the return required flag
        */
        virtual void setReturnRequired(bool fRequired);

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
        static const int32_t type_id = 5;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The entry expiration value.
        */
        int64_t m_cExpiryDelay;

        /**
        * If true, this PutRequest should return the old value back to the
        * caller; otherwise the return value will be ignored.
        */
        bool m_fReturnRequired;

        /**
        * The entry value.
        */
        mutable FinalView<Object> f_vValue;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_PUT_REQUEST_HPP
