/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IO_EXCEPTION_HPP
#define COH_IO_EXCEPTION_HPP

#include "coherence/lang.ns"



COH_OPEN_NAMESPACE2(coherence,io)


/**
* Signals that an I/O exception of some sort has occurred. This class is the
* general class of exceptions produced by failed or interrupted I/O
* operations.
*
* @author jh  2007.12.30
*/
class COH_EXPORT IOException
    : public throwable_spec<IOException,
        extends<Exception, std::ios_base::failure> >
    {
    friend class factory<IOException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an IOException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new IOException
        */
        IOException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_IO_EXCEPTION_HPP

