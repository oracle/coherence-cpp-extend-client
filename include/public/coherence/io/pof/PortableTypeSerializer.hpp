/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_TYPE_SERIALIZER_HPP
#define COH_PORTABLE_TYPE_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

/**
 * A PofSerializer implementation that serializes classes that implement
 * PortableObject interface (and optionally EvolvableObject interface).
 *
 * Unlike legacy PortableObjectSerializer, this class serializes attributes
 * of each class in the object's hierarchy into a separate nested POF stream,
 * which allows for independent evolution of each class in the hierarchy, as well
 * as the evolution of the hierarchy itself (addition of new classes at any level
 * in the hierarchy).
 *
 * @author as  2013.05.01

 * @since  12.1.3
 */
class COH_EXPORT PortableTypeSerializer
    : public class_spec<PortableTypeSerializer,
        extends<Object>,
        implements<PofSerializer> >
    {
    friend class factory<PortableTypeSerializer>;

    // ---- constructors ----------------------------------------------------

    protected:
        /**
         * Create a new PortableTypeSerializer for the user type with the given type
         * identifier.
         *
         * @param nTypeId  the type identifier of the user type to serialize and deserialize
         */
        PortableTypeSerializer(int32_t nTypeId);

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

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The type identifier of the user type to serialize and deserialize.
        */
        int32_t m_nTypeId;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PORTABLE_TYPE_SERIALIZER_HPP
