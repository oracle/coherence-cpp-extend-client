/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/transformer/ExtractorEventTransformer.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ConverterCollections.hpp"

COH_OPEN_NAMESPACE3(coherence,util,transformer)

COH_REGISTER_PORTABLE_CLASS(121, ExtractorEventTransformer);


// ----- constructors -------------------------------------------------------

ExtractorEventTransformer::ExtractorEventTransformer()
    : f_vExtractorOld(self()), f_vExtractorNew(self())
   {
   }

ExtractorEventTransformer::ExtractorEventTransformer(
        ValueExtractor::View vExtractor)
    : f_vExtractorOld(self(), vExtractor),
      f_vExtractorNew(self(), vExtractor)
    {
    }

ExtractorEventTransformer::ExtractorEventTransformer(
        ValueExtractor::View vExtractorOld,
        ValueExtractor::View vExtractorNew)
    : f_vExtractorOld(self(), vExtractorOld),
      f_vExtractorNew(self(), vExtractorNew)
    {
    }


// ----- MapEventTransformer interface --------------------------------------

MapEvent::Handle
    ExtractorEventTransformer::transform(MapEvent::Handle hEvent) const
    {
    ValueExtractor::View vExtractorOld = f_vExtractorOld;
    ValueExtractor::View vExtractorNew = f_vExtractorNew;

    return MapEvent::create(hEvent->getMap(), hEvent->getId(),
            hEvent->getKey(),
        vExtractorOld == NULL ? NULL : vExtractorOld->extract(
                hEvent->getOldValue()),
        vExtractorNew == NULL ? NULL : vExtractorNew->extract(
                hEvent->getNewValue()));
    }


// ----- PortableObject interface -------------------------------------------

void ExtractorEventTransformer::readExternal(PofReader::Handle hIn)
    {
    bool fSame = hIn->readBoolean(0);

    initialize(f_vExtractorOld, cast<ValueExtractor::View>(hIn->readObject(1)));
    initialize(f_vExtractorNew, fSame ? f_vExtractorOld :
        cast<ValueExtractor::View>(hIn->readObject(2)));
    }

void ExtractorEventTransformer:: writeExternal(PofWriter::Handle hOut) const
    {
    ValueExtractor::View vExtractorOld = f_vExtractorOld;
    ValueExtractor::View vExtractorNew = f_vExtractorNew;

    if (Object::equals(vExtractorOld, vExtractorNew))
        {
        hOut->writeBoolean(0, true);
        hOut->writeObject(1, vExtractorNew);
        }
    else
        {
        hOut->writeBoolean(0, false);
        hOut->writeObject(1, vExtractorOld);
        hOut->writeObject(2, vExtractorNew);
        }
    }


// ----- Object interface ---------------------------------------------------

bool ExtractorEventTransformer::equals(Object::View v) const
    {
    if (instanceof<ExtractorEventTransformer::View>(v))
        {
        ExtractorEventTransformer::View that =
            cast<ExtractorEventTransformer::View>(v);
        return Object::equals(f_vExtractorOld, that->f_vExtractorOld)
            && Object::equals(f_vExtractorNew, that->f_vExtractorNew);
        }
    return false;
    }

size32_t ExtractorEventTransformer::hashCode() const
    {
    return Object::hashCode(f_vExtractorOld) +
           Object::hashCode(f_vExtractorNew);
    }

TypedHandle<const String> ExtractorEventTransformer::toString() const
    {
    ValueExtractor::View vExtractorOld = f_vExtractorOld;
    ValueExtractor::View vExtractorNew = f_vExtractorNew;
    String::View vs = COH_TO_STRING(Class::getClassName(this) << '{');
    if (Object::equals(vExtractorOld, vExtractorNew))
        {
        vs = COH_TO_STRING(vs << "extractors=" << vExtractorOld);
        }
    else
        {
        vs = COH_TO_STRING(vs << "extractor old=" << vExtractorOld
                << ", extractor new=" << vExtractorNew);
        }
    return COH_TO_STRING(vs << '}');
    }


// ----- data member accessors ------------------------------------------

ValueExtractor::View ExtractorEventTransformer::getOldValueExtractor() const
    {
    return f_vExtractorOld;
    }

ValueExtractor::View ExtractorEventTransformer::getNewValueExtractor() const
    {
    return f_vExtractorNew;
    }

COH_CLOSE_NAMESPACE3
