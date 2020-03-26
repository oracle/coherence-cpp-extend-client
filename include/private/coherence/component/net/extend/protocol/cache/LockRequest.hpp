/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOCK_REQUEST_HPP
#define COH_LOCK_REQUEST_HPP

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
* NamedCache::lock(Object::View vKey, int64_t cWait) Request message.
*
* @author jh  2008.02.29
*/
class COH_EXPORT LockRequest
    : public class_spec<LockRequest,
        extends<AbstractKeyRequest> >
    {
    friend class factory<LockRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new LockRequest instance.
        */
        LockRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        LockRequest(const LockRequest&);


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
        * Return the number of milliseconds to wait to obtain the lock.
        *
        * @return the number of milliseconds to wait; 0 to return
        *         immediately, -1 to wait indefinitely
        */
        virtual int64_t getTimeoutMillis() const;

        /**
        * Configure the number of milliseconds to wait to obtain the lock.
        *
        * @param cMillis  the number of milliseconds to wait; 0 to return
        *                 immediately, -1 to wait indefinitely
        */
        virtual void setTimeoutMillis(int64_t cMillis);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 31;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of milliseconds to wait to obtain the lock; 0 to return
        * immediately; -1 to wait indefinitely.
        */
        int64_t m_cTimeoutMillis;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_LOCK_REQUEST_HPP
