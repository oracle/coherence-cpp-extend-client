/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_POF_REQUEST_HPP
#define COH_ABSTRACT_POF_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Describable.hpp"

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"
#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/PofChannel.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Request.hpp"
#include "private/coherence/net/messaging/Response.hpp"



COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Request;
using coherence::net::messaging::Response;
using coherence::util::Describable;


/**
* Abstract implementation of the Request interface that supports POF
* serialization.
*
* @author jh  2008.02.12
*/
class COH_EXPORT AbstractPofRequest
    : public class_spec<AbstractPofRequest,
        extends<AbstractPofMessage>,
        implements<Request> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractPofRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractPofRequest(const AbstractPofRequest&);


    // ----- AbstractPofRequest interface -----------------------------------

    public:
        /**
        * Determine if this Request is an incoming Request sent by a peer.
        *
        * @return true iff this is an incoming Request sent by a peer.
        */
        bool isIncoming() const;


    // ----- Request interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int64_t getId() const;

        /**
        * {@inheritDoc}
        */
        virtual void setId(int64_t lId);

        /**
        * {@inheritDoc}
        */
        virtual Response::Handle ensureResponse();

        /**
        * {@inheritDoc}
        */
        virtual Request::Status::Handle getStatus();

        /**
        * {@inheritDoc}
        */
        virtual Request::Status::View getStatus() const;

        /**
        * {@inheritDoc}
        */
        virtual void setStatus(Request::Status::Handle hStatus);

        /**
        * {@inheritDoc}
        */
        virtual int64_t getRequestTimeoutMillis() const;


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void run();

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


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Factory method: create a new Response instance.
        *
        * @param  the MessageFactory used to create the new Response object
        *
        * @return a new Response object
        */
        virtual AbstractPofResponse::Handle instantiateResponse(
                Protocol::MessageFactory::View vFactory) const;

        /**
        * Called when an Exception is caught while executing the Request.
        *
        * @param ohe  the caught Exception
        */
        virtual void onException(Exception::Holder ohe);

        /**
        * Called when the Request is run.
        *
        * @param hResponse  the Response that should be populated with the
        *                   result of running the Request
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse) = 0;


    // ----- inner class: Status --------------------------------------------

    public:
        /**
        * Concrete implementation of the Status interface that can be used
        * with any AbstractPofRequest subclasses.
        */
        class COH_EXPORT Status
            : public class_spec<Status,
                extends<Describable>,
                implements<Request::Status> >
            {
            friend class factory<Status>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new Status object.
                */
                Status();

            private:
                /**
                * Blocked copy constructor.
                */
                Status(const Status&);

            // ----- Status interface -----------------------------------

            public:
                /**
                * @internal
                */
                virtual bool isClosed() const;

                /**
                * @internal
                */
                virtual void cancel();

                /**
                * @internal
                */
                virtual void cancel(Exception::Holder ohe);

                /**
                * @internal
                */
                virtual Request::Handle getRequest();

                /**
                * @internal
                */
                virtual Request::View getRequest() const;

                /**
                * @internal
                */
                virtual Response::Handle getResponse();

                /**
                * @internal
                */
                virtual Response::View getResponse() const;

                /**
                * @internal
                */
                virtual Response::Handle waitForResponse(int64_t cMillis);

                /**
                * @internal
                */
                virtual Response::Handle waitForResponse();

            // ----- Describable interface ------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual String::View getDescription() const;

            // ----- internal methods -----------------------------------

            protected:
                /**
                * Called after the Request represented by this Status has
                * completed (successfully or unsuccessfully) or been
                * canceled.
                */
                virtual void onCompletion();

            // ----- accessors ------------------------------------------

            public:
                /**
                * Getter for property DefaultTimeoutMillis.<p>
                * The default request timeout in milliseconds.
                *
                * @return the default timeout
                */
                virtual int64_t getDefaultTimeoutMillis() const;

                /**
                * Return the Exception associated with a failed or canceled
                * Request.
                *
                * @return the Exception
                */
                virtual Exception::Holder getError() const;

                /**
                * Return The time (in millseconds) that this Status object
                * was initialized
                *
                * @return the initialization time
                */
                virtual int64_t getInitTimeMillis() const;

                /**
                * Return the PofChannel associated with this Status.
                *
                * @return the PofChannel
                */
                virtual PofChannel::Handle getChannel();

                /**
                * Return the PofChannel associated with this Status.
                *
                * @return the PofChannel
                */
                virtual PofChannel::View getChannel() const;

                /**
                * Associate the given PofChannel with this Status.
                *
                * @param hChannel  the PofChannel
                */
                virtual void setChannel(PofChannel::Handle hChannel);

                /**
                * Associate the given Request with this Status.
                *
                * @param hRequest  the PofRequest
                */
                virtual void setRequest(Request::Handle hRequest);

                /**
                * Associate the given Response with this Status.
                *
                * @param hResponse  the Response
                */
                virtual void setResponse(Response::Handle hResponse);

                /**
                * Setter for property DefaultTimeoutMillis.
                *
                * @param cMillis  the default request timeout in milliseconds
                */
                virtual void setDefaultTimeoutMillis(int64_t cMillis);


            // ----- data members ---------------------------------------

            protected:
                /**
                * The Channel associated with this Status.
                */
                FinalHandle<PofChannel> f_hChannel;

                /**
                * Flag that indicates whether or not the Request represented
                * by this Status has completed successfully, completed
                * unsucessfully, or been canceled.
                */
                bool m_fClosed;

                /**
                * Property DefaultTimeoutMillis
                *
                * The default request timeout in milliseconds.
                */
                int64_t m_ldtDefaultTimeoutMillis;

                /**
                * The Exception associated with a failed or canceled Request.
                */
                FinalHolder<Exception> f_ohError;

                /**
                * The time (in millseconds) that this Status object was
                * initialized.
                */
                int64_t m_ldtInitTimeMillis;

                /**
                * The Request represented by this Status.
                */
                FinalHandle<Request> f_hRequest;

                /**
                * The Response sent by the peer.
                */
                FinalHandle<Response> f_hResponse;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The unique identifier of this Request.
        */
        int64_t m_lId;

        /**
        * The Response associated with this Request.
        */
        FinalHandle<Response> f_hResponse;

        /**
        * The Status of this Request.
        */
        WeakHandle<Request::Status> m_whStatus;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_POF_REQUEST_HPP
