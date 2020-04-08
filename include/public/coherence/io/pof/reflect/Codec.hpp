/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_REFLECT_CODEC_HPP
#define COH_POF_REFLECT_CODEC_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

COH_OPEN_NAMESPACE4(coherence,io,pof,reflect)

/**
* A Codec provides an interception point for any specific code that needs to
* be executed pre or post (de)serialization. In the case of deserialization
* this could be to return a concrete implementation and with serialization
* this could be to explicitly call a specific method on PofWriter
* that is not carried out by PofWriter::writeObject.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT Codec
    : public interface_spec<Codec>
    {
    public:
        /**
        * Deserialize an object from the provided PofReader. Implementing
        * this interface allows introducing specific return implementations.
        *
        * @param hIn    the PofReader to read from
        * @param nIndex the index of the POF property to deserialize
        *
        * @return a specific implementation of the POF property
        *
        * @throws IOException if an I/O error occurs
        */
        virtual Object::Holder decode(PofReader::Handle hIn, int32_t nIndex) const = 0;

        /**
        * Serialize an object using the provided PofWriter.
        *
        * @param hOut   the PofWriter to read from
        * @param nIndex the index of the POF property to serialize
        * @param v      the value to serialize
        *
        * @throws IOException if an I/O error occurs
        */
        virtual void encode(PofWriter::Handle hOut, int32_t nIndex, Object::View v) const = 0;
    };

COH_CLOSE_NAMESPACE4

#endif /* COH_POF_REFLECT_CODEC_HPP */
