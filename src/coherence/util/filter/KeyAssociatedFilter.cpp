/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/KeyAssociatedFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(200, KeyAssociatedFilter);


// ----- constructors -------------------------------------------------------

KeyAssociatedFilter::KeyAssociatedFilter()
    : f_vFilter(self()), f_vHostKey(self())
    {
    }

KeyAssociatedFilter::KeyAssociatedFilter(Filter::View vFilter,
        Object::View vHostKey)
    : f_vFilter(self(), vFilter), f_vHostKey(self(), vHostKey)
    {
    if (vFilter == NULL || instanceof<KeyAssociatedFilter::View>(vFilter))
        {
        COH_THROW_STREAM(IllegalArgumentException,"Invalid filter: " <<
                vFilter);
        }
    if (vHostKey == NULL)
        {
        COH_THROW (IllegalArgumentException::create(
                "Host key must be specifies"));
        }
    }


// ----- Filter interface ---------------------------------------------------

bool KeyAssociatedFilter::evaluate(Object::View v) const
    {
    return f_vFilter->evaluate(v);
    }


// ----- PortableObject interface -------------------------------------------

void KeyAssociatedFilter::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    initialize(f_vHostKey, hIn->readObject(1));
   }

void KeyAssociatedFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    hOut->writeObject(1, f_vHostKey);
    }


// ----- Object interface ---------------------------------------------------

bool KeyAssociatedFilter::equals(Object::View v) const
    {
    KeyAssociatedFilter::View that = cast<KeyAssociatedFilter::View>(v, false);
    if (NULL != that)
        {
        return f_vFilter->equals(that->f_vFilter)
              && f_vHostKey->equals(that->f_vHostKey);
        }
    return false;
    }

size32_t KeyAssociatedFilter::hashCode() const
    {
    return f_vFilter->hashCode() + f_vHostKey->hashCode();
    }

TypedHandle<const String> KeyAssociatedFilter::toString() const
    {
    return COH_TO_STRING("KeyAssociatedFilter(" << f_vFilter << ", " << f_vHostKey);
    }


// ----- data members accessor-----------------------------------------------

Filter::View KeyAssociatedFilter::getFilter() const
    {
    return f_vFilter;
    }

Object::View KeyAssociatedFilter::getHostKey() const
    {
    return f_vHostKey;
    }

COH_CLOSE_NAMESPACE3
