/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/NumberMultiplier.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/processor/NumberIncrementor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(97, NumberMultiplier);


// ----- constructors -------------------------------------------------------

NumberMultiplier::NumberMultiplier()
    : f_vNumFactor(self()), m_fPostFactor(false)
    {
    }

NumberMultiplier::NumberMultiplier(ValueManipulator::View vManipulator,
        Number::View vNumFactor, bool fPostFactorrement)
    : super(vManipulator),
      f_vNumFactor(self(), vNumFactor),
      m_fPostFactor(fPostFactorrement)
    {
    COH_ENSURE_PARAM(vNumFactor);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder NumberMultiplier::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    if (!hEntry->isPresent())
        {
        return NULL;
        }

    Number::View vNumFactor = f_vNumFactor;

    Number::View vNumOld = cast<Number::View>(get(hEntry));
    if (vNumOld == NULL)
        {
        vNumOld = instanceof<Float32::View>(vNumFactor) ?
                NumberIncrementor::getFloat32Zero()
        : instanceof<Float64::View>(vNumFactor)         ?
                NumberIncrementor::getFloat64Zero()
        : instanceof<Integer16::View>(vNumFactor)       ?
                NumberIncrementor::getInteger16Zero()
        : instanceof<Integer32::View>(vNumFactor)       ?
                NumberIncrementor::getInteger32Zero()
        : instanceof<Integer64::View>(vNumFactor)       ?
                NumberIncrementor::getInteger64Zero()
        : (Number::View)NULL;
        }

    Number::View vNumNew;

    if (instanceof<Float32::View>(vNumOld))
        {
        vNumNew = Float32::valueOf(vNumOld->getFloat32Value()
                * vNumFactor->getFloat32Value());
        }
    else if (instanceof<Float64::View>(vNumOld))
        {
        vNumNew = Float64::valueOf(vNumOld->getFloat64Value()
                * vNumFactor->getFloat64Value());
        }
    else if (instanceof<Integer16::View>(vNumOld))
        {
        vNumNew = Integer16::valueOf(vNumOld->getInt16Value()
                * vNumFactor->getInt16Value());
        }
    else if (instanceof<Integer32::View>(vNumOld))
        {
        vNumNew = Integer32::valueOf(vNumOld->getInt32Value()
                * vNumFactor->getInt32Value());
        }
    else if (instanceof<Integer64::View>(vNumOld))
        {
        vNumNew = Integer64::valueOf(vNumOld->getInt64Value()
                * vNumFactor->getInt64Value());
        }
    else
        {
        COH_THROW_STREAM(IllegalStateException, "Unsupported type:" <<
                (vNumOld == NULL ? Class::getClassName(vNumFactor) :
                Class::getClassName(vNumOld)) << "" );
        }

    if (!vNumNew->equals(vNumOld))
        {
        set(hEntry, vNumNew);
        }

    return m_fPostFactor ? vNumOld : vNumNew;
    }


// ----- PortableObject interface -------------------------------------------

void NumberMultiplier::readExternal(PofReader::Handle hIn)
    {
    PropertyProcessor::readExternal(hIn);

    initialize(f_vNumFactor, cast<Number::View>(hIn->readObject(1)));
    m_fPostFactor = hIn->readBoolean(2);
    }

void NumberMultiplier::writeExternal(PofWriter::Handle hOut) const
    {
    PropertyProcessor::writeExternal(hOut);

    hOut->writeObject(1, f_vNumFactor);
    hOut->writeBoolean(2, m_fPostFactor);
    }


// ----- Describable interface ----------------------------------------------

String::View NumberMultiplier::getDescription() const
    {
    return COH_TO_STRING((m_fPostFactor ? ", post" : ", pre") << "-factor=" << f_vNumFactor);
    }

COH_CLOSE_NAMESPACE3
