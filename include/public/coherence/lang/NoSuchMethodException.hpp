/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NO_SUCH_METHOD_EXCEPTION_HPP
#define COH_NO_SUCH_METHOD_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Exception.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown when a definition for a named Method could not be found.
*
* @author mf  20011.02.25
*/
class COH_EXPORT NoSuchMethodException
    : public throwable_spec<NoSuchMethodException,
        extends<Exception, std::invalid_argument> >
    {
    friend class factory<NoSuchMethodException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an NoSuchMethodException.
        *
        * @param vsMsg   the error description
        * @param vCause  the underlying cause of the exception;
        *                can be <tt>NULL</tt>
        *
        * @return a Handle to the created Exception
        */
        NoSuchMethodException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NO_SUCH_METHOD_EXCEPTION_HPP
