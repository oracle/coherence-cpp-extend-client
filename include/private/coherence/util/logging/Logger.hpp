/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOGGER_HPP
#define COH_LOGGER_HPP

#include "coherence/lang.ns"

#include "coherence/net/OperationalContext.hpp"

#include "private/coherence/util/Date.hpp"
#include "private/coherence/util/logging/LogOutput.hpp"
#include "private/coherence/component/util/QueueProcessor.hpp"

#include <iostream> // This is for the Logging Macro
#include <sstream>

COH_OPEN_NAMESPACE3(coherence,util,logging)

using coherence::component::util::QueueProcessor;
using coherence::net::OperationalContext;


/**
* The Logger class is used to log messages for a specific system
* or application component.
*
* Each logger instance has an associated logging level. Only Log
* messages that meet or exceed this level are logged. Currently,
* the Logger defines 10 logging levels (from highest to lowest
* level):
* level_internal (All messages without a Log level)
* level_error    (Error messages)
* level_warning  (Warning messages)
* level_d4
* level_d5
* level_d6
* level_d7
* level_d8
* level_d9
*
* Additionally the logger defines two "psuedo" levels that
* instruct the Logger to either log all messages or to not log
* any messages:
*
* level_all
* level_none
*
* Log messages are logged using the log() method. There are
* several versions of the log() method that allow both string
* messages and Exceptions to be logged. The logger uses a string
* template to format the Log message before it is logged using
* the underlying logging mechanism. The template may contain the
* following parameterizable strings:
*
* {date}    The date and time that the message was logged
* {uptime}  The duration the process has been running for
* {level}   The level of the log message
* {thread}  The thread that logged the message
* {text}    The text of the message
* {product} The executing assembly of the product
* {version} The executing assembly version
*
* Subclasses of the Logger are free to define additional
* parameters.
*
* The Logger class uses a LogOutput to log messages to an
* underlying logging mechanism, such as Out, Error, or a file.
* See the configure() method for additional detail
*
*
* @author 2007.11.5 nsa
*/
class COH_EXPORT Logger
    : public class_spec<Logger,
        extends<QueueProcessor> >
    {
    friend class factory<Logger>;

    // ----- log level typedef ----------------------------------------------

    public:
        /**
        * Log level definitions.
        */
        typedef enum
            {
            level_none     = -1,
            level_internal =  0,
            level_error    =  1,
            level_warning  =  2,
            level_info     =  3,
            level_d4       =  4,
            level_d5       =  5,
            level_d6       =  6,
            level_d7       =  7,
            level_d8       =  8,
            level_d9       =  9,
            level_all      =  1000 //Leave room below ALL for additional levels
            } Level_t;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new logger instance.
        */
        Logger();


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Return Logger Singleton object to write messages to the default log
        *
        * @return The default logger.
        */
        static Handle getLogger();

        /**
        * Test if the singleton logger would log a message with the given log level.
        *
        * @param nLevel  the log level to test the logger with
        *
        * @return true iff the logger would log the message
        */
        static bool isEnabledSingleton(int32_t nLevel);


    // ----- Logger interface -----------------------------------------------

    public:
        /**
        * Initialize the logger state.
        */
        virtual void init();

        /**
        * Return the log message format template.
        *
        * @return the log message format template
        */
        virtual String::View getFormat() const;

        /**
        * Set the log message format template
        *
        * @param vsFormat  the String template for a log message
        */
        virtual void setFormat(String::View vsFormat);

        /**
        * Return the current log level of the logger.
        *
        * @return the current log level of the logger
        */
        virtual int32_t getLevel() const;

        /**
        * Set the current log level for the logger.
        *
        * @param nLevel  the log level to set the logger to.
        */
        virtual void setLevel(int32_t nLevel);

        /**
        * Return the logging character limit.
        *
        * @return the logging character limit
        */
        virtual int32_t getLimit() const;

        /**
        * Set the logging character limit.
        *
        * @param cChar  the count of the number of characters
        */
        virtual void setLimit(int32_t cChar);

        /**
        * Return set of paramaterizable strings that may appear in a formatted
        * log message.
        *
        * @return  the parameterizable strings that may appear in a  formatted
        *          log message.
        */
        virtual ObjectArray::View getParameters() const;

        /**
        * Set the paramaterizable strings that may appear in a formatted log
        * message.
        *
        * @param vasParameters  the paramaterizable strings that may appear
        *                       in a formatted log message
        */
        virtual void setParameters(ObjectArray::View vasParameters);

        /**
        * Return the logging destination of this logger. The destination can be
        * 'Error', 'Out', or a file name.
        *
        * @return The logging destination of this logger
        */
        virtual String::View getDestination() const;

        /**
        * Set the logging destination of this logger. The destination can be
        * Error, Out, or a file name.
        *
        * @param sDestination  the destination this logger should write to
        */
        virtual void setDestination(String::View sDestination);

        /**
        * Return the assembly product.
        *
        * @return the assembly product
        */
        virtual String::View getProduct() const;

        /**
        * Set the assembly product.
        *
        * @param vsProduct  the assembly product to set.
        */
        virtual void setProduct(String::View vsProduct);

        /**
        * Return the assembly version.
        *
        * @return the assembly version
        */
        virtual String::View getVersion() const;

        /**
        * Set the assembly version.
        *
        * @param vsVersion  the assembly version to set.
        */
        virtual void setVersion(String::View vsVersion);

        /**
        * Return the product edition.
        *
        * @return the product edition
        */
        virtual String::View getEdition() const;

        /**
        * Set the product edition.
        *
        * @param vsEdition  the product edition
        */
        virtual void setEdition(String::View vsEdition);

        /**
        * Test if the logger would log a message with the given log level.
        *
        * @param nLevel  the log level to test the logger with
        *
        * @return true iff the logger would log the message
        */
        virtual bool isEnabled(int32_t nLevel) const;

        /**
        * Configure a newly created Logger.
        *
        * @param vContext  the OperationalContext for this Logger
        */
        virtual void configure(OperationalContext::View vContext);

        /**
        * Format the given log message into a string by replacing the tokens in
        * the message format string with the values contained in the given
        * message.
        *
        * Log messages must be in the form of an ObjectArray with the following
        * structure:
        * {
        * [TimeStamp], [UpTime], [Level], [Thread], [Throwable], [Message],
        * [Parameter Value]*
        * }
        *
        * @param vaoMessage  the message to format
        *
        * @return The formatted message
        */
        virtual String::View formatMessage(ObjectArray::View vaoMessage) const;

        /**
        * Log the given message with the specified log level.
        *
        * @param nLevel      the log level of the message
        * @param vsMessage    the message to write to the log
        * @param vaoParameters  the extra parameters for message formatting
        */
        virtual void log(int32_t nLevel, String::View vsMessage,
                         ObjectArray::View vaoParams);


        /**
        * Log the given exception with the specified log level.
        *
        * @param nLevel     the log level of the message
        * @param vEx        the exception to write to the log
        * @param vaoParams  the extra parameters for message formatting
        */
        virtual void log(int32_t nLevel, Exception::View vEx,
                         ObjectArray::View vaoParams);

        /**
        * Log the given exception and message with the specified log level.
        *
        * @param nLevel         the log level of the message
        * @param vEx            the exception to write to the log
        * @param vsMessage      the message to write to the log
        * @param vaoParameters  the extra parameters for message formatting
        */
        virtual void log(int32_t nLevel, Exception::View vEx,
                         String::View vsMessage, ObjectArray::View vaoParams);

        /**
        * Stop the logger and release any resources held by the logger.
        *
        * This method has no effect if the Logger has already been stopped.
        * Stopping the logger makes it unusable. Any attempt to use a stopped
        * Logger may result in an exception.
        */
        virtual void shutdown();

        /**
        * Return the current LogOutput of this logger.
        *
        * @return the current LogOutput of this logger
        */
        virtual LogOutput::Handle getLogOutput();

        /**
        * Set the LogOutput of this logger.
        *
        * @param hLogOutput  the LogOutput this logger should use
        */
        virtual void setLogOutput(LogOutput::Handle hLogOutput);

        /**
        * Return the default parameters.
        *
        * @return the default parameterizable strings that may appear in the
        *         formatted log messages.
        */
        static ObjectArray::View getDefaultParameters();

        /**
        * Return the default destination for loggers.
        *
        * @return the default destination for loggers
        */
        static String::View getDefaultDestination();

        /**
        * Return the default format for a logger.
        *
        * @return the default log format
        */
        static String::View getDefaultFormat();

        /**
        * Return the default log level.
        *
        * @return the default log level
        */
        static int32_t getDefaultLevel();

        /**
        * Return the default character limit.
        *
        * @return The default character limit
        */
        static int32_t getDefaultLimit();

    protected:
        /**
        * Create a new Log message. A log message is an
        * ObjectArray with the following structure:
        * {
        * [TimeStamp], [UpTime], [Level], [Thread], [Throwable], [Message],
        * [Parameter Value] }
        *
        * @param nLevel     the log level of the message
        * @param vEx        the Exception to log
        * @param vsMessage  the message to log
        * @param vaoParams  an array of parameter values to be used when
        *                   formatting the message
        *
        * @return The array of objects representing a log message.
        */
        virtual ObjectArray::Handle createMessage(int32_t nLevel,
                Exception::View vEx, String::View vsMessage,
                ObjectArray::View vaoParams) const;

        /**
        * Format a given parameter with the given name for output to the
        * underlying logger.
        *
        * @param vsName  the name of the parameter
        * @param vValue  the value of the parameter
        *
        * @return the string representation of the formatted parameter
        */
        virtual String::View formatParameter(String::View vsName,
                Object::View vValue) const;

        /**
        * Called immediately before a Log message is logged to the underlying
        * LogOutput.
        */
        virtual void onLog();


    // ----- Daemon interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onNotify();

        /**
        * {@inheritDoc}
        */
        virtual void onExit();


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The logging destination.
        */
        MemberView<String> m_vsDestination;

        /**
        * The Log message format.
        */
        MemberView<String> m_vsFormat;

        /**
        * The logging level.
        */
        int32_t m_nLogLevel;

        /**
        *  Time pattern to use when printing log messages.
        */
        MemberView<String> m_vsTimePattern;

        /**
        * The logging character limit.
        */
        int32_t m_cCharLimit;

        /**
        * The LogOutput used to log out formatted messages.
        */
        MemberHandle<LogOutput> m_hLogOutput;

        /**
        * The set of paramaterizable strings that may appear in a formatted log
        * message.
        */
        MemberView<ObjectArray> m_vasParameters;

        /**
        * The assembly product.
        */
        MemberView<String> m_vsProduct;

        /**
        * The assembly version.
        */
        MemberView<String> m_vsVersion;

        /**
        * The product edition.
        */
        MemberView<String> m_vsEdition;
    };

