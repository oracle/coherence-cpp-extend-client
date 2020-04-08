/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/transformer/SemiLiteEventTransformer.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ConverterCollections.hpp"

COH_OPEN_NAMESPACE3(coherence,util,transformer)

COH_REGISTER_PORTABLE_CLASS(120, SemiLiteEventTransformer);


// ----- constructors -------------------------------------------------------

SemiLiteEventTransformer::SemiLiteEventTransformer()
    {
    }


// ----- MapEventTransformer interface --------------------------------------

MapEvent::Handle
    SemiLiteEventTransformer::transform(MapEvent::Handle hEvent) const
    {
    ConverterCollections::ConverterMapEvent::Handle hCEvent =
        cast<ConverterCollections::ConverterMapEvent::Handle>(hEvent,false);
    if (NULL != hCEvent)
        {
        hCEvent->setOldValue(NULL);
        return hCEvent;
        }
    return MapEvent::create(hEvent->getMap(), hEvent->getId(),
            hEvent->getKey(), (Object::View) NULL, hEvent->getNewValue());
    }


// ----- PortableObject interface -------------------------------------------

void SemiLiteEventTransformer::readExternal(PofReader::Handle /*hIn*/)
    {
    }

void SemiLiteEventTransformer:: writeExternal(PofWriter::Handle /*hOut*/) const
    {
    }


// ----- Object interface ---------------------------------------------------

bool SemiLiteEventTransformer::equals(Object::View v) const
    {
    return instanceof<SemiLiteEventTransformer::View>(v);
    }

size32_t SemiLiteEventTransformer::hashCode() const
    {
    return 79;
    }

TypedHandle<const String> SemiLiteEventTransformer::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) << '@' << hashCode());
    }


// ---- constants -----------------------------------------------------------

SemiLiteEventTransformer::Handle SemiLiteEventTransformer::getInstance()
    {
    static FinalHandle<SemiLiteEventTransformer> hSLET(
            System::common(), SemiLiteEventTransformer::create());
    return hSLET;
    }
COH_STATIC_INIT(SemiLiteEventTransformer::getInstance());

COH_CLOSE_NAMESPACE3
