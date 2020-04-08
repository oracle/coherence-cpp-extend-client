/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/pof/reflect/internal/ClassAttributeBuilder.hpp"

#include "coherence/util/Hashtable.hpp"

#include "private/coherence/io/pof/reflect/internal/ClassMetadata.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

using coherence::util::Hashtable;

// ----- constructors: ClassAttributeBuilder --------------------------------

ClassAttributeBuilder::ClassAttributeBuilder()
    : m_hAttribute(self(), ClassMetadata::ClassAttribute::create())
    {
    }

// ----- ClassAttributeBuilder: interface -----------------------------------

ClassAttributeBuilder::Handle ClassAttributeBuilder::setName(String::View vsName)
    {
    m_hAttribute->setName(vsName);
    return this;
    }

ClassAttributeBuilder::Handle ClassAttributeBuilder::setVersionId(int32_t nVersionId)
    {
    m_hAttribute->setVersionId(nVersionId);
    return this;
    }

ClassAttributeBuilder::Handle ClassAttributeBuilder::setIndex(int32_t nIndex)
    {
    m_hAttribute->setIndex(nIndex);
    return this;
    }

ClassAttributeBuilder::Handle ClassAttributeBuilder::setCodec(Codec::View vCodec)
    {
    m_hAttribute->setCodec(vCodec);
    return this;
    }

ClassAttributeBuilder::Handle ClassAttributeBuilder::setInvocationStrategy(InvocationStrategy::View vStrategy)
    {
    m_hAttribute->setInvocationStrategy(vStrategy);
    return this;
    }

ClassMetadata::ClassAttribute::Handle ClassAttributeBuilder::build()
    {
    ClassMetadata::ClassAttribute::Handle attribute = m_hAttribute;
    m_hAttribute = ClassMetadata::ClassAttribute::create();
    return attribute;
    }

COH_CLOSE_NAMESPACE5
