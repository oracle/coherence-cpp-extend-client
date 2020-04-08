/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/MapEventTransformerFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(88, MapEventTransformerFilter);


// ----- constructors -------------------------------------------------------

MapEventTransformerFilter::MapEventTransformerFilter()
        : f_vFilter(self()), f_vTransformer(self())
    {
    }

MapEventTransformerFilter::MapEventTransformerFilter(
            Filter::View vFilter, MapEventTransformer::View vTransformer)
        : f_vFilter(self(), vFilter), f_vTransformer(self(), vTransformer)
    {
    COH_ENSURE_PARAM(vTransformer);
    }


// ----- Filter interface ---------------------------------------------------

bool MapEventTransformerFilter::evaluate(Object::View v) const
    {
    if (!instanceof<MapEvent::View>(v))
        {
        COH_THROW_STREAM(IllegalStateException,
                Class::getClassName(this)
                <<  " should not be used as a general purpose filter");
        }
    Filter::View vFilter = f_vFilter;
    return vFilter == NULL || vFilter->evaluate(v);
    }


// ----- MapEventTransformer interface --------------------------------------

MapEvent::Handle
    MapEventTransformerFilter::transform(MapEvent::Handle hEvent) const
    {
    return f_vTransformer->transform(hEvent);
    }


// ----- PortableObject interface -------------------------------------------

void MapEventTransformerFilter::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    initialize(f_vTransformer, cast<MapEventTransformer::View>(hIn->readObject(1)));
    }

void MapEventTransformerFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeObject(1,f_vTransformer);
    }


// ----- Object interface ---------------------------------------------------

bool MapEventTransformerFilter::equals(Object::View v) const
    {
    MapEventTransformerFilter::View that =
        cast<MapEventTransformerFilter::View>(v,false);
    if (NULL != that)
        {
        return f_vFilter->equals(that->f_vFilter)
              && f_vTransformer->equals(that->f_vTransformer);
        }
    return false;
    }

size32_t MapEventTransformerFilter::hashCode() const
    {
    Filter::View vFilter = f_vFilter;
    return (f_vFilter == NULL ? 79 : vFilter->hashCode() ) +
            f_vTransformer->hashCode();
    }

TypedHandle<const String> MapEventTransformerFilter::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) << '(' << f_vFilter <<
        " ," << f_vTransformer << ')');
    }

COH_CLOSE_NAMESPACE3
