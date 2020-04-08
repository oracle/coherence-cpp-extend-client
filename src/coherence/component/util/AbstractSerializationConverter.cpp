/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/AbstractSerializationConverter.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

// ----- constructors -------------------------------------------------------

AbstractSerializationConverter::AbstractSerializationConverter()
    : f_vSerializer(self())
    {
    }

// ----- accessors ----------------------------------------------------------

Serializer::View AbstractSerializationConverter::getSerializer() const
    {
    return f_vSerializer;
    }

void AbstractSerializationConverter::setSerializer(Serializer::View vSerializer)
    {
    initialize(f_vSerializer, vSerializer);
    }

COH_CLOSE_NAMESPACE3
