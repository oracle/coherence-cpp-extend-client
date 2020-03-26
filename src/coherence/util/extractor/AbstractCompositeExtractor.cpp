/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/AbstractCompositeExtractor.hpp"


COH_OPEN_NAMESPACE3(coherence,util,extractor)

namespace
    {
    ObjectArray::View makeArray(ValueExtractor::View vExtractor1,
            ValueExtractor::View vExtractor2)
        {
        ObjectArray::Handle haExtractor = ObjectArray::create(2);
        haExtractor[0] = vExtractor1;
        haExtractor[1] = vExtractor2;
        return haExtractor;
        }
    }

// ----- constructors -------------------------------------------------------

AbstractCompositeExtractor::AbstractCompositeExtractor()
    : f_vaExtractor(self())
    {
    }

AbstractCompositeExtractor::AbstractCompositeExtractor(
        ObjectArray::View vaExtractor)
    : f_vaExtractor(self(), vaExtractor)
    {
    COH_ENSURE_PARAM(vaExtractor);
    }

AbstractCompositeExtractor::AbstractCompositeExtractor(
        ValueExtractor::View vE1, ValueExtractor::View vE2)
    : f_vaExtractor(self(), makeArray(vE1, vE2))
    {
    }


// ----- PortableObject interface -------------------------------------------

void AbstractCompositeExtractor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vaExtractor, (ObjectArray::View) hIn->readObjectArray(0));
    }

void AbstractCompositeExtractor:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObjectArray(0, getExtractors());
    }


// ----- Object interface ---------------------------------------------------

bool AbstractCompositeExtractor::equals(Object::View v) const
    {
    AbstractCompositeExtractor::View that =
        cast<AbstractCompositeExtractor::View>(v, false);
    if (that != NULL)
        {
        return Class::typeEquals(this, that) &&
            this->getExtractors()->equals(that->getExtractors());
        }
    return false;
    }

size32_t AbstractCompositeExtractor::hashCode() const
    {
    return getExtractors()->hashCode();
    }

TypedHandle<const String> AbstractCompositeExtractor::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) << ' ' << getExtractors());
    }


// ----- data member accessors ----------------------------------------------

ObjectArray::View AbstractCompositeExtractor::getExtractors() const
    {
    return f_vaExtractor;
    }

COH_CLOSE_NAMESPACE3
