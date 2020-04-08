/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/pof/reflect/internal/ClassMetadataBuilder.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Collections.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

COH_REGISTER_TYPED_CLASS(ClassMetadataBuilder);

using coherence::util::ArrayList;
using coherence::util::Collections;
using coherence::util::HashSet;
using coherence::util::Set;

using coherence::util::logging::Logger;

// ----- constructors: ClassMetadataBuilder ---------------------------------

ClassMetadataBuilder::ClassMetadataBuilder()
    : m_hCmd(self(), ClassMetadata::create()), m_hKey(self(), ClassMetadata::ClassKey::create())
    {
    m_hCmd->setKey(m_hKey);
    }

// ----- ClassMetadataBuilder interface -------------------------------------

ClassMetadataBuilder::Handle ClassMetadataBuilder::setClass(Class::View vClz)
    {
    m_hCmd->setClass(vClz);
    return this;
    }

ClassMetadataBuilder::Handle ClassMetadataBuilder::addAttribute(TypeMetadata::AttributeMetadata::Handle hAttribute)
    {
    m_hCmd->addAttribute(hAttribute);
    return this;
    }

ClassAttributeBuilder::Handle ClassMetadataBuilder::newAttribute() const
    {
    return ClassAttributeBuilder::create();
    }

ClassMetadata::Holder ClassMetadataBuilder::build()
    {
    ClassMetadata::Handle         cmd  = m_hCmd;
    ClassMetadata::ClassKey::View vKey = cast<ClassMetadata::ClassKey::View>(cmd->getKey());

    // now that we are aware of entirety of this ClassMetadata instance
    // determine the appropriate indexes or ensure they are explicitly
    // defined
    List::Handle hListNonSorted = ArrayList::create();

    // create an exclusion list of indexes that are explicitly defined
    // i.e. we must be aware of indexes that have been explicitly
    // requested and ensure we only allocate around these reserved blocks
    Set::Handle hSetReserved  = HashSet::create();

    for (Iterator::Handle hAttributes = cmd->getAttributes(); hAttributes->hasNext(); )
        {
        ClassMetadata::ClassAttribute::Handle hIter = cast<ClassMetadata::ClassAttribute::Handle>(hAttributes->next());
        if (hIter->getIndex() >= 0)
            {
            int32_t iProp, iAttr = iProp = hIter->getIndex();
            while (hSetReserved->contains(Integer32::valueOf(iProp)))
                {
                ++iProp;
                }

            if (iProp != iAttr)
                {
                COH_LOG("The requested index " << iAttr
                        << " on a PortableProperty annotation "
                        << "for [typeId=" << vKey->getTypeId()
                        << ", version=" << vKey->getVersionId()
                        << ", property-name=" << hIter->getName()
                        << "] is already allocated to an existing PortableProperty. "
                        << "Allocated index " << iProp
                        << " instead.",
                        Logger::level_d5);

                hIter->setIndex(iProp);
                }

            hSetReserved->add(Integer32::valueOf(iProp));
            }
        }

    int32_t i = 0;
    for (Iterator::Handle hIter = cmd->getAttributes(); hIter->hasNext(); ++i)
        {
        ClassMetadata::ClassAttribute::Handle hAttribute = cast<ClassMetadata::ClassAttribute::Handle>(hIter->next());
        if (hAttribute->getIndex() < 0)
            {
            for ( ; hSetReserved->contains(Integer32::valueOf(i)); ++i) { }
            hAttribute->setIndex(i);
            }
        hListNonSorted->add(hAttribute);
        }
    cmd->setAttributes(hListNonSorted);

    // inform key of the hash of the class structure now that we are
    // primed
    m_hKey->setHash(cmd->hashCode());

    m_hCmd = ClassMetadata::create();
    m_hKey = ClassMetadata::ClassKey::create();
    m_hCmd->setKey(m_hKey);

    return cmd;
    }

// ----- accessors ----------------------------------------------------------

TypeMetadata::View ClassMetadataBuilder::getTypeMetadata() const
    {
    return m_hCmd;
    }

ClassMetadataBuilder::Handle ClassMetadataBuilder::setTypeId(int32_t nTypeId)
    {
    m_hKey->setTypeId(nTypeId);
    return this;
    }

ClassMetadataBuilder::Handle ClassMetadataBuilder::setVersionId(int32_t nVersionId)
    {
    m_hKey->setVersionId(nVersionId);
    return this;
    }

ClassMetadataBuilder::Handle ClassMetadataBuilder::setHash(int32_t nHash)
    {
    m_hKey->setHash(nHash);
    return this;
    }

// ----- Visitor::Recipient interface ---------------------------------------

void ClassMetadataBuilder::accept(Visitor::View vVisitor, Class::View vClz)
    {
    Class::View  vClzObject    = SystemClassLoader::getInstance()->loadByType(typeid(Object));
    Class::View  vClzRecipient = vClz;
    List::Handle hListHiearchy = ArrayList::create();
    while (NULL != vClzRecipient && !vClzObject->equals(vClzRecipient))
        {
        hListHiearchy->add(vClzRecipient);
        vClzRecipient = vClzRecipient->getSuperclass();
        }

    for (int32_t i = hListHiearchy->size() - 1; i >= 0; --i)
        {
        vVisitor->visit(this, cast<Class::View>(hListHiearchy->get(i)));
        }
    }

COH_CLOSE_NAMESPACE5
