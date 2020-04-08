/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CODECS_HPP
#define COH_CODECS_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/reflect/Codec.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

/**
* Codecs is a container for accessing default Codec implementations.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT Codecs
    : public extends<Object>
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Based on the Class::View provided either instantiate if it
        * is unknown or use a default codec.
        *
        * @param vClz   the class defining the codec to use or the type the
        *               codec should handle
        *
        * @return codec that supports encoding and decoding of objects
        *               of the specified type
        */
        static Codec::View getCodec(Class::View vClz);

    // ----- inner class: AbstractCodec -------------------------------------

    public:
        /**
        * Abstract Codec implementations that encodes objects by simply
        * delegating to PofWriter::writeObject. Generally the default
        * writeObject implementation does not need to be modified as the
        * current accommodation of types and conversion to POF is generally
        * accepted, with the deserialization being more likely to be
        * specific.
        *
        * @author hr 2011.06.29
        *
        * @since 3.7.1
        */
        class COH_EXPORT AbstractCodec
            : public abstract_spec<AbstractCodec,
                extends<Object>,
                implements<Codec> >
            {
            // ----- Codec interface ----------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void encode(PofWriter::Handle hOut, int32_t nIndex, Object::View v) const;
            };

    // ----- inner class: DefaultCodec --------------------------------------

    public:
        /**
        * Implementation of Codec that simply delegates to
        * PofReader::readObject and PofWriter::writeObject to deserialize and
        * serialize an object.
        *
        * @author hr 2011.06.29
        *
        * @since 3.7.1
        */
        class COH_EXPORT DefaultCodec
            : public class_spec<DefaultCodec,
                extends<AbstractCodec> >
            {
            friend class factory<DefaultCodec>;

            // ----- Codec interface ----------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder decode(PofReader::Handle hIn, int32_t nIndex) const;
            };

    // ----- constants ------------------------------------------------------

    public:
        /**
        * Returns a singleton instance of a Codecs::DefaultCodec.
        *
        * @return Codecs::DefaultCodec
        */
        static Codec::View getDefault();
    };

COH_CLOSE_NAMESPACE4

#endif /* COH_CODECS_HPP */
