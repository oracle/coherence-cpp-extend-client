/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/PropertyManipulator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/extractor/ChainedExtractor.hpp"
#include "coherence/util/extractor/CompositeUpdater.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"

#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

COH_REGISTER_PORTABLE_CLASS(99, PropertyManipulator);

using coherence::util::StringHelper;
using coherence::util::extractor::ChainedExtractor;
using coherence::util::extractor::CompositeUpdater;
using coherence::util::extractor::ReflectionExtractor;
using coherence::util::extractor::ReflectionUpdater;


// ----- constructors -------------------------------------------------------

PropertyManipulator::PropertyManipulator()
    : f_vsName(self(), StringHelper::getEmptyString()),
      m_fUseIs(false),
      f_vExtractorPart(self(), NULL, /*fMutable*/ true),
      f_vExtractor(self(), NULL, /*fMutable*/ true),
      f_vUpdater(self(), NULL, /*fMutable*/ true)
    {
    }

PropertyManipulator::PropertyManipulator(String::View vsName,
    bool fUseIs)
    : f_vsName(self(),vsName),
      m_fUseIs(fUseIs),
      f_vExtractorPart(self()),
      f_vExtractor(self()),
      f_vUpdater(self())
    {
    COH_ENSURE_PARAM(vsName);
    if (0 == vsName->length())
        {
        COH_THROW_STREAM(IllegalArgumentException, "Invalid method name");
        }
    }


// ----- ValueManipulator interface -----------------------------------------

ValueExtractor::View PropertyManipulator::getExtractor() const
    {
    ValueExtractor::View vExtractor = f_vExtractor;
    if (NULL == vExtractor)
        {
        init();
        vExtractor = f_vExtractor;
        }
    return vExtractor;
    }

ValueUpdater::View PropertyManipulator::getUpdater() const
    {
    ValueUpdater::View vUpdater = f_vUpdater;
    if (NULL == vUpdater)
        {
        init();
        vUpdater = f_vUpdater;
        }

    return vUpdater;
    }


// ----- internal -----------------------------------------------------------

void PropertyManipulator::init() const
    {
    // allow composite property name (not documented)
    String::View vsName  = f_vsName;
    size32_t     ofLast  = vsName->lastIndexOf('.');

    String::View vsProp  = vsName->substring(
            String::npos == ofLast ? 0 : ofLast + 1);

    ValueExtractor::View vExtractor = ReflectionExtractor::create(
        COH_TO_STRING((m_fUseIs ? "is" : "get") << vsProp));
    ValueUpdater::View vUpdater =
            ReflectionUpdater::create(COH_TO_STRING("set" << vsProp));

    if (String::npos != ofLast)
        {
        ObjectArray::Handle asProp =
                StringHelper::split(vsName->substring(0, ofLast), ".");
        size32_t            cParts = asProp->length;

        ObjectArray::Handle veGet = ObjectArray::create(cParts + 1);
        ObjectArray::Handle vePut = ObjectArray::create(cParts);
        for (size32_t i = 0; i < cParts; i++)
            {
            veGet[i] = vePut[i] =
                ReflectionExtractor::create(
                        COH_TO_STRING("get" << asProp[i]));
            }
        veGet[cParts] = vExtractor;

        vExtractor = ChainedExtractor::create(veGet);
        vUpdater   = CompositeUpdater::create(
                cParts == 1 ?
                        cast<ValueExtractor::View>(vePut[0]) :
                        cast<ValueExtractor::View>(
                                ChainedExtractor::create(vePut)),
                vUpdater);
        }
    initialize(f_vExtractor, vExtractor);
    initialize(f_vUpdater, vUpdater);
    }


// ----- PortableObject interface -------------------------------------------

void PropertyManipulator::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vsName, hIn->readString(0));
    m_fUseIs = hIn->readBoolean(1);
    }

void PropertyManipulator::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeString(0, f_vsName);
    hOut->writeBoolean(1, m_fUseIs);
    }


// ----- Object interface ---------------------------------------------------

bool PropertyManipulator::equals(Object::View v) const
    {
    if (!instanceof<PropertyManipulator::View>(v))
        {
        return false;
        }
    PropertyManipulator::View vThat = cast<PropertyManipulator::View>(v);
    if (this == vThat)
        {
        return true;
        }
    return Object::equals(this->f_vsName, vThat->f_vsName) &&
            this->m_fUseIs == vThat->m_fUseIs;
    }

size32_t PropertyManipulator::hashCode() const
    {
    return f_vsName->hashCode();
    }

COH_CLOSE_NAMESPACE3
