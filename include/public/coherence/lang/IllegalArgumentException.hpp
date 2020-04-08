/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ILLEGAL_ARGUMENT_EXCEPTION_HPP
#define COH_ILLEGAL_ARGUMENT_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown to indicate that a function has been passed an illegal or
* inappropriate argument.
*/
class COH_EXPORT IllegalArgumentException
    : public throwable_spec<IllegalArgumentException,
        extends<RuntimeException, std::invalid_argument> >
    {
    friend class factory<IllegalArgumentException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an IllegalArgumentException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new IllegalArgumentException
        */
        IllegalArgumentException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };


// ------ helper macros -----------------------------------------------------

/**
* Ensure the relation between two expressions.
*
* @param PARAM  the parameter expression
* @param OP     the expected relation, i.e. ==, <, >, ...
* @param VALUE  the right expression
*
* @throw IllegalArgumentException if an unexpected result is returned
*/
#define COH_ENSURE_PARAM_RELATION(PARAM, OP, VALUE) \
    if (!(PARAM OP VALUE)) \
        { \
        COH_THROW_STREAM (coherence::lang::IllegalArgumentException, \
            #PARAM << " is " << PARAM << "; must be " << #OP << " " << #VALUE); \
        }

/**
* Ensure that a handle/view is not NULL.
*
* @param HANDLE  the handle/view to check for NULL
*
* @throw IllegalArgumentException if the passed in handle/view is NULL
*/
#define COH_ENSURE_PARAM(HANDLE) COH_ENSURE_PARAM_RELATION(HANDLE, !=, NULL)

COH_CLOSE_NAMESPACE2

#endif // COH_ILLEGAL_ARGUMENT_EXCEPTION_HPP