COH_CLOSE_NAMESPACE3


// ----- logger macros ------------------------------------------------------

// The following are burned in a build time by ant, these definitions are
// added only for the dev team so that we may build within IDEs without
// having to redefine them
#ifndef COH_PRODUCT
# define COH_PRODUCT Oracle Coherence for C++
#endif

#ifndef COH_VERSION
#  define COH_VERSION internal // when set by ant this will be version/build
#endif

#ifndef COH_BUILD_TYPE
#  define COH_BUILD_TYPE dev // when set by ant this will be debug/release
#endif

#ifndef COH_BUILD_INFO
#  define COH_BUILD_INFO 0 // when set by ant this will be the build number
#endif

#ifndef COH_COPYRIGHT
// unoffical copyright for internal dev builds
#  define COH_COPYRIGHT "Copyright (c) Oracle. For internal use only."
#endif

/**
* @hideinitializer
*
* Log a message to the Coherence logger at a given log level.
*
* @param CONTENTS  the streamable contents to write to the stream
* @param LEVEL     the level at which to log the message
*/
#define COH_LOG(CONTENTS, LEVEL) \
    COH_LOGEXMSG((coherence::lang::Exception::View) NULL, CONTENTS, LEVEL)

/**
* @hideinitializer
*
* Log an exception to the Coherence logger at a given log level.
*
* @param EX     the Exception::View to log to the message
* @param LEVEL  the level at which to log the message
*/
#define COH_LOGEX(EX, LEVEL) COH_LOGEXMSG(EX, (String::View) String::null_string, LEVEL)

