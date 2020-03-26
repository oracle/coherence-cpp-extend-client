/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERVICE_INFO_HPP
#define COH_SERVICE_INFO_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,net)


/**
* The ServiceInfo represents information about a Service that operates in a
* clustered network environment.
*
* @author jh  2007.12.20
*/
class COH_EXPORT ServiceInfo
    : public interface_spec<ServiceInfo>
    {
    // ----- enums ----------------------------------------------------------

    public:
        /**
        * Service type constants.
        */
        enum ServiceType
            {
            // REVIEW: will there be a local CacheService implementation?
            // we don't currently have one in .NET
            local_cache,       // a local cache service provides the means
                               // for handling a collection  of resources
                               // limited to a single client with concurrent
                               // access control

            remote_cache,      // a remote cache service provides the means
                               // for handling a collection of resources
                               // managed by a remote cluster with concurrent
                               // access control

            remote_invocation, // a remote invocation service enables the
                               // execution of invocable objects by a remote
                               // cluster member
                               
            remote_nameservice // a remote name service allows a JVM to use 
                               // a remote NameService without having to join
                               // the Cluster.
            };


    // ----- ServiceInfo interface ------------------------------------------

    public:
        /**
        * Return the name of the Service.
        *
        * @return the name of the Service
        */
        virtual String::View getServiceName() const = 0;

        /**
        * Return the type of the Service.
        *
        * @return the type of the Service
        */
        virtual ServiceType getServiceType() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERVICE_INFO_HPP
