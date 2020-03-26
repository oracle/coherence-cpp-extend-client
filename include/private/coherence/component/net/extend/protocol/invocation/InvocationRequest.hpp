/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_REQUEST_HPP
#define COH_INVOCATION_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/net/Invocable.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/invocation/InvocationServiceRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::Invocable;


/**
* InvocationService::query(Invocable::Handle hTask, Set::View vSetMembers)
* Request message.
*
* @author jh  2008.02.15
*/
class COH_EXPORT InvocationRequest
    : public class_spec<InvocationRequest,
        extends<InvocationServiceRequest> >
    {
    friend class factory<InvocationRequest>;
    
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InvocationRequest instance.
        */
        InvocationRequest();


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
        * Return the Invocable task to execute.
        *
        * @return the Invocable
        */
        virtual Invocable::View getTask() const;

        /**
        * Configure the Invocable task to execute.
        *
        * @param vTask  the Invocable
        */
        virtual void setTask(Invocable::View vTask);


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
        FinalView<Invocable> f_vTask;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_INVOCATION_REQUEST_HPP
