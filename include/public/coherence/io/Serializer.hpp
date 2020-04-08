/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERIALIZER_HPP
#define COH_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


/**
* The Serializer interface provides the capability of reading and writing an
* object from and to an in-memory buffer.
*
* @author jh  2007.12.20
*
* @see ReadBuffer
* @see WriteBuffer
*/
class COH_EXPORT Serializer
    : public interface_spec<Serializer>
    {
    // ----- Serializer interface -------------------------------------------

    public:
        /**
        * Serialize an object to a WriteBuffer by writing its state using the
        * specified BufferOutput object.
        *
        * @param hOut  the BufferOutput with which to write the object's
        *              state
        * @param v    the object to serialize
        *
        *        virtual void IOException  if an I/O error occurs
        */
        virtual void serialize(WriteBuffer::BufferOutput::Handle hOut,
                Object::View v) const = 0;

        /**
        * Deserialize an object from a ReadBuffer by reading its state using
        * the specified BufferInput object.
        *
        * @param hIn  the BufferInput with which to read the object's state
        *
        * @return the deserialized user type instance
        *
        *        virtual void IOException  if an I/O error occurs
        */
        virtual Object::Holder deserialize(
                ReadBuffer::BufferInput::Handle hIn) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERIALIZER_HPP
