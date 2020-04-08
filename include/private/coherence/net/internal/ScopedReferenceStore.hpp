/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SCOPED_REFERENCE_STORE_HPP
#define COH_SCOPED_REFERENCE_STORE_HPP

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,net,internal)

using coherence::net::NamedCache;
using coherence::net::OperationalContext;
using coherence::net::Service;
using coherence::security::SecurityHelper;
using coherence::util::Collection;
using coherence::util::Set;


/**
* ScopedReferenceStore holds scoped cache or service references.
*
* Cache references and service references are scoped, optionally, by Subject.
* Subject scoping is handled automatically; ScopedReferenceStore requires no
* explicit input about Subjects from its clients. Subject scoping is
* configured in the operational configuration and applies only to remote cache
* and remote services.
*
* An instance of ScopedReferenceStore must contain either cache references
* or service references, but not both simultaneously.
*
* ScopedReferenceStore is not thread-safe; multi-threaded clients must
* provide their own locking mechanism.
*
* @author dag 2010.03.25
*
* @since Coherence 3.6
*/
class COH_EXPORT ScopedReferenceStore
    : public class_spec<ScopedReferenceStore>
    {
    friend class factory<ScopedReferenceStore>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a ScopedReferenceStore.
        *
        * @param vContext  the OperationalContext containing security
        *                  configuration
        */
        ScopedReferenceStore(OperationalContext::View vContext);


    // ----- ScopedReferenceStore methods -----------------------------------

    public:
        /**
        * Remove all referenced objects from this store.
        */
        virtual void clear();

        /**
        * Retrieve the cache reference associated with the name (and
        * Subject if applicable).
        *
        * @param vsCacheName  the name of the cache
        *
        * @return the hCache reference
        */
        virtual NamedCache::Handle getCache(String::View vsCacheName) const;

        /**
        * Retrieve the service reference based on the passed in service name.
        *
        * @param vsServiceName  the service name
        *
        * @return the Service reference
        */
        virtual Service::Handle getService(String::View vsServiceName) const;

        /**
        * Retrieve all cache references in the store.
        *
        * @return all cache references
        */
        virtual Collection::View getAllCaches() const;

        /**
        * Retrieve all cache references for this name.
        *
        * @param vsCacheName  the name of the cache
        *
        * @return all cache references for this name.
        */
        virtual Collection::View getAllCaches(String::View vsCacheName) const;

        /**
        * Retrieve all service references.
        *
        * @return all cache references for this name.
        */
        virtual Collection::View getAllServices() const;

        /**
        * Retrieve the names of all stored cache or service references.
        *
        * @return the names of all stored references
        */
        virtual Set::View getNames() const;

        /**
        * Store a cache reference.
        *
        * @param hCache   the cache reference
        */
        virtual void putCache(NamedCache::Handle hCache);

        /**
        * Store a service reference.
        *
        * Service name and type are passed in rather than using
        * service.getInfo() because the service may not have been configured
        * and started yet, so the info may not be safely available.
        *
        * @param hService  the referenced service
        * @param vsName    the service name
        * @param vType     the service type
        */
        virtual void putService(Service::Handle hService, String::View vsName,
                ServiceInfo::ServiceType vType);

        /**
        * Remove the referenced cache item from the store.
        *
        * @param hCache  the cache reference
        *
        * @return whether the item was found
        */
        virtual bool releaseCache(NamedCache::Handle hCache);

        /**
        * Remove all items referenced by this name
        *
        * @param  sName the service or cache name
        */
        virtual void remove(String::View vsName);


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Determine if the service type is remote.
        *
        * @param nType  the service type
        *
        * @return whether the service type is remote
        */
        static bool isRemoteServiceType(ServiceInfo::ServiceType nType);


    // ----- inner class: SubjectScopedReference ----------------------------

    protected:
        /**
        * SubjectScopedReference scopes (associates) an object with a Subject.
        */
        class SubjectScopedReference
            : public class_spec<SubjectScopedReference>
            {
            friend class factory<SubjectScopedReference>;

            // ----- handle definitions (needed for nested classes) -----

            public:
                typedef this_spec::Handle Handle;
                typedef this_spec::View   View;
                typedef this_spec::Holder Holder;


            // ----- constructors ---------------------------------------

            protected:
                 /**
                 * Construct a SubjectScopedReference.
                 */
                SubjectScopedReference();


            // ----- SubjectScopedReference methods ---------------------

            public:
                /**
                * Obtain the object referenced by the current subject.
                *
                * @return the referenced object
                */
                virtual Object::Handle get() const;

                /**
                * Determine if there are any referenced objects.
                *
                * @return whether there are any referenced objects.
                */
                virtual bool isEmpty() const;

                /**
                * Add a referenced object based on the current subject.
                *
                * @param hRef  the referenced object
                */
                virtual void set(Object::Handle hRef);

                /**
                * Remove the object referenced by the current subject.
                *
                * @return the previously referenced object
                */
                virtual Object::Handle remove();

                /**
                * Obtain all referenced objects.
                *
                * @return the Collection of referenced objects.
                */
                virtual Collection::View values() const;


            // ----- data fields ----------------------------------------

            private:
                /**
                * The map contains referenced objects keyed by Subject.
                */
                FinalHandle<Map> f_hMapSubjectScope;

                /**
                * Reference stored when the subject is NULL.
                */
                MemberHolder<Object> m_ohRef;
            };


    // ----- data members ---------------------------------------------------

    private:
        /**
        * When storing cache references, it is a map keyed by a cache name with
        * a corresponding value being a NamedCache reference or a
        * SubjectScopedReference that contains a NamedCache reference.
        *
        * When storing service references, it is a map keyed by a service name
        * with a corresponding value being a service reference or a
        * SubjectScopedReference that contains a service reference.
        */
        FinalHandle<Map> f_hMapByName;

        /**
        * OperationalContext for this ScopedReferenceStore.
        */
        FinalView<OperationalContext> f_vContext;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SCOPED_REFERENCE_STORE_HPP
