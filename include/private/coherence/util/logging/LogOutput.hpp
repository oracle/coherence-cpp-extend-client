/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOG_OUTPUT_HPP
#define COH_LOG_OUTPUT_HPP

#include "coherence/lang.ns"

#include "coherence/net/OperationalContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,logging)

using coherence::net::OperationalContext;


/**
* Abstract component used to log formatted messages to an underlying logging
* mechanism.
*
* A concrete LogOutput must be able to translate between internal
* Logger::LogLevel levels and the equivalent Log level objects appropriate
* for the underlying logging mechanism. See the LogOutput::TranslateLevel
* method for additional details.
*
* @author 2007.11.07 nsa
*/
class COH_EXPORT LogOutput
    : public abstract_spec<LogOutput>
    {
    // ----- LogOutput interface --------------------------------------------

    public:
        /**
        * Configure a newly created LogOutput instance.
        *
        * @param vContext  the OperationalContext for this LogOutput
        */
        virtual void configure(OperationalContext::View vContext) = 0;

        /**
        * Log the exception and message with the specified log level
        *
        * @param nLevel     the log level of the message
        * @param vsMessage  the message to log
        * @param vEx        the exception to log
        */
        virtual void log(int32_t nLevel, Exception::View vEx,
                         String::View vsMessage);

        /**
        * Log the message with the specified log level
        *
        * @param nLevel     the LogLevel the message is to be logged at
        * @param vsMessage  the message to log
        */
        virtual void log(int32_t nLevel, String::View vsMessage);

        /**
        * Log the given exception and with the specified log level
        *
        * @param nLevel  the LogLevel the message is to be logged at
        * @param vEx    the exception to log
        */
        virtual void log(int32_t nLevel, Exception::View vEx);

        /**
        * Close the LogOutput and release any resources held by the
        * LogOutput. This method has no effect if the LogOutput has already
        * been closed. Closing a LogOutput makes it unusable. Any attempt to
        * use a closed LogOutput may result in an exception.
        */
        virtual void close();


    // ----- LogOutput interface --------------------------------------------

    protected:

        /**
        * Log the message and exception at the specified log level.
        *
        * @param nLevel     the output specific log level
        * @param vsMessage  the message to log
        * @param vEx        the exception to log
        */
        virtual void write(Object::View nLevel, Exception::View vEx,
                         String::View message) = 0;

        /**
        * Log the message at the specified log level.
        *
        * @param nLevel     the output specific log level
        * @param vsMessage  the message to log
        */
        virtual void write(Object::View nLevel, String::View message) = 0;

        /**
        * Log exception at the specified log level.
        *
        * @param nLevel  the output specific log level
        * @param vEx     the exception to log
        */
        virtual void write(Object::View nLevel, Exception::View vEx) = 0;

        /**
        * Translate the Logger::LogLevel to the underlying Logger specific
        * level.
        *
        * @param nLevel  the Logger::LogLevel to translate
        *
        * @return the corresponding log level object of the underlying
        *         LogOutput implementation
        */
        virtual Object::View translateLevel(int32_t nLevel) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_LOG_OUTPUT_HPP
