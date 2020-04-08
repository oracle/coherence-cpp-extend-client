/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PortableTypeSerializer.hpp"

#include "coherence/io/IOException.hpp"

#include "coherence/io/pof/EvolvableHolder.hpp"
#include "coherence/io/pof/EvolvableObject.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/UserTypeReader.hpp"

#include "coherence/util/Binary.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/SortedSet.hpp"
#include "coherence/util/TreeSet.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::IOException;
using coherence::util::Binary;
using coherence::util::Iterator;
using coherence::util::Set;
using coherence::util::SortedSet;
using coherence::util::TreeSet;

namespace
    {
    // ---- helper methods --------------------------------------------------

    /**
     * Return a sorted set of type identifiers for all user types in a class
     * hierarchy.
     *
     * @param v     the object to return type identifiers for
     * @param vCtx  the POF context
     *
     * @return  a sorted set of type identifiers for all user types in a class
     *          hierarchy
     */
    SortedSet::View getTypeIds(Object::View v, PofContext::View vCtx)
        {
        SortedSet::Handle hTypeIds = TreeSet::create();

        Class::View vClz =
                SystemClassLoader::getInstance()->loadByName(Class::getClassName(v));
        while (vCtx->isUserType(vClz))
            {
            hTypeIds->add(Integer32::create(vCtx->getUserTypeIdentifier(vClz)));
            vClz = vClz->getSuperclass();
            }

        if (instanceof<EvolvableObject::View>(v))
            {
            EvolvableHolder::View vEvolvableHolder = cast<EvolvableObject::View>(v)->getEvolvableHolder();
            if (!vEvolvableHolder->isEmpty())
                {
                hTypeIds->addAll(vEvolvableHolder->getTypeIds());
                }
            }

        return hTypeIds;
        }

    /**
     * Return the class associated with a specified type identifier, or null
     * if the identifier is not defined in the current POF context.
     *
     * @param vCtx     the POF context
     * @param nTypeId  the type identifier to lookup
     *
     * @return  the class associated with a specified type identifier, or NULL
     *          if the identifier is not defined in the current POF context
     */
    Class::View getClassForTypeId(PofContext::View vCtx, int32_t nTypeId)
        {
        try
            {
            return vCtx->getClass(nTypeId);
            }
        catch (IllegalArgumentException::View /* ve */)
            {
            return NULL;
            }
        }
    }

// ----- constructors -------------------------------------------------------

PortableTypeSerializer::PortableTypeSerializer(int32_t nTypeId)
        : m_nTypeId(nTypeId)
    {
    }


// ----- PofSerializer interface --------------------------------------------

void PortableTypeSerializer::serialize(PofWriter::Handle hOut, Object::View v) const
    {
    if (!(instanceof<PortableObject::View>(v)))
        {
        COH_THROW_STREAM(IOException, "Class [" << Class::getClassName(v)
                << "] does not implement the PortableType interface");
        }

    PortableObject::View  vPO        = cast<PortableObject::View>(v);
    bool                  fEvolvable = instanceof<EvolvableObject::View>(v);
    EvolvableObject::View vEO        = fEvolvable ? cast<EvolvableObject::View>(v) : NULL;

    try
        {
        PofContext::View vCtx     = hOut->getPofContext();
        Set::View        vTypeIds = getTypeIds(v, vCtx);

        for (Iterator::Handle hIter = vTypeIds->iterator(); hIter->hasNext(); )
            {
            int32_t typeId = cast<Integer32::View>(hIter->next())->getInt32Value();
            Evolvable::View vE = NULL;
            if (fEvolvable)
                {
                vE = vEO->getEvolvable(typeId);
                }

            PofWriter::Handle hNestedWriter = hOut->createNestedPofWriter(typeId, typeId);

            if (fEvolvable)
                {
                hNestedWriter->setVersionId(std::max(vE->getDataVersion(),
                                                     vE->getImplVersion()));
                }

            Class::View vClz = getClassForTypeId(vCtx, typeId);
            if (NULL != vClz)
                {
                vPO->writeExternal(hNestedWriter);
                }

            hNestedWriter->writeRemainder(fEvolvable ? vE->getFutureData() : NULL);
            }

        hOut->writeRemainder(NULL);
        }
    catch (Exception::View ve)
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

        COH_THROW_STREAM(IOException,
                "An exception occurred writing a PortableObject"
                << " user type to a POF stream: type-id=" << m_nTypeId
                << (vsClass == NULL
                        ? String::null_string
                        : COH_TO_STRING(", class-name=" << vsClass))
                << (vsActual == NULL
                        ? String::null_string
                        : COH_TO_STRING(", actual class-name=" << vsActual))
                << ", exception=" << ve);
        }
    }

Object::Holder PortableTypeSerializer::deserialize(PofReader::Handle hIn) const
    {
    try
        {
        PortableObject::Handle hPO = cast<PortableObject::Handle>
                ((getClassForTypeId(hIn->getPofContext(), m_nTypeId))->newInstance());

        bool fEvolvable = instanceof<EvolvableObject::Handle>(hPO);
        EvolvableObject::Handle hEO = fEvolvable ? cast<EvolvableObject::Handle>(hPO) : NULL;

        UserTypeReader::Handle hUserTypeReader = cast<UserTypeReader::Handle>(hIn);
        for(int32_t typeId = hUserTypeReader->getNextPropertyIndex(); typeId > 0;
                typeId = hUserTypeReader->getNextPropertyIndex())
            {
            Evolvable::Handle hE = NULL;
            PofReader::Handle hNestedReader = hUserTypeReader->createNestedPofReader(typeId);

            if (fEvolvable)
                {
                hE = hEO->getEvolvable(typeId);
                hE->setDataVersion(hNestedReader->getVersionId());
                }

            hPO->readExternal(hNestedReader);

            Binary::View binRemainder = hNestedReader->readRemainder();
            if (fEvolvable)
                {
                hE->setFutureData(binRemainder);
                }
            }

        hIn->readRemainder();

        return hPO;
        }
    catch (Exception::View ve)
        {
        String::View vsClass;
        try
            {
            vsClass = hIn->getPofContext()->getClassName(m_nTypeId);
            }
        catch (...) {}

        COH_THROW_STREAM(IOException,
                "An exception occurred instantiating a PortableObject"
                << " user type from a POF stream: type-id=" << m_nTypeId
                << (vsClass == NULL
                        ? String::null_string
                        : COH_TO_STRING(", class-name=" << vsClass))
                << ", exception=\n" << ve);
        }
    }

COH_CLOSE_NAMESPACE3
