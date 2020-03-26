/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONNECTION_EXCEPTION_HPP
#define COH_CONNECTION_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PortableException.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::io::pof::PortableException;

/**
* Signals that an underlying communication channel used by a Connection may
* have been closed, severed, or become unusable.
*
* After this exception is thrown, any attempt to use the Connection (or any
* Channel created by the Connection) may result in an exception.
*
* @author jh  2007.12.19
*/
class COH_EXPORT ConnectionException
    : public throwable_spec<ConnectionException,
        extends<PortableException> >
    {
    friend class factory<ConnectionException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a ConnectionException.
        *
        * @param vsMsg        the detail message
        * @param vCause       the underlying cause of the exception
        * @param vConnection  the connection where the exception occurred
        *
        * @return a new ConnectionException
        */
        ConnectionException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL,
                Object::View vConnection = NULL);

    protected:
        /**
        * Blocked copy constructor.
        */
        ConnectionException(const ConnectionException&);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONNECTION_EXCEPTION_HPP
