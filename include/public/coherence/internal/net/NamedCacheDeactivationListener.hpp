/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAMEDCACHEDEACTIVATIONLISTENER_HPP
#define COH_NAMEDCACHEDEACTIVATIONLISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapListenerSupport.hpp"

COH_OPEN_NAMESPACE3(coherence,internal,net)

using namespace coherence::lang;

using coherence::util::MapListenerSupport;

/**
 * Pseudo MapListener that can be used to listen for a deactivation event
 * from a NamedCache.
 *
 * Instances of this interface can be added to a NamedCache with the single
 * parameter addMapListener()method. The reason for calling 
 * each method is defined below:
 * <ol>
 *     <li>entryDeleted - a cache has been destroyed</li>
 *     <li>entryUpdated - a cache has been truncated.</li>
 *     <li>entryInserted - Unused.</li>
 * </ol>
 *
 * @author jh 2013.06.27
 */
class COH_EXPORT NamedCacheDeactivationListener
    : public interface_spec<NamedCacheDeactivationListener,
            implements<MapListenerSupport::SynchronousListener> >
    {
    };

COH_CLOSE_NAMESPACE3

#endif /* COH_NAMEDCACHEDEACTIVATIONLISTENER_HPP */
