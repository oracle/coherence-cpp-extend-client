/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/internal/ScopedReferenceStore.hpp"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,net,internal)

using coherence::net::CacheService;
using coherence::net::ServiceInfo;
using coherence::security::auth::Subject;
using coherence::security::SecurityHelper;
using coherence::util::Collections;
using coherence::util::HashSet;
using coherence::util::Iterator;
using coherence::util::SafeHashMap;
using coherence::util::Set;


// ----- constructors -------------------------------------------------------

ScopedReferenceStore::ScopedReferenceStore(OperationalContext::View vContext)
    : f_hMapByName(self(), SafeHashMap::create()),
      f_vContext(self(), vContext)
    {
    }


// ----- ScopedReferenceStore methods ---------------------------------------

void ScopedReferenceStore::clear()
    {
    f_hMapByName->clear();
    }

NamedCache::Handle
ScopedReferenceStore::getCache(String::View vsCacheName) const
    {
    Object::Holder ohHolder = f_hMapByName->get(vsCacheName);

    if (ohHolder == NULL || instanceof<NamedCache::Handle>(ohHolder))
        {
        return cast<NamedCache::Handle>(ohHolder);
        }
    else if (instanceof<SubjectScopedReference::Handle>(ohHolder))
        {
        SubjectScopedReference::Handle hScopedRef =
                cast<SubjectScopedReference::Handle>(ohHolder);
        return cast<NamedCache::Handle>(hScopedRef->get());
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create("invalid type"));
        }
    }

Service::Handle
ScopedReferenceStore::getService(String::View vsServiceName) const
    {
    Object::Holder ohHolder = f_hMapByName->get(vsServiceName);

    if (ohHolder == NULL || instanceof<Service::Handle>(ohHolder))
        {
        return cast<Service::Handle>(ohHolder);
        }
    else if (instanceof<SubjectScopedReference::Handle>(ohHolder))
        {
        SubjectScopedReference::Handle hScopedRef =
                cast<SubjectScopedReference::Handle>(ohHolder);
        return cast<Service::Handle>(hScopedRef->get());
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create("invalid type"));
        }
    }

Collection::View ScopedReferenceStore::getAllCaches() const
    {
    Set::Handle hSetRef = HashSet::create();

    for (Iterator::Handle hIter = f_hMapByName->values()->iterator();
            hIter->hasNext(); )
        {
        Object::Holder ohHolder = hIter->next();

        if (instanceof<SubjectScopedReference::Handle>(ohHolder))
            {
            hSetRef->addAll(cast<SubjectScopedReference::Handle>(ohHolder)->values());
            }
        else if (instanceof<NamedCache::Handle>(ohHolder))
            {
            hSetRef->add(ohHolder);
            }
        else
            {
            COH_THROW (UnsupportedOperationException::create("invalid type"));
            }
        }
    return hSetRef;
    }

Collection::View
ScopedReferenceStore::getAllCaches(String::View vsCacheName) const
    {
    Set::Handle    hSetRef  = HashSet::create();
    Object::Holder ohHolder = f_hMapByName->get(vsCacheName);

    if (instanceof<NamedCache::Handle>(ohHolder))
        {
        hSetRef->add(ohHolder);
        }
    else if (instanceof<SubjectScopedReference::Handle>(ohHolder))
        {
        hSetRef->addAll(cast<SubjectScopedReference::Handle>(ohHolder)->values());
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create("invalid type"));
        }
    return hSetRef;
    }

Collection::View ScopedReferenceStore::getAllServices() const
    {
    Set::Handle hSetRef = HashSet::create();

    for (Iterator::Handle hIter = f_hMapByName->values()->iterator();
            hIter->hasNext(); )
        {
        Object::Holder ohHolder = hIter->next();

        if (instanceof<SubjectScopedReference::Handle>(ohHolder))
            {
            hSetRef->addAll(cast<SubjectScopedReference::Handle>(ohHolder)->values());
            }
        else if (instanceof<Service::Handle>(ohHolder))
            {
            hSetRef->add(ohHolder);
            }
        else
            {
            COH_THROW (UnsupportedOperationException::create("invalid type"));
            }
        }
    return hSetRef;
    }

Set::View ScopedReferenceStore::getNames() const
    {
    return f_hMapByName->keySet();
    }

