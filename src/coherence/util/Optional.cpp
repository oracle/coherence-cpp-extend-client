/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Optional.hpp"
#include "coherence/util/Objects.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_PORTABLE_CLASS(903, Optional);

using coherence::util::Objects;

// ----- constants ----------------------------------------------------------

Optional::View Optional::emptyInstance()
    {
    static FinalView<Optional> vOptional
        (System::common(), create());
    return vOptional;
    }
COH_STATIC_INIT(Optional::emptyInstance());

// ----- constructors -------------------------------------------------------

Optional::Optional()
        : f_oh(self())
    {
    }

Optional::Optional(Object::Holder oh)
        : f_oh(self(), oh)
    {
    if (NULL == f_oh)
        {
        COH_THROW (IllegalArgumentException::create("value may not be NULL"));
        }
    }

// ----- Optional methods ---------------------------------------------------

Optional::View Optional::empty()
    {
    return emptyInstance();
    }

Optional::View Optional::of(Object::Holder oh)
    {
    return Optional::create(oh);
    }

Optional::View Optional::ofNullable(Object::Holder oh)
    {
    return NULL == oh ? empty() : cast<Optional::View>(of(oh));
    }

Object::Holder Optional::get() const
    {
    return f_oh;
    }

bool Optional::isPresent() const
    {
    return f_oh != NULL;
    }

Object::Holder Optional::orElse(Object::Holder oh) const
    {
    return f_oh == NULL ? oh : f_oh;
    }

// ----- PortableObject interface -------------------------------------------

void Optional::readExternal(PofReader::Handle hIn)
    {
    bool fPresent = hIn->readBoolean(0);
    if (fPresent)
        {
        initialize(f_oh, hIn->readObject(1));
        }
    }

void Optional:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeBoolean(0, isPresent());
    if (isPresent())
        {
        hOut->writeObject(1, f_oh);
        }
    }

// ----- Object interface ---------------------------------------------------

bool Optional::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }
    if (!instanceof<Optional::View>(v))
        {
        return false;
        }
    Optional::View vThat = cast<Optional::View>(v);
    return Objects::equals(f_oh, vThat->f_oh);
    }

size32_t Optional::hashCode() const
    {
    return Objects::hashCode(f_oh);
    }

TypedHandle<const String> Optional::toString() const
    {
    return NULL != f_oh
        ? COH_TO_STRING("Optional[" << f_oh << ']')
        : "Optional::empty";
    }

COH_CLOSE_NAMESPACE2
