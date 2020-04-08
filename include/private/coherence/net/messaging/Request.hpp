/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REQUEST_HPP
#define COH_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/net/messaging/Response.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)


/**
* Request is the root interface for all request messages sent by peer
* endpoints through a {@link Channel}.
* <p/>
* A Request object is created by a {@link Protocol.MessageFactory} and has an
* identifier that uniquely identifies the Message instance.
*
* @author jh  2007.12.19
*
* @see Protocol::MessageFactory
*/
class COH_EXPORT Request
    : public interface_spec<Request,
        implements<Message> >
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- Request interface ----------------------------------------------

    public:
        /**
        * Return the unique identifier for this Request.
        *
        * @return an identifier that uniquely identifies this Request object
        */
        virtual int64_t getId() const = 0;

        /**
        * Configure the unique identifier for this Request.
        *
        * @param lId  an identifier that uniquely identifies this Request
        *             object
        */
        virtual void setId(int64_t lId) = 0;

        /**
        * Return the Response for this Request.
        * <p/>
        * If not already available, the Response must be created using the
        * {@link Protocol.MessageFactory} associated with the Channel that
        * this Request was sent through.
        *
        * @return the Response; must not be null
        */
        virtual Response::Handle ensureResponse() = 0;


    // ----- Status inner interface -----------------------------------------

    public:
        /**
        * A Status represents an asynchronous {@link Request} sent to a peer.
        * <p/>
        * The status of the Request can be determined by calling the
        * {@link #isClosed()} method. If this method returns false, the
        * Request is still in progress. A return value of true indicates that
        * the Request has either completed successfully, completed
        * unsuccessfully, or has been canceled.
        * <p/>
        * When the Request completes, the Response sent by the peer can be
        * retrieved using the {@link #getResponse()} method. If this method
        * returns null, the Request was explicitly {@link #cancel()canceled}.
        * <p/>
        * Rather than constantly polling the Request for the outcome of the
        * Request, a thread can alternatively use the Status to {@link
        * #waitForResponse(long) wait} for the Request to complete.
        *
        * @author jh  2007.12.13
        *
        * @see Response
        */
        class COH_EXPORT Status
            : public interface_spec<Status>
            {
            // ----- Status interface -----------------------------------

            public:
                /**
                * Determine if the Request represented by this Status has
                * been completed successfully, completed unsuccessfully, or
                * canceled.
                *
                * @return true if the Request has been completed successfully,
                *         completed unsuccessfully, or canceled; false if the
                *         Request is still pending
                */
                virtual bool isClosed() const = 0;

                /**
                * Cancel the Request represented by this Status.
                * <p/>
                * The requestor can call this method when it is no longer
                * interested in a Response or outcome of the Request.
                */
                virtual void cancel() = 0;

                /**
                * Cancel the Request represented by this Status due to an
                * error condition.
                * <p/>
                * After this method is called, the {@link #getResponse}
                * method will throw this exception (wrapping it if necessary).
                *
                * @param ohe  the reason that the Request is being cancelled
                */
                virtual void cancel(Exception::Holder ohe) = 0;

                /**
                * Return the Request represented by this Status.
                *
                * @return the Request
                */
                virtual Request::Handle getRequest() = 0;

                /**
                * Return the Request represented by this Status.
                *
                * @return the Request
                */
                virtual Request::View getRequest() const = 0;

                /**
                * Return the Response sent by the peer.
                * <p/>
                * This method will only return a non-null value if
                * {@link #isClosed()} is true.
                *
                * @return the Response
                *
                * @throws Exception if the Request is cancelled
                */
                virtual Response::Handle getResponse() = 0;

                /**
                * Return the Response sent by the peer.
                * <p/>
                * This method will only return a non-null value if
                * {@link #isClosed()} is true.
                *
                * @return the Response
                *
                * @throws Exception if the Request is cancelled
                */
                virtual Response::View getResponse() const = 0;

                /**
                * Block the calling thread until the Request is completed
                * successfully, completed unsuccessfully, canceled, or a
                * timeout occurs.
                *
                * @param cMillis  the number of milliseconds to wait for the
                *                 result of the Request; pass zero to block
                *                 the calling thread indefinitely
                *
                * @return the Response
                *
                * @throws Exception if the Request is cancelled, a timeout
                *         occurs, or the waiting thread is interrupted
                */
                virtual Response::Handle waitForResponse(int64_t cMillis) = 0;

                /**
                * Block the calling thread until the Request is completed
                * successfully, completed unsuccessfully, or canceled.
                *
                * @return the Response
                *
                * @throws Exception if the Request is cancelled, or the 
                *         waiting thread is interrupted
                */
                virtual Response::Handle waitForResponse() = 0;
            };

        /**
        * Return the Status for this Request that can be used to wait for and
        * retrieve the Response.
        *
        * @return the Status or null if the Status hasn't been initialized
        */
        virtual Status::Handle getStatus() = 0;

        /**
        * Return the Status for this Request that can be used to wait for and
        * retrieve the Response.
        *
        * @return the Status or null if the Status hasn't been initialized
        */
        virtual Status::View getStatus() const = 0;

        /**
        * Configure the Status for this Request.
        *
        * @param status  the new Status
        *
        * @throws IllegalStateException if the Status has already been
        *                               configured
        */
        virtual void setStatus(Status::Handle hStatus) = 0;

        /**
        * Obtain the maximum amount of time a calling thread is willing to
        * wait for a result of the request execution. The request time is
        * measured on the client side as the time elapsed from the moment a
        * request is sent for execution to the corresponding server node(s)
        * and includes:
        * <ul>
        * <li> the time it takes to deliver the request to the executing
        *      node(s);
        * <li> the interval between the time the task is received and placed
        *      into a service queue until the execution starts;
        * <li> the task execution time;
        * <li> the time it takes to deliver a result back to the client.
        * </ul>
        *
        * The value of timeout_default  indicates a default timeout value
        * configured for the corresponding service; the
        * value of #timeout_none timeout_none indicates that the client
        * thread is willing to wait indefinitely until the task execution
        * completes or is canceled by the service due to a task execution
        * timeout specified by the #getExecutionTimeoutMillis() value.
        *
        * If the specified amount of time elapsed and the client has not
        * received any response from the server, an
        * RequestTimeoutException will be thrown to the caller.
        *
        * @return the request timeout value in milliseconds or one of the
        *          special PriorityTask::TIMEOUT_* values
        */
        virtual int64_t getRequestTimeoutMillis() const = 0;
        // getRequestTimeoutMillis is pulled from PriortyTask to avoid the
        // need for an expensive cast in PofChannel::calculateRequestTimeout
    };

COH_CLOSE_NAMESPACE3

#endif // COH_REQUEST_HPP
