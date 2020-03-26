/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVENT_DEATH_EXCEPTION_HPP
#define COH_EVENT_DEATH_EXCEPTION_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,component,run)


/**
* The EventDeathException is used to prevent delivering an event to
* registered listeners.
*
* @author nsa 01.17.08
*/
class COH_EXPORT EventDeathException
    : public throwable_spec<EventDeathException,
        extends<Exception> >
    {
    friend class factory<EventDeathException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an EventDeathException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        EventDeathException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_EVENT_DEATH_EXCEPTION_HPP
