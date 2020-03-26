/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PortableObjectSerializer.hpp"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Binary.hpp"

#include <algorithm>
#include <typeinfo>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Evolvable;
using coherence::io::IOException;
using coherence::util::Binary;


// ----- constructors -------------------------------------------------------

PortableObjectSerializer::PortableObjectSerializer(int32_t nTypeId)
        : m_nTypeId(nTypeId)
    {
    }


// ----- PofSerializer interface --------------------------------------------

void PortableObjectSerializer::serialize(PofWriter::Handle hOut, Object::View v) const
    {
    PortableObject::View vPortable;
    try
        {
        vPortable = cast<PortableObject::View>(v);
        }
    catch (Exception::View e)
        {
        String::View vsClass;
        try
            {
            vsClass = hOut->getPofContext()->getClassName(m_nTypeId);
            }
        catch (...) {}

        String::View vsActual;
        try
            {
            vsActual = Class::getClassName(v);
            }
        catch (...) {}

        COH_THROW_STREAM (IOException,
                "An exception occurred writing a PortableObject"
                << " user type to a POF stream: type-id=" << m_nTypeId
                << ", class-name="
                << (NULL == vsClass ? "N/A" : vsClass)
                << ", actual class-name="
                << (NULL == vsActual ? "N/A" : vsActual)
                << ", exception=" << e);
        }

    // set the version identifier
    Evolvable::View vEvolvable = cast<Evolvable::View>(vPortable, false);
    bool            fEvolvable = NULL != vEvolvable;
    if (fEvolvable)
        {
        hOut->setVersionId(std::max(vEvolvable->getDataVersion(),
                vEvolvable->getImplVersion()));
        }

    // write out the object's properties
    vPortable->writeExternal(hOut);

    // write out any future properties
    Binary::View vBin;
    if (fEvolvable)
        {
        vBin = vEvolvable->getFutureData();
        }
    hOut->writeRemainder(vBin);
    }

Object::Holder PortableObjectSerializer::deserialize(PofReader::Handle hIn) const
    {
    // create a new instance of the user type
    PortableObject::Handle hPortable;
    try
        {
        hPortable = cast<PortableObject::Handle>(hIn->getPofContext()
                ->getClass(m_nTypeId)->newInstance());
        hIn->registerIdentity(hPortable);
        }
    catch (Exception::View e)
        {
        String::View vsClass;
        try
            {
            vsClass = hIn->getPofContext()->getClassName(m_nTypeId);
            }
        catch (...) {}

        COH_THROW_STREAM (IOException,
                "An exception occurred instantiating a PortableObject"
                << " user type from a POF stream: type-id=" << m_nTypeId
                << ", class-name=" << (NULL == vsClass ? "N/A" : vsClass)
                << ", exception=" << e);
        }

    // set the version identifier
    Evolvable::Handle hEvolvable = cast<Evolvable::Handle>(hPortable, false);
    bool              fEvolvable = NULL != hEvolvable;
    if (fEvolvable)
        {
        hEvolvable->setDataVersion(hIn->getVersionId());
        }

    // read the object's properties
    hPortable->readExternal(hIn);

    // read any future properties
    Binary::View vBin = hIn->readRemainder();
    if (fEvolvable)
        {
        hEvolvable->setFutureData(vBin);
        }

    return hPortable;
    }

COH_CLOSE_NAMESPACE3
