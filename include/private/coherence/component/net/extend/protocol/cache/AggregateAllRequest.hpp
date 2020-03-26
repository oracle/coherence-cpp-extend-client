/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_AGGREGATE_ALL_REQUEST_HPP
#define COH_AGGREGATE_ALL_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "coherence/net/PriorityTask.hpp"

#include "coherence/util/InvocableMap.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

#include "private/coherence/component/net/extend/protocol/cache/AbstractKeySetRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::PriorityTask;
using coherence::util::InvocableMap;


/**
* InvocableMap::aggregate(Collection::View vCollKeys, EntryAggregator::View
* vAgent) Request message.
*
* @author jh  2008.02.29
*/
class COH_EXPORT AggregateAllRequest
    : public class_spec<AggregateAllRequest,
        extends<AbstractKeySetRequest>,
        implements<PriorityTask> >
    {
    friend class factory<AggregateAllRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new AggregateAllRequest instance.
        */
        AggregateAllRequest();


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


    // ----- PriorityTask interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getSchedulingPriority() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getExecutionTimeoutMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getRequestTimeoutMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual void runCanceled(bool fAbandoned);


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
        * Return the EntryAggregator associated with this Request.
        *
        * @return the EntryAggregator associated with this Request
        */
        virtual InvocableMap::EntryAggregator::View getAggregator() const;

        /**
        * Return the EntryAggregator associated with this Request.
        *
        * @return the EntryAggregator associated with this Request
        */
        virtual InvocableMap::EntryAggregator::Handle getAggregator();

        /**
        * Associate the given EntryAggregator with this Request.
        *
        * @param hAggregator  the EntryAggregator to associate with this
        *                     Request
        */
        virtual void setAggregator(
                    InvocableMap::EntryAggregator::Handle hAggregator);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 51;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The EntryAggregator associated with this Request.
        */
        FinalHandle<InvocableMap::EntryAggregator> f_hAggregator;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_AGGREGATE_ALL_REQUEST_HPP
