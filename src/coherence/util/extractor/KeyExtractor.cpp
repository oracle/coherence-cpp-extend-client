/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/KeyExtractor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(53, KeyExtractor);


// ----- constructors -------------------------------------------------------

KeyExtractor::KeyExtractor()
    : m_vExtractor(self(), (ValueExtractor::View) IdentityExtractor::getInstance())
    {
    m_nTarget = key;
    }

KeyExtractor::KeyExtractor(ValueExtractor::View vExtractor)
    : m_vExtractor(self(), (NULL == vExtractor
            ? (ValueExtractor::View) IdentityExtractor::getInstance()
            : vExtractor))
    {
    m_nTarget = key;
    }


// ----- ValueExtractor interface -------------------------------------------

Object::Holder KeyExtractor::extract(Object::Holder ohTarget) const
    {
    return m_vExtractor->extract(ohTarget);
    }


// ----- PortableObject interface -------------------------------------------

void KeyExtractor::readExternal(PofReader::Handle hIn)
    {
    m_vExtractor = cast<ValueExtractor::View>(hIn->readObject(0));
    }

void KeyExtractor:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, m_vExtractor);
    }


// ----- Object interface ---------------------------------------------------

bool KeyExtractor::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }
    KeyExtractor::View that = cast<KeyExtractor::View>(v, false);
    if (that != NULL)
        {
        return Object::equals(this->m_vExtractor, that->m_vExtractor);
        }
    return false;
    }

size32_t KeyExtractor::hashCode() const
    {
    return m_vExtractor->hashCode();
    }

TypedHandle<const String> KeyExtractor::toString() const
    {
    return COH_TO_STRING("KeyExtractor: !(" << m_vExtractor << ")");
    }


// ----- data member accessors ----------------------------------------------

ValueExtractor::View KeyExtractor::getExtractor() const
    {
    return m_vExtractor;
    }

COH_CLOSE_NAMESPACE3
