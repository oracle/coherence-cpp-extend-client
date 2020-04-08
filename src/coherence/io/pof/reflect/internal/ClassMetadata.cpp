/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/pof/reflect/internal/ClassMetadata.hpp"

#include "coherence/util/Collections.hpp"
#include "coherence/util/HashHelper.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/TreeSet.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

using coherence::util::Collections;
using coherence::util::HashHelper;
using coherence::util::HashMap;
using coherence::util::TreeSet;

// ----- ClassMetadata: constructors ----------------------------------------

ClassMetadata::ClassMetadata()
    : m_vClz(self()), f_vKey(self()), f_hSetAttr(self(), TreeSet::create())
    , f_hMapAttrByName(self(), HashMap::create())
    {
    }

// ----- ClassKey: interface ------------------------------------------------

int32_t ClassMetadata::ClassKey::getTypeId() const
    {
    return m_nTypeId;
    }

int32_t ClassMetadata::ClassKey::getVersionId() const
    {
    return m_nVersionId;
    }

int32_t ClassMetadata::ClassKey::getHash() const
    {
    return m_nHash;
    }

// ----- ClassKey: Object interface -----------------------------------------

bool ClassMetadata::ClassKey::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    ClassMetadata::ClassKey::View that = cast<ClassMetadata::ClassKey::View>(v, false);
    if (NULL == that)
        {
        return false;
        }

    return m_nVersionId == that->getVersionId() &&
           m_nTypeId    == that->getTypeId() &&
           m_nHash      == that->getHash();
    }

size32_t ClassMetadata::ClassKey::hashCode() const
    {
    size32_t nHash = HashHelper::hash(m_nTypeId, 31);
             nHash = HashHelper::hash(m_nVersionId, nHash);
             nHash = HashHelper::hash(m_nHash, nHash);
     return nHash;
    }

TypedHandle<const String>  ClassMetadata::ClassKey::toString() const
    {
    return COH_TO_STRING("ClassKey (typeId=" << m_nTypeId
        << ", versionId="      << m_nVersionId
        << ", hash="           << m_nHash << ")");
    }

// ----- ClassKey: accessors ------------------------------------------------

void ClassMetadata::ClassKey::setTypeId(int32_t nTypeId)
    {
    m_nTypeId = nTypeId;
    }

void ClassMetadata::ClassKey::setVersionId(int32_t nVersionId)
    {
    m_nVersionId = nVersionId;
    }

void ClassMetadata::ClassKey::setHash(int32_t nHash)
    {
    m_nHash = nHash;
    }

// ----- ClassAttribute: constructors ---------------------------------------

ClassMetadata::ClassAttribute::ClassAttribute(String::View vsName,
                                              int32_t      nVersion,
                                              int32_t      nIndex)
    : f_vsName(self(), vsName), m_nVersionId(nVersion), m_nIndex(nIndex)
    , f_vCodec(self()), f_vStrategy(self())
    {
    }

// ----- ClassAttribute interface -------------------------------------------

String::View ClassMetadata::ClassAttribute::getName() const
    {
    return f_vsName;
    }

int32_t ClassMetadata::ClassAttribute::getVersionId() const
    {
    return m_nVersionId;
    }

int32_t ClassMetadata::ClassAttribute::getIndex() const
    {
    return m_nIndex;
    }

Codec::View ClassMetadata::ClassAttribute::getCodec() const
    {
    return f_vCodec;
    }

Object::Holder ClassMetadata::ClassAttribute::get(Object::View ovContainer) const
    {
    assertState();

    return f_vStrategy->get(ovContainer);
    }

void ClassMetadata::ClassAttribute::set(Object::Holder ohContainer, Object::Holder ohValue) const
    {
    assertState();

    f_vStrategy->set(cast<Object::Handle>(ohContainer), ohValue);
    }

// ----- ClassAttribute: helpers --------------------------------------------

void ClassMetadata::ClassAttribute::assertState() const
    {
    if (f_vStrategy == NULL)
        {
        COH_THROW_STREAM(IllegalStateException,
                "ClassMetadata attribute requires an invocation strategy")
        }
    }

// ----- ClassAttribute: Object interface -----------------------------------

bool ClassMetadata::ClassAttribute::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    ClassMetadata::ClassAttribute::View that = cast<ClassMetadata::ClassAttribute::View>(v, false);
    if (NULL == that)
        {
        return false;
        }

    return m_nVersionId == that->getVersionId() &&
           m_nIndex     == that->getIndex()     &&
           f_vsName->equals(that->getName());
    }

size32_t ClassMetadata::ClassAttribute::hashCode() const
    {
    size32_t nHash = HashHelper::hash(m_nVersionId, 31);
             nHash = HashHelper::hash(m_nIndex, nHash);
             nHash = HashHelper::hash(cast<Object::View>(f_vsName), nHash);
    return nHash;
    }

TypedHandle<const String>  ClassMetadata::ClassAttribute::toString() const
    {
    return COH_TO_STRING("ClassAttribute (name=" << f_vsName
        << ", versionId=" << m_nVersionId
        << ", index=" << m_nIndex <<")");
    }

