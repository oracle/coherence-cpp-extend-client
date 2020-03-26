/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_NOT_FOUND_EXCEPTION_HPP
#define COH_CLASS_NOT_FOUND_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Exception.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown when a definition for a named Class could be found.
*
* @author mf  2008.04.03
*/
class COH_EXPORT ClassNotFoundException
    : public throwable_spec<ClassNotFoundException,
        extends<Exception, std::bad_typeid> >
    {
    friend class factory<ClassNotFoundException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an ClassNotFoundException.
        *
        * @param vsName  the name of the Class that could not be located
        * @param vCause  the underlying cause of the exception;
        *                can be <tt>NULL</tt>
        *
        * @return a Handle to the created Exception
        */
        ClassNotFoundException(String::View vsName = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CLASS_NOT_FOUND_EXCEPTION_HPP
