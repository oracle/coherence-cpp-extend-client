/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UTF_DATA_FORMAT_EXCEPTION_HPP
#define COH_UTF_DATA_FORMAT_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


/**
* Signals that a malformed UTF-8 string has been read in a data input stream.
*
* @author jh  2007.12.31
*/
class COH_EXPORT UTFDataFormatException
    : public throwable_spec<UTFDataFormatException,
        extends<IOException> >
    {
    friend class factory<UTFDataFormatException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an UTFDataFormatException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new UTFDataFormatException
        */
        UTFDataFormatException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_UTF_DATA_FORMAT_EXCEPTION_HPP
