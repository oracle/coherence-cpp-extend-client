/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLEAR_REQUEST_HPP
#define COH_CLEAR_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;


/**
* Map::clear() Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT ClearRequest
    : public class_spec<ClearRequest,
        extends<NamedCacheRequest> >
    {
    friend class factory<ClearRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ClearRequest instance.
        */
        ClearRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        ClearRequest(const ClearRequest&);


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
        /** Set whether the request is for cache truncate.
        *
        * @param fTruncate  true if it is a truncate request
        */
        virtual void setTruncate(bool fTruncate);

        /**
        * Determine whether the request is cache truncate.
        *
        * @return true if the request is cache truncate
        */
        virtual bool isTruncate() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 8;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The value of true indicates that the request is to truncate the cache.
        */
        bool m_fTruncate;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_CLEAR_REQUEST_HPP
