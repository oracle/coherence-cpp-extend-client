/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RESPONSE_HPP
#define COH_RESPONSE_HPP

#include "coherence/lang.ns"

#include "private/coherence/net/messaging/Message.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)


/**
* Response is the root interface for all response messages sent by peer
* endpoints through a {@link Channel}.
* <p/>
* Response objects are created by a {@link Protocol.MessageFactory} and are
* associated with a corresponding Request. The status of the Response may be
* determined by calling the {@link #isFailure()} method:
* <ul>
*   <li>if false, the Request was successfully processed by the receiver and
*       the Response object contains a valid result</li>
*   <li>if true, an error or exception occured while processing the
*       Request</li>
* </ul>
* <p/>
*
* @author jh  2007.12.20
*
* @see Protocol::MessageFactory
*/
class COH_EXPORT Response
    : public interface_spec<Response,
        implements<Message> >
    {
    // ----- Response interface ---------------------------------------------

    public:
        /**
        * Return the unique identifier of the Request for which this Response
        * is being sent.
        *
        * @return the unique identifier of the Request associated with this
        *         Response
        */
        virtual int64_t getRequestId() const = 0;

        /**
        * Associate a Request with this Response.
        *
        * @param lId  the unique identifier of the Request associated with
        *             this Response
        */
        virtual void setRequestId(int64_t lId) = 0;

        /**
        * Determine if an error or exception occured while processing the
        * Request.
        * <p/>
        * If this method returns false, the result of processing the Request
        * can be determined using the {@link #getResult()} method. If this
        * method returns true, {@link #getResult()} may return the cause of
        * the failure (in the form of an Error or Exception object).
        *
        * @return false if the Request was processed successfully; true if an
        *         error or exception occured while processing the Request
        */
        virtual bool isFailure() const = 0;

        /**
        * Set the error state of this Response.
        *
        * @param fFailure  if true, an error or exception occured while
        *                  processing the Request associated with this
        *                  Response
        */
        virtual void setFailure(bool fFailure) = 0;

        /**
        * Return the result of processing the Request.
        *
        * @return the result of processing the Request associated with this
        *         Response
        */
        virtual Object::Holder getResult() const = 0;

        /**
        * Set the result of processing the Request.
        *
        * @param ohResult  the result of processing the Request
        */
        virtual void setResult(Object::Holder ohResult) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RESPONSE_HPP
