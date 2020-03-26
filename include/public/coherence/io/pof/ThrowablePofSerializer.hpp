/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_THROWABLE_POF_SERIALIZER_HPP
#define COH_THROWABLE_POF_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Collection.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::Collection;


/**
* PofSerializer implementation that can serialize and deserialize an
* Exception to/from a POF stream.
*
* This serializer is provides a catch-all mechanism for serializing exceptions.
* Any deserialized exception will loose type information, and simply be
* represented as a PortableException.  The basic detail information of the
* exception are retained.
*
* PortableException and this class work asymmetricly to provide the
* serialization routines for exceptions.
*
* @author mf  2008.08.25
*/
class COH_EXPORT ThrowablePofSerializer
    : public class_spec<ThrowablePofSerializer,
        extends<Object>,
        implements<PofSerializer> >
    {
    friend class factory<ThrowablePofSerializer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        ThrowablePofSerializer();


    // ----- PofSerializer interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void serialize(PofWriter::Handle hOut, Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder deserialize(PofReader::Handle hIn) const;


    // ----- helpers --------------------------------------------------------

    public:
        /**
        * Write the Exception to the specified stream.
        *
        * @param hOut  the stream to write to
        * @param ve    the Exception to write
        */
        static void writeThrowable(PofWriter::Handle hOut, Exception::View ve);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_THROWABLE_POF_SERIALIZER_HPP
