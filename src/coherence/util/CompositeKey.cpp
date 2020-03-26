/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/CompositeKey.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_PORTABLE_CLASS(18, CompositeKey);


// ----- constructors/destructor  -------------------------------------------

CompositeKey::CompositeKey()
    : f_voPrimary(self()),
      f_voSecondary(self())
    {
    }

CompositeKey::CompositeKey(Object::View voPrimary, Object::View voSecondary)
    : f_voPrimary(self(), voPrimary),
      f_voSecondary(self(), voSecondary)
    {
    if (voPrimary == NULL || voSecondary == NULL)
        {
        COH_THROW (IllegalArgumentException::create(""));
        }
    }


// ----- KeyAssociation interface -------------------------------------------

Object::View CompositeKey::getAssociatedKey() const
    {
    return f_voPrimary;
    }


// ----- PortableObject interface -------------------------------------------

void CompositeKey::readExternal(PofReader::Handle hIn)
    {
    initialize(f_voPrimary, cast<Object::View>(hIn->readObject(0)));
    initialize(f_voSecondary, cast<Object::View>(hIn->readObject(1)));
    }

void CompositeKey::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_voPrimary);
    hOut->writeObject(1, f_voSecondary);
    }


// ----- Object methods -----------------------------------------------------

size32_t CompositeKey::hashCode() const
    {
    return f_voPrimary->hashCode() ^ f_voSecondary->hashCode();
    }

bool CompositeKey::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    CompositeKey::View vThat = cast<CompositeKey::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }

    return this->f_voPrimary->equals(vThat->f_voPrimary) &&
        this->f_voSecondary->equals(vThat->f_voSecondary);
    }

TypedHandle<const String> CompositeKey::toString() const
    {
    return COH_TO_STRING(f_voPrimary << ":" << f_voSecondary);
    }


// ----- accessors ----------------------------------------------------------

Object::View CompositeKey::getPrimaryKey() const
    {
      return f_voPrimary;
    }

Object::View CompositeKey::getSecondaryKey() const
    {
      return f_voSecondary;
    }

COH_CLOSE_NAMESPACE2
