/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/CompositeUpdater.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/extractor/ChainedExtractor.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"


COH_OPEN_NAMESPACE3(coherence,util,extractor)

COH_REGISTER_PORTABLE_CLASS(51, CompositeUpdater);


// ----- constructors -------------------------------------------------------

CompositeUpdater::CompositeUpdater()
    : f_vExtractor(self()), f_vUpdater(self())
    {
    }

CompositeUpdater::CompositeUpdater(String::View vsName)
    : f_vExtractor(self()), f_vUpdater(self())
    {
    COH_ENSURE_PARAM(vsName);
    COH_ENSURE(vsName->length() > 0);
    size32_t ofLast = vsName->lastIndexOf('.');
    if (ofLast == String::npos)
        {
        initialize(f_vExtractor, IdentityExtractor::getInstance());
        }
    else
        {
        initialize(f_vExtractor, ChainedExtractor::create(
                        ChainedExtractor::createExtractors(
                                vsName->substring(0, ofLast))));
        }
    initialize(f_vUpdater,
        ReflectionUpdater::create(vsName->substring(ofLast + 1)));
    }

CompositeUpdater::CompositeUpdater(ValueExtractor::View vExtractor,
            ValueUpdater::View vUpdater)
    : f_vExtractor(self(), vExtractor), f_vUpdater(self(),vUpdater)
    {
    COH_ENSURE_PARAM(vExtractor);
    COH_ENSURE_PARAM(vUpdater);
    }


// ----- ValueUpdater interface ---------------------------------------------

void CompositeUpdater::update(Object::Handle hTarget,
        Object::Holder ohValue) const
    {
    getUpdater()->update(
            cast<Object::Handle>(getExtractor()->extract(hTarget)),
            ohValue);
    }


// ----- PortableObject interface -------------------------------------------

void CompositeUpdater::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObject(0)));
    initialize(f_vUpdater, cast<ValueUpdater::View>(hIn->readObject(1)));
    }

void CompositeUpdater:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, getExtractor());
    hOut->writeObject(1, getUpdater());
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> CompositeUpdater::toString() const
    {
    return COH_TO_STRING("CompositeUpdater(" << getExtractor() << ", " << getUpdater());
    }


// ----- data member accessors ----------------------------------------------

ValueExtractor::View CompositeUpdater::getExtractor() const
    {
    return f_vExtractor;
    }

ValueUpdater::View CompositeUpdater::getUpdater() const
    {
    return f_vUpdater;
    }

COH_CLOSE_NAMESPACE3
