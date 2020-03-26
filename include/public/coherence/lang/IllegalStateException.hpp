/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ILLEGAL_STATE_EXCEPTION_HPP
#define COH_ILLEGAL_STATE_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Signals that a function has been called at an illegal or inappropriate
* time. In other words, the environment or application is not in an
* appropriate state for the requested operation.
*/
class COH_EXPORT IllegalStateException
    : public throwable_spec<IllegalStateException,
        extends<RuntimeException> >
    {
    friend class factory<IllegalStateException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an IllegalStateException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new IllegalStateException
        */
        IllegalStateException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };


// ----- helper macros ------------------------------------------------------

/**
* Ensure the relation between two expressions.
*
* @param TYPE  the result type of the expressions
* @param EXPL  the left expression
* @param OP    the expected relation, i.e. ==, <, >, ...
* @param EXPR  the right expression
*
* @throw IllegalStateException if an unexpected result is returned
*/
#define COH_ENSURE_RELATION(TYPE, EXPL, OP, EXPR) \
    { \
    TYPE expl = (EXPL); \
    TYPE expr = (EXPR); \
    if (!(expl OP expr)) \
        { \
        COH_THROW_STREAM (coherence::lang::IllegalStateException, \
            "ensuring: '" << #EXPL << ' ' << #OP << ' ' << \
            #EXPR << "'; result " << expl << " not " #OP << \
            ' ' << expr); \
        } \
    }

/**
* Ensure that the expression returns the expected result.
*
* @param EXPRESSION       the expression to evaluate, must return a result
*                         which is castable to size32_t
* @param EXPECTED_RESULT  the expected result, which is castable to size32_t
*
* @throw IllegalStateException if an unexpected result is returned
*/
#define COH_ENSURE_EQUALITY(EXPRESSION, EXPECTED_RESULT) \
    COH_ENSURE_RELATION(size32_t, EXPRESSION, ==, EXPECTED_RESULT)

/**
* Ensure that the expression evaluates to true.
*
* @param EXPRESSION      the expression to evaluate, must return a result
*                        which is castable to size32_t
*
* @throw IllegalStateException if an unexpected result is returned
*/
#define COH_ENSURE(EXPRESSION) \
    COH_ENSURE_RELATION(bool, EXPRESSION, ==, true)

COH_CLOSE_NAMESPACE2

#endif // COH_ILLEGAL_STATE_EXCEPTION_HPP