void ScopedReferenceStore::putCache(NamedCache::Handle hCache)
    {
    String::View       vsCacheName   = hCache->getCacheName();
    CacheService::View vCacheService = hCache->getCacheService();

    if (vCacheService != NULL &&
            isRemoteServiceType(vCacheService->getInfo()->getServiceType())
            && f_vContext->isSubjectScopingEnabled())
        {
        SubjectScopedReference::Handle hScopedRef =
                cast<SubjectScopedReference::Handle>(f_hMapByName->get(vsCacheName));
        if (hScopedRef == NULL)
            {
            hScopedRef = SubjectScopedReference::create();
            f_hMapByName->put(vsCacheName, hScopedRef);
            }
        hScopedRef->set(hCache);
        }
    else
        {
        f_hMapByName->put(vsCacheName, hCache);
        }
    }

void ScopedReferenceStore::putService(Service::Handle hService,
        String::View vsName, ServiceInfo::ServiceType vType)
    {

    if (isRemoteServiceType(vType) && f_vContext->isSubjectScopingEnabled())
        {
        SubjectScopedReference::Handle hScopedRef =
                cast<SubjectScopedReference::Handle>(f_hMapByName->get(vsName));
        if (hScopedRef == NULL)
            {
            hScopedRef = SubjectScopedReference::create();
            f_hMapByName->put(vsName, hScopedRef);
            }
        hScopedRef->set(hService);
        }
    else
        {
        f_hMapByName->put(vsName, hService);
        }
    }

bool ScopedReferenceStore::releaseCache(NamedCache::Handle hCache)
    {
    String::View   vsCacheName = hCache->getCacheName();
    bool           fFound      = false;
    Object::Holder ohHolder    = f_hMapByName->get(vsCacheName);

    if (ohHolder == hCache)
        {
        // remove the mapping
        f_hMapByName->remove(vsCacheName);
        fFound = true;
        }
    else if (instanceof<SubjectScopedReference::Handle>(ohHolder))
        {
        SubjectScopedReference::Handle hScopedRef =
                cast<SubjectScopedReference::Handle>(ohHolder);

        if (hScopedRef->get() == hCache)
            {
            hScopedRef->remove();
            fFound = true;
            if (hScopedRef->isEmpty())
                {
                f_hMapByName->remove(vsCacheName);
                }
            }
        }

    return fFound;
    }

void ScopedReferenceStore::remove(String::View vsName)
    {
    f_hMapByName->remove(vsName);
    }

bool ScopedReferenceStore::isRemoteServiceType(ServiceInfo::ServiceType nType)
    {
    return nType == ServiceInfo::remote_cache ||
           nType == ServiceInfo::remote_invocation;
    }


// ----- inner class: SubjectScopedReference --------------------------------

// ----- constructors ---------------------------------------------------

ScopedReferenceStore::SubjectScopedReference::SubjectScopedReference()
    : f_hMapSubjectScope(self(), SafeHashMap::create()), m_ohRef(self())
    {
    }

Object::Handle ScopedReferenceStore::SubjectScopedReference::get() const
    {
    Subject::View vSubject = SecurityHelper::getCurrentSubject();

    if (vSubject == NULL)
        {
        return cast<Object::Handle>(m_ohRef);
        }
    else
        {
        return cast<Object::Handle>(f_hMapSubjectScope->get(vSubject));
        }
    }

bool ScopedReferenceStore::SubjectScopedReference::isEmpty() const
    {
    Subject::View vSubject = SecurityHelper::getCurrentSubject();

    return vSubject == NULL ? m_ohRef == NULL
            : f_hMapSubjectScope->isEmpty();
    }

void ScopedReferenceStore::SubjectScopedReference::set(Object::Handle hRef)
    {
    Subject::View vSubject = SecurityHelper::getCurrentSubject();

    if (vSubject == NULL)
        {
        m_ohRef = hRef;
        }
    else
        {
        f_hMapSubjectScope->put(vSubject, hRef);
        }
    }

Object::Handle ScopedReferenceStore::SubjectScopedReference::remove()
    {
    Subject::View vSubject = SecurityHelper::getCurrentSubject();

    return vSubject == NULL ? cast<Object::Handle>(m_ohRef = NULL)
            : cast<Object::Handle>(f_hMapSubjectScope->remove(vSubject));
    }

Collection::View ScopedReferenceStore::SubjectScopedReference::values() const
    {
    Object::Holder ohRef = m_ohRef;

    // return any subject scoped entries
    if (ohRef == NULL)
        {
        return f_hMapSubjectScope->values();
        }

    // a single non-subject scoped entry
    if (f_hMapSubjectScope->isEmpty())
        {
        return Collections::singleton(ohRef);
        }

    // both one or more subject scoped entries and a non-subject scoped entry
    Collection::Handle hCol = HashSet::create(f_hMapSubjectScope->values());
    hCol->add(ohRef);
    return hCol;
    }

COH_CLOSE_NAMESPACE3

