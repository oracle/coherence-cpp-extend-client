/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
/**
* @mainpage
*
* Coherence for C++ allows C++ applications to access Coherence clustered
* services, including data, data events, and data processing from outside
* the Coherence cluster. Typical uses of Coherence for C++ include desktop
* and that require access to Coherence caches.
*
* The Coherence for C++ API consists of:
* @li Shared libraries for a variety of compilers and operating systems
* @li C++ API header files
* @li API documentation
*
* The library allows applications to connect to Coherence*Extend proxies
* running within the Coherence cluster using a high performance TCP/IP-based
* communication layer. The library sends all client requests to the
* Coherence*Extend clustered service which, in turn, responds to client
* requests by delegating to an actual Coherence clustered service (for example
* a Partitioned or Replicated cache service). As on other Coherence platforms,
* C++ Coherence {@link coherence::net::NamedCache caches} are obtained from
* the {@link coherence::net::CacheFactory CacheFactory}.
*
* In order for application data objects to be stored in the cluster they must
* be serializable to the Coherence POF (Portable Object Format). This may be
* accomplished by either modifying the data object to implement the
* {@link coherence::io::pof::PortableObject PortableObject} interface, or
* though the use of the helper template
* {@link coherence::lang::Managed Managed<T>}. The latter offers an easy
* integration route for existing C++ data objects as in most cases no changes
* to the existing data object code are required.
*
* The Coherence for C++ API is pure C++ and is built on top of a managed object
* model which handles the task of memory management. It is thus not necessary
* or allowable to manually delete objects returned from or injected into the
* cache. See {@link coherence::lang::Object Object} for an introduction to the
* object model.
*/
