/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INTERRUPTED_IO_EXCEPTION_HPP
#define COH_INTERRUPTED_IO_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


/**
* Signals that the current thread has been interrupted while blocked on IO.
*/
class COH_EXPORT InterruptedIOException
    : public throwable_spec<InterruptedIOException,
        extends<IOException> >
    {
    friend class factory<InterruptedIOException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an InterruptedIOException.
        *
        * @param vsMsg             the detail message
        * @param cBytesTransfered  the number of bytes transfered during the
        *                          blocked operation prior to the interrupt
        * @param vCause            the underlying cause of the exception
        *
        * @return a new InterruptedIOException
        */
        InterruptedIOException(String::View vsMsg = String::null_string,
                size32_t cBytesTransfered = 0,
                Exception::View vCause = NULL);

    // ----- InterruptedIOException -----------------------------------------

    public:
        /**
        * Return the number of bytes trasfered during the blocking operation
        * prior to the interrupt.
        */
        virtual size32_t getBytesTransfered() const;


    // ----- data members ---------------------------------------------------

    public:
        /**
        * The number of bytes transfered during the blocking operation prior
        * to the interrupt.
        */
        size32_t m_cBytesTransfered;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INTERRUPTED_IO_EXCEPTION_HPP
