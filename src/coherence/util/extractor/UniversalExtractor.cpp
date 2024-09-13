/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/UniversalExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(192, UniversalExtractor);

// ----- constructors -------------------------------------------------------

UniversalExtractor::UniversalExtractor()
    : f_vsName(self()), f_vaParam(self())
    {
    m_nTarget = value;
    }

UniversalExtractor::UniversalExtractor(String::View vsName,
        ObjectArray::View vaParam, int32_t nTarget)
    : f_vsName(self(), vsName), f_vaParam(self(), vaParam)
    {
    COH_ENSURE_PARAM(vsName);
    m_nTarget = nTarget;
    }

UniversalExtractor::UniversalExtractor(const UniversalExtractor& that)
    : f_vsName(self(), that.f_vsName),
      f_vaParam(self(), cast<ObjectArray::View>(Object::clone(that.f_vaParam)))
    {
    // UniversalExtractor is cloneable only because MSVC tries to generate
    // a copy constructor for it, which blows up as MemberHandles do not
    // support automatic copy construction
    }

// ----- PortableObject interface -------------------------------------------

void UniversalExtractor::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vsName, hIn->readString(0));
    initialize(f_vaParam, hIn->readObjectArray(1));
    m_nTarget = hIn->readInt32(2);
    }

void UniversalExtractor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeString(0, f_vsName);
    hOut->writeObjectArray(1, f_vaParam);
    hOut->writeInt32(2, m_nTarget);
    }

// ----- Object interface ---------------------------------------------------

bool UniversalExtractor::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    UniversalExtractor::View that = cast<UniversalExtractor::View>(v, false);
    if (that != NULL)
        {
        return this->m_nTarget == that->m_nTarget &&
            Object::equals(f_vsName, that->f_vsName) &&
            Object::equals(f_vaParam, that->f_vaParam);
        }

    return false;
    }

size32_t UniversalExtractor::hashCode() const
    {
    return f_vsName->hashCode();
    }

TypedHandle<const String> UniversalExtractor::toString() const
    {
    String::View vs = "";

    if (m_nTarget == key)
        {
        vs = COH_TO_STRING(vs << ".getKey()");
        }

    return COH_TO_STRING(vs << '.' << getName() << ' ' << getParameters());
    }

// ----- data member accessors ----------------------------------------------

String::View UniversalExtractor::getName() const
    {
    return f_vsName;
    }

ObjectArray::View UniversalExtractor::getParameters() const
    {
    return f_vaParam;
    }

COH_CLOSE_NAMESPACE3
