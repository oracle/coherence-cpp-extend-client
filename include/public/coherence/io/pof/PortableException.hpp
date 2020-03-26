/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_EXCEPTION_HPP
#define COH_PORTABLE_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* A PortableException is an exception that allows information about a remote
* exception or error to be serialized and deserialized to/from a POF stream.
*
* @author jh,mf  2008.01.23
*/
class COH_EXPORT PortableException
    : public throwable_spec<PortableException,
        extends<RuntimeException>,
        implements<PortableObject> >
    {
    friend class factory<PortableException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a PortableException from a Throwable object and an
        * additional description.
        *
        * @param vsMessage  the additional description
        * @param ve         the cause exception
        */
        PortableException(String::View vsMessage = String::null_string,
                Exception::View ve = NULL);

    protected:
        /**
        * Copy constructor.
        */
        PortableException(const PortableException&);


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


    // ----- Exception interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getName() const;

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::View getStackTrace() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The exception's name.
        */
        mutable FinalView<String> f_vsName;

        /**
        * A raw representaion of the remote stack trace for this exception.
        */
        FinalView<ObjectArray> f_vaStackRemote;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PORTABLE_EXCEPTION_HPP
