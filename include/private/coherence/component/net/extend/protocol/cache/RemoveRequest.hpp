/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REMOVE_REQUEST_HPP
#define COH_REMOVE_REQUEST_HPP

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
* Map::remove(Object::View vKey) Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT RemoveRequest
    : public class_spec<RemoveRequest,
        extends<AbstractKeyRequest> >
    {
    friend class factory<RemoveRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new RemoveRequest instance.
        */
        RemoveRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        RemoveRequest(const RemoveRequest&);


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


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- accessors ------------------------------------------------------

    public:
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


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 6;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * If true, this PutRequest should return the old value back to the
        * caller; otherwise the return value will be ignored.
        */
        bool m_fReturnRequired;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_REMOVE_REQUEST_HPP
