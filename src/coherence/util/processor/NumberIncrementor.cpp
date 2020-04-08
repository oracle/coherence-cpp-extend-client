/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/NumberIncrementor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(96, NumberIncrementor);


// ----- constructors -------------------------------------------------------

NumberIncrementor::NumberIncrementor()
    : f_vNumInc(self()), m_fPostInc(false)
    {
    }

NumberIncrementor::NumberIncrementor(ValueManipulator::View vManipulator,
        Number::View vNumInc, bool fPostIncrement)
    : super(vManipulator),
      f_vNumInc(self(), vNumInc),
      m_fPostInc(fPostIncrement)
    {
    COH_ENSURE_PARAM(vNumInc);
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder NumberIncrementor::process(
        InvocableMap::Entry::Handle hEntry) const
    {
    if (!hEntry->isPresent())
        {
        return NULL;
        }

    Number::View vNumInc = f_vNumInc;
    if (vNumInc == NULL)
        {
        COH_THROW_STREAM(IllegalArgumentException,
                "Incorrectly constructed NumberIncrementor");
        }

    Number::View vNumOld = cast<Number::View>(get(hEntry));
    if (vNumOld == NULL)
        {
        vNumOld = instanceof<Float32::View>  (vNumInc) ? getFloat32Zero()
                : instanceof<Float64::View>  (vNumInc) ? getFloat64Zero()
                : instanceof<Integer16::View>(vNumInc) ? getInteger16Zero()
                : instanceof<Integer32::View>(vNumInc) ? getInteger32Zero()
                : instanceof<Integer64::View>(vNumInc) ? getInteger64Zero()
                : (Number::View) NULL;
        }

    Number::View vNumNew;

    if (instanceof<Float32::View>(vNumOld))
        {
        vNumNew = Float32::valueOf(vNumOld->getFloat32Value()
                + vNumInc->getFloat32Value());
        }
    else if (instanceof<Float64::View>(vNumOld))
        {
        vNumNew = Float64::valueOf(vNumOld->getFloat64Value()
                + vNumInc->getFloat64Value());
        }
    else if (instanceof<Integer16::View>(vNumOld))
        {
        vNumNew = Integer16::valueOf(vNumOld->getInt16Value()
                + vNumInc->getInt16Value());
        }
    else if (instanceof<Integer32::View>(vNumOld))
        {
        vNumNew = Integer32::valueOf(vNumOld->getInt32Value()
                + vNumInc->getInt32Value());
        }
    else if (instanceof<Integer64::View>(vNumOld))
        {
        vNumNew = Integer64::valueOf(vNumOld->getInt64Value()
                + vNumInc->getInt64Value());
        }
    else
        {
        COH_THROW_STREAM(IllegalStateException, "Unsupported type:" <<
                (vNumOld == NULL ? Class::getClassName(vNumInc) :
                Class::getClassName(vNumOld)) << "" );
        }

    set(hEntry, vNumNew);

    return m_fPostInc ? vNumOld : vNumNew;
    }


// ----- PortableObject interface -------------------------------------------

void NumberIncrementor::readExternal(PofReader::Handle hIn)
    {
    PropertyProcessor::readExternal(hIn);

    initialize(f_vNumInc, cast<Number::View>(hIn->readObject(1)));
    m_fPostInc = hIn->readBoolean(2);
    }

void NumberIncrementor::writeExternal(PofWriter::Handle hOut) const
    {
    PropertyProcessor::writeExternal(hOut);

    hOut->writeObject(1, f_vNumInc);
    hOut->writeBoolean(2, m_fPostInc);
    }


// ----- Describable interface ----------------------------------------------

String::View NumberIncrementor::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << (m_fPostInc ? ", post" : ", pre") << "-increment=" << f_vNumInc);
    }


// ----- constants ----------------------------------------------------------

Number::View NumberIncrementor::getInteger16Zero()
    {
    static FinalView<Number> Integer16Zero(System::common(),
            Integer16::valueOf(0));
    return Integer16Zero;
    }
COH_STATIC_INIT(NumberIncrementor::getInteger16Zero());

Number::View NumberIncrementor::getInteger32Zero()
    {
    static FinalView<Number> Integer32Zero(System::common(),
            Integer32::valueOf(0));
    return Integer32Zero;
    }
COH_STATIC_INIT(NumberIncrementor::getInteger32Zero());

Number::View NumberIncrementor::getInteger64Zero()
    {
    static FinalView<Number> Integer64Zero(System::common(),
            Integer64::valueOf(0));
    return Integer64Zero;
    }
COH_STATIC_INIT(NumberIncrementor::getInteger64Zero());

Number::View NumberIncrementor::getFloat32Zero()
    {
    static FinalView<Number> Float32Zero(System::common(),
            Float32::valueOf(0));
    return Float32Zero;
    }
COH_STATIC_INIT(NumberIncrementor::getFloat32Zero());

Number::View NumberIncrementor::getFloat64Zero()
    {
    static FinalView<Number> Float64Zero(System::common(),
            Float64::valueOf(0));
    return Float64Zero;
    }
COH_STATIC_INIT(NumberIncrementor::getFloat64Zero());

COH_CLOSE_NAMESPACE3
