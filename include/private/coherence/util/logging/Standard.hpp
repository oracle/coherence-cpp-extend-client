/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_STANDARD_HPP
#define COH_STANDARD_HPP

#include "coherence/lang.ns"

#include "coherence/net/OperationalContext.hpp"

#include "private/coherence/util/logging/LogOutput.hpp"



COH_OPEN_NAMESPACE3(coherence,util,logging)

using coherence::net::OperationalContext;


/**
* Concrete extension that logs messages to either stderr, stdout, or a file.
*
* The Standard Logger will use the following configuration parameters:
*
* destination: Specifies the output device used by the logging system; can be
* one of Error, Out, or a file name
*
* @see The coherence.xsd for additional documentation for each of these
*      parameters.
*/
class COH_EXPORT Standard
    : public class_spec<Standard,
        extends<LogOutput> >
    {
    friend class factory<Standard>;

    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new Standard LogOutput instance.
        */
        Standard();

        /**
        * Destructor.
        */
        ~Standard();


    // ----- LogOutput interface --------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void configure(OperationalContext::View vContext);

        /**
        * @inheritDoc
        */
        virtual void close();

    protected:
        /**
        * @inheritDoc
        */
        virtual void write(Object::View voLevel, Exception::View vEx,
                         String::View vsMessage);

        /**
        * @inheritDoc
        */
        virtual void write(Object::View voLevel, String::View vsMessage);

        /**
        * @inheritDoc
        */
        virtual void write(Object::View voLevel, Exception::View vEx);

        /**
         * @inheritDoc
         */
        virtual Object::View translateLevel(int32_t nLevel) const;


    // ----- test interface (not thread safe) -------------------------------

    public:
        /**
        * Return the output stream of this Standard Logger.
        *
        * Note: This is not thread safe.
        *
        * @return the output stream reference
        */
        virtual std::ostream* getOutputStream();

        /**
        * set the output stream of this Standard Logger
        *
        * Note: This is not thread safe.
        *
        * @param stream The output stream to set for this logger
        */
        virtual void setOutputStream(std::ostream* stream);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The output stream for this LogOutput
        */
        std::ostream* m_ostream;
    };

COH_CLOSE_NAMESPACE3

#endif //ifndef COH_STANDARD_HPP
