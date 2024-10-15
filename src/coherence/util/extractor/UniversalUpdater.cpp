/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/UniversalUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(193, UniversalUpdater);

// ----- constructors -------------------------------------------------------

UniversalUpdater::UniversalUpdater()
    : f_vsName(self())
    {
    }

UniversalUpdater::UniversalUpdater(String::View vsName)
    : f_vsName(self(), vsName)
    {
    COH_ENSURE_PARAM(vsName);
    }

UniversalUpdater::UniversalUpdater(const UniversalUpdater& that)
    : f_vsName(self(), that.f_vsName)
    {
    // UniversalUpdater is cloneable only because MSVC tries to generate
    // a copy constructor for it, which blows up as MemberHandles do not
    // support automatic copy construction
    }

// ----- PortableObject interface -------------------------------------------

void UniversalUpdater::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vsName, hIn->readString(0));
    }

void UniversalUpdater:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeString(0, f_vsName);
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> UniversalUpdater::toString() const
    {
    return f_vsName;
    }

COH_CLOSE_NAMESPACE3
