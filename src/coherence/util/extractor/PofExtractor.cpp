/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/PofExtractor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/io/pof/reflect/SimplePofPath.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::io::pof::reflect::SimplePofPath;

COH_REGISTER_PORTABLE_CLASS(58, PofExtractor);


// ----- constructors -------------------------------------------------------

PofExtractor::PofExtractor()
    : f_vNavigator(self()),
      f_vClass(self(), NULL),
      m_nType(PofConstants::t_unknown)
    {
    }

PofExtractor::PofExtractor(const std::type_info& info, int32_t iProp)
    : f_vNavigator(self(), SimplePofPath::create(iProp)),
      f_vClass(self(), info == typeid(void)
              ? (Class::View) NULL
              : SystemClassLoader::getInstance()->loadByType(info))
    {
    m_nTarget = value;
    if (f_vClass == NULL)
        {
        m_nType = PofConstants::t_unknown;
        }
    }

PofExtractor::PofExtractor(const std::type_info& info, PofNavigator::View vNavigator,
        int32_t nTarget)
    : f_vNavigator(self(), vNavigator),
      f_vClass(self(), info == typeid(void)
              ? (Class::View) NULL
              : SystemClassLoader::getInstance()->loadByType(info))
    {
    m_nTarget = nTarget;
    if (f_vClass == NULL)
        {
        m_nType = PofConstants::t_unknown;
        }
    }


// ----- AbstractExtractor methods ------------------------------------------

Object::Holder PofExtractor::extractFromEntry(Map::Entry::Holder /*ohEntry*/) const
    {
    COH_THROW (UnsupportedOperationException::create());
    }


// ----- PortableObject interface -------------------------------------------

void PofExtractor::readExternal(PofReader::Handle hIn)
    {
    m_nTarget = hIn->readInt32(0);
    initialize(f_vNavigator, cast<PofNavigator::View>(hIn->readObject(1)));

    // Note: we write out the TypeId offset by t_unknown to allow for pre
    // 3.5.2 backwards compatibility in the reader, i.e. the lack of this
    // property in the stream will result in t_unknown, and the old behavior
    // Note 2: this offset fix unfortunately could cause us to push the
    // written TypeId out of the legal int32_t range.  To fix this we write
    // it as a int64_t on the wire.
    m_nType = (int32_t) (hIn->readInt64(2) + PofConstants::t_unknown);
    }

void PofExtractor::writeExternal(PofWriter::Handle hOut) const
    {
    PofNavigator::View vNavigator = f_vNavigator;
    if (NULL == vNavigator)
        {
        COH_THROW (IllegalStateException::create(
                "PofExtractor was constructed without a navigator"));
        }
    hOut->writeInt32(0, m_nTarget);
    hOut->writeObject(1, vNavigator);
    // see note in readExternal regarding t_unknown offset
    hOut->writeInt64(2, (int64_t) getPofTypeId(hOut->getPofContext()) -
            (int64_t) PofConstants::t_unknown);
    }


// ----- Object interface ---------------------------------------------------

bool PofExtractor::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }
    PofExtractor::View that = cast<PofExtractor::View>(v, false);
    if (that != NULL)
        {
        return (this->m_nTarget == that->m_nTarget) &&
             Object::equals(this->f_vNavigator, that->f_vNavigator);
        }
    return false;
    }

size32_t PofExtractor::hashCode() const
    {
    return f_vNavigator->hashCode() + m_nTarget;
    }

TypedHandle<const String> PofExtractor::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << "(target=" << (m_nTarget == 0 ? "value" : "key")
        << ", navigator="  << f_vNavigator
        << ")");
    }


// ----- helper methods -------------------------------------------------

int32_t PofExtractor::getPofTypeId(PofContext::View vCtx) const
    {
    Class::View vClass = f_vClass;

    return vClass == NULL
            ? m_nType
            : PofHelper::getPofTypeId(vClass, vCtx);
    }


// ----- accessors ------------------------------------------------------

Class::View PofExtractor::getClassExtracted() const
    {
    return f_vClass;
    }

PofNavigator::View PofExtractor::getNavigator() const
    {
    return f_vNavigator;
    }

COH_CLOSE_NAMESPACE3
