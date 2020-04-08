/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONST_CAST_EXCEPTION
#define COH_CONST_CAST_EXCEPTION

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/ClassCastException.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown when an attempt is made to cast away constness from a Holder assigned
* from a View.
*
* @author mf  2008.08.06
*/
class COH_EXPORT ConstCastException
    : public throwable_spec<ConstCastException,
        extends<ClassCastException> >
    {
    friend class factory<ConstCastException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an ConstCastException.
        *
        * @param vsMessage  the exception message
        * @param vCause     the underlying cause of the exception;
        *                   can be <tt>NULL</tt>
        */
        ConstCastException(String::View vsMessage = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONST_CAST_EXCEPTION
