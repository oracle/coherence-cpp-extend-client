/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/ComparisonValueExtractor.hpp"

#include <algorithm>
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include <typeinfo>

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::util::comparator::SafeComparator;

COH_REGISTER_PORTABLE_CLASS(57, ComparisonValueExtractor);

namespace
{
    const int32_t FMT_NONE     = -1;

    const int32_t FMT_BYTE     =  0;

    const int32_t FMT_SHORT    =  1;

    const int32_t FMT_INT      =  2;

    const int32_t FMT_LONG     =  3;

    const int32_t FMT_FLOAT    =  4;

    const int32_t FMT_DOUBLE   =  5;

    int32_t getNumberCode(Number::View v)
        {
        return      instanceof<Integer32::View>(v)        ? FMT_INT
                  : instanceof<Integer64::View>(v)        ? FMT_LONG
                  : instanceof<Float64::View>(v)          ? FMT_DOUBLE
                  : instanceof<Float32::View>(v)          ? FMT_FLOAT
                  : instanceof<Integer16::View>(v)        ? FMT_SHORT
                  :                                         FMT_NONE;
        }

}


// ----- constructors -------------------------------------------------------

ComparisonValueExtractor::ComparisonValueExtractor()
    : super(), f_vComparator(self())
    {
    }

ComparisonValueExtractor::ComparisonValueExtractor(ValueExtractor::View vE1,
        ValueExtractor::View vE2, Comparator::View vComp)
    : super(vE1, vE2), f_vComparator(self(), vComp)
    {
    }


// ----- ValueExtractor interface -------------------------------------------

Object::Holder ComparisonValueExtractor::extract(
            Object::Holder ohTarget) const
    {
    ObjectArray::View vaExtractor = getExtractors();
    Comparator::View vComparator  = getComparator();

    Object::View v1 =
         cast<ValueExtractor::View>(vaExtractor[0])->extract(ohTarget);
    Object::View v2 =
         cast<ValueExtractor::View>(vaExtractor[1])->extract(ohTarget);

    if (instanceof<Number::View>(v1) && instanceof<Number::View>(v2)
             && vComparator == NULL)
        {
        Number::View vNum1 = cast<Number::View>(v1);
        Number::View vNum2 = cast<Number::View>(v2);
        int32_t    nType;
        if (typeid(vNum1) == typeid(vNum2))
            {
            // most common case; same types
            nType = getNumberCode(vNum1);
            }
        else
            {
            int32_t ix1 = getNumberCode(vNum1);
            int32_t ix2 = getNumberCode(vNum2);
            switch (std::max(ix1, ix2))
                {
                case 1: case 2:
                    nType = FMT_INT;
                    break;
                case 3:
                    nType = FMT_LONG;
                    break;
                case 4: case 5:
                    nType = FMT_DOUBLE;
                    break;
                default:
                    nType = FMT_NONE;
                    break;
                }
            }

        switch (nType)
            {
            case FMT_BYTE:
            case FMT_SHORT:
            case FMT_INT:
                return Integer32::create(vNum1->getInt32Value()
                        - vNum2->getInt32Value());

            case FMT_LONG:
                return Integer64::create(vNum1->getInt64Value()
                        - vNum2->getInt64Value());

            case FMT_FLOAT:
                return Float32::create(vNum1->getFloat32Value()
                        - vNum2->getFloat32Value());

            case FMT_DOUBLE:
                return Float64::create(vNum1->getFloat64Value()
                        - vNum2->getFloat64Value());
            }
        }
    return Integer32::create(
        SafeComparator::compareSafe(vComparator, v1, v2));
    }


// ----- PortableObject interface -------------------------------------------

void ComparisonValueExtractor::readExternal(PofReader::Handle hIn)
    {
    AbstractCompositeExtractor::readExternal(hIn);
    initialize(f_vComparator, cast<Comparator::View>(hIn->readObject(1)));
    }

void ComparisonValueExtractor:: writeExternal(PofWriter::Handle hOut) const
    {
    AbstractCompositeExtractor::writeExternal(hOut);
    hOut->writeObject(1,getComparator());
    }


// ----- data member accessors ----------------------------------------------

Comparator::View ComparisonValueExtractor::getComparator() const
    {
    return f_vComparator;
    }

COH_CLOSE_NAMESPACE3
