/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERIALIZER_FACTORY_HPP
#define COH_SERIALIZER_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


/**
* A factory for Serializer objects.
*
* @author wl  2011.11.07
*
* @since Coherence 12.1.2
*/
class COH_EXPORT SerializerFactory
    : public interface_spec<SerializerFactory>
    {
    // ----- SerializerFactory interface ------------------------------------

    public:
        /**
        * Create a new Serializer.
        *
        * @param vLoader  the optional ClassLoader with which to configure the
        *                 new Serializer.
        *
        * @return the new Serializer
        */
        virtual Serializer::View createSerializer(ClassLoader::View vLoader) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERIALIZER_FACTORY_HPP
