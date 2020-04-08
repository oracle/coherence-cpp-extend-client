/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_POF_RESPONSE_HPP
#define COH_ABSTRACT_POF_RESPONSE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"
#include "private/coherence/net/messaging/Response.hpp"



COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::messaging::Response;
using coherence::util::Collection;
using coherence::util::Describable;
using coherence::util::Set;


/**
* Abstract implementation of the Response interface that supports POF
* serialization.
*
* @author jh  2008.02.12
*/
class COH_EXPORT AbstractPofResponse
    : public abstract_spec<AbstractPofResponse,
        extends<AbstractPofMessage>,
        implements<Response> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractPofResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractPofResponse(const AbstractPofResponse&);


    // ----- Response interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int64_t getRequestId() const;

        /**
        * {@inheritDoc}
        */
        virtual void setRequestId(int64_t lId);

        /**
        * {@inheritDoc}
        */
        virtual bool isFailure() const;

        /**
        * {@inheritDoc}
        */
        virtual void setFailure(bool fFailure);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getResult() const;

        /**
        * {@inheritDoc}
        */
        virtual void setResult(Object::Holder ohResult);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isSolicited() const;

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
        * Set the result of processing the Request as a Collection.
        *
        * @param hCol  the result as a Collection
        */
        virtual void setResultAsCollection(Collection::Handle hCol);

        /**
        * Set the result of processing the Request as a Set of Map::Entry
        * objects.
        *
        * @param hSet  the result as a Set of Map::Entry objects
        */
        virtual void setResultAsEntrySet(Set::Handle hSet);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Result POF format: Generic
        */
        static const int32_t fmt_generic    = 0;

        /**
        * Result POF format: Collection
        */
        static const int32_t fmt_collection = 1;

        /**
        * Result POF format: Map
        */
        static const int32_t fmt_map        = 2;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The status of the Response. If false, the Request was processed
        * successfully. If true, an error or exception occured while
        * processing the Request.
        */
        bool m_fFailure;

        /**
        * The unique identifier of the Request associated with this Response.
        */
        int64_t m_lRequestId;

        /**
        * The result of processing the Request associated with this Response.
        */
        MemberHolder<Object> m_ohResult;

        /**
        * The POF format of the result. The value may be one of:
        *
        * fmt_generic
        * fmt_collection
        * fmt_map
        */
        int32_t m_nResultFormat;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_POF_RESPONSE_HPP
