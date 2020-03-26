/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYSTEM_POF_CONTEXT_SERIALIZER_FACTORY_HPP
#define COH_SYSTEM_POF_CONTEXT_SERIALIZER_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/io/SerializerFactory.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::SerializerFactory;


/**
* A SerializerFactory for Serializer objects.
*
* @author par  2012.10.03
*
* @since Coherence 12.1.2
*/
class COH_EXPORT SystemPofContextSerializerFactory
    : public class_spec<SystemPofContextSerializerFactory,
          extends<Object>,
          implements<SerializerFactory> >
    {
    friend class factory<SystemPofContextSerializerFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new SystemPofContextSerializerFactory.
        */
        SystemPofContextSerializerFactory();


    // ----- SerializerFactory interface ------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Serializer::View createSerializer(ClassLoader::View vLoader) const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SYSTEM_POF_CONTEXT_SERIALIZER_FACTORY_HPP

