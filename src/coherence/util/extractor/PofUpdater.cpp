/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/PofUpdater.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/io/pof/reflect/SimplePofPath.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::reflect::SimplePofPath;

COH_REGISTER_PORTABLE_CLASS(59, PofUpdater);


// ----- constructors -------------------------------------------------------

PofUpdater::PofUpdater()
    : f_vNavigator(self())
    {
    }

PofUpdater::PofUpdater(int32_t iProp)
    : f_vNavigator(self(), SimplePofPath::create(iProp))
    {
    }

PofUpdater::PofUpdater(PofNavigator::View vNavigator)
    : f_vNavigator(self(), vNavigator)
    {
    }


// ----- ValueUpdater interface ---------------------------------------------

void PofUpdater::update(Object::Handle /*hTarget*/, Object::Holder /*ohValue*/) const
    {
    COH_THROW (UnsupportedOperationException::create());
    }


// ----- PortableObject interface -------------------------------------------

void PofUpdater::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vNavigator, cast<PofNavigator::View>(hIn->readObject(0)));
    }

void PofUpdater::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vNavigator);
    }


// ----- Object interface ---------------------------------------------------

bool PofUpdater::equals(Object::View v) const
    {
    PofUpdater::View that = cast<PofUpdater::View>(v, false);
    if (that != NULL)
        {
        return Object::equals(this->f_vNavigator, that->f_vNavigator);
        }
    return false;
    }

size32_t PofUpdater::hashCode() const
    {
    return f_vNavigator->hashCode();
    }

TypedHandle<const String> PofUpdater::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << "(path="  << f_vNavigator << ")");
    }

COH_CLOSE_NAMESPACE3