// ----- ClassAttribute::Comparable interface -------------------------------

int32_t ClassMetadata::ClassAttribute::compareTo(Object::View v) const
    {
    if (v == this)
        {
        return 0;
        }

    ClassMetadata::ClassAttribute::View that = cast<ClassMetadata::ClassAttribute::View>(v);

    int32_t n = m_nVersionId - that->getVersionId();
    if (n == 0)
        {
        n = m_nIndex - that->getIndex();
        if (n == 0)
            {
            String::View vsThis = f_vsName;
            String::View vsThat = that->getName();

            n = vsThis == NULL
                ? (vsThat == NULL ? 0 : -1)
                : (vsThat == NULL ? 1 : vsThis->compareTo(vsThat));
            }
        }

    return n;
    }

// ----- ClassAttribute: accessors ------------------------------------------

void ClassMetadata::ClassAttribute::setName(String::View vsName)
    {
    initialize(f_vsName, vsName);
    }

void ClassMetadata::ClassAttribute::setVersionId(int32_t nVersionId)
    {
    m_nVersionId = nVersionId;
    }

void ClassMetadata::ClassAttribute::setIndex(int32_t nIndex)
    {
    m_nIndex = nIndex;
    }

void ClassMetadata::ClassAttribute::setCodec(Codec::View vCodec)
    {
    initialize(f_vCodec, vCodec);
    }

void ClassMetadata::ClassAttribute::setInvocationStrategy(InvocationStrategy::View vStrategy)
    {
    initialize(f_vStrategy, vStrategy);
    }

// ----- ClassMetadata: interface -------------------------------------------

TypeMetadata::TypeKey::View ClassMetadata::getKey() const
    {
    return f_vKey;
    }

Object::Holder ClassMetadata::newInstance() const
    {
    return m_vClz == NULL ? NULL : m_vClz->newInstance();
    }

Iterator::Handle ClassMetadata::getAttributes() const
    {
    return f_hSetAttr->iterator();
    }

TypeMetadata::AttributeMetadata::View ClassMetadata::getAttribute(String::View vsName) const
    {
    return cast<TypeMetadata::AttributeMetadata::View>(f_hMapAttrByName->get(vsName));
    }

// ----- ClassMetadata::Object interface ------------------------------------

bool ClassMetadata::equals(Object::View v) const
    {
    if (this == v)
        {
        return true;
        }

    ClassMetadata::View that = cast<ClassMetadata::View>(v, false);
    if (NULL == that)
        {
        return false;
        }

    if (!Object::equals(getKey(),that->getKey()))
        {
        return false;
        }

    for (Iterator::Handle hIterThis = f_hSetAttr->iterator(), hIterThat = that->f_hSetAttr->iterator();
         hIterThis->hasNext() || hIterThat->hasNext(); )
        {
        ClassMetadata::ClassAttribute::Holder hAttrThis = hIterThis->hasNext()
                ? cast<ClassMetadata::ClassAttribute::Holder>(hIterThis->next()) : NULL;
        ClassMetadata::ClassAttribute::Holder hAttrThat = hIterThat->hasNext()
                ? cast<ClassMetadata::ClassAttribute::Holder>(hIterThat->next()) : NULL;

        if (!Object::equals(hAttrThis, hAttrThat))
            {
            return false;
            }
        }

    return true;
    }

size32_t ClassMetadata::hashCode() const
    {
    TypeKey::View vKey  = getKey();
    size32_t      nHash = HashHelper::hash(vKey->getTypeId(), 31);
                  nHash = HashHelper::hash(vKey->getVersionId(), nHash);
                  nHash = HashHelper::hash(cast<Collection::View>(f_hSetAttr), nHash);
    return nHash;
    }

TypedHandle<const String> ClassMetadata::toString() const
    {
    return COH_TO_STRING("ClassMetadata (key=" << f_vKey << ", attributes=" << f_vKey << ")");
    }

// ----- ClassMetadata: accessors -------------------------------------------

void ClassMetadata::setClass(Class::View vClz)
    {
    m_vClz = vClz;
    }

void ClassMetadata::setKey(TypeKey::View vKey)
    {
    initialize(f_vKey, vKey);
    }

void ClassMetadata::setAttributes(Collection::View vColAttr)
    {
    f_hSetAttr->clear();
    f_hSetAttr->addAll(vColAttr);

    f_hMapAttrByName->clear();
    for (Iterator::Handle hIter = f_hSetAttr->iterator(); hIter->hasNext(); )
        {
        AttributeMetadata::View vAttribute = cast<AttributeMetadata::View>(hIter->next());
        f_hMapAttrByName->put(vAttribute->getName(), vAttribute);
        }
    }

bool ClassMetadata::addAttribute(AttributeMetadata::Handle vAttribute)
    {
    bool fAdded = f_hSetAttr->add(vAttribute);

    // maintain reference
    if (fAdded)
        {
        f_hMapAttrByName->put(vAttribute->getName(), vAttribute);
        }

    return fAdded;
    }

COH_CLOSE_NAMESPACE5
