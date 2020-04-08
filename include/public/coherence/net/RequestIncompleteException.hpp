/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REQUEST_INCOMPLETE_EXCEPTION_HPP
#define COH_REQUEST_INCOMPLETE_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableException.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableException;


/**
* Signals that a request execution in a clustered environment failed to
* complete successfully. For some specific requests this exception could
* carry a partial execution result or failure information.
*
* @author bbc  2013.05.16
* @since Coherence 12.1.3 
*/
class COH_EXPORT RequestIncompleteException
    : public throwable_spec<RequestIncompleteException,
        extends<PortableException> >
    {
    friend class factory<RequestIncompleteException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a RequestIncompleteException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        RequestIncompleteException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);

        /**
        * Blocked copy constructor.
        */
        RequestIncompleteException(const RequestIncompleteException&);


    // ----- RequestIncompleteException interface ------------------------------

    public:
        /**
        * Return a partial execution result that may have been assembled
        * prior to the timeout. The result type is usually the same as the
        * type of the return value for the corresponding request.
        *
        * @return a partial execution result
        */
        virtual Object::Holder getPartialResult() const;

        /**
        * Specify a partial execution result.
        *
        * @param ohPartialResult  a partial execution result
        */
        virtual void setPartialResult(Object::Holder ohPartialResult);


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Partial execution result (optional).
        */
        FinalHolder<Object> f_ohPartialResult;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_REQUEST_INCOMPLETE_EXCEPTION_HPP
