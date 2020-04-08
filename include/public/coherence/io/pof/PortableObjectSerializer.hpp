/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_OBJECT_SERIALIZER_HPP
#define COH_PORTABLE_OBJECT_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* A PofSerializer implementation that supports the serialization and
* deserialization of any class that implements PortableObject to and
* from a POF stream.
*
* @author jh  2008.01.18
*/
class COH_EXPORT PortableObjectSerializer
    : public class_spec<PortableObjectSerializer,
        extends<Object>,
        implements<PofSerializer> >
    {
    friend class factory<PortableObjectSerializer>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PortableObjectSerializer for the user type with the
        * given type identifier.
        *
        * @param nTypeId  the type identifier of a class that needs to be
        *                 serialized or deserialized
        */
        PortableObjectSerializer(int32_t nTypeId);


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


    // ---- data members ----------------------------------------------------

    protected:
        /**
        * POF user type supported by this PortableObjectSerializer.
        */
        int32_t m_nTypeId;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PORTABLE_OBJECT_SERIALIZER_HPP