/**
* @hideinitializer
*
* Log a message and an exception to the Coherence logger at a given log level.
*
* @param EX        the Exception::View to log to the message
* @param CONTENTS  the streamable contents to write to the stream
* @param LEVEL     the level at which to log the message
*/
//TODO: Race condition checking logger state
//TODO: Use logger message formatting
#define COH_LOGEXMSG(EX, CONTENTS, LEVEL) \
    { \
    int32_t _nLevel = LEVEL; \
    if ((_nLevel > -4 /*D4*/ && _nLevel < 0) || \
        (coherence::util::logging::Logger::isEnabledSingleton(_nLevel))) \
        { \
        coherence::util::logging::Logger::Handle _hLog = \
                coherence::util::logging::Logger::getLogger(); \
        bool fLogger   = NULL != _hLog && _hLog->isStarted(); \
        if (_nLevel >= 0 && fLogger) \
            { \
            _hLog->log(_nLevel, EX, \
                    COH_TO_STRING(CONTENTS), NULL); \
            } \
        else \
            { \
            _nLevel = _nLevel < 0 ? -_nLevel : _nLevel; \
            std::stringstream ssAtomic; \
            if (_nLevel != 0) \
                { \
                ssAtomic << coherence::util::Date::create() << ' ' \
                     << COH_SYMB_TO_STRING(COH_PRODUCT) \
                     << ' ' << COH_SYMB_TO_STRING(COH_VERSION) \
                     << (_nLevel >= 4 ? " <D"       : \
                         _nLevel == 3 ? " <Info"    : \
                         _nLevel == 2 ? " <Warning" : \
                                        " <Error")    \
                     << (_nLevel >= 4 ? # LEVEL : "") \
                     << "> (thread=" << Thread::currentThread()->getName() << "): "; \
                } \
            ssAtomic << CONTENTS \
             << (NULL == EX ? "" \
               : (EX)->getDescription()->getCString())  \
             << std::ends; \
            std::cerr << ssAtomic.str() << std::endl; \
            } \
        } \
    }

/**
* @hideinitializer
*
* Return whether a specific log level is enabled or not
*
* @param LEVEL  the log level to test
*
* @return true if the specified level is true
*/
#define COH_LOGENABLED(LEVEL) \
        coherence::util::logging::Logger::isEnabledSingleton(LEVEL)

#endif // COH_LOGGER_HPP
