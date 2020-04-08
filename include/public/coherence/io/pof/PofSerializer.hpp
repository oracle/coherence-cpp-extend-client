/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_SERIALIZER_HPP
#define COH_POF_SERIALIZER_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,io,pof)

class PofReader;
class PofWriter;


/**
* The PofSerializer interface provides the capability of reading and writing
* an object from and to a Portable Object Format (POF) stream.
*
* @author jh  2008.01.14
*/
class COH_EXPORT PofSerializer
    : public interface_spec<PofSerializer>
    {
    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * PofReader Handle definition.
        */
        typedef TypedHandle<PofReader> PofReaderHandle;

        /**
        * PofWriter Handle definition.
        */
        typedef TypedHandle<PofWriter> PofWriterHandle;


    // ----- PofSerializer interface ----------------------------------------

    public:
        /**
        * Serialize a user type instance to a POF stream by writing its state
        * using the specified PofWriter.
        *
        * An implementation of PofSerializer is required to follow the
        * following steps in sequence for writing out an Object of a user
        * type:
        * <ol>
        * <li>If the object is Evolvable, the implementation must set the
        *     version by calling PofWriter::setVersionId().</li>
        * <li>The implementation may write any combination of the properties
        *     of the user type by using the "write" methods of the PofWriter,
        *     but it must do so in the order of the property indexes.</li>
        * <li>After all desired properties of the user type have been
        *     written, the implementation must terminate the writing of
        *     the user type by calling PofWriter::writeRemainder(). </li>
        * </ol>
        *
        * @param hOut  a PofWriter to use for writing the object's state
        * @param v     an object to serialize
        */
        virtual void serialize(PofWriterHandle hOut, Object::View v) const = 0;

        /**
        * Deserialize a user type instance from a POF stream by reading its
        * state using the specified PofReader.
        *
        * An implementation of PofSerializer is required to follow the
        * following steps in sequence for reading in an object of a user
        * type:
        * <ol>
        * <li>If the object is Evolvable, the implementation must get the
        *     version by calling PofWriter::getVersionId().</li>
        * <li>The implementation may read any combination of the properties
        *     of the user type by using "read" methods of the PofReader, but
        *     it must do so in the order of the property indexes.</li>
        * <li>After all desired properties of the user type have been read,
        *     the implementation must terminate the reading of the user type
        *     by calling PofReader::readRemainder().</li>
        * </ol>
        *
        * @param hIn a PofReader to use for reading the Object's state
        *
        * @return the deserialized user type instance
        */
        virtual Object::Holder deserialize(PofReaderHandle hIn) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_SERIALIZER_HPP
