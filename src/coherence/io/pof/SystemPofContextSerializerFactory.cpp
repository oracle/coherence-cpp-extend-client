/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/pof/SystemPofContextSerializerFactory.hpp"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


// ----- constructors -------------------------------------------------------

SystemPofContextSerializerFactory::SystemPofContextSerializerFactory()
    {
    }

// ----- SerializerFactory interface ----------------------------------------

Serializer::View SystemPofContextSerializerFactory::createSerializer(ClassLoader::View /* vLoader */) const
    {
    return SystemPofContext::getInstance();
    }

COH_CLOSE_NAMESPACE3

